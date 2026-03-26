# Dashboard and Racing Telemetry (DART) - Zips Racing

Firmware and operating system creation tool for Zips Racing's DART data acquisition system.

## Project Information

This project can only be installed on a Raspberry Pi running Raspberry Pi OS, similar hardware, or a virtual machine. This is due to the fact that a 64-bit ARM compiler and Debian ARM packages are required to generate the OS image. For accessing pre-built images, see the [Releases](https://github.com/ZipsRacingElectric/ZRE-DARTOS/releases) section of this repository.

The remainder of this documentation will assume a Raspberry Pi is being used. The "DART Programmer" RPi is configured exactly for this.

## Setup

### RPI-Image-Generator

RPI-Image-Generator (https://github.com/raspberrypi/rpi-image-gen/) is the main tool used in creating OS images for the DART.

To install RPI-Image-Generator:

```
git clone https://github.com/raspberrypi/rpi-image-gen.git
cd rpi-image-gen
sudo ./install_deps.sh
```

To add RPI-Image-Generator to your system path, add the below to your `~/.bashrc` file (replacing the path appropriately):

```
export PATH=$PATH:/path/to/rpi-image-gen/
```

### RPIBoot

RPIBoot is the first tool used to flash the OS image onto the CM5 system-on-module.

To install RPIBoot:

```
sudo apt install git libusb-1.0-0-dev pkg-config build-essential
git clone --recurse-submodules --shallow-submodules --depth=1 https://github.com/raspberrypi/usbboot
cd usbboot
make
sudo make install
```

RPIBoot is installed by the last command, so no need to add to the system path.

### RPI-Imager

RPI-Imager is the second tool used to flash the OS image on the CM5 system-on-module.

To install RPI-Imager

```
sudo apt install rpi-imager
```

### Project Setup

- Clone this repo
	- Via SSH: `git clone https://github.com/ZipsRacingElectric/DART-ZR.git`
	- Via HTTPS: `git clone git@github.com:ZipsRacingElectric/DART-ZR.git`

- Initialize/update submodules, run:
	- `git submodule init`
	- `git submodule update`

## Usage

### Generating an OS image

To generate an OS image, in a terminal:
- Change directories to this project's root.
- Run `rpi-image-gen build -S . -c dart.yaml`

Note this process normally takes ~10 minutes.

Once complete, the OS image can be found at `work/image-dart_os/dart_os.img`.

### Commissioning a DART System

To commission a new DART system, see:

[doc/commissioning_guide.md](doc/commissioning_guide.md)

### Updating Firmware

To update the firmware installed on an existing DART, the device does not need disassembled. Rather, see:

[doc/updating_firmware.md](doc/updating_firmware.md)

## Technical Documentation

For technical documentation, see:

[doc/index.md](doc/index.md)