BIN_DIR		:= ./bin
SRC_DIR		:= .

C_BIN		:= $(BIN_DIR)/init-system
C_SRC		:= $(wildcard $(SRC_DIR)/*.c)
SH_SRC		:= $(wildcard $(SRC_DIR)/*.sh)
SH_BIN		:= $(patsubst $(SRC_DIR)/%, $(BIN_DIR)/%, $(SH_SRC))

all: $(C_BIN) $(SH_BIN)

# The major version (ex 1.x.x => 1, 2.x.x => 2) of the GPIOD library. I cannot
# find a better way of doing this, and unfortunately the implementation depends
# on this.
LIBGPIOD_MAJOR_VERSION = $(shell gpioinfo --version | grep -oP 'v\K\w+')

$(BIN_DIR)/%.sh: $(SRC_DIR)/%.sh
	mkdir -p $(BIN_DIR)
	cp $< $@
	chmod a+x $@

$(C_BIN): $(C_SRC)
	mkdir -p $(BIN_DIR)
	gcc -DLIBGPIOD_MAJOR_VERSION=$(LIBGPIOD_MAJOR_VERSION) $^ -o $@ -lgpiod

clean:
	rm -rf $(BIN_DIR)