SRC := zre-can1.dts
BIN := zre-can1.dtbo

$(BIN): $(SRC)
	dtc $(SRC) -o $(BIN)
	cp $(BIN) /boot/firmware/overlays