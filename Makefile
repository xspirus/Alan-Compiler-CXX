# COMPILER #
CXX := clang++
CXXFLAGS := -std=c++14 -Wall -O3 -g

# SOURCE FILE EXTENSION
SRC_EXT := cpp

# DIRECTORIES #
SRC_DIR := src
INCLUDE_DIR := $(shell pwd)/include/
BUILD_DIR := build
BIN_DIR := bin
EXE := test

SOURCES := $(shell find $(SRC_DIR) -name "*.cpp" -exec basename {} \;) main.cpp
OBJECTS := $(patsubst %.$(SRC_EXT),$(BUILD_DIR)/%.o,$(SOURCES))

.PHONY: release

release: dirs
	@$(MAKE) all

.PHONY: dirs

dirs:
	@echo "Creating directories"
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(BIN_DIR)

.PHONY: clean

clean:
	@echo "Deleting $(EXE) symlink"
	@$(RM) $(EXE)
	@echo "Deleting directories"
	@$(RM) -r $(BUILD_DIR)
	@$(RM) -r $(BIN_DIR)

.PHONY: all

all: $(BIN_DIR)/$(EXE)
	@$(RM) $(EXE)
	@ln -s $(BIN_DIR)/$(EXE) $(EXE)

$(BIN_DIR)/$(EXE): $(OBJECTS)
	@echo "Linking : $@ with objects $(OBJECTS)"
	$(CXX) $(OBJECTS) -o $@

$(BUILD_DIR)/%.o: %.$(SRC_EXT)
	@echo "Compiling : $< --> $@"
	$(CXX) $(CXXFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/**/%.$(SRC_EXT)
	@echo "Compiling : $< --> $@"
	$(CXX) $(CXXFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

.PHONY: print

print:
	$(info $(SOURCES))
	$(info $(OBJECTS))
