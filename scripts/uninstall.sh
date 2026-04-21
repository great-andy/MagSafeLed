#!/bin/zsh

BINDIR='/usr/local/bin'
APPNAME='MagSafeLed'

DAEMONDIR='/Library/LaunchDaemons'
DAEMON='at.GreatAndy.MagSafeLed'
DAEMONPLIST="$DAEMON.plist"

# Check privileges
if [[ $EUID -ne 0 ]]; then
   echo "Superuser privileges required to uninstall launch daemon, use: sudo ${APPNAME}_uninstall"
   exit 1
fi

# Uninstall daemon if running
if launchctl list | grep -q $DAEMON; then
    if launchctl bootout system "$DAEMONDIR/$DAEMONPLIST"; then
        echo "Uninstalled LaunchDaemon $DAEMON"
    else
        pkill -x $APPNAME
    fi

    rm "$DAEMONDIR/$DAEMONPLIST"
    rm "$BINDIR/$APPNAME"
    rm "$BINDIR/${APPNAME}_uninstall"

    pkgutil --forget $DAEMON 2>/dev/null
else
    echo "LaunchDaemon $DAEMON is not installed"
fi
