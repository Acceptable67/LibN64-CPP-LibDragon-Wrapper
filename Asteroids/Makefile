BUILD_DIR=build
SOURCE_DIR=.
N64_INST=/usr/local
include $(N64_INST)/include/n64.mk

N64_CXXFLAGS += -Wno-narrowing 
all: CPPTesting.z64

$(BUILD_DIR)/CPPTesting.elf: \
	$(BUILD_DIR)/CPPTesting.o  \

CPPTesting.z64: N64_ROM_TITLE="CPPTesting Test"

$(BUILD_DIR)/CPPTesting.dfs: filesystem/data.txt

CPPTesting.z64: $(BUILD_DIR)/CPPTesting.dfs

clean:
	rm -rf $(BUILD_DIR) CPPTesting.z64

-include $(wildcard $(BUILD_DIR)/*.d)

.PHONY: all clean
