BUILD_DIR=build
SOURCE_DIR=.
N64_INST=/usr/local
include $(N64_INST)/include/n64.mk

N64_CXXFLAGS += -Wno-narrowing 
all: PanningAndZooming.z64

$(BUILD_DIR)/PanningAndZooming.elf: \
	$(BUILD_DIR)/PanningAndZooming.o  \

PanningAndZooming.z64: N64_ROM_TITLE="PanningAndZooming Test"

$(BUILD_DIR)/PanningAndZooming.dfs: filesystem/data.txt

PanningAndZooming.z64: $(BUILD_DIR)/PanningAndZooming.dfs

clean:
	rm -rf $(BUILD_DIR) PanningAndZooming.z64

-include $(wildcard $(BUILD_DIR)/*.d)

.PHONY: all clean
