BUILD_DIR=build
SOURCE_DIR=.
N64_INST=/usr/local
include $(N64_INST)/include/n64.mk

N64_CXXFLAGS += -Wno-narrowing 
all: MemPakTesting.z64

$(BUILD_DIR)/MemPakTesting.elf: \
	$(BUILD_DIR)/MemPakTesting.o  \

MemPakTesting.z64: N64_ROM_TITLE="MemPakTesting Test"

$(BUILD_DIR)/MemPakTesting.dfs: filesystem/data.txt

MemPakTesting.z64: $(BUILD_DIR)/MemPakTesting.dfs

clean:
	rm -rf $(BUILD_DIR) MemPakTesting.z64

-include $(wildcard $(BUILD_DIR)/*.d)

.PHONY: all clean
