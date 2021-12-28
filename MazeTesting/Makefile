BUILD_DIR=build
SOURCE_DIR=.
N64_INST=/usr/local
include $(N64_INST)/include/n64.mk

N64_CXXFLAGS += -Wno-narrowing 
all: MazeTesting.z64

$(BUILD_DIR)/MazeTesting.elf: \
	$(BUILD_DIR)/MazeTesting.o  \

MazeTesting.z64: N64_ROM_TITLE="MazeTesting Test"

$(BUILD_DIR)/MazeTesting.dfs: filesystem/data.txt

MazeTesting.z64: $(BUILD_DIR)/MazeTesting.dfs

clean:
	rm -rf $(BUILD_DIR) MazeTesting.z64

-include $(wildcard $(BUILD_DIR)/*.d)

.PHONY: all clean
