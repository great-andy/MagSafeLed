/*
 * Apple System Management Control (SMC) Manager
 * Copyright (C) 2006 devnull 
 * Portions Copyright (C) 2013 Michael Wilber
 * C++ Version Copyright (C) 2026 Great Andy
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

#pragma once

#include <IOKit/IOTypes.h>
#include <os/lock.h>

#include <cstring>
#include <map>

struct SMCKeyDataVersion
{
    UInt8   mMajor = 0;
    UInt8   mMinor = 0;
    UInt8   mBuild = 0;
    UInt8   mReserved = 0;
    UInt16  mRelease = 0;
    UInt8   _padding[2] = {0, 0};
};

struct SMCKeyDataLimit
{
    UInt16  mVersion = 0;
    UInt16  mLength = 0;
    UInt32  mCpuPLimit = 0;
    UInt32  mGpuPLimit = 0;
    UInt32  mMemPLimit = 0;
};

struct SMCKeyDataKeyInfo
{
    UInt32  mDataSize = 0;
    UInt32  mDataType = 0;
    UInt8   mDataAttributes = 0;
    UInt8   _padding[3] = {0, 0, 0};
};

class SMCBytes
{
public:
    SMCBytes();

    size_t SetHexString(const std::string& hexString);
    void PrintBytesHex(size_t numBytes) const;

    UInt8 mData[32];
};

struct SMCKeyData
{
    UInt32              mKey = 0;
    SMCKeyDataVersion   mVersion;
    SMCKeyDataLimit     mLimit;
    SMCKeyDataKeyInfo   mKeyInfo;
    UInt8               mResult = 0;
    UInt8               mStatus = 0;
    UInt8               mCommand = 0;
    UInt8               _padding = 0;
    UInt32              mData32 = 0;
    SMCBytes            mBytes;
};

static_assert(sizeof(SMCKeyData) == 80, "SMCKeyData has wrong size.");

class SMCValue
{
public:
    std::string mKey;
    std::string mDataType;

    UInt32      mDataSize = 0;
    SMCBytes    mBytes;

    static constexpr std::string_view DATATYPE_UINT8  = "ui8 ";
    static constexpr std::string_view DATATYPE_UINT16 = "ui16";
    static constexpr std::string_view DATATYPE_UINT32 = "ui32";

    static constexpr std::string_view DATATYPE_FLT  = "flt ";
    static constexpr std::string_view DATATYPE_FP1F = "fp1f";
    static constexpr std::string_view DATATYPE_FP4C = "fp4c";
    static constexpr std::string_view DATATYPE_FP5B = "fp5b";
    static constexpr std::string_view DATATYPE_FP6A = "fp6a";
    static constexpr std::string_view DATATYPE_FP79 = "fp79";
    static constexpr std::string_view DATATYPE_FP88 = "fp88";
    static constexpr std::string_view DATATYPE_FPA6 = "fpa6";
    static constexpr std::string_view DATATYPE_FPC4 = "fpc4";
    static constexpr std::string_view DATATYPE_FPE2 = "fpe2";

    static constexpr std::string_view DATATYPE_SP1E = "sp1e";
    static constexpr std::string_view DATATYPE_SP3C = "sp3c";
    static constexpr std::string_view DATATYPE_SP4B = "sp4b";
    static constexpr std::string_view DATATYPE_SP5A = "sp5a";
    static constexpr std::string_view DATATYPE_SP69 = "sp69";
    static constexpr std::string_view DATATYPE_SP78 = "sp78";
    static constexpr std::string_view DATATYPE_SP87 = "sp87";
    static constexpr std::string_view DATATYPE_SP96 = "sp96";
    static constexpr std::string_view DATATYPE_SPB4 = "spb4";
    static constexpr std::string_view DATATYPE_SPF0 = "spf0";

    static constexpr std::string_view DATATYPE_SI8  = "si8 ";
    static constexpr std::string_view DATATYPE_SI16 = "si16";

    static constexpr std::string_view DATATYPE_PWM  = "{pwm";

    void PrintUInt() const;
    void PrintFLT() const;
    void PrintFP1F() const;
    void PrintFP4C() const;
    void PrintFP5B() const;
    void PrintFP6A() const;
    void PrintFP79() const;
    void PrintFP88() const;
    void PrintFPA6() const;
    void PrintFPC4() const;
    void PrintFPE2() const;
    void PrintSP1E() const;
    void PrintSP3C() const;
    void PrintSP4B() const;
    void PrintSP5A() const;
    void PrintSP69() const;
    void PrintSP78() const;
    void PrintSP87() const;
    void PrintSP96() const;
    void PrintSPB4() const;
    void PrintSPF0() const;
    void PrintSI8() const;
    void PrintSI16() const;
    void PrintPWM() const;
    void PrintBytesHex() const;

    void Print() const;
};

class SMCUtils
{
public:
    static UInt32 GetUInt32(const std::string& str);
    static std::string GetString(UInt32 value);

    static SInt32 HexCharToInt(char c);
};

class SMCManager
{
public:
    virtual ~SMCManager();

    enum SMCSelector
    {
        KERNEL_INDEX_SMC = 2
    };

    enum SMCCommand
    {
        SMC_CMD_READ_BYTES   = 5,
        SMC_CMD_WRITE_BYTES  = 6,
        SMC_CMD_READ_INDEX   = 8,
        SMC_CMD_READ_KEYINFO = 9,
        SMC_CMD_READ_PLIMIT  = 11,
        SMC_CMD_READ_VERS    = 12
    };

    bool Open();
    void Close();
    bool IsOpen() const;

    bool WriteKey(const std::string& key, UInt8 value);
    bool WriteKey(const std::string& key, const std::string& hexString);

    kern_return_t ReadKey(const std::string& key, SMCValue& returnValue);

    kern_return_t GetKeyInfo(UInt32 key, SMCKeyDataKeyInfo& keyInfo);

    kern_return_t WriteValue(const SMCValue& writeVal);

    kern_return_t IOConnectCall(SMCSelector index, const SMCKeyData& keyData, SMCKeyData& returnData);

protected:
    io_connect_t mConn = 0;

    typedef std::unordered_map<UInt32, SMCKeyDataKeyInfo> KeyInfoMapT;
    KeyInfoMapT mCacheItems;

    os_unfair_lock mLock = OS_UNFAIR_LOCK_INIT;
};
