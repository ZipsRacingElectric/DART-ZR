#!/bin/sh

# Set up the can0 interface at a baudrate of 1 Mbit
ip link set up can0 type can bitrate 1000000

# Set up the can1 interface at a baudrate of 1 Mbit
ip link set up can1 type can bitrate 1000000