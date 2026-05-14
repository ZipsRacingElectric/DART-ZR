#!/bin/sh

# This shell script starts the dashboard-gui application.

CAN0="can0@$DART_CAN0_BAUDRATE"

if [ "$DART_CAN1_ENABLED" = "1" ]; then
	CAN1="can1@$DART_CAN1_BAUDRATE"
fi

# As we are not in a desktop environment, no wayland compositor is running yet. To start a graphical application, the 'cage'
# application is used to first start the wayland compositor.

# Note: Do not forget the 'exec' command. This command replaces the current shell instance with the process to execute, meaning
# all received signals are forwarded to the new process.
exec cage -- $ZRE_CANTOOLS_DIR/bin/dashboard-gui --fullscreen --unbuffered $DART_CONFIG/dashboard_gui.json $CAN0 $CAN1
