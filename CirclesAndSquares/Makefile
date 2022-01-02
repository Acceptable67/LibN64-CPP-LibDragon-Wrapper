BUILD_DIR=build
SOURCE_DIR=.
N64_INST=/usr/local
include $(N64_INST)/include/n64.mk

N64_CXXFLAGS += -Wno-narrowing 
all: CirclesAndSquares.z64

$(BUILD_DIR)/CirclesAndSquares.elf: \
	$(BUILD_DIR)/CirclesAndSquares.o  \

CirclesAndSquares.z64: N64_ROM_TITLE="CirclesAndSquares Test"

$(BUILD_DIR)/CirclesAndSquares.dfs: filesystem/data.txt

CirclesAndSquares.z64: $(BUILD_DIR)/CirclesAndSquares.dfs

clean:
	rm -rf $(BUILD_DIR) CirclesAndSquares.z64

-include $(wildcard $(BUILD_DIR)/*.d)

.PHONY: all clean
