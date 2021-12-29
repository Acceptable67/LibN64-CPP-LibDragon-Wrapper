BUILD_DIR=build
SOURCE_DIR=.
N64_INST=/usr/local
include $(N64_INST)/include/n64.mk

CXXFLAGS += 

all: CPPLibDragonN64.z64

$(BUILD_DIR)/CPPLibDragonN64.elf: \
	$(BUILD_DIR)/LibVector.o \
	$(BUILD_DIR)/CPPLibDragonN64.o \
	$(BUILD_DIR)/LibString.o \
	
CPPLibDragonN64.z64: N64_ROM_TITLE="C++ Framework"

$(BUILD_DIR)/CPPLibDragonN64.dfs: filesystem/data.txt

CPPLibDragonN64.z64: $(BUILD_DIR)/CPPLibDragonN64.dfs

clean:
	rm -rf $(BUILD_DIR) CPPLibDragonN64.z64

-include $(wildcard $(BUILD_DIR)/*.d)

.PHONY: all clean
