#!/bin/zsh

BINDIR='/usr/local/bin'
APPNAME='MagSafeLed'

DAEMON='at.GreatAndy.MagSafeLed'
DAEMONPLIST="$DAEMON.plist"
TEMP_PLIST="/tmp/$DAEMONPLIST"

# Change directory to script location
cd "$(dirname "$0")"

# Check privileges
if [[ $EUID -ne 0 ]]; then
   echo "Superuser privileges required to install launch daemon, use: sudo ./install.sh"
   exit 1
fi

# Check arguments
ARGUMENTS=""
if [ "$#" -eq 0 ]; then
    # Get arguments from interactive choice
    source "preinstall"
elif [[ $1 == -* ]]; then
    # Get arguments from command line
    for ARG in "$@"; do
        ARGUMENTS+=$'\n		<string>'$ARG'</string>'
    done

    # Create daemon plist file
    cat <<EOF > $TEMP_PLIST
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
	<key>Label</key>
	<string>$DAEMON</string>
	<key>ProgramArguments</key>
	<array>
		<string>$BINDIR/$APPNAME</string>$ARGUMENTS
	</array>
	<key>RunAtLoad</key>
	<true/>
	<key>KeepAlive</key>
	<false/>
</dict>
</plist>
EOF
else
    echo "Cancel, no valid arguments."
    exit 1
fi

# Install payload binaries
install -d -m 755 $BINDIR
install -o root -g wheel -m 755 "../payload/usr/local/bin/$APPNAME" $BINDIR
install -o root -g wheel -m 755 "../payload/usr/local/bin/${APPNAME}_uninstall" $BINDIR

source "postinstall"
