BUILD_DIR=build
SOURCE_DIR=.
N64_INST=/usr/local
include $(N64_INST)/include/n64.mk

N64_CXXFLAGS += -Wno-narrowing 
all: 3DViewer.z64

$(BUILD_DIR)/3DViewer.elf: \
	$(BUILD_DIR)/3DViewer.o  \

3DViewer.z64: N64_ROM_TITLE="3DViewer Test"

$(BUILD_DIR)/3DViewer.dfs: filesystem/data.txt

3DViewer.z64: $(BUILD_DIR)/3DViewer.dfs

clean:
	rm -rf $(BUILD_DIR) 3DViewer.z64

-include $(wildcard $(BUILD_DIR)/*.d)

.PHONY: all clean
