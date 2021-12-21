BUILD_DIR=build
SOURCE_DIR=.
N64_INST=/usr/local
include $(N64_INST)/include/n64.mk

N64_CXXFLAGS += -Wno-narrowing 
all: CustomLangParser.z64

$(BUILD_DIR)/CustomLangParser.elf: \
	$(BUILD_DIR)/CustomLangParser.o  \
	$(BUILD_DIR)/LangParser.o

CustomLangParser.z64: N64_ROM_TITLE="CustomLangParser Test"

$(BUILD_DIR)/CustomLangParser.dfs: filesystem/data.txt

CustomLangParser.z64: $(BUILD_DIR)/CustomLangParser.dfs

clean:
	rm -rf $(BUILD_DIR) CustomLangParser.z64

-include $(wildcard $(BUILD_DIR)/*.d)

.PHONY: all clean
