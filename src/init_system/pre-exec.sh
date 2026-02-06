#!/bin/sh

# This shell script is run when the init-system is first started. This runs before any of the applications have been started.

# Set up the can0 interface at a baudrate of 1 Mbit
sudo ip link set up can0 type can bitrate 1000000