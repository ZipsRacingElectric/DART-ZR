#!/bin/sh

# This shell script starts the can-mdf-logger application

CAN0="can0@$DART_CAN0_BAUDRATE"

if [ "$DART_CAN1_ENABLED" = "1" ]; then
	CAN1="can1@$DART_CAN1_BAUDRATE"
fi

# Note: Do not forget the 'exec' command. This command replaces the current shell instance with the process to execute, meaning
# all received signals are forwarded to the new process.
exec $ZRE_CANTOOLS_DIR/bin/can-mdf-logger --quiet --unbuffered $ZRE_CANTOOLS_LOGGING_DIR $DART_CONFIG/can_mdf_logger.json $CAN0 $CAN1
