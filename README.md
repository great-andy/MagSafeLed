[![License](https://badgen.net)](LICENSE) [![Release](https://badgen.net)](../../releases/latest) ![macOS](https://badgen.net) ![Apple Silicon](https://badgen.net)

# MagSafe LED Daemon (Apple Silicon)

A macOS tool to control the MagSafe LED on Apple Silicon Macs (M1, M2, M3, etc.).
It adjusts the MagSafe light based on screen activity and charging status.

## Features
- **Apple Silicon Native:** Specifically designed for modern Mac hardware.
- **Sleep Mode:** Turns off the LED when the screen is off.
- **Smart Logic:** Only disables the LED when not charging and sleeping.
- **Color Control:** Option to set a green LED when not charging.

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

### 2. Fix Permission (Important)
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
_Note: Since the tool interacts with system hardware (SMC), root privileges are required._

## Uninstallation
To completely remove the tool and the background daemon, use the terminal:
```bash
sudo MagSafeLed_uninstall
```

## Usage (Optional)
Run `MagSafeLed` with one of the following options:

| Option | Description |
| :--- | :--- |
| `-s, --sleep` | Set LED off if screens sleep |
| `-f, --full` | Set LED off if screens sleep and not charging |
| `-g, --green` | Set LED green if not charging |
| `-h, --help` | Print help message and exit |
| `-v, --version` | Print version and exit |
| *(no options)* | Stop daemon and exit |

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
