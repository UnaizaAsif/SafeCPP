# SafeCPP Makefile
# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -I.
DEBUG_FLAGS = -g -O0
RELEASE_FLAGS = -O2

# Directories
LEXER_DIR = lexer
PARSER_DIR = parser
ANALYZER_DIR = analyzer
UTILS_DIR = utils
BIN_DIR = bin
OBJ_DIR = obj

# Source files
LEXER_SOURCES = $(LEXER_DIR)/Lexer.cpp $(LEXER_DIR)/Token.cpp
PARSER_SOURCES = $(PARSER_DIR)/Parser.cpp
ANALYZER_SOURCES = $(ANALYZER_DIR)/SafetyAnalyzer.cpp $(ANALYZER_DIR)/DependencyGraph.cpp
UTILS_SOURCES = $(UTILS_DIR)/SymbolTable.cpp
MAIN_SOURCE = main.cpp

ALL_SOURCES = $(LEXER_SOURCES) $(PARSER_SOURCES) $(ANALYZER_SOURCES) $(UTILS_SOURCES) $(MAIN_SOURCE)

# Object files
OBJECTS = $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(notdir $(ALL_SOURCES)))

# Target executable
TARGET = $(BIN_DIR)/SafeCPP

# Default target
all: $(TARGET)

# Build executable
$(TARGET): $(OBJECTS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(RELEASE_FLAGS) -o $@ $^

# Debug build
debug: CXXFLAGS += $(DEBUG_FLAGS)
debug: clean $(TARGET)

# Compile lexer files
$(OBJ_DIR)/Lexer.o: $(LEXER_DIR)/Lexer.cpp $(LEXER_DIR)/Lexer.h | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/Token.o: $(LEXER_DIR)/Token.cpp $(LEXER_DIR)/Token.h | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile parser files
$(OBJ_DIR)/Parser.o: $(PARSER_DIR)/Parser.cpp $(PARSER_DIR)/Parser.h | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile analyzer files
$(OBJ_DIR)/SafetyAnalyzer.o: $(ANALYZER_DIR)/SafetyAnalyzer.cpp $(ANALYZER_DIR)/SafetyAnalyzer.h | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/DependencyGraph.o: $(ANALYZER_DIR)/DependencyGraph.cpp $(ANALYZER_DIR)/DependencyGraph.h | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile utils files
$(OBJ_DIR)/SymbolTable.o: $(UTILS_DIR)/SymbolTable.cpp $(UTILS_DIR)/SymbolTable.h | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile main
$(OBJ_DIR)/main.o: $(MAIN_SOURCE) | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Create directories
$(BIN_DIR):
	mkdir -p $@

$(OBJ_DIR):
	mkdir -p $@

# Clean build artifacts
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

# Run the program
run: $(TARGET)
	./$(TARGET) samples/demo1.cpp

# Run all demos
demo: $(TARGET)
	./$(TARGET) samples/demo1.cpp
	./$(TARGET) samples/demo2.cpp
	./$(TARGET) samples/full_demo.cpp

# Phony targets
.PHONY: all debug clean run demo

