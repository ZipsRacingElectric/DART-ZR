#!/bin/sh

# TODO(Barach): Remove
ip link add dev vcan type vcan
ip link set up vcan

# Set up the can0 interface at a baudrate of 1 Mbit
ip link set up can0 type can bitrate 1000000