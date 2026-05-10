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
        if (source[position] == '\n') { line++; column = 1; }
        else column++;
        position++;
    }
}

void Lexer::skipWhitespace() {
    while (currentChar() != '\n' && std::isspace(currentChar())) advance();
}

void Lexer::skipComment() {
    if (currentChar() == '/' && peekChar() == '/') {
        while (currentChar() != '\n' && currentChar() != '\0') advance();
    } else if (currentChar() == '/' && peekChar() == '*') {
        advance(); advance();
        while (!(currentChar() == '*' && peekChar() == '/') && currentChar() != '\0') advance();
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
        num += currentChar(); advance();
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
                case '"':  str += '"';  break;
                default:   str += currentChar();
            }
            advance();
        } else { str += currentChar(); advance(); }
    }
    if (currentChar() == quote) advance();
    if (quote == '\'') {
        return Token(TokenType::CHAR_LITERAL, str, startLine, startColumn);
    }
    return Token(TokenType::STRING, str, startLine, startColumn);
}

Token Lexer::readIdentifier() {
    int startLine = line, startColumn = column;
    std::string id;
    while (std::isalnum(currentChar()) || currentChar() == '_') {
        id += currentChar(); advance();
    }
    static const std::map<std::string, TokenType> keywords = {
        {"int",TokenType::INT},{"float",TokenType::FLOAT_KW},{"double",TokenType::DOUBLE},
        {"char",TokenType::CHAR},{"void",TokenType::VOID},{"bool",TokenType::BOOL},
        {"if",TokenType::IF},{"else",TokenType::ELSE},{"while",TokenType::WHILE},
        {"for",TokenType::FOR},{"return",TokenType::RETURN},{"break",TokenType::BREAK},
        {"continue",TokenType::CONTINUE},{"switch",TokenType::SWITCH},{"case",TokenType::CASE},
        {"goto",TokenType::GOTO},{"default",TokenType::DEFAULT},
        {"class",TokenType::CLASS},{"struct",TokenType::STRUCT},{"public",TokenType::PUBLIC},
        {"private",TokenType::PRIVATE},{"protected",TokenType::PROTECTED},
        {"virtual",TokenType::VIRTUAL},{"this",TokenType::THIS},{"operator",TokenType::OPERATOR},
        {"static",TokenType::STATIC},{"const",TokenType::CONST},{"auto",TokenType::AUTO},
        {"mutable",TokenType::MUTABLE},{"explicit",TokenType::EXPLICIT},
        {"final",TokenType::FINAL},{"override",TokenType::OVERRIDE},
        {"short",TokenType::SHORT},{"long",TokenType::LONG},{"signed",TokenType::SIGNED},
        {"unsigned",TokenType::UNSIGNED},{"wchar_t",TokenType::WCHAR_T},
        {"char16_t",TokenType::CHAR16_T},{"char32_t",TokenType::CHAR32_T},
        {"char8_t",TokenType::CHAR8_T},
        {"template",TokenType::TEMPLATE},{"typename",TokenType::TYPENAME},
        {"using",TokenType::USING},{"namespace",TokenType::NAMESPACE},
        {"try",TokenType::TRY},{"catch",TokenType::CATCH},{"throw",TokenType::THROW},
        {"true",TokenType::TRUE_KW},{"false",TokenType::FALSE_KW},
        {"function",TokenType::FUNCTION},{"safe",TokenType::SAFE},{"let",TokenType::LET},
        {"nullable",TokenType::NULLABLE},{"nonnull",TokenType::NONNULL},
        {"new",TokenType::NEW},{"delete",TokenType::DELETE},{"nullptr",TokenType::NULLPTR},
        {"NULL",TokenType::NULL_KW},{"sizeof",TokenType::SIZEOF},
        {"cout",TokenType::COUT},{"cin",TokenType::CIN},
        {"include",TokenType::INCLUDE},{"define",TokenType::DEFINE},
        {"ifndef",TokenType::IFNDEF},{"endif",TokenType::ENDIF}
    };
    auto it = keywords.find(id);
    if (it != keywords.end()) return Token(it->second, id, startLine, startColumn);
    return Token(TokenType::IDENTIFIER, id, startLine, startColumn);
}

Token Lexer::nextToken() {
    skipWhitespace();
    while (currentChar() == '/' && (peekChar() == '/' || peekChar() == '*')) {
        skipComment(); skipWhitespace();
    }
    int startLine = line, startColumn = column;
    char ch = currentChar();
    if (ch == '\0') return Token(TokenType::END_OF_FILE, "", startLine, startColumn);
    if (ch == '\n') { advance(); return Token(TokenType::NEWLINE, "\\n", startLine, startColumn); }
    if (std::isdigit(ch)) return readNumber();
    if (ch == '"' || ch == '\'') return readString();
    if (std::isalpha(ch) || ch == '_') return readIdentifier();
    advance();
    if (ch == '=' && currentChar() == '=') { advance(); return Token(TokenType::EQUAL, "==", startLine, startColumn); }
    if (ch == '!' && currentChar() == '=') { advance(); return Token(TokenType::NOT_EQUAL, "!=", startLine, startColumn); }
    if (ch == '<' && currentChar() == '=') { advance(); return Token(TokenType::LESS_EQUAL, "<=", startLine, startColumn); }
    if (ch == '<' && currentChar() == '<') { advance(); return Token(TokenType::STREAM_OUT, "<<", startLine, startColumn); }
    if (ch == '>' && currentChar() == '=') { advance(); return Token(TokenType::GREATER_EQUAL, ">=", startLine, startColumn); }
    if (ch == '>' && currentChar() == '>') { advance(); return Token(TokenType::STREAM_IN, ">>", startLine, startColumn); }
    if (ch == '&' && currentChar() == '&') { advance(); return Token(TokenType::LOGICAL_AND, "&&", startLine, startColumn); }
    if (ch == '|' && currentChar() == '|') { advance(); return Token(TokenType::LOGICAL_OR, "||", startLine, startColumn); }
    if (ch == '-' && currentChar() == '>') { advance(); return Token(TokenType::ARROW, "->", startLine, startColumn); }
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
        default:  return Token(TokenType::UNKNOWN, std::string(1, ch), startLine, startColumn);
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
    
    // ===== SYSTEM 6: Insert STMT_END tokens for semicolon-free syntax =====
    std::vector<Token> processedTokens;
    
    for (size_t i = 0; i < tokens.size(); ++i) {
        Token& current = tokens[i];
        processedTokens.push_back(current);
        
        // Check if we need to insert STMT_END
        if (current.type != TokenType::NEWLINE && 
            current.type != TokenType::SEMICOLON && 
            current.type != TokenType::END_OF_FILE &&
            current.type != TokenType::STMT_END) {
            
            // Look ahead to next non-newline token
            Token* nextNonNewline = nullptr;
            int nextIdx = i + 1;
            while (nextIdx < tokens.size() && tokens[nextIdx].type == TokenType::NEWLINE) {
                nextIdx++;
            }
            
            if (nextIdx < tokens.size()) {
                nextNonNewline = &tokens[nextIdx];
            }
            
            // Rule 1: If followed by newline + start of new statement → insert STMT_END
            if (nextIdx < tokens.size() && 
                i + 1 < tokens.size() && 
                tokens[i + 1].type == TokenType::NEWLINE &&
                canEndStatement(current) &&
                isStatementBoundary(*nextNonNewline)) {
                
                // Skip the NEWLINE tokens
                while (i + 1 < tokens.size() && tokens[i + 1].type == TokenType::NEWLINE) {
                    processedTokens.push_back(tokens[++i]);
                }
                // Insert virtual STMT_END
                processedTokens.push_back(Token(TokenType::STMT_END, "", current.line, current.column));
            }
            // Rule 2: RIGHT_PAREN or RIGHT_BRACE followed by newline → insert STMT_END
            else if ((current.type == TokenType::RIGHT_PAREN || current.type == TokenType::RIGHT_BRACE) &&
                     i + 1 < tokens.size() && 
                     tokens[i + 1].type == TokenType::NEWLINE &&
                     nextIdx < tokens.size() &&
                     isStatementBoundary(*nextNonNewline)) {
                
                while (i + 1 < tokens.size() && tokens[i + 1].type == TokenType::NEWLINE) {
                    processedTokens.push_back(tokens[++i]);
                }
                processedTokens.push_back(Token(TokenType::STMT_END, "", current.line, current.column));
            }
        }
    }
    
    return processedTokens;
}

bool Lexer::canEndStatement(const Token& token) {
    // Tokens that can end a statement
    return token.type == TokenType::IDENTIFIER ||
           token.type == TokenType::INTEGER ||
           token.type == TokenType::FLOAT ||
           token.type == TokenType::CHAR_LITERAL ||
           token.type == TokenType::STRING ||
           token.type == TokenType::TRUE_KW ||
           token.type == TokenType::FALSE_KW ||
           token.type == TokenType::RIGHT_PAREN ||
           token.type == TokenType::RIGHT_BRACE ||
           token.type == TokenType::RIGHT_BRACKET ||
           token.type == TokenType::RETURN ||
           token.type == TokenType::BREAK ||
           token.type == TokenType::CONTINUE ||
           token.type == TokenType::DELETE;
}

bool Lexer::isStatementBoundary(const Token& token) {
    // Keywords that typically start new statements
    return token.type == TokenType::INT ||
           token.type == TokenType::FLOAT_KW ||
           token.type == TokenType::DOUBLE ||
           token.type == TokenType::CHAR ||
           token.type == TokenType::BOOL ||
           token.type == TokenType::VOID ||
           token.type == TokenType::IF ||
           token.type == TokenType::ELSE ||
           token.type == TokenType::WHILE ||
           token.type == TokenType::FOR ||
           token.type == TokenType::RETURN ||
           token.type == TokenType::BREAK ||
           token.type == TokenType::CONTINUE ||
           token.type == TokenType::LET ||
           token.type == TokenType::CONST ||
           token.type == TokenType::STATIC ||
           token.type == TokenType::CLASS ||
           token.type == TokenType::STRUCT ||
           token.type == TokenType::RIGHT_BRACE ||
           token.type == TokenType::END_OF_FILE;
}

// ===== SYSTEM 5: Type Inference from Literal Tokens =====
TokenType Lexer::inferTypeFromToken(const Token& token) {
    // Check token value for type suffixes (L, f, ULL, etc.)
    const std::string& val = token.value;
    
    if (token.type == TokenType::INTEGER) {
        // Check for integer suffixes
        if (!val.empty()) {
            // Check for long long (LL or ll)
            if (val.find("LL") != std::string::npos || val.find("ll") != std::string::npos) {
                if (val.find("U") != std::string::npos || val.find("u") != std::string::npos) {
                    return TokenType::TYPE_INFERRED_UNSIGNED_LONG;
                }
                return TokenType::TYPE_INFERRED_LONG;
            }
            // Check for unsigned (U or u)
            if (val.find("U") != std::string::npos || val.find("u") != std::string::npos) {
                return TokenType::TYPE_INFERRED_UNSIGNED_INT;
            }
            // Check for long (L or l, but not followed by another L)
            if ((val.find("L") != std::string::npos || val.find("l") != std::string::npos) &&
                val.find("LL") == std::string::npos && val.find("ll") == std::string::npos) {
                return TokenType::TYPE_INFERRED_LONG;
            }
        }
        return TokenType::TYPE_INFERRED_INT;
    } 
    else if (token.type == TokenType::FLOAT) {
        // Check for float suffixes
        if (!val.empty()) {
            // Check for float (f or F)
            if (val.find("f") != std::string::npos || val.find("F") != std::string::npos) {
                return TokenType::TYPE_INFERRED_FLOAT;
            }
            // Check for long double (L or l)
            if (val.find("L") != std::string::npos || val.find("l") != std::string::npos) {
                return TokenType::TYPE_INFERRED_DOUBLE;  // Treating long double as double for simplicity
            }
        }
        return TokenType::TYPE_INFERRED_DOUBLE;  // Default for floating point
    } 
    else if (token.type == TokenType::STRING) {
        return TokenType::TYPE_INFERRED_STRING;
    } 
    else if (token.type == TokenType::TRUE_KW || token.type == TokenType::FALSE_KW) {
        return TokenType::TYPE_INFERRED_BOOL;
    }
    else if (token.type == TokenType::CHAR_LITERAL) {
        return TokenType::TYPE_INFERRED_CHAR;
    }
    
    return TokenType::UNKNOWN;
}

std::string Lexer::getInferredTypeName(TokenType inferredType) {
    switch (inferredType) {
        case TokenType::TYPE_INFERRED_INT:              return "int";
        case TokenType::TYPE_INFERRED_LONG:             return "long";
        case TokenType::TYPE_INFERRED_UNSIGNED_INT:     return "unsigned int";
        case TokenType::TYPE_INFERRED_UNSIGNED_LONG:    return "unsigned long long";
        case TokenType::TYPE_INFERRED_FLOAT:            return "float";
        case TokenType::TYPE_INFERRED_DOUBLE:           return "double";
        case TokenType::TYPE_INFERRED_CHAR:             return "char";
        case TokenType::TYPE_INFERRED_BOOL:             return "bool";
        case TokenType::TYPE_INFERRED_STRING:           return "string";
        case TokenType::TYPE_INFERRED_POINTER:          return "pointer";
        case TokenType::TYPE_INFERRED_ARRAY:            return "array";
        default:                                        return "unknown";
    }
}