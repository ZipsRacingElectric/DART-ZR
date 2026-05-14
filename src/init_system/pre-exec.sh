#!/bin/sh

# Set up the can0 interface at the specified baudrate
ip link set up can0 type can bitrate $DART_CAN0_BAUDRATE

# If enabled, set up the can1 interface at the specified baudrate
if [ "$DART_CAN1_ENABLED" = "1" ]; then
	ip link set up can1 type can bitrate $DART_CAN1_BAUDRATE
fi