#!/bin/sh

# Note: Do not forget the 'exec' command. This command replaces the current shell instance with the process to execute, meaning
# all received signals are forwarded to the new process.
exec $ZRE_CANTOOLS_DIR/bin/can-mdf-logger --quiet --unbuffered $ZRE_CANTOOLS_LOGGING_DIR $DART_CONFIG/can_mdf_logger.json can0@1000000 can1@1000000
