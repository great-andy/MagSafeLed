[![Release](https://img.shields.io/github/release/great-andy/MagSafeLed.svg)](../../releases/latest)
[![License](https://img.shields.io/github/license/great-andy/MagSafeLed.svg)](LICENSE)

# MagSafe LED Daemon (Apple Silicon)

A macOS tool to control the MagSafe LED on Apple Silicon Macs (M1, M2, M3, etc.).
It adjusts the MagSafe light based on screen activity and charging status.

macOS sets the MagSafe LED to green only when the battery reaches 100%. If a charging limit (e.g., 80%) is active, the LED remains orange indefinitely. This tool sets the LED to green as soon as the battery stops charging.

Additionally, if your MacBook screen is off or the lid is closed, the MagSafe LED often stays on. This tool can automatically turn off the LED in these situations to keep your room dark.

## Features
- **Apple Silicon Native:** Specifically designed for modern Mac hardware.
- **Sleep Mode:** Turns off the LED when the screen is off.
- **Smart Logic:** Disables the LED only when the battery is not charging and the screen is off.
- **Color Control:** Set a green LED when charging is complete or paused by macOS.

## Tech Stack
- **Languages:** C++ / Objective-C
- **Frameworks:** AppKit, IOKit, SMCManager (Internal)
- **Target:** Apple Silicon (ARM64)
- **Deployment:** macOS Installer Package (.pkg)

## Daemon Mode
This tool is designed to run as a **system daemon**. Once installed via the `.pkg` file, it will automatically load as a background process and start according to your chosen settings every time you boot your Mac.

### Daemon Management
You can easily manage the service without using the terminal:

1. Open **System Settings**.
2. Navigate to **General** > **Login Items & Extensions**.
3. Under **Allow in the Background**, you can find and toggle the **MagSafeLed** daemon on or off.

## Installation

### 1. Download (Recommended)
Download the latest `MagSafeLed_V1.0.pkg` from the **[Releases](../../releases)** section.

### 2. Fix Permissions (Important)
Since this tool uses IOKit for hardware access and is not notarized, macOS will block the execution by default.
To fix this, run the following command in your terminal after downloading:

```bash
xattr -c ~/Downloads/MagSafeLed_V1.0.pkg
```

### 3. Install
- Simply double-click the .pkg file in your Downloads folder

- or use the terminal
```bash
sudo installer -pkg ~/Downloads/MagSafeLed.pkg -target /
```
The installer will ask for your preferred settings. If you want to change them later, simply run the installer again; there is no need to uninstall the previous version.

_Note: Since the tool interacts with system hardware (SMC), root privileges are required._

## Uninstallation
To completely remove the tool and the background daemon, use the terminal:
```bash
sudo MagSafeLed_uninstall
```

## Usage (Optional)
Run `MagSafeLed` in the terminal with one of the following options:

| Option | Description |
| :--- | :--- |
| `-s, --sleep` | Set LED off if screens sleep |
| `-f, --full` | Set LED off if screens sleep and not charging |
| `-g, --green` | Set LED green if not charging |
| `-h, --help` | Print help message and exit |
| `-v, --version` | Print version and exit |
| *(no options)* | Stop daemon and exit |

The MagSafeLed tool can be used in Terminal or e.g. in an Xcode session for testing purposes.  
If MagSafeLed is started, any other instance of MagSafeLed running in the system will be terminated to ensure that only one instance is modifying the SMC.
MagSafeLed is running as a daemon without the KeepAlive option to enable this scenario.  
To re-run the daemon simply switch the service off and on in system settings, see [Daemon Management](#daemon-management).

## Troubleshooting / FAQ
**Q: Why does it need sudo?**  
A: Accessing the System Management Controller (SMC) via IOKit to control hardware LEDs requires root privileges.  

**Q: Does it work on Intel Macs?**  
A: No, this version is specifically built for the Apple Silicon SMC architecture.  

**Q: The LED doesn't change immediately?**  
A: The daemon reacts to system events. Try locking your screen or toggling the power cable to trigger an update.

## License
Licensed under the [GNU General Public License v3.0](LICENSE).

## Disclaimer
This tool interacts with the system hardware via IOKit. Use at your own risk. The authors are not responsible for any hardware issues.
