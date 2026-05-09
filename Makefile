CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -I.

SRCS := main.cpp \
        lexer/Token.cpp \
        lexer/Lexer.cpp \
        parser/Parser.cpp \
        utils/SymbolTable.cpp \
        semantic/SemanticAnalyzer.cpp

OBJS := $(SRCS:.cpp=.o)
TARGET := compiler

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

demo: $(TARGET)
	./$(TARGET) --demo

.PHONY: all clean demo