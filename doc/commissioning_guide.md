# DART Commissioning Guide

This guide details to process of setting up a DART system for use in a formula vehicle. This guide details specifically the software of the device, however testing and validation of important hardware is included in this guide.

## Flashing the System-on-Module

In order to test the majority of the functionality of a DART's PCB, a programmed SoM must be installed.

To program, or "flash", a SoM, see the below guide:

[som_flashing.md](som_flashing.md)

## Hardware Validation

Before installing a SoM, the SoM's 5V rail should be measured to validate it is the correct voltage.

After the SoM has been flashed and validated, it can be transferred to the DART's PCB.

**Note: It is very important the SoM's connectors are cleaned thoroughly of flux. Leaving flux in the connector will cause intermittent hardware issues.**

After the SoM has been installed, each of the DART's hardware components must be tested.

### HDMI

HDMI is rather simple to validate: If the display renders video, it is likely funtional. Depending on the state of the DART's other hardware, the dashboard-gui may or may not boot. If the dashboard-gui does not boot, that does not necessarily indicate HDMI is not working.

If the dashboard-gui does not start, the screen should prompt you for login credentials. If you can see this prompt, HDMI is likely okay.

If the display does not work, the PCB likely needs rework, either on one of the SoM's connectors or the HDMI connector itself. There is little room for software errors to cause HDMI issues.

### USB

Once HDMI is functional, the next thing to validate is USB. This can be done by connecting a USB keyboard to the DART's USB host connector.

- If the dashboard-gui is open, close it using `F4`.
- Login with the username `root` and password `zre`.
	- You may need to backspace any characters in the field after closing the dashboard-gui.

If you can login, USB is likely working as intended.

### Ethernet

To validate the ethernet interface of the DART:

- Connect a PC with ZRE-CAN-Tools installed to the DART's ethernet connector.
- After 10-30 seconds, the PC should recognize the connection.
- Use the `dart-cli` to connect to the DART.

If this connection is successful, ethernet is likely working as intended.

### CAN Bus

To validate the DART's CAN interfaces are working as expected:

- Use `ip link` to list all of the DART's network devices.
- Both `can0` and `can1` should appear in this list.
	- If either device is offline, attempt to bring it up using `ip link set up can* type can bitrate 1000000`

After both interfaces are online, connect them to an available CAN bus to validate they can receive CAN messages (the test bench for instance).

- `candump can*`

After both CAN interfaces are functional, the dashboard-gui should boot on the next boot.

### Touchscreen

With the dashboard-gui functioning, touchscreen can be validated by attempting to navigate the GUI's pages.

### Hold-up Circuitry (DO NOT SKIP)

The last hardware component to validate is the DART's hold-up circuitry. This is very important to validate, as data log corruption can occur otherwise.

The hold-up circuitry can be tested without assembling the entire fixture, however, the display power must be connected in order to load the input power rail. **If the display power is not connected, the test will fail.**

- Power cycle the DART:
	- Shutdown the OS (`shutdown now`).
	- Turn off the power supply for 30 seconds.
	- Turn the power supply back on.
- Validate the DART boots normally
	- The dashboard-gui should come online, do not close it.
	- Check the dashboard-gui reports a data logging session number (requires CAN bus to be connected).
- Physically unplug the power supply of the DART (the banana jack, not the 120V outlet).
	- This should be done quickly.
	- Turning off the supply is not enough, it must be unplugged.
- After ~5 seconds, plug the supply back in.
- Once the DART has booted, open the `dart-cli`:
	- Use the `b` option to print the system journal of the last boot.
	- Validate the end of the journal looks similar to below:

```

init-system[472]: [DART-OS INIT-SYSTEM] Shutdown interrupt received.
init-system[472]: [DART-OS INIT-SYSTEM] Terminating...
init-system[477]: Terminating...
dashboard-gui[487]: Lost connection to Wayland compositor.
init-system[477]: Closing MDF file...
init-system[472]: Application '/root/init_system/bin/exec-can-mdf-logger.sh' terminated in 3.573092 ms.
init-system[472]: Application '/root/init_system/bin/exec-dashboard-gui.sh' terminated in 117.406738 ms.
init-system[472]: [DART-OS INIT-SYSTEM] Executing application '/root/init_system/bin/post-exec.sh'...
systemd[1]: Halt requested from client PID 644 ('systemctl') (unit init_system.service)...
systemd[1]: Shutting down.
init-system[472]: [DART-OS INIT-SYSTEM] All processes terminated in 130.085602 ms.
systemd-shutdown[1]: Syncing filesystems and block devices.
systemd-shutdown[1]: Sending SIGTERM to remaining processes...
systemd-journald[230]: Received SIGTERM from PID 1 (systemd-shutdow).
systemd[624]: Activating special unit exit.target...
sshd[456]: Received signal 15; terminating.
systemd[624]: Stopped target default.target - Main User Target.
dnsmasq[458]: exiting on receipt of SIGTERM
systemd[624]: Stopped target basic.target - Basic System.
systemd[624]: Stopped target paths.target - Paths.
systemd[624]: Stopped target sockets.target - Sockets.
systemd[624]: Stopped target timers.target - Timers.
systemd[624]: Closed dbus.socket - D-Bus User Message Bus Socket.
systemd[624]: ssh-agent.socket: Failed to spawn executor: No such file or directory
systemd[624]: ssh-agent.socket: Failed to spawn 'stop-pre' task: No such file or directory
systemd[624]: ssh-agent.socket: Failed with result 'resources'.
systemd[624]: Closed ssh-agent.socket - OpenSSH Agent socket.
systemd[624]: Removed slice app.slice - User Application Slice.
systemd[624]: Reached target shutdown.target - Shutdown.
systemd[624]: Finished systemd-exit.service - Exit the Session.
systemd[624]: Reached target exit.target - Exit the Session.
systemd-journald[230]: Journal stopped

```

Importantly, the journal must contain the lines:

- `init-system[472]: [DART-OS INIT-SYSTEM] Shutdown interrupt received.`
- `init-system[472]: [DART-OS INIT-SYSTEM] All processes terminated in ### ms.`
- `systemd-shutdown[1]: Syncing filesystems and block devices.`
- `systemd-journald[230]: Journal stopped`

If the journal is missing any of these, it indicates the device was not gracefully powered off. Debugging with an oscilloscope is likely required to determine the cause.

## Device Configuration

After the device is working as intended, the device can be configured for the system it is to be installed in. To do this, change the `DART_CONFIG` environment variable to point to the intended configuration directory.

- Use `nano` to open the `/etc/environment` file.
	- This can be done with the `m` option in the `dart-cli`.
- Edit the line starting with `DART_CONFIG` to point to the intended config directory.

For example, a DART configured for the charging cart of ZR26 would be:
```
DART_CONFIG=/root/zre_cantools/config/zr26/charger
```

Depending on the device configuration, the CAN bus configuration may also need modified (ex, to only use 1 CAN bus, or to use 500 kilobaud instead of 1 megabaud).

```
DART_CAN0_BAUDRATE - The baudrate of the can0 CAN bus, in bit/s.
DART_CAN1_BAUDRATE - The baudrate of the can1 CAN bus, in bit/s (if enabled).
DART_CAN1_ENABLED - Disables / enables the can1 CAN bus.
```