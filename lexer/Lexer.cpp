#include "Lexer.h"
#include <cctype>
#include <algorithm>

Lexer::Lexer(const std::string& source)
    : source(source), position(0), line(1), column(1) {}

Lexer::~Lexer() {}

char Lexer::currentChar() {
    if (position >= source.length()) return '\0';
    return source[position];
}

char Lexer::peekChar() {
    if (position + 1 >= source.length()) return '\0';
    return source[position + 1];
}

void Lexer::advance() {
    if (position < source.length()) {
        if (source[position] == '\n') {
            line++;
            column = 1;
        } else {
            column++;
        }
        position++;
    }
}

void Lexer::skipWhitespace() {
    while (currentChar() != '\n' && std::isspace(currentChar())) {
        advance();
    }
}

void Lexer::skipComment() {
    if (currentChar() == '/' && peekChar() == '/') {
        while (currentChar() != '\n' && currentChar() != '\0') {
            advance();
        }
    } else if (currentChar() == '/' && peekChar() == '*') {
        advance(); advance();
        while (!(currentChar() == '*' && peekChar() == '/') && currentChar() != '\0') {
            advance();
        }
        if (currentChar() == '*') advance();
        if (currentChar() == '/') advance();
    }
}

Token Lexer::readNumber() {
    int startLine = line, startColumn = column;
    std::string num;
    bool isFloat = false;
    
    while (std::isdigit(currentChar()) || (currentChar() == '.' && !isFloat)) {
        if (currentChar() == '.') isFloat = true;
        num += currentChar();
        advance();
    }
    
    return Token(isFloat ? TokenType::FLOAT : TokenType::INTEGER, num, startLine, startColumn);
}

Token Lexer::readString() {
    int startLine = line, startColumn = column;
    char quote = currentChar();
    std::string str;
    advance();
    
    while (currentChar() != quote && currentChar() != '\0') {
        if (currentChar() == '\\') {
            advance();
            switch (currentChar()) {
                case 'n': str += '\n'; break;
                case 't': str += '\t'; break;
                case '\\': str += '\\'; break;
                case '"': str += '"'; break;
                default: str += currentChar();
            }
            advance();
        } else {
            str += currentChar();
            advance();
        }
    }
    
    if (currentChar() == quote) advance();
    return Token(TokenType::STRING, str, startLine, startColumn);
}

Token Lexer::readIdentifier() {
    int startLine = line, startColumn = column;
    std::string id;
    
    while (std::isalnum(currentChar()) || currentChar() == '_') {
        id += currentChar();
        advance();
    }
    
    // Keyword mapping
    static const std::map<std::string, TokenType> keywords = {
        // Data types
        {"int", TokenType::INT},
        {"float", TokenType::FLOAT_KW},
        {"double", TokenType::DOUBLE},
        {"char", TokenType::CHAR},
        {"void", TokenType::VOID},
        {"bool", TokenType::BOOL},
        
        // Control flow
        {"if", TokenType::IF},
        {"else", TokenType::ELSE},
        {"while", TokenType::WHILE},
        {"for", TokenType::FOR},
        {"return", TokenType::RETURN},
        {"break", TokenType::BREAK},
        {"continue", TokenType::CONTINUE},
        {"switch", TokenType::SWITCH},
        {"case", TokenType::CASE},
        {"goto", TokenType::GOTO},
        {"default", TokenType::DEFAULT},

        // Class/struct keywords
        {"class", TokenType::CLASS},
        {"struct", TokenType::STRUCT},
        {"public", TokenType::PUBLIC},
        {"private", TokenType::PRIVATE},
        {"protected", TokenType::PROTECTED},
        {"virtual", TokenType::VIRTUAL},
        {"this", TokenType::THIS},
        {"operator", TokenType::OPERATOR},
        
        // Qualifiers
        {"static", TokenType::STATIC},
        {"const", TokenType::CONST},
        {"auto", TokenType::AUTO},
        {"mutable", TokenType::MUTABLE},
        {"explicit", TokenType::EXPLICIT},
        {"final", TokenType::FINAL},
        {"override", TokenType::OVERRIDE},
        
        // Extended type keywords
        {"short", TokenType::SHORT},
        {"long", TokenType::LONG},
        {"signed", TokenType::SIGNED},
        {"unsigned", TokenType::UNSIGNED},
        {"wchar_t", TokenType::WCHAR_T},
        {"char16_t", TokenType::CHAR16_T},
        {"char32_t", TokenType::CHAR32_T},
        {"char8_t", TokenType::CHAR8_T},
        
        // Template/namespace
        {"template", TokenType::TEMPLATE},
        {"typename", TokenType::TYPENAME},
        {"using", TokenType::USING},
        {"namespace", TokenType::NAMESPACE},
        
        // Exceptions
        {"try", TokenType::TRY},
        {"catch", TokenType::CATCH},
        {"throw", TokenType::THROW},
        
        // Boolean literals
        {"true", TokenType::TRUE_KW},
        {"false", TokenType::FALSE_KW},
        
        // Function keyword
        {"function", TokenType::FUNCTION},
        
        // Safety keywords (Extended C++)
        {"safe", TokenType::SAFE},
        {"let", TokenType::LET},
        {"nullable", TokenType::NULLABLE},
        {"nonnull", TokenType::NONNULL},
        
        // Memory/type keywords
        {"new", TokenType::NEW},
        {"delete", TokenType::DELETE},
        {"nullptr", TokenType::NULLPTR},
        {"NULL", TokenType::NULL_KW},
        {"sizeof", TokenType::SIZEOF},
        
        // I/O stream keywords
        {"cout", TokenType::COUT},
        {"cin", TokenType::CIN},
        
        // Preprocessor
        {"include", TokenType::INCLUDE},
        {"define", TokenType::DEFINE},
        {"ifndef", TokenType::IFNDEF},
        {"endif", TokenType::ENDIF}
    };
    
    auto it = keywords.find(id);
    if (it != keywords.end()) {
        return Token(it->second, id, startLine, startColumn);
    }
    
    return Token(TokenType::IDENTIFIER, id, startLine, startColumn);
}

Token Lexer::nextToken() {
    skipWhitespace();
    
    while (currentChar() == '/' && (peekChar() == '/' || peekChar() == '*')) {
        skipComment();
        skipWhitespace();
    }
    
    int startLine = line, startColumn = column;
    char ch = currentChar();
    
    if (ch == '\0') {
        return Token(TokenType::END_OF_FILE, "", startLine, startColumn);
    }
    
    if (ch == '\n') {
        advance();
        return Token(TokenType::NEWLINE, "\\n", startLine, startColumn);
    }
    
    if (std::isdigit(ch)) {
        return readNumber();
    }
    
    if (ch == '"' || ch == '\'') {
        return readString();
    }
    
    if (std::isalpha(ch) || ch == '_') {
        return readIdentifier();
    }
    
    advance();
    
    // Two-character operators
    if (ch == '=' && currentChar() == '=') {
        advance();
        return Token(TokenType::EQUAL, "==", startLine, startColumn);
    }
    if (ch == '!' && currentChar() == '=') {
        advance();
        return Token(TokenType::NOT_EQUAL, "!=", startLine, startColumn);
    }
    if (ch == '<' && currentChar() == '=') {
        advance();
        return Token(TokenType::LESS_EQUAL, "<=", startLine, startColumn);
    }
    if (ch == '>' && currentChar() == '=') {
        advance();
        return Token(TokenType::GREATER_EQUAL, ">=", startLine, startColumn);
    }
    if (ch == '&' && currentChar() == '&') {
        advance();
        return Token(TokenType::LOGICAL_AND, "&&", startLine, startColumn);
    }
    if (ch == '|' && currentChar() == '|') {
        advance();
        return Token(TokenType::LOGICAL_OR, "||", startLine, startColumn);
    }
    if (ch == '-' && currentChar() == '>') {
        advance();
        return Token(TokenType::ARROW, "->", startLine, startColumn);
    }
    
    // Single character tokens
    switch (ch) {
        case '+': return Token(TokenType::PLUS, "+", startLine, startColumn);
        case '-': return Token(TokenType::MINUS, "-", startLine, startColumn);
        case '*': return Token(TokenType::MULTIPLY, "*", startLine, startColumn);
        case '/': return Token(TokenType::DIVIDE, "/", startLine, startColumn);
        case '%': return Token(TokenType::MODULO, "%", startLine, startColumn);
        case '(': return Token(TokenType::LEFT_PAREN, "(", startLine, startColumn);
        case ')': return Token(TokenType::RIGHT_PAREN, ")", startLine, startColumn);
        case '{': return Token(TokenType::LEFT_BRACE, "{", startLine, startColumn);
        case '}': return Token(TokenType::RIGHT_BRACE, "}", startLine, startColumn);
        case '[': return Token(TokenType::LEFT_BRACKET, "[", startLine, startColumn);
        case ']': return Token(TokenType::RIGHT_BRACKET, "]", startLine, startColumn);
        case ';': return Token(TokenType::SEMICOLON, ";", startLine, startColumn);
        case ',': return Token(TokenType::COMMA, ",", startLine, startColumn);
        case '.': return Token(TokenType::DOT, ".", startLine, startColumn);
        case '=': return Token(TokenType::ASSIGN, "=", startLine, startColumn);
        case '!': return Token(TokenType::LOGICAL_NOT, "!", startLine, startColumn);
        case '<': return Token(TokenType::LESS_THAN, "<", startLine, startColumn);
        case '>': return Token(TokenType::GREATER_THAN, ">", startLine, startColumn);
        case '&': return Token(TokenType::AMPERSAND, "&", startLine, startColumn);
        case ':': return Token(TokenType::COLON, ":", startLine, startColumn);
        default: return Token(TokenType::UNKNOWN, std::string(1, ch), startLine, startColumn);
    }
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    Token token = nextToken();
    
    while (token.type != TokenType::END_OF_FILE) {
        tokens.push_back(token);
        token = nextToken();
    }
    
    tokens.push_back(token);
    return tokens;
}
