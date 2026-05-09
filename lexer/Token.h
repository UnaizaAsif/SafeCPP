#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <iostream>

enum class TokenType {
    // ========== LITERALS ==========
    INTEGER,
    FLOAT,
    STRING,
    IDENTIFIER,

    // ========== STANDARD KEYWORDS ==========
    INT,
    FLOAT_KW,
    VOID,
    CHAR,
    DOUBLE,
    BOOL,
    
    // ========== CONTROL FLOW ==========
    IF,
    ELSE,
    WHILE,
    FOR,
    RETURN,
    BREAK,
    CONTINUE,
    SWITCH,
    CASE,
    GOTO,
    DEFAULT,

    // ========== CLASS/STRUCT KEYWORDS ==========
    CLASS,
    STRUCT,
    PUBLIC,
    PRIVATE,
    PROTECTED,
    VIRTUAL,
    THIS,
    OPERATOR,

    // ========== QUALIFIER KEYWORDS ==========
    STATIC,
    CONST,
    AUTO,
    MUTABLE,
    EXPLICIT,
    FINAL,
    OVERRIDE,

    // ========== EXTENDED TYPE KEYWORDS ==========
    SHORT,
    LONG,
    SIGNED,
    UNSIGNED,
    WCHAR_T,
    CHAR16_T,
    CHAR32_T,
    CHAR8_T,

    // ========== TEMPLATE/NAMESPACE KEYWORDS ==========
    TEMPLATE,
    TYPENAME,
    USING,
    NAMESPACE,

    // ========== EXCEPTION KEYWORDS ==========
    TRY,
    CATCH,
    THROW,

    // ========== BOOLEAN LITERALS ==========
    TRUE_KW,
    FALSE_KW,

    // ========== FUNCTION KEYWORD ==========
    FUNCTION,

    // ========== SAFETY KEYWORDS (Extended C++) ==========
    SAFE,
    LET,
    NULLABLE,
    NONNULL,

    // ========== MEMORY/TYPE KEYWORDS ==========
    NEW,
    DELETE,
    NULLPTR,
    NULL_KW,
    SIZEOF,

    // ========== I/O STREAM KEYWORDS ==========
    COUT,
    CIN,

    // ========== OPERATORS ==========
    PLUS,
    MINUS,
    MULTIPLY,
    DIVIDE,
    MODULO,
    ASSIGN,
    EQUAL,
    NOT_EQUAL,
    LESS_THAN,
    GREATER_THAN,
    LESS_EQUAL,
    GREATER_EQUAL,
    LOGICAL_AND,
    LOGICAL_OR,
    LOGICAL_NOT,
    AMPERSAND,
    ARROW,
    STREAM_OUT,
    STREAM_IN,
    BITWISE_AND,
    BITWISE_OR,
    BITWISE_XOR,
    SHIFT_LEFT,
    SHIFT_RIGHT,

    // ========== DELIMITERS ==========
    LEFT_PAREN,
    RIGHT_PAREN,
    LEFT_BRACE,
    RIGHT_BRACE,
    LEFT_BRACKET,
    RIGHT_BRACKET,
    SEMICOLON,
    COMMA,
    DOT,
    COLON,
    NEWLINE,
    STMT_END,  // Virtual statement end (inserted for optional semicolons)

    // ========== PREPROCESSOR ==========
    INCLUDE,
    DEFINE,
    IFNDEF,
    ENDIF,

    // ========== SPECIAL ==========
    END_OF_FILE,
    UNKNOWN
};

class Token {
public:
    TokenType type;
    std::string value;
    int line;
    int column;

    Token(TokenType type, const std::string& value, int line, int column);
    ~Token();

    std::string toString() const;
    std::string getTokenTypeString() const;
    std::string getTokenDescription() const;
    std::string getTokenPattern() const;
    std::string getDetailedFormat(int tokenNumber) const;
};

#endif // TOKEN_H