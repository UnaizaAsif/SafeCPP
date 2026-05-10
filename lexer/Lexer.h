#ifndef LEXER_H
#define LEXER_H

#include "Token.h"
#include <vector>
#include <string>
#include <map>

class Lexer {
private:
    std::string source;
    size_t position;
    int line;
    int column;

    char currentChar();
    char peekChar();
    void advance();
    void skipWhitespace();
    void skipComment();
    Token readNumber();
    Token readString();
    Token readIdentifier();
    
    // System 6: Statement end detection
    bool isStatementBoundary(const Token& token);
    bool canEndStatement(const Token& token);
    
    // System 5: Type inference from value token
    TokenType inferTypeFromToken(const Token& token);
    std::string getInferredTypeName(TokenType inferredType);

public:
    Lexer(const std::string& source);
    ~Lexer();

    Token nextToken();
    std::vector<Token> tokenize();
};

#endif // LEXER_H