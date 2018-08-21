# COMPILER #
CXX := clang++
CXXFLAGS := -std=c++14 -Wall -O3 -g# -DDEBUG
LIBS := -lfl

# SOURCE FILE EXTENSION
SRC_EXT := cpp

# DIRECTORIES #
LEXER_DIR := src/lexer
PARSER_DIR := src/parser
SRC_DIR := src
INCLUDE_DIR := $(shell pwd)/src/
BUILD_DIR := build
BIN_DIR := bin
EXE := test

SOURCES := $(shell find $(SRC_DIR) -name "*.cpp" -exec basename {} \;) main.cpp
OBJECTS := $(patsubst %.$(SRC_EXT),$(BUILD_DIR)/%.o,$(SOURCES))

.PHONY: release

release: parse-lex dirs
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
	@$(RM) $(LEXER_DIR)/lexer.cpp
	@find $(PARSER_DIR)/. -type f -not -name "*.y" -delete
	@$(RM) *.plist

.PHONY: parse-lex

parse-lex: $(LEXER_DIR)/lexer.cpp $(PARSER_DIR)/parser.cpp
	@echo "Parser and Lexer files done successfully"

$(LEXER_DIR)/lexer.cpp: $(LEXER_DIR)/lexer.l
	flex -s -o $@ $^

$(PARSER_DIR)/parser.hpp $(PARSER_DIR)/parser.cpp: $(PARSER_DIR)/parser.y
	bison -dv -o $(PARSER_DIR)/parser.cpp $^

.PHONY: all

all: $(BIN_DIR)/$(EXE)
	@$(RM) $(EXE)
	@ln -s $(BIN_DIR)/$(EXE) $(EXE)

$(BIN_DIR)/$(EXE): $(OBJECTS)
	@echo "Linking : $@ with objects $(OBJECTS)"
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $@ $(LIBS)

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
