# System-on-Module Operating System Image Flashing

**Note: If you are intending to flash a SoM for installing into a DART system, please start with from the below file:**

[commissioning_guide.md](commissioning_guide.md)

To flash an OS image to the CM5 system-on-module, the USB Type-C interface can be used.

This section will describe the process using the "DART Programmer" RPi and "DART Dev Board" hardware.

- Unplug the DART Dev Board's power supply, if not already.
- Set the "EMMC Boot" switch to "Disabled".
- Connect a USB cable from the USB 3.0 Type-A port of the DART Programmer to the USB Type-C port of the DART Dev Board.
- On the DART Programmer, run `sudo rpiboot`.
- On the DART Programmer, run `sudo rpi-imager`.
	- Select any RPi device (doesn't matter with a custom image)
	- Select "Use custom" and navigate to the DART's `.img` file.
	- Select the SoM as the storage device.
	- Confirm the writing.

Note if you are just testing an image, the validation step can be skipped pretty safely. If you are flashing a SoM for installation in a DART system, **do not** skip the validation.

After flashing the SoM, the device should be booted at least once while still on the DART Dev Board. In addition to validating the flashing was successful, this also allows the OS to perform its first boot setup without interruption.

To boot the SoM after flashing:
- Unplug the USB Type-C cable from the DART Dev Board.
- Set the "EMMC Boot" switch to "Enabled".
- Plug in the DART Dev Board's power supply.

During the first boot, the SoM will resize the root filesystem and reboot automatically. It is important this process is not interrupted. After this process is complete, the device will boot normally.