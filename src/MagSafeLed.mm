/*
 * MagSafeLed
 * Copyright (C) 2026 Great Andy
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#import "MagSafeLed.h"
#import "SMCManager.h"

#import <charconv>
#import <errno.h>
#import <getopt.h>

#import <IOKit/ps/IOPowerSources.h>
#import <IOKit/ps/IOPSKeys.h>

enum SMC_ACLC_Led
{
    SMC_ACLC_LED_SYSTEM = 0,
    SMC_ACLC_LED_OFF = 1,
    SMC_ACLC_LED_GREEN = 3,
    SMC_ACLC_LED_ORANGE = 4,
    SMC_ACLC_LED_ORANGE_SLOW = 6,
    SMC_ACLC_LED_ORANGE_FAST = 7
};

@implementation MagSafeLed
{
    NSString* mAppName;
    ::dispatch_source_t mSigtermSource;
    int mLockFileDescriptor;

    BOOL mOptLedSleep;
    BOOL mOptLedSleepFull;
    BOOL mOptLedGreen;

    BOOL mOptHelp;
    BOOL mOptVersion;
    BOOL mOptInvalid;

    int  mBatteryPercent;
    BOOL mIsOnAC;
    BOOL mIsBatteryCharging;
    BOOL mIsBatteryFull;
    BOOL mIsDisplaySleep;

    SMCManager mSMCManager;
}

- (void)applicationDidFinishLaunching:(NSNotification*)aNotification
{
    mAppName = [[NSRunningApplication currentApplication] localizedName];
    mLockFileDescriptor = -1;

    BOOL isOptSet = mOptLedSleep || mOptLedSleepFull || mOptLedGreen;
    if (mOptHelp || mOptInvalid)
    {
        [self PrintUsage];
        isOptSet = NO;
    }
    else if (mOptVersion)
    {
        [self PrintVersion];
        isOptSet = NO;
    }
    else if (![self IsRunningAsRoot])
    {
        printf("Root privileges required, run with: sudo %s\n", [mAppName UTF8String]);
        isOptSet = NO;
    }
    else
    {
        if ([self SendSignalToLockedInstance])
        {
            printf("Stopping MagSafeLed daemon\n");
        }

        if (isOptSet)
        {
            if ([self WaitForInstanceLock])
            {
                if (mSMCManager.Open())
                {
                    printf("Starting MagSafeLed daemon\n");

                    [self SetSignalHandler];
                    [self SetupCallbacks];
                    [self SetupNotificationObservers];
                    [self UpdatePowerSources];
                }
                else
                {
                    isOptSet = NO;
                }
            }
            else
            {
                isOptSet = NO;
            }
        }
    }

    if (!isOptSet)
    {
        [NSApp terminate:nil];
    }
}

- (void)applicationWillTerminate:(NSNotification*)notification
{
    if (mSMCManager.IsOpen())
    {
        mSMCManager.WriteKey("ACLC", SMC_ACLC_LED_SYSTEM);
        mSMCManager.Close();
    }

    [self ReleaseInstanceLock];
}

- (void)ParseOptions:(int)argc argV:(char* const*)argv
{
    struct ::option longOptions[] =
    {
        {"sleep",   no_argument, 0, 's'},
        {"full",    no_argument, 0, 'f'},
        {"green",   no_argument, 0, 'g'},
        {"help",    no_argument, 0, 'h'},
        {"version", no_argument, 0, 'v'},
        {0, 0, 0, 0}
    };

    ::opterr = 0;

    int opt;
    while ((opt = ::getopt_long(argc, argv, "sfghv", longOptions, NULL)) != -1)
    {
        switch (opt)
        {
            case 's':
                mOptLedSleep = YES;
                break;
            case 'f':
                mOptLedSleepFull = YES;
                break;
            case 'g':
                mOptLedGreen = YES;
                break;
            case 'h':
                mOptHelp = YES;
                break;
            case 'v':
                mOptVersion = YES;
                break;
            case '?':
                if (::optopt != 0)
                {
                    printf("%s: Invalid option -%c\n", argv[0], ::optopt);
                }
                else if ((::optind > 0) && (::optind <= argc))
                {
                    printf("%s: Invalid option %s\n", argv[0], argv[::optind - 1]);
                }
                mOptInvalid = true;
                break;
            default:
                break;
        }
    }

    for (int i = ::optind; i < argc; i++)
    {
        printf("%s: Invalid option %s\n", argv[0], argv[i]);
        mOptInvalid = true;
    }
}

- (void)PrintUsage
{
    const char* appName = [mAppName UTF8String];
    printf("MagSafe LED daemon\n");
    printf("Usage: %s [options]\n", appName);
    printf("Options:\n");
    printf("  -s, --sleep   Set led off if screens did sleep.\n");
    printf("  -f, --full    Set led off if screens did sleep and not charging.\n");
    printf("  -g, --green   Set led green if not charging.\n");
    printf("  -h, --help    Print this message and exit.\n");
    printf("  -v, --version Print version and exit.\n");
    printf("  no options    Stop daemon and exit.\n");
}

- (void)PrintVersion
{
    const char* appName = [mAppName UTF8String];
#ifdef APP_VERSION
    const char* version = APP_VERSION;
#else
    const char* version = "V0.0.0-unknown";
#endif
    printf("%s version %s\n", appName, version);
}

- (BOOL)IsRunningAsRoot
{
    return (::geteuid() == 0);
}

- (BOOL)SetSignalHandler
{
    if (mSigtermSource == nil)
    {
        if (::signal(SIGTERM, SIG_IGN) != SIG_ERR)
        {
            mSigtermSource = ::dispatch_source_create(DISPATCH_SOURCE_TYPE_SIGNAL, SIGTERM, 0, ::dispatch_get_main_queue());
            if (mSigtermSource == nil)
            {
                ::signal(SIGTERM, SIG_DFL);
            }
            else
            {
                __weak MagSafeLed* weakSelf = self;
                ::dispatch_source_set_event_handler(mSigtermSource, ^{
                    printf("Terminate MagSafeLed daemon\n");
                    [weakSelf terminate];
                });
                ::dispatch_resume(mSigtermSource);
            }
        }
    }
    return (mSigtermSource != nil);
}

- (void)terminate
{
    [NSApp terminate:nil];
}

- (NSString*)GetLockFilePath
{
    return [NSTemporaryDirectory() stringByAppendingPathComponent:@"MagSafeLed.lock"];
}

- (BOOL)WaitForInstanceLock
{
    BOOL success = NO;
    for (int i = 0; i < 30; i++)
    {
        if ([self AcquireInstanceLock])
        {
            success = YES;
            break;
        }
        ::usleep(100000);
    }
    return success;
}

- (BOOL)AcquireInstanceLock
{
    if (mLockFileDescriptor < 0)
    {
        NSString* lockFilePath = [self GetLockFilePath];
        int fd = ::open(lockFilePath.fileSystemRepresentation, O_CREAT | O_RDWR, 0644);
        if (fd == -1)
        {
            printf("Error: Unable to open lock file %s (%d)\n", lockFilePath.fileSystemRepresentation, errno);
        }
        else
        {
            if (::flock(fd, LOCK_EX | LOCK_NB) != 0)
            {
                ::close(fd);
            }
            else
            {
                ::ftruncate(fd, 0);
                ::dprintf(fd, "%d\n", ::getpid());
                mLockFileDescriptor = fd;
            }
        }
    }
    return (mLockFileDescriptor >= 0);
}

- (void)ReleaseInstanceLock
{
    if (mLockFileDescriptor >= 0)
    {
        ::flock(mLockFileDescriptor, LOCK_UN);
        ::close(mLockFileDescriptor);
        mLockFileDescriptor = -1;
    }
}

- (pid_t)GetLockedProcessIdentifier
{
    pid_t otherPid = 0;
    NSString* lockFilePath = [self GetLockFilePath];
    int fd = ::open(lockFilePath.fileSystemRepresentation, O_CREAT | O_RDWR, 0644);
    if (fd != -1)
    {
        if (::flock(fd, LOCK_EX | LOCK_NB) == 0)
        {
            ::flock(fd, LOCK_UN);
        }
        else if (errno == EWOULDBLOCK)
        {
            ::lseek(fd, 0, SEEK_SET);
            char buffer[32] = {0};
            ssize_t bytesRead = ::read(fd, buffer, sizeof(buffer) - 1);
            if (bytesRead > 0)
            {
                auto result = std::from_chars(buffer, buffer + bytesRead, otherPid);
                if (result.ec != std::errc())
                {
                    otherPid = 0;
                }
            }
        }
        ::close(fd);
    }
    return otherPid;
}

- (BOOL)SendSignalToLockedInstance
{
    BOOL success = NO;
    pid_t otherPid = [self GetLockedProcessIdentifier];
    if ((otherPid > 0) && (otherPid != ::getpid()))
    {
        if (::kill(otherPid, SIGTERM) == 0)
        {
            success = YES;
        }
    }
    return success;
}

void PowerSourceChangedCallback(void* context)
{
    if (context != NULL)
    {
        MagSafeLed* magSafeLed = (__bridge MagSafeLed*)context;
        [magSafeLed UpdatePowerSources];
    }
}

- (void)SetupCallbacks
{
    CFRunLoopSourceRef runLoopSource = IOPSNotificationCreateRunLoopSource(PowerSourceChangedCallback, (__bridge void*)self);
    if (runLoopSource)
    {
        CFRunLoopAddSource(CFRunLoopGetCurrent(), runLoopSource, kCFRunLoopDefaultMode);
        CFRelease(runLoopSource);
    }
}

- (void)SetupNotificationObservers
{
    NSNotificationCenter* center = [[NSWorkspace sharedWorkspace] notificationCenter];

    [center addObserver:self
               selector:@selector(ScreensDidSleep:)
                   name:NSWorkspaceScreensDidSleepNotification
                 object:nil];

    [center addObserver:self
               selector:@selector(ScreensDidWake:)
                   name:NSWorkspaceScreensDidWakeNotification
                 object:nil];

    [center addObserver:self
               selector:@selector(WillSleep:)
                   name:NSWorkspaceWillSleepNotification
                 object:nil];

    [center addObserver:self
               selector:@selector(DidWake:)
                   name:NSWorkspaceDidWakeNotification
                 object:nil];
}

- (void)ScreensDidSleep:(NSNotification*)notification
{
    mIsDisplaySleep = YES;
    [self UpdateMagSafeLed];
}

- (void)ScreensDidWake:(NSNotification*)notification
{
    mIsDisplaySleep = NO;
    [self UpdateMagSafeLed];
}

- (void)WillSleep:(NSNotification*)notification
{
    NSLog(@"WillSleepNotification, update LED");
    [self UpdateMagSafeLed];
}

- (void)DidWake:(NSNotification*)notification
{
    NSLog(@"DidWakeNotification, update LED");
    [self UpdateMagSafeLed];
}

- (void)UpdatePowerSources
{
    CFTypeRef snapshot = IOPSCopyPowerSourcesInfo();
    if (snapshot != NULL)
    {
        NSArray* sources = (__bridge_transfer NSArray*)IOPSCopyPowerSourcesList(snapshot);
        for (id source in sources)
        {
            NSDictionary* dict = (__bridge NSDictionary*)IOPSGetPowerSourceDescription(snapshot, (__bridge CFTypeRef)source);
            if (dict != nil)
            {
                NSString* transportType = dict[@kIOPSTransportTypeKey];
                if ([transportType isEqualToString:@kIOPSInternalType])
                {
                    mBatteryPercent = [dict[@kIOPSCurrentCapacityKey] intValue];
                    mIsOnAC = [dict[@kIOPSPowerSourceStateKey] isEqualToString:@kIOPSACPowerValue];
                    mIsBatteryCharging = [dict[@kIOPSIsChargingKey] boolValue];
                    mIsBatteryFull = [dict[@kIOPSIsChargedKey] boolValue];
                    [self UpdateMagSafeLed];
                }
            }
        }
        CFRelease(snapshot);
    }
}

- (void)UpdateMagSafeLed
{
    if (mIsOnAC)
    {
        SMC_ACLC_Led led = SMC_ACLC_LED_SYSTEM;

        BOOL isBatteryFull = (mIsBatteryFull || !mIsBatteryCharging || (mBatteryPercent > 99));
        if (mOptLedGreen)
        {
            if (isBatteryFull)
            {
                led = SMC_ACLC_LED_GREEN;
            }
        }
        if (mIsDisplaySleep)
        {
            if (mOptLedSleep || mOptLedSleepFull)
            {
                if (!mOptLedSleepFull || isBatteryFull)
                {
                    led = SMC_ACLC_LED_OFF;
                }
            }
        }

        if (mSMCManager.WriteKey("ACLC", UInt8(led)))
        {
            NSString* ledText;
            if (led == SMC_ACLC_LED_SYSTEM) ledText = @"LED SYSTEM";
            else if (led == SMC_ACLC_LED_OFF) ledText = @"LED OFF";
            else if (led == SMC_ACLC_LED_GREEN) ledText = @"LED GREEN";
            else if (led == SMC_ACLC_LED_ORANGE) ledText = @"LED ORANGE";
            NSLog(@"DisplaySleep %d, Battery %d%%, Charging %d, Full %d => %@ (%d)",
                  mIsDisplaySleep, mBatteryPercent, mIsBatteryCharging, mIsBatteryFull, ledText, led);
        }
    }
    else
    {
        NSLog(@"DisplaySleep %d, Battery %d%%, Charging %d, Full %d => not connected",
              mIsDisplaySleep, mBatteryPercent, mIsBatteryCharging, mIsBatteryFull);
    }
}

@end
