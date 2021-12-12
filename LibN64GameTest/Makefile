BUILD_DIR=build
SOURCE_DIR=.
N64_INST=/usr/local
include $(N64_INST)/include/n64.mk

N64_CXXFLAGS += -Wno-narrowing 
all: LibN64GameTest.z64

$(BUILD_DIR)/LibN64GameTest.elf: \
	$(BUILD_DIR)/LibN64GameTest.o  \

LibN64GameTest.z64: N64_ROM_TITLE="LibN64GameTest Test"

$(BUILD_DIR)/LibN64GameTest.dfs: filesystem/data.txt

LibN64GameTest.z64: $(BUILD_DIR)/LibN64GameTest.dfs

clean:
	rm -rf $(BUILD_DIR) LibN64GameTest.z64

-include $(wildcard $(BUILD_DIR)/*.d)

.PHONY: all clean
