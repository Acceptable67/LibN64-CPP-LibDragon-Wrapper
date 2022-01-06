BUILD_DIR=build
SOURCE_DIR=.
N64_INST=/usr/local
include $(N64_INST)/include/n64.mk

N64_CXXFLAGS += -Wno-narrowing 
all: MenuTesting.z64

$(BUILD_DIR)/MenuTesting.elf: \
	$(BUILD_DIR)/MenuTesting.o  \

MenuTesting.z64: N64_ROM_TITLE="MenuTesting Test"

$(BUILD_DIR)/MenuTesting.dfs: filesystem/data.txt

MenuTesting.z64: $(BUILD_DIR)/MenuTesting.dfs

clean:
	rm -rf $(BUILD_DIR) MenuTesting.z64

-include $(wildcard $(BUILD_DIR)/*.d)

.PHONY: all clean
