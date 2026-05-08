#include "Token.h"

Token::Token(TokenType type, const std::string& value, int line, int column)
    : type(type), value(value), line(line), column(column) {}

Token::~Token() {}

std::string Token::getTokenTypeString() const {
    switch (type) {
        // Literals
        case TokenType::INTEGER: return "INTEGER";
        case TokenType::FLOAT: return "FLOAT";
        case TokenType::STRING: return "STRING";
        case TokenType::IDENTIFIER: return "IDENTIFIER";

        // Standard Keywords
        case TokenType::INT: return "INT";
        case TokenType::FLOAT_KW: return "FLOAT";
        case TokenType::VOID: return "VOID";
        case TokenType::CHAR: return "CHAR";
        case TokenType::DOUBLE: return "DOUBLE";
        case TokenType::BOOL: return "BOOL";

        // Control Flow
        case TokenType::IF: return "IF";
        case TokenType::ELSE: return "ELSE";
        case TokenType::WHILE: return "WHILE";
        case TokenType::FOR: return "FOR";
        case TokenType::RETURN: return "RETURN";
        case TokenType::BREAK: return "BREAK";
        case TokenType::CONTINUE: return "CONTINUE";
        case TokenType::SWITCH: return "SWITCH";
        case TokenType::CASE: return "CASE";
        case TokenType::GOTO: return "GOTO";
        case TokenType::DEFAULT: return "DEFAULT";

        // Class/struct keywords
        case TokenType::CLASS: return "CLASS";
        case TokenType::STRUCT: return "STRUCT";
        case TokenType::PUBLIC: return "PUBLIC";
        case TokenType::PRIVATE: return "PRIVATE";
        case TokenType::PROTECTED: return "PROTECTED";
        case TokenType::VIRTUAL: return "VIRTUAL";
        case TokenType::THIS: return "THIS";
        case TokenType::OPERATOR: return "OPERATOR";

        // Qualifier keywords
        case TokenType::STATIC: return "STATIC";
        case TokenType::CONST: return "CONST";
        case TokenType::AUTO: return "AUTO";
        case TokenType::MUTABLE: return "MUTABLE";
        case TokenType::EXPLICIT: return "EXPLICIT";
        case TokenType::FINAL: return "FINAL";
        case TokenType::OVERRIDE: return "OVERRIDE";

        // Extended type keywords
        case TokenType::SHORT: return "SHORT";
        case TokenType::LONG: return "LONG";
        case TokenType::SIGNED: return "SIGNED";
        case TokenType::UNSIGNED: return "UNSIGNED";
        case TokenType::WCHAR_T: return "WCHAR_T";
        case TokenType::CHAR16_T: return "CHAR16_T";
        case TokenType::CHAR32_T: return "CHAR32_T";
        case TokenType::CHAR8_T: return "CHAR8_T";

        // Template/namespace keywords
        case TokenType::TEMPLATE: return "TEMPLATE";
        case TokenType::TYPENAME: return "TYPENAME";
        case TokenType::USING: return "USING";
        case TokenType::NAMESPACE: return "NAMESPACE";

        // Exception keywords
        case TokenType::TRY: return "TRY";
        case TokenType::CATCH: return "CATCH";
        case TokenType::THROW: return "THROW";

        // Boolean literals
        case TokenType::TRUE_KW: return "TRUE";
        case TokenType::FALSE_KW: return "FALSE";

        // Function keyword
        case TokenType::FUNCTION: return "FUNCTION";

        // Safety Keywords (Extended C++)
        case TokenType::SAFE: return "SAFE";
        case TokenType::LET: return "LET";
        case TokenType::NULLABLE: return "NULLABLE";
        case TokenType::NONNULL: return "NONNULL";

        // Memory Keywords
        case TokenType::NEW: return "NEW";
        case TokenType::DELETE: return "DELETE";
        case TokenType::NULLPTR: return "NULLPTR";
        case TokenType::NULL_KW: return "NULL";
        case TokenType::SIZEOF: return "SIZEOF";

        // I/O Stream Keywords
        case TokenType::COUT: return "COUT";
        case TokenType::CIN: return "CIN";

        // Operators
        case TokenType::PLUS: return "PLUS";
        case TokenType::MINUS: return "MINUS";
        case TokenType::MULTIPLY: return "MULTIPLY";
        case TokenType::DIVIDE: return "DIVIDE";
        case TokenType::MODULO: return "MODULO";
        case TokenType::ASSIGN: return "ASSIGN";
        case TokenType::EQUAL: return "EQUAL";
        case TokenType::NOT_EQUAL: return "NOT_EQUAL";
        case TokenType::LESS_THAN: return "LESS_THAN";
        case TokenType::GREATER_THAN: return "GREATER_THAN";
        case TokenType::LESS_EQUAL: return "LESS_EQUAL";
        case TokenType::GREATER_EQUAL: return "GREATER_EQUAL";
        case TokenType::LOGICAL_AND: return "LOGICAL_AND";
        case TokenType::LOGICAL_OR: return "LOGICAL_OR";
        case TokenType::LOGICAL_NOT: return "LOGICAL_NOT";
        case TokenType::AMPERSAND: return "AMPERSAND";
        case TokenType::ARROW: return "ARROW";

        // Delimiters
        case TokenType::LEFT_PAREN: return "LEFT_PAREN";
        case TokenType::RIGHT_PAREN: return "RIGHT_PAREN";
        case TokenType::LEFT_BRACE: return "LEFT_BRACE";
        case TokenType::RIGHT_BRACE: return "RIGHT_BRACE";
        case TokenType::LEFT_BRACKET: return "LEFT_BRACKET";
        case TokenType::RIGHT_BRACKET: return "RIGHT_BRACKET";
        case TokenType::SEMICOLON: return "SEMICOLON";
        case TokenType::COMMA: return "COMMA";
        case TokenType::DOT: return "DOT";
        case TokenType::COLON: return "COLON";
        case TokenType::NEWLINE: return "NEWLINE";

        // Preprocessor
        case TokenType::INCLUDE: return "INCLUDE";
        case TokenType::DEFINE: return "DEFINE";
        case TokenType::IFNDEF: return "IFNDEF";
        case TokenType::ENDIF: return "ENDIF";

        // Special
        case TokenType::END_OF_FILE: return "EOF";
        case TokenType::UNKNOWN: return "UNKNOWN";
        default: return "UNKNOWN";
    }
}

std::string Token::toString() const {
    return "[" + getTokenTypeString() + "] '" + value + "' at " + 
           std::to_string(line) + ":" + std::to_string(column);
}

std::string Token::getTokenDescription() const {
    switch (type) {
        // Literals
        case TokenType::INTEGER: return "Integer constant";
        case TokenType::FLOAT: return "Float constant";
        case TokenType::STRING: return "String literal";
        case TokenType::IDENTIFIER: return "Identifier";

        // Standard Keywords
        case TokenType::INT: return "Keyword (int)";
        case TokenType::FLOAT_KW: return "Keyword (float)";
        case TokenType::VOID: return "Keyword (void)";
        case TokenType::CHAR: return "Keyword (char)";
        case TokenType::DOUBLE: return "Keyword (double)";
        case TokenType::BOOL: return "Keyword (bool)";

        // Control Flow
        case TokenType::IF: return "Keyword (if)";
        case TokenType::ELSE: return "Keyword (else)";
        case TokenType::WHILE: return "Keyword (while)";
        case TokenType::FOR: return "Keyword (for)";
        case TokenType::RETURN: return "Keyword (return)";
        case TokenType::BREAK: return "Keyword (break)";
        case TokenType::CONTINUE: return "Keyword (continue)";
        case TokenType::SWITCH: return "Keyword (switch)";
        case TokenType::CASE: return "Keyword (case)";
        case TokenType::GOTO: return "Keyword (goto)";
        case TokenType::DEFAULT: return "Keyword (default)";

        // Class/struct keywords
        case TokenType::CLASS: return "Keyword (class)";
        case TokenType::STRUCT: return "Keyword (struct)";
        case TokenType::PUBLIC: return "Keyword (public)";
        case TokenType::PRIVATE: return "Keyword (private)";
        case TokenType::PROTECTED: return "Keyword (protected)";
        case TokenType::VIRTUAL: return "Keyword (virtual)";
        case TokenType::THIS: return "Keyword (this)";
        case TokenType::OPERATOR: return "Keyword (operator)";

        // Qualifier keywords
        case TokenType::STATIC: return "Keyword (static)";
        case TokenType::CONST: return "Keyword (const)";
        case TokenType::AUTO: return "Keyword (auto)";
        case TokenType::MUTABLE: return "Keyword (mutable)";
        case TokenType::EXPLICIT: return "Keyword (explicit)";
        case TokenType::FINAL: return "Keyword (final)";
        case TokenType::OVERRIDE: return "Keyword (override)";

        // Extended type keywords
        case TokenType::SHORT: return "Keyword (short)";
        case TokenType::LONG: return "Keyword (long)";
        case TokenType::SIGNED: return "Keyword (signed)";
        case TokenType::UNSIGNED: return "Keyword (unsigned)";
        case TokenType::WCHAR_T: return "Keyword (wchar_t)";
        case TokenType::CHAR16_T: return "Keyword (char16_t)";
        case TokenType::CHAR32_T: return "Keyword (char32_t)";
        case TokenType::CHAR8_T: return "Keyword (char8_t)";

        // Template/namespace keywords
        case TokenType::TEMPLATE: return "Keyword (template)";
        case TokenType::TYPENAME: return "Keyword (typename)";
        case TokenType::USING: return "Keyword (using)";
        case TokenType::NAMESPACE: return "Keyword (namespace)";

        // Exception keywords
        case TokenType::TRY: return "Keyword (try)";
        case TokenType::CATCH: return "Keyword (catch)";
        case TokenType::THROW: return "Keyword (throw)";

        // Boolean literals
        case TokenType::TRUE_KW: return "Boolean literal (true)";
        case TokenType::FALSE_KW: return "Boolean literal (false)";

        // Function keyword
        case TokenType::FUNCTION: return "Keyword (function)";

        // Safety Keywords
        case TokenType::SAFE: return "Keyword (@safe)";
        case TokenType::LET: return "Keyword (let)";
        case TokenType::NULLABLE: return "Keyword (nullable)";
        case TokenType::NONNULL: return "Keyword (nonnull)";

        // Memory Keywords
        case TokenType::NEW: return "Keyword (new)";
        case TokenType::DELETE: return "Keyword (delete)";
        case TokenType::NULLPTR: return "Keyword (nullptr)";
        case TokenType::NULL_KW: return "Keyword (NULL)";
        case TokenType::SIZEOF: return "Keyword (sizeof)";

        // I/O Stream Keywords
        case TokenType::COUT: return "Keyword (cout)";
        case TokenType::CIN: return "Keyword (cin)";

        // Operators
        case TokenType::PLUS: return "Addition operator";
        case TokenType::MINUS: return "Subtraction operator";
        case TokenType::MULTIPLY: return "Multiplication operator";
        case TokenType::DIVIDE: return "Division operator";
        case TokenType::MODULO: return "Modulo operator";
        case TokenType::ASSIGN: return "Assignment operator";
        case TokenType::EQUAL: return "Equality operator";
        case TokenType::NOT_EQUAL: return "Inequality operator";
        case TokenType::LESS_THAN: return "Less than operator";
        case TokenType::GREATER_THAN: return "Greater than operator";
        case TokenType::LESS_EQUAL: return "Less than or equal operator";
        case TokenType::GREATER_EQUAL: return "Greater than or equal operator";
        case TokenType::LOGICAL_AND: return "Logical AND operator";
        case TokenType::LOGICAL_OR: return "Logical OR operator";
        case TokenType::LOGICAL_NOT: return "Logical NOT operator";
        case TokenType::AMPERSAND: return "Address-of operator";
        case TokenType::ARROW: return "Arrow operator";

        // Delimiters
        case TokenType::LEFT_PAREN: return "Left parenthesis";
        case TokenType::RIGHT_PAREN: return "Right parenthesis";
        case TokenType::LEFT_BRACE: return "Left brace";
        case TokenType::RIGHT_BRACE: return "Right brace";
        case TokenType::LEFT_BRACKET: return "Left bracket";
        case TokenType::RIGHT_BRACKET: return "Right bracket";
        case TokenType::SEMICOLON: return "Semicolon";
        case TokenType::COMMA: return "Comma";
        case TokenType::DOT: return "Dot operator";
        case TokenType::COLON: return "Colon";
        case TokenType::NEWLINE: return "Newline";

        // Preprocessor
        case TokenType::INCLUDE: return "Preprocessor (include)";
        case TokenType::DEFINE: return "Preprocessor (define)";
        case TokenType::IFNDEF: return "Preprocessor (ifndef)";
        case TokenType::ENDIF: return "Preprocessor (endif)";

        // Special
        case TokenType::END_OF_FILE: return "End of file";
        case TokenType::UNKNOWN: return "Unknown token";
        default: return "Unknown";
    }
}

std::string Token::getTokenPattern() const {
    switch (type) {
        // Literals
        case TokenType::INTEGER: return "[0-9]+ -- sequence of one or more decimal digits representing a whole number (e.g., 0, 42, 12345)";
        case TokenType::FLOAT: return "[0-9]+\\.[0-9]+ -- sequence of digits with a decimal point representing a floating-point number (e.g., 3.14, 0.5, 123.456)";
        case TokenType::STRING: return "\\\"[^\\\"]*\\\" -- characters enclosed in double quotation marks, may contain escape sequences (e.g., \"hello\", \"line\\nbreak\")";
        case TokenType::IDENTIFIER: return "[a-zA-Z_][a-zA-Z0-9_]* -- starts with letter or underscore, followed by zero or more alphanumeric characters or underscores";

        // Keywords
        case TokenType::INT: return "Reserved keyword: 'int' -- specifies integer type for variable declarations";
        case TokenType::FLOAT_KW: return "Reserved keyword: 'float' -- specifies floating-point type for variable declarations";
        case TokenType::VOID: return "Reserved keyword: 'void' -- specifies absence of return type for functions";
        case TokenType::CHAR: return "Reserved keyword: 'char' -- specifies character type for variable declarations";
        case TokenType::DOUBLE: return "Reserved keyword: 'double' -- specifies double-precision floating-point type for variable declarations";
        case TokenType::BOOL: return "Reserved keyword: 'bool' -- specifies boolean type (true/false) for variable declarations";
        case TokenType::IF: return "Reserved keyword: 'if' -- begins conditional statement for branching logic";
        case TokenType::ELSE: return "Reserved keyword: 'else' -- alternative branch in conditional statement";
        case TokenType::WHILE: return "Reserved keyword: 'while' -- begins loop statement that continues while condition is true";
        case TokenType::FOR: return "Reserved keyword: 'for' -- begins loop statement with initialization, condition, and increment";
        case TokenType::RETURN: return "Reserved keyword: 'return' -- exits function and optionally returns a value to caller";
        case TokenType::BREAK: return "Reserved keyword: 'break' -- exits current loop or switch statement";
        case TokenType::CONTINUE: return "Reserved keyword: 'continue' -- skips remaining iterations and continues to next iteration";
        case TokenType::SWITCH: return "Reserved keyword: 'switch' -- begins multi-way conditional statement";
        case TokenType::CASE: return "Reserved keyword: 'case' -- label for switch statement alternative";
        case TokenType::GOTO: return "Reserved keyword: 'goto' -- unconditional jump to labeled location in code";
        case TokenType::DEFAULT: return "Reserved keyword: 'default' -- default case label in switch statement, executed if no other cases match";

        // Class/struct keywords
        case TokenType::CLASS: return "Reserved keyword: 'class' -- defines a class type with members and methods";
        case TokenType::STRUCT: return "Reserved keyword: 'struct' -- defines a structure type, similar to class but members default to public";
        case TokenType::PUBLIC: return "Access modifier: 'public' -- members accessible from outside the class";
        case TokenType::PRIVATE: return "Access modifier: 'private' -- members accessible only within the class";
        case TokenType::PROTECTED: return "Access modifier: 'protected' -- members accessible within class and derived classes";
        case TokenType::VIRTUAL: return "Reserved keyword: 'virtual' -- enables dynamic polymorphism for member functions";
        case TokenType::THIS: return "Reserved keyword: 'this' -- pointer to current object instance";
        case TokenType::OPERATOR: return "Reserved keyword: 'operator' -- overloads built-in operators for user-defined types";

        // Qualifier keywords
        case TokenType::STATIC: return "Reserved keyword: 'static' -- declares static storage duration or static class member";
        case TokenType::CONST: return "Reserved keyword: 'const' -- declares constant that cannot be modified after initialization";
        case TokenType::AUTO: return "Reserved keyword: 'auto' -- automatic type deduction for variable declarations";
        case TokenType::MUTABLE: return "Reserved keyword: 'mutable' -- allows modification of class member in const member functions";
        case TokenType::EXPLICIT: return "Reserved keyword: 'explicit' -- prevents implicit type conversion via constructor";
        case TokenType::FINAL: return "Reserved keyword: 'final' -- prevents derived class inheritance or virtual method override";
        case TokenType::OVERRIDE: return "Reserved keyword: 'override' -- explicitly marks method as overriding virtual base class method";

        // Extended type keywords
        case TokenType::SHORT: return "Reserved keyword: 'short' -- type modifier for reduced size integer (typically 16-bit)";
        case TokenType::LONG: return "Reserved keyword: 'long' -- type modifier for extended size integer (typically 32 or 64-bit)";
        case TokenType::SIGNED: return "Reserved keyword: 'signed' -- type modifier for signed integer allowing negative values";
        case TokenType::UNSIGNED: return "Reserved keyword: 'unsigned' -- type modifier for unsigned integer (non-negative values only)";
        case TokenType::WCHAR_T: return "Reserved keyword: 'wchar_t' -- wide character type for international character support";
        case TokenType::CHAR16_T: return "Reserved keyword: 'char16_t' -- 16-bit character type for UTF-16 encoded text";
        case TokenType::CHAR32_T: return "Reserved keyword: 'char32_t' -- 32-bit character type for UTF-32 encoded text";
        case TokenType::CHAR8_T: return "Reserved keyword: 'char8_t' -- 8-bit character type for UTF-8 encoded text";

        // Template/namespace keywords
        case TokenType::TEMPLATE: return "Reserved keyword: 'template' -- declares template class or function";
        case TokenType::TYPENAME: return "Reserved keyword: 'typename' -- specifies a template parameter is a type";
        case TokenType::USING: return "Reserved keyword: 'using' -- declares namespace alias or using declaration";
        case TokenType::NAMESPACE: return "Reserved keyword: 'namespace' -- defines namespace scope for identifiers";

        // Exception keywords
        case TokenType::TRY: return "Reserved keyword: 'try' -- begins exception handling block";
        case TokenType::CATCH: return "Reserved keyword: 'catch' -- catches specific exception type in exception handling";
        case TokenType::THROW: return "Reserved keyword: 'throw' -- throws an exception to interrupt normal flow";

        // Boolean literals
        case TokenType::TRUE_KW: return "Boolean literal: 'true' -- represents true value in boolean expressions";
        case TokenType::FALSE_KW: return "Boolean literal: 'false' -- represents false value in boolean expressions";

        // Function keyword
        case TokenType::FUNCTION: return "Extended C++ keyword: 'function' -- declares function type or function object";

        // Safety Keywords
        case TokenType::SAFE: return "Extended C++ keyword: 'safe' -- marks declaration as safe with runtime bounds checking";
        case TokenType::LET: return "Extended C++ keyword: 'let' -- keyword for type inference, type is automatically deduced from initializer";
        case TokenType::NULLABLE: return "Extended C++ keyword: 'nullable' -- marks pointer as potentially null, requires null checking before dereference";
        case TokenType::NONNULL: return "Extended C++ keyword: 'nonnull' -- marks pointer as guaranteed non-null, assumes pointer is always valid";

        // Memory Keywords
        case TokenType::NEW: return "Reserved keyword: 'new' -- allocates dynamic memory on heap and returns pointer to allocated object";
        case TokenType::DELETE: return "Reserved keyword: 'delete' -- deallocates dynamic memory previously allocated with 'new'";
        case TokenType::NULLPTR: return "Reserved keyword: 'nullptr' -- null pointer literal representing absence of valid memory address";
        case TokenType::NULL_KW: return "Reserved keyword: 'NULL' -- null pointer constant (legacy, equivalent to nullptr)";
        case TokenType::SIZEOF: return "Reserved keyword: 'sizeof' -- unary operator returning size of type or variable in bytes";

        // I/O Stream Keywords
        case TokenType::COUT: return "I/O stream keyword: 'cout' -- standard output stream for printing to console";
        case TokenType::CIN: return "I/O stream keyword: 'cin' -- standard input stream for reading from console";

        // Operators
        case TokenType::PLUS: return "Arithmetic operator: '+' -- binary operator for addition or unary operator for positive value";
        case TokenType::MINUS: return "Arithmetic operator: '-' -- binary operator for subtraction or unary operator for negation";
        case TokenType::MULTIPLY: return "Arithmetic operator: '*' -- binary operator for multiplication or pointer dereference operator";
        case TokenType::DIVIDE: return "Arithmetic operator: '/' -- binary operator for division of two operands";
        case TokenType::MODULO: return "Arithmetic operator: '%' -- binary operator for remainder after integer division";
        case TokenType::ASSIGN: return "Assignment operator: '=' -- binary operator for variable assignment (right-to-left)";
        case TokenType::EQUAL: return "Relational operator: '==' -- binary operator testing equality of two operands";
        case TokenType::NOT_EQUAL: return "Relational operator: '!=' -- binary operator testing inequality of two operands";
        case TokenType::LESS_THAN: return "Relational operator: '<' -- binary operator testing if left operand is less than right operand";
        case TokenType::GREATER_THAN: return "Relational operator: '>' -- binary operator testing if left operand is greater than right operand";
        case TokenType::LESS_EQUAL: return "Relational operator: '<=' -- binary operator testing if left operand is less than or equal to right";
        case TokenType::GREATER_EQUAL: return "Relational operator: '>=' -- binary operator testing if left operand is greater than or equal to right";
        case TokenType::LOGICAL_AND: return "Logical operator: '&&' -- binary operator for logical AND with short-circuit evaluation";
        case TokenType::LOGICAL_OR: return "Logical operator: '||' -- binary operator for logical OR with short-circuit evaluation";
        case TokenType::LOGICAL_NOT: return "Logical operator: '!' -- unary operator for logical negation of boolean value";
        case TokenType::AMPERSAND: return "Unary operator: '&' -- address-of operator, retrieves memory address of variable";
        case TokenType::ARROW: return "Member access operator: '->' -- binary operator for accessing member through pointer (dereferencing + member access)";

        // Delimiters
        case TokenType::LEFT_PAREN: return "Delimiter: '(' -- left parenthesis for grouping expressions or function parameter lists";
        case TokenType::RIGHT_PAREN: return "Delimiter: ')' -- right parenthesis matching left parenthesis";
        case TokenType::LEFT_BRACE: return "Delimiter: '{' -- left brace denoting start of code block or compound statement";
        case TokenType::RIGHT_BRACE: return "Delimiter: '}' -- right brace denoting end of code block or compound statement";
        case TokenType::LEFT_BRACKET: return "Delimiter: '[' -- left bracket for array subscript operator or array declaration";
        case TokenType::RIGHT_BRACKET: return "Delimiter: ']' -- right bracket matching left bracket";
        case TokenType::SEMICOLON: return "Statement terminator: ';' -- marks end of declaration or statement (can be optional in Extended C++)";
        case TokenType::COMMA: return "Separator: ',' -- separates elements in parameter lists, argument lists, or initializer lists";
        case TokenType::DOT: return "Member access operator: '.' -- binary operator for accessing member of structure or class directly";
        case TokenType::COLON: return "Delimiter: ':' -- used in case labels, ternary operator, and scope resolution";
        case TokenType::NEWLINE: return "Whitespace token: newline character -- significant in Extended C++ for implicit statement termination";

        // Preprocessor
        case TokenType::INCLUDE: return "Preprocessor directive: 'include' -- includes contents of another file at this location";
        case TokenType::DEFINE: return "Preprocessor directive: 'define' -- defines macro constant or function-like macro";
        case TokenType::IFNDEF: return "Preprocessor directive: 'ifndef' -- conditional compilation: true if identifier not defined";
        case TokenType::ENDIF: return "Preprocessor directive: 'endif' -- ends conditional compilation block (#if, #ifdef, #ifndef)";

        // Special
        case TokenType::END_OF_FILE: return "Special token: EOF -- represents end of input stream, signals completion of tokenization";
        case TokenType::UNKNOWN: return "Unknown token: invalid character sequence -- unrecognized character that does not match any token pattern";
        default: return "Unknown pattern";
    }
}

std::string Token::getDetailedFormat(int tokenNumber) const {
    std::string result = "\nToken-" + std::to_string(tokenNumber) + " - \"" + value + "\"\n";
    result += "<\n";
    result += "  Token: \"" + getTokenDescription() + "\"\n";
    result += "  Pattern: \"" + getTokenPattern() + "\"\n";
    result += "  Lexeme: \"" + value + "\"\n";
    result += ">";
    return result;
}
