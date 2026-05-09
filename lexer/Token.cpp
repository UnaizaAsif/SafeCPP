#include "Token.h"

Token::Token(TokenType type, const std::string& value, int line, int column)
    : type(type), value(value), line(line), column(column) {}

Token::~Token() {}

std::string Token::getTokenTypeString() const {
    switch (type) {
        case TokenType::INTEGER:    return "INTEGER";
        case TokenType::FLOAT:      return "FLOAT";
        case TokenType::STRING:     return "STRING";
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::INT:        return "INT";
        case TokenType::FLOAT_KW:   return "FLOAT_KW";
        case TokenType::VOID:       return "VOID";
        case TokenType::CHAR:       return "CHAR";
        case TokenType::DOUBLE:     return "DOUBLE";
        case TokenType::BOOL:       return "BOOL";
        case TokenType::IF:         return "IF";
        case TokenType::ELSE:       return "ELSE";
        case TokenType::WHILE:      return "WHILE";
        case TokenType::FOR:        return "FOR";
        case TokenType::RETURN:     return "RETURN";
        case TokenType::BREAK:      return "BREAK";
        case TokenType::CONTINUE:   return "CONTINUE";
        case TokenType::SWITCH:     return "SWITCH";
        case TokenType::CASE:       return "CASE";
        case TokenType::GOTO:       return "GOTO";
        case TokenType::DEFAULT:    return "DEFAULT";
        case TokenType::CLASS:      return "CLASS";
        case TokenType::STRUCT:     return "STRUCT";
        case TokenType::PUBLIC:     return "PUBLIC";
        case TokenType::PRIVATE:    return "PRIVATE";
        case TokenType::PROTECTED:  return "PROTECTED";
        case TokenType::VIRTUAL:    return "VIRTUAL";
        case TokenType::THIS:       return "THIS";
        case TokenType::OPERATOR:   return "OPERATOR";
        case TokenType::STATIC:     return "STATIC";
        case TokenType::CONST:      return "CONST";
        case TokenType::AUTO:       return "AUTO";
        case TokenType::MUTABLE:    return "MUTABLE";
        case TokenType::EXPLICIT:   return "EXPLICIT";
        case TokenType::FINAL:      return "FINAL";
        case TokenType::OVERRIDE:   return "OVERRIDE";
        case TokenType::SHORT:      return "SHORT";
        case TokenType::LONG:       return "LONG";
        case TokenType::SIGNED:     return "SIGNED";
        case TokenType::UNSIGNED:   return "UNSIGNED";
        case TokenType::WCHAR_T:    return "WCHAR_T";
        case TokenType::CHAR16_T:   return "CHAR16_T";
        case TokenType::CHAR32_T:   return "CHAR32_T";
        case TokenType::CHAR8_T:    return "CHAR8_T";
        case TokenType::TEMPLATE:   return "TEMPLATE";
        case TokenType::TYPENAME:   return "TYPENAME";
        case TokenType::USING:      return "USING";
        case TokenType::NAMESPACE:  return "NAMESPACE";
        case TokenType::TRY:        return "TRY";
        case TokenType::CATCH:      return "CATCH";
        case TokenType::THROW:      return "THROW";
        case TokenType::TRUE_KW:    return "TRUE";
        case TokenType::FALSE_KW:   return "FALSE";
        case TokenType::FUNCTION:   return "FUNCTION";
        case TokenType::SAFE:       return "SAFE";
        case TokenType::LET:        return "LET";
        case TokenType::NULLABLE:   return "NULLABLE";
        case TokenType::NONNULL:    return "NONNULL";
        case TokenType::NEW:        return "NEW";
        case TokenType::DELETE:     return "DELETE";
        case TokenType::NULLPTR:    return "NULLPTR";
        case TokenType::NULL_KW:    return "NULL";
        case TokenType::SIZEOF:     return "SIZEOF";
        case TokenType::COUT:       return "COUT";
        case TokenType::CIN:        return "CIN";
        case TokenType::PLUS:       return "PLUS";
        case TokenType::MINUS:      return "MINUS";
        case TokenType::MULTIPLY:   return "MULTIPLY";
        case TokenType::DIVIDE:     return "DIVIDE";
        case TokenType::MODULO:     return "MODULO";
        case TokenType::ASSIGN:     return "ASSIGN";
        case TokenType::EQUAL:      return "EQUAL";
        case TokenType::NOT_EQUAL:  return "NOT_EQUAL";
        case TokenType::LESS_THAN:  return "LESS_THAN";
        case TokenType::GREATER_THAN:  return "GREATER_THAN";
        case TokenType::LESS_EQUAL:    return "LESS_EQUAL";
        case TokenType::GREATER_EQUAL: return "GREATER_EQUAL";
        case TokenType::LOGICAL_AND:   return "LOGICAL_AND";
        case TokenType::LOGICAL_OR:    return "LOGICAL_OR";
        case TokenType::LOGICAL_NOT:   return "LOGICAL_NOT";
        case TokenType::AMPERSAND:     return "AMPERSAND";
        case TokenType::ARROW:         return "ARROW";
        case TokenType::STREAM_OUT:    return "STREAM_OUT";
        case TokenType::STREAM_IN:     return "STREAM_IN";
        case TokenType::BITWISE_AND:   return "BITWISE_AND";
        case TokenType::BITWISE_OR:    return "BITWISE_OR";
        case TokenType::BITWISE_XOR:   return "BITWISE_XOR";
        case TokenType::SHIFT_LEFT:    return "SHIFT_LEFT";
        case TokenType::SHIFT_RIGHT:   return "SHIFT_RIGHT";
        case TokenType::LEFT_PAREN:    return "LEFT_PAREN";
        case TokenType::RIGHT_PAREN:   return "RIGHT_PAREN";
        case TokenType::LEFT_BRACE:    return "LEFT_BRACE";
        case TokenType::RIGHT_BRACE:   return "RIGHT_BRACE";
        case TokenType::LEFT_BRACKET:  return "LEFT_BRACKET";
        case TokenType::RIGHT_BRACKET: return "RIGHT_BRACKET";
        case TokenType::SEMICOLON:     return "SEMICOLON";
        case TokenType::COMMA:         return "COMMA";
        case TokenType::DOT:           return "DOT";
        case TokenType::COLON:         return "COLON";
        case TokenType::NEWLINE:       return "NEWLINE";
        case TokenType::STMT_END:      return "STMT_END";
        case TokenType::INCLUDE:       return "INCLUDE";
        case TokenType::DEFINE:        return "DEFINE";
        case TokenType::IFNDEF:        return "IFNDEF";
        case TokenType::ENDIF:         return "ENDIF";
        case TokenType::END_OF_FILE:   return "EOF";
        default:                       return "UNKNOWN";
    }
}

std::string Token::toString() const {
    return "[" + getTokenTypeString() + "] '" + value + "' at " +
           std::to_string(line) + ":" + std::to_string(column);
}

std::string Token::getTokenDescription() const { return getTokenTypeString(); }
std::string Token::getTokenPattern()     const { return ""; }

std::string Token::getDetailedFormat(int tokenNumber) const {
    std::string result = "\nToken-" + std::to_string(tokenNumber) +
                         " - \"" + value + "\" at Line " +
                         std::to_string(line) + ", Column " +
                         std::to_string(column) + "\n";
    result += "<\n";
    result += "  Token: \"" + getTokenDescription() + "\"\n";
    result += "  Lexeme: \"" + value + "\"\n";
    result += "  Location: Line " + std::to_string(line) +
              ", Column " + std::to_string(column) + "\n";
    result += ">";
    return result;
}