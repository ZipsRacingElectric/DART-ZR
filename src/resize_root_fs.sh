#! /bin/bash

# Referenced from: https://github.com/jonnymacs/rpi-auto-resize-root

if [ ! -f /root/resize_root_fs_done ]; then
	raspi-config --expand-rootfs
	touch /root/resize_root_fs_done
	sync
	reboot now
fi