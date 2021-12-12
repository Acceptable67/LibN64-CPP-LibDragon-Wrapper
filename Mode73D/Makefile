BUILD_DIR=build
SOURCE_DIR=.
N64_INST=/usr/local
include $(N64_INST)/include/n64.mk

N64_CXXFLAGS += -Wno-narrowing 
all: Mode73D.z64

$(BUILD_DIR)/Mode73D.elf: \
	$(BUILD_DIR)/Mode73D.o  \

Mode73D.z64: N64_ROM_TITLE="Mode73D Test"

$(BUILD_DIR)/Mode73D.dfs: filesystem/data.txt

Mode73D.z64: $(BUILD_DIR)/Mode73D.dfs

clean:
	rm -rf $(BUILD_DIR) Mode73D.z64

-include $(wildcard $(BUILD_DIR)/*.d)

.PHONY: all clean
