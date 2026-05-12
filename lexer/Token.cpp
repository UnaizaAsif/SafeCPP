#include "Token.h"

Token::Token(TokenType type, const std::string& value, int line, int column)
    : type(type), value(value), line(line), column(column) {}

Token::~Token() {}

std::string Token::getTokenTypeString() const {
    switch (type) {
        case TokenType::INTEGER:    return "INTEGER";
        case TokenType::FLOAT:      return "FLOAT";
        case TokenType::STRING:     return "STRING";
        case TokenType::CHAR_LITERAL:return "CHAR_LITERAL";
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::INT:        return "KW_INT";
        case TokenType::FLOAT_KW:   return "FLOAT_KW";
        case TokenType::VOID:       return "Keyword";
        case TokenType::CHAR:       return "KW_CHAR";
        case TokenType::DOUBLE:     return "KW_DOUBLE";
        case TokenType::BOOL:       return "Keyword";
        case TokenType::IF:         return "Keyword";
        case TokenType::ELSE:       return "Keyword";
        case TokenType::WHILE:      return "Keyword";
        case TokenType::FOR:        return "Keyword";
        case TokenType::RETURN:     return "Keyword";
        case TokenType::BREAK:      return "Keyword";
        case TokenType::CONTINUE:   return "Keyword";
        case TokenType::SWITCH:     return "Keyword";
        case TokenType::CASE:       return "Keyword";
        case TokenType::GOTO:       return "Keyword";
        case TokenType::DEFAULT:    return "Keyword";
        case TokenType::CLASS:      return "Keyword";
        case TokenType::STRUCT:     return "Keyword";
        case TokenType::PUBLIC:     return "KEYWORD";
        case TokenType::PRIVATE:    return "KEYWORD";
        case TokenType::PROTECTED:  return "KEYWORD";
        case TokenType::VIRTUAL:    return "KEYWORD";
        case TokenType::THIS:       return "KEYWORD";
        case TokenType::OPERATOR:   return "KEYWORD";
        case TokenType::STATIC:     return "KEYWORD";
        case TokenType::CONST:      return "KEYWORD";
        case TokenType::AUTO:       return "KEYWORD";
        case TokenType::MUTABLE:    return "KEYWORD";
        case TokenType::EXPLICIT:   return "KEYWORD";
        case TokenType::FINAL:      return "KEYWORD";
        case TokenType::OVERRIDE:   return "KEYWORD";
        case TokenType::SHORT:      return "Keyword";
        case TokenType::LONG:       return "Keyword";
        case TokenType::SIGNED:     return "Keyword";
        case TokenType::UNSIGNED:   return "Keyword";
        case TokenType::WCHAR_T:    return "WCHAR_T";
        case TokenType::CHAR16_T:   return "CHAR16_T";
        case TokenType::CHAR32_T:   return "CHAR32_T";
        case TokenType::CHAR8_T:    return "CHAR8_T";
        case TokenType::TEMPLATE:   return "Keyword";
        case TokenType::TYPENAME:   return "Keyword";
        case TokenType::USING:      return "Keyword";
        case TokenType::NAMESPACE:  return "Keyword";
        case TokenType::TRY:        return "Keyword";
        case TokenType::CATCH:      return "Keyword";
        case TokenType::THROW:      return "Keyword";
        case TokenType::TRUE_KW:    return "Keyword";
        case TokenType::FALSE_KW:   return "Keyword";
        case TokenType::FUNCTION:   return "Keyword";
        case TokenType::SAFE:       return "Keyword";
        case TokenType::LET:        return "Keyword";
        case TokenType::NULLABLE:   return "Keyword";
        case TokenType::NONNULL:    return "Keyword";
        case TokenType::TYPE_INFERRED_INT:              return "TYPE_INFERRED_INT";
        case TokenType::TYPE_INFERRED_LONG:             return "TYPE_INFERRED_LONG";
        case TokenType::TYPE_INFERRED_UNSIGNED_INT:     return "TYPE_INFERRED_UNSIGNED_INT";
        case TokenType::TYPE_INFERRED_UNSIGNED_LONG:    return "TYPE_INFERRED_UNSIGNED_LONG";
        case TokenType::TYPE_INFERRED_FLOAT:            return "TYPE_INFERRED_FLOAT";
        case TokenType::TYPE_INFERRED_DOUBLE:           return "TYPE_INFERRED_DOUBLE";
        case TokenType::TYPE_INFERRED_CHAR:             return "TYPE_INFERRED_CHAR";
        case TokenType::TYPE_INFERRED_BOOL:             return "TYPE_INFERRED_BOOL";
        case TokenType::TYPE_INFERRED_STRING:           return "TYPE_INFERRED_STRING";
        case TokenType::TYPE_INFERRED_POINTER:          return "TYPE_INFERRED_POINTER";
        case TokenType::TYPE_INFERRED_ARRAY:            return "TYPE_INFERRED_ARRAY";
        case TokenType::NEW:        return "Keyword";
        case TokenType::DELETE:     return "Keyword";
        case TokenType::NULLPTR:    return "Keyword";
        case TokenType::NULL_KW:    return "Keyword";
        case TokenType::SIZEOF:     return "Keyword";
        case TokenType::COUT:       return "Keyword";
        case TokenType::CIN:        return "Keyword";
        case TokenType::PLUS:       return "Operator";
        case TokenType::MINUS:      return "Operator";
        case TokenType::MULTIPLY:   return "Operator";
        case TokenType::DIVIDE:     return "Operator";
        case TokenType::MODULO:     return "Operator";
        case TokenType::ASSIGN:     return "Assignment Operator";
        case TokenType::EQUAL:      return "Relational Operator";
        case TokenType::NOT_EQUAL:  return "Relational Operator";
        case TokenType::LESS_THAN:  return "Relational Operator";
        case TokenType::GREATER_THAN:  return "Relational Operator";
        case TokenType::LESS_EQUAL:    return "Relational Operator";
        case TokenType::GREATER_EQUAL: return "Relational Operator";
        case TokenType::LOGICAL_AND:   return "LOGICAL_AND";
        case TokenType::LOGICAL_OR:    return "LOGICAL_OR";
        case TokenType::LOGICAL_NOT:   return "LOGICAL_NOT";
        case TokenType::AMPERSAND:     return "Operator";
        case TokenType::ARROW:         return "Operator";
        case TokenType::STREAM_OUT:    return "Operator";
        case TokenType::STREAM_IN:     return "Operator";
        case TokenType::BITWISE_AND:   return "Operator";
        case TokenType::BITWISE_OR:    return "Operator";
        case TokenType::BITWISE_XOR:   return "Operator";
        case TokenType::SHIFT_LEFT:    return "Operator";
        case TokenType::SHIFT_RIGHT:   return "Delimiter";
        case TokenType::LEFT_PAREN:    return "Delimiter";
        case TokenType::RIGHT_PAREN:   return "Delimiter";
        case TokenType::LEFT_BRACE:    return "Delimiter";
        case TokenType::RIGHT_BRACE:   return "Delimiter";
        case TokenType::LEFT_BRACKET:  return "Delimiter";
        case TokenType::RIGHT_BRACKET: return "Delimiter";
        case TokenType::SEMICOLON:     return "Delimiter";
        case TokenType::COMMA:         return "Delimiter";
        case TokenType::DOT:           return "Delimiter";
        case TokenType::COLON:         return "Delimiter";
        case TokenType::NEWLINE:       return "NEWLINE";
        case TokenType::STMT_END:      return "STMT_END";
        case TokenType::INCLUDE:       return "Header";
        case TokenType::DEFINE:        return "Keyword";
        case TokenType::IFNDEF:        return "Keyword";
        case TokenType::ENDIF:         return "Keyword";
        case TokenType::END_OF_FILE:   return "EOF";
        default:                       return "Keyword";
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