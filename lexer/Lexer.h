#ifndef LEXER_H
#define LEXER_H

#include "Token.h"
#include <vector>
#include <string>
#include <map>

/**
 * Lexical Analyzer (Lexer)
 * Converts source code into tokens
 * 
 * This is a pure lexical analyzer - it ONLY tokenizes.
 * No semantic analysis is done here.
 */
class Lexer {
private:
    std::string source;
    size_t position;
    int line;
    int column;

    // Character operations
    char currentChar();
    char peekChar();
    void advance();

    // Lexing utilities
    void skipWhitespace();
    void skipComment();

    // Token readers
    Token readNumber();
    Token readString();
    Token readIdentifier();

public:
    Lexer(const std::string& source);
    ~Lexer();

    Token nextToken();
    std::vector<Token> tokenize();
};

#endif // LEXER_H
