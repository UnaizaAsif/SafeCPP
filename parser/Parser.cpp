#include "Parser.h"
#include <stdexcept>
#include <iostream>

// ========== AST VISITOR IMPLEMENTATIONS ==========
void Program::accept(ASTVisitor* visitor) { visitor->visit(this); }
void Declaration::accept(ASTVisitor* visitor) { visitor->visit(this); }
void VariableDecl::accept(ASTVisitor* visitor) { visitor->visit(this); }
void FunctionDecl::accept(ASTVisitor* visitor) { visitor->visit(this); }
void Statement::accept(ASTVisitor* visitor) { visitor->visit(this); }
void Expression::accept(ASTVisitor* visitor) { visitor->visit(this); }

// ========== PARSER IMPLEMENTATION ==========

Parser::Parser(const std::vector<Token>& tokens)
    : tokens(tokens), position(0) {}

Parser::~Parser() {}

Token Parser::currentToken() {
    if (position >= tokens.size()) {
        return Token(TokenType::END_OF_FILE, "", 0, 0);
    }
    return tokens[position];
}

Token Parser::peekToken() {
    if (position + 1 >= tokens.size()) {
        return Token(TokenType::END_OF_FILE, "", 0, 0);
    }
    return tokens[position + 1];
}

void Parser::advance() {
    if (position < tokens.size()) {
        position++;
    }
}

void Parser::expect(TokenType type) {
    if (currentToken().type != type) {
        error("Expected " + Token(type, "", 0, 0).getTokenTypeString() + 
              " but got " + currentToken().getTokenTypeString());
    }
    advance();
}

bool Parser::match(TokenType type) {
    if (currentToken().type == type) {
        advance();
        return true;
    }
    return false;
}

bool Parser::match(TokenType type1, TokenType type2) {
    if (currentToken().type == type1 || currentToken().type == type2) {
        advance();
        return true;
    }
    return false;
}

void Parser::error(const std::string& message) {
    std::string errorMsg = "Parse error at line " + std::to_string(currentToken().line) + 
                           ", column " + std::to_string(currentToken().column) + 
                           ": " + message;
    errors.push_back(errorMsg);
    throw std::runtime_error(errorMsg);
}

void Parser::synchronize() {
    advance();
    while (currentToken().type != TokenType::END_OF_FILE) {
        if (currentToken().type == TokenType::SEMICOLON) {
            advance();
            return;
        }
        switch (currentToken().type) {
            case TokenType::IF:
            case TokenType::WHILE:
            case TokenType::FOR:
            case TokenType::RETURN:
                return;
            default:
                break;
        }
        advance();
    }
}

bool Parser::isStatementEnd() {
    return currentToken().type == TokenType::SEMICOLON || 
           currentToken().type == TokenType::NEWLINE ||
           currentToken().type == TokenType::RIGHT_BRACE ||
           currentToken().type == TokenType::END_OF_FILE;
}

std::shared_ptr<Program> Parser::parse() {
    return parseProgram();
}

std::shared_ptr<Program> Parser::parseProgram() {
    auto program = std::make_shared<Program>();
    
    while (currentToken().type != TokenType::END_OF_FILE) {
        // Skip newlines
        while (match(TokenType::NEWLINE)) {}
        
        if (currentToken().type == TokenType::END_OF_FILE) break;
        
        try {
            // Skip preprocessor directives
            if (currentToken().type == TokenType::INCLUDE ||
                currentToken().type == TokenType::DEFINE ||
                currentToken().type == TokenType::IFNDEF ||
                currentToken().type == TokenType::ENDIF) {
                skipPreprocessor();
                continue;
            }
            
            auto decl = parseDeclaration();
            if (decl) {
                program->declarations.push_back(decl);
            }
        } catch (const std::exception& e) {
            synchronize();
        }
    }
    
    return program;
}

std::shared_ptr<ASTNode> Parser::skipPreprocessor() {
    // Skip preprocessor directive tokens
    while (currentToken().type != TokenType::NEWLINE && 
           currentToken().type != TokenType::END_OF_FILE) {
        advance();
    }
    match(TokenType::NEWLINE);
    
    return std::make_shared<Statement>();
}

std::shared_ptr<ASTNode> Parser::parseDeclaration() {
    // Skip newlines
    while (match(TokenType::NEWLINE)) {}
    
    // Check for class declaration
    if (currentToken().type == TokenType::CLASS || currentToken().type == TokenType::STRUCT) {
        return parseClassDecl();
    }
    
    // Check for namespace declaration
    if (currentToken().type == TokenType::NAMESPACE) {
        return parseNamespaceDecl();
    }
    
    // Check for template declaration
    if (currentToken().type == TokenType::TEMPLATE) {
        advance(); // Skip 'template'
        if (match(TokenType::LESS_THAN)) {
            // Skip template parameters
            int depth = 1;
            while (depth > 0 && currentToken().type != TokenType::END_OF_FILE) {
                if (currentToken().type == TokenType::LESS_THAN) depth++;
                if (currentToken().type == TokenType::GREATER_THAN) depth--;
                advance();
            }
        }
        // Parse the actual declaration following template
        return parseDeclaration();
    }
    
    // Check for let declaration
    if (currentToken().type == TokenType::LET) {
        return parseLetDecl();
    }
    
    // Skip access modifiers at namespace level
    if (currentToken().type == TokenType::PUBLIC || 
        currentToken().type == TokenType::PRIVATE || 
        currentToken().type == TokenType::PROTECTED) {
        advance();
        if (match(TokenType::COLON)) {
            // Access modifier with colon - skip it
            return std::make_shared<Statement>();
        }
    }
    
    // Try to parse function or variable declaration
    return parseFunctionOrVariable();
}

std::shared_ptr<ASTNode> Parser::parseClassDecl() {
    bool isFinal = false;
    if (currentToken().type == TokenType::FINAL) {
        isFinal = true;
        advance();
    }
    
    advance(); // Skip CLASS or STRUCT
    
    auto classDecl = std::make_shared<Statement>();
    classDecl->line = currentToken().line;
    
    // Class name
    if (currentToken().type == TokenType::IDENTIFIER) {
        advance();
    }
    
    // Optional inheritance
    if (match(TokenType::COLON)) {
        // Skip inheritance list until we hit {
        while (currentToken().type != TokenType::LEFT_BRACE && 
               currentToken().type != TokenType::END_OF_FILE) {
            advance();
        }
    }
    
    // Class body
    if (match(TokenType::LEFT_BRACE)) {
        int braceDepth = 1;
        while (braceDepth > 0 && currentToken().type != TokenType::END_OF_FILE) {
            if (currentToken().type == TokenType::LEFT_BRACE) braceDepth++;
            if (currentToken().type == TokenType::RIGHT_BRACE) braceDepth--;
            
            if (braceDepth > 0) {
                advance();
            } else {
                advance(); // Consume closing }
            }
        }
    }
    match(TokenType::SEMICOLON, TokenType::NEWLINE);
    
    return classDecl;
}

std::shared_ptr<ASTNode> Parser::parseNamespaceDecl() {
    expect(TokenType::NAMESPACE);
    
    auto nsDecl = std::make_shared<Statement>();
    nsDecl->line = currentToken().line;
    
    // Namespace name
    if (currentToken().type == TokenType::IDENTIFIER) {
        advance();
    }
    
    // Namespace body
    if (match(TokenType::LEFT_BRACE)) {
        int braceDepth = 1;
        while (braceDepth > 0 && currentToken().type != TokenType::END_OF_FILE) {
            if (currentToken().type == TokenType::LEFT_BRACE) braceDepth++;
            if (currentToken().type == TokenType::RIGHT_BRACE) braceDepth--;
            
            if (braceDepth > 0) {
                advance();
            } else {
                advance(); // Consume closing }
            }
        }
    }
    
    return nsDecl;
}

std::shared_ptr<ASTNode> Parser::parseFunctionOrVariable() {
    // Skip virtual and static modifiers
    while (currentToken().type == TokenType::VIRTUAL || 
           currentToken().type == TokenType::STATIC) {
        advance();
    }
    
    // Look ahead to determine if it's a function or variable
    size_t lookAhead = position;
    
    if (currentToken().type == TokenType::SAFE) {
        lookAhead++;
    }
    if (lookAhead < tokens.size() && tokens[lookAhead].type == TokenType::NULLABLE) {
        lookAhead++;
    }
    
    // Skip type modifiers
    while (lookAhead < tokens.size() && 
           (tokens[lookAhead].type == TokenType::SIGNED ||
            tokens[lookAhead].type == TokenType::UNSIGNED ||
            tokens[lookAhead].type == TokenType::SHORT ||
            tokens[lookAhead].type == TokenType::LONG ||
            tokens[lookAhead].type == TokenType::CONST)) {
        lookAhead++;
    }
    
    // Skip base type and qualified name
    if (lookAhead < tokens.size() && 
        (tokens[lookAhead].type == TokenType::IDENTIFIER ||
         tokens[lookAhead].type == TokenType::INT ||
         tokens[lookAhead].type == TokenType::FLOAT_KW ||
         tokens[lookAhead].type == TokenType::DOUBLE ||
         tokens[lookAhead].type == TokenType::CHAR ||
         tokens[lookAhead].type == TokenType::BOOL ||
         tokens[lookAhead].type == TokenType::VOID ||
         tokens[lookAhead].type == TokenType::AUTO)) {
        lookAhead++;
        
        // Handle qualified names (std::string)
        while (lookAhead < tokens.size() && tokens[lookAhead].type == TokenType::COLON) {
            lookAhead++; // Skip first colon
            if (lookAhead < tokens.size() && tokens[lookAhead].type == TokenType::COLON) {
                lookAhead++; // Skip second colon
            }
            if (lookAhead < tokens.size() && tokens[lookAhead].type == TokenType::IDENTIFIER) {
                lookAhead++; // Skip identifier after ::
            }
        }
        
        // Handle template parameters
        if (lookAhead < tokens.size() && tokens[lookAhead].type == TokenType::LESS_THAN) {
            lookAhead++;
            int depth = 1;
            while (depth > 0 && lookAhead < tokens.size()) {
                if (tokens[lookAhead].type == TokenType::LESS_THAN) depth++;
                if (tokens[lookAhead].type == TokenType::GREATER_THAN) depth--;
                lookAhead++;
            }
        }
        
        // Skip pointers/references
        while (lookAhead < tokens.size() && 
               (tokens[lookAhead].type == TokenType::MULTIPLY ||
                tokens[lookAhead].type == TokenType::AMPERSAND)) {
            lookAhead++;
        }
    }
    
    // Check if next is identifier followed by ( (function) or = or ; (variable)
    if (lookAhead < tokens.size() && tokens[lookAhead].type == TokenType::IDENTIFIER) {
        lookAhead++;
        if (lookAhead < tokens.size()) {
            if (tokens[lookAhead].type == TokenType::LEFT_PAREN) {
                return parseFunctionDecl();
            }
        }
    }
    
    return parseVariableDecl();
}

std::shared_ptr<ASTNode> Parser::parseFunctionDecl() {
    auto func = std::make_shared<FunctionDecl>();
    func->line = currentToken().line;
    func->column = currentToken().column;
    func->isSafe = false;
    func->isNullable = false;
    
    // Skip virtual keyword if present
    if (match(TokenType::VIRTUAL)) {}
    
    // Parse return type
    func->type = currentToken().value;
    advance();
    
    // Parse function name
    if (currentToken().type != TokenType::IDENTIFIER) {
        error("Expected function name");
    }
    func->name = currentToken().value;
    advance();
    
    // Parse parameters
    expect(TokenType::LEFT_PAREN);
    while (currentToken().type != TokenType::RIGHT_PAREN && 
           currentToken().type != TokenType::END_OF_FILE) {
        std::string paramType = currentToken().value;
        advance();
        
        if (currentToken().type != TokenType::IDENTIFIER) {
            error("Expected parameter name");
        }
        std::string paramName = currentToken().value;
        advance();
        
        func->parameters.push_back({paramType, paramName});
        
        if (match(TokenType::COMMA)) {
            // Continue to next parameter
        } else if (currentToken().type != TokenType::RIGHT_PAREN) {
            error("Expected ',' or ')' in parameter list");
        }
    }
    expect(TokenType::RIGHT_PAREN);
    
    // Skip override and final modifiers
    if (match(TokenType::OVERRIDE)) {}
    if (match(TokenType::FINAL)) {}
    
    // Skip function body or : (for constructor initializer list)
    if (match(TokenType::COLON)) {
        // Constructor initializer list - skip it
        while (currentToken().type != TokenType::LEFT_BRACE &&
               currentToken().type != TokenType::END_OF_FILE) {
            advance();
        }
    }
    
    // Parse function body if present
    if (match(TokenType::LEFT_BRACE)) {
        int braceDepth = 1;
        while (braceDepth > 0 && currentToken().type != TokenType::END_OF_FILE) {
            if (currentToken().type == TokenType::LEFT_BRACE) braceDepth++;
            if (currentToken().type == TokenType::RIGHT_BRACE) braceDepth--;
            advance();
        }
    } else {
        // Function declaration only (no body)
        match(TokenType::SEMICOLON, TokenType::NEWLINE);
    }
    
    return func;
}

std::shared_ptr<ASTNode> Parser::parseVariableDecl() {
    auto varDecl = std::make_shared<VariableDecl>();
    varDecl->line = currentToken().line;
    varDecl->column = currentToken().column;
    
    // Parse @safe modifier
    varDecl->isSafe = match(TokenType::SAFE);
    
    // Parse nullable modifier
    varDecl->isNullable = match(TokenType::NULLABLE);
    
    // Skip type modifiers
    while (currentToken().type == TokenType::SIGNED ||
           currentToken().type == TokenType::UNSIGNED ||
           currentToken().type == TokenType::SHORT ||
           currentToken().type == TokenType::LONG ||
           currentToken().type == TokenType::CONST) {
        advance();
    }
    
    // Parse type (including qualified names like std::string)
    if (currentToken().type == TokenType::IDENTIFIER ||
        currentToken().type == TokenType::INT ||
        currentToken().type == TokenType::FLOAT_KW ||
        currentToken().type == TokenType::DOUBLE ||
        currentToken().type == TokenType::CHAR ||
        currentToken().type == TokenType::BOOL ||
        currentToken().type == TokenType::VOID ||
        currentToken().type == TokenType::AUTO ||
        currentToken().type == TokenType::WCHAR_T ||
        currentToken().type == TokenType::CHAR16_T ||
        currentToken().type == TokenType::CHAR32_T ||
        currentToken().type == TokenType::CHAR8_T) {
        varDecl->type = currentToken().value;
        advance();
        
        // Handle qualified names (std::string)
        while (currentToken().type == TokenType::COLON) {
            advance(); // Skip first colon
            if (currentToken().type == TokenType::COLON) {
                advance(); // Skip second colon
            }
            if (currentToken().type == TokenType::IDENTIFIER) {
                varDecl->type += "::" + currentToken().value;
                advance();
            }
        }
        
        // Handle template parameters (vector<int>)
        if (currentToken().type == TokenType::LESS_THAN) {
            varDecl->type += "<";
            advance();
            int depth = 1;
            while (depth > 0 && currentToken().type != TokenType::END_OF_FILE) {
                if (currentToken().type == TokenType::LESS_THAN) depth++;
                if (currentToken().type == TokenType::GREATER_THAN) depth--;
                varDecl->type += currentToken().value;
                advance();
            }
        }
        
        // Handle pointers/references
        while (currentToken().type == TokenType::MULTIPLY ||
               currentToken().type == TokenType::AMPERSAND) {
            varDecl->type += currentToken().value;
            advance();
        }
    } else {
        // Skip this declaration if type is not recognized
        while (!isStatementEnd() && currentToken().type != TokenType::END_OF_FILE) {
            advance();
        }
        match(TokenType::SEMICOLON, TokenType::NEWLINE);
        return varDecl;
    }
    
    // Parse variable name
    if (currentToken().type == TokenType::IDENTIFIER) {
        varDecl->name = currentToken().value;
        advance();
    } else {
        // Skip malformed declaration
        while (!isStatementEnd() && currentToken().type != TokenType::END_OF_FILE) {
            advance();
        }
        match(TokenType::SEMICOLON, TokenType::NEWLINE);
        return varDecl;
    }
    
    // Parse optional initializer
    if (match(TokenType::ASSIGN)) {
        varDecl->initializer = parseExpression();
    } else if (match(TokenType::LEFT_BRACE)) {
        // Handle brace initialization {}
        int braceDepth = 1;
        while (braceDepth > 0 && currentToken().type != TokenType::END_OF_FILE) {
            if (currentToken().type == TokenType::LEFT_BRACE) braceDepth++;
            if (currentToken().type == TokenType::RIGHT_BRACE) braceDepth--;
            advance();
        }
    } else if (match(TokenType::LEFT_PAREN)) {
        // Handle parentheses initialization ()
        int parenDepth = 1;
        while (parenDepth > 0 && currentToken().type != TokenType::END_OF_FILE) {
            if (currentToken().type == TokenType::LEFT_PAREN) parenDepth++;
            if (currentToken().type == TokenType::RIGHT_PAREN) parenDepth--;
            advance();
        }
    }
    
    // Semicolon-free support: accept newline or semicolon or nothing
    match(TokenType::SEMICOLON, TokenType::NEWLINE);
    
    return varDecl;
}

std::shared_ptr<ASTNode> Parser::parseLetDecl() {
    auto letDecl = std::make_shared<VariableDecl>();
    letDecl->line = currentToken().line;
    letDecl->column = currentToken().column;
    
    expect(TokenType::LET);
    
    if (currentToken().type != TokenType::IDENTIFIER) {
        error("Expected identifier after 'let'");
    }
    letDecl->name = currentToken().value;
    advance();
    
    expect(TokenType::ASSIGN);
    letDecl->initializer = parseExpression();
    
    letDecl->type = "auto"; // Type will be inferred
    
    // Semicolon-free support
    match(TokenType::SEMICOLON, TokenType::NEWLINE);
    
    return letDecl;
}

std::shared_ptr<ASTNode> Parser::parseIncludeStmt() {
    // Skip preprocessor - already handled by skipPreprocessor()
    return std::make_shared<Statement>();
}

std::shared_ptr<ASTNode> Parser::parseStatement() {
    // Skip newlines
    while (match(TokenType::NEWLINE)) {}
    
    switch (currentToken().type) {
        case TokenType::LEFT_BRACE:
            return parseBlock();
        case TokenType::IF:
            return parseIfStatement();
        case TokenType::WHILE:
            return parseWhileStatement();
        case TokenType::FOR:
            return parseForStatement();
        case TokenType::SWITCH:
            return parseSwitchStatement();
        case TokenType::RETURN:
            return parseReturnStatement();
        case TokenType::GOTO:
            return parseGotoStatement();
        case TokenType::BREAK:
            advance();
            match(TokenType::SEMICOLON, TokenType::NEWLINE);
            return std::make_shared<Statement>();
        case TokenType::CONTINUE:
            advance();
            match(TokenType::SEMICOLON, TokenType::NEWLINE);
            return std::make_shared<Statement>();
        default:
            return parseExpressionStatement();
    }
}

std::shared_ptr<ASTNode> Parser::parseBlock() {
    expect(TokenType::LEFT_BRACE);
    
    auto block = std::make_shared<Statement>();
    block->line = currentToken().line;
    
    while (currentToken().type != TokenType::RIGHT_BRACE &&
           currentToken().type != TokenType::END_OF_FILE) {
        while (match(TokenType::NEWLINE)) {}
        if (currentToken().type == TokenType::RIGHT_BRACE) break;
        parseStatement();
    }
    
    expect(TokenType::RIGHT_BRACE);
    return block;
}

std::shared_ptr<ASTNode> Parser::parseIfStatement() {
    expect(TokenType::IF);
    expect(TokenType::LEFT_PAREN);
    auto condition = parseExpression();
    expect(TokenType::RIGHT_PAREN);
    auto thenBranch = parseStatement();
    
    if (match(TokenType::ELSE)) {
        auto elseBranch = parseStatement();
    }
    
    return thenBranch;
}

std::shared_ptr<ASTNode> Parser::parseWhileStatement() {
    expect(TokenType::WHILE);
    expect(TokenType::LEFT_PAREN);
    auto condition = parseExpression();
    expect(TokenType::RIGHT_PAREN);
    auto body = parseStatement();
    
    return body;
}

std::shared_ptr<ASTNode> Parser::parseForStatement() {
    expect(TokenType::FOR);
    expect(TokenType::LEFT_PAREN);
    
    auto init = (currentToken().type != TokenType::SEMICOLON) ? 
                parseExpressionStatement() : nullptr;
    expect(TokenType::SEMICOLON);
    
    auto condition = (currentToken().type != TokenType::SEMICOLON) ? 
                     parseExpression() : nullptr;
    expect(TokenType::SEMICOLON);
    
    auto increment = (currentToken().type != TokenType::RIGHT_PAREN) ? 
                     parseExpression() : nullptr;
    expect(TokenType::RIGHT_PAREN);
    
    auto body = parseStatement();
    
    return body;
}

std::shared_ptr<ASTNode> Parser::parseReturnStatement() {
    expect(TokenType::RETURN);
    
    std::shared_ptr<ASTNode> value = nullptr;
    if (!isStatementEnd()) {
        value = parseExpression();
    }
    
    match(TokenType::SEMICOLON, TokenType::NEWLINE);
    
    return std::make_shared<Statement>();
}

std::shared_ptr<ASTNode> Parser::parseSwitchStatement() {
    expect(TokenType::SWITCH);
    expect(TokenType::LEFT_PAREN);
    auto expr = parseExpression();
    expect(TokenType::RIGHT_PAREN);
    expect(TokenType::LEFT_BRACE);
    
    auto switchStmt = std::make_shared<Statement>();
    switchStmt->line = currentToken().line;
    
    while (currentToken().type != TokenType::RIGHT_BRACE &&
           currentToken().type != TokenType::END_OF_FILE) {
        while (match(TokenType::NEWLINE)) {}
        
        if (currentToken().type == TokenType::CASE) {
            expect(TokenType::CASE);
            auto caseExpr = parseExpression();
            expect(TokenType::COLON);
            
            while (currentToken().type != TokenType::CASE &&
                   currentToken().type != TokenType::DEFAULT &&
                   currentToken().type != TokenType::RIGHT_BRACE &&
                   currentToken().type != TokenType::END_OF_FILE) {
                while (match(TokenType::NEWLINE)) {}
                if (currentToken().type == TokenType::CASE ||
                    currentToken().type == TokenType::DEFAULT ||
                    currentToken().type == TokenType::RIGHT_BRACE) break;
                parseStatement();
            }
        } else if (currentToken().type == TokenType::DEFAULT) {
            expect(TokenType::DEFAULT);
            expect(TokenType::COLON);
            
            while (currentToken().type != TokenType::CASE &&
                   currentToken().type != TokenType::DEFAULT &&
                   currentToken().type != TokenType::RIGHT_BRACE &&
                   currentToken().type != TokenType::END_OF_FILE) {
                while (match(TokenType::NEWLINE)) {}
                if (currentToken().type == TokenType::CASE ||
                    currentToken().type == TokenType::DEFAULT ||
                    currentToken().type == TokenType::RIGHT_BRACE) break;
                parseStatement();
            }
        } else {
            break;
        }
    }
    
    expect(TokenType::RIGHT_BRACE);
    return switchStmt;
}

std::shared_ptr<ASTNode> Parser::parseGotoStatement() {
    expect(TokenType::GOTO);
    
    if (currentToken().type != TokenType::IDENTIFIER) {
        error("Expected label after 'goto'");
    }
    
    std::string label = currentToken().value;
    advance();
    
    match(TokenType::SEMICOLON, TokenType::NEWLINE);
    
    auto gotoStmt = std::make_shared<Statement>();
    gotoStmt->line = currentToken().line;
    return gotoStmt;
}

std::shared_ptr<ASTNode> Parser::parseExpressionStatement() {
    auto expr = parseExpression();
    
    if (!isStatementEnd() && currentToken().type != TokenType::RIGHT_BRACE) {
        // Allow missing semicolon in Extended C++
    }
    
    match(TokenType::SEMICOLON, TokenType::NEWLINE);
    
    return std::make_shared<Statement>();
}

std::shared_ptr<ASTNode> Parser::parseExpression() {
    return parseAssignment();
}

std::shared_ptr<ASTNode> Parser::parseAssignment() {
    auto expr = parseLogicalOr();
    
    if (match(TokenType::ASSIGN)) {
        auto right = parseAssignment();
    }
    
    return expr;
}

std::shared_ptr<ASTNode> Parser::parseLogicalOr() {
    auto expr = parseLogicalAnd();
    
    while (match(TokenType::LOGICAL_OR)) {
        advance();
        auto right = parseLogicalAnd();
    }
    
    return expr;
}

std::shared_ptr<ASTNode> Parser::parseLogicalAnd() {
    auto expr = parseEquality();
    
    while (match(TokenType::LOGICAL_AND)) {
        advance();
        auto right = parseEquality();
    }
    
    return expr;
}

std::shared_ptr<ASTNode> Parser::parseEquality() {
    auto expr = parseComparison();
    
    while (currentToken().type == TokenType::EQUAL ||
           currentToken().type == TokenType::NOT_EQUAL) {
        advance();
        auto right = parseComparison();
    }
    
    return expr;
}

std::shared_ptr<ASTNode> Parser::parseComparison() {
    auto expr = parseShift();
    
    while (currentToken().type == TokenType::LESS_THAN ||
           currentToken().type == TokenType::GREATER_THAN ||
           currentToken().type == TokenType::LESS_EQUAL ||
           currentToken().type == TokenType::GREATER_EQUAL) {
        advance();
        auto right = parseShift();
    }
    
    return expr;
}

std::shared_ptr<ASTNode> Parser::parseShift() {
    auto expr = parseTerm();
    
    while (currentToken().type == TokenType::SHIFT_LEFT ||
           currentToken().type == TokenType::SHIFT_RIGHT ||
           currentToken().type == TokenType::STREAM_OUT ||
           currentToken().type == TokenType::STREAM_IN) {
        advance();
        auto right = parseTerm();
    }
    
    return expr;
}

std::shared_ptr<ASTNode> Parser::parseTerm() {
    auto expr = parseFactor();
    
    while (currentToken().type == TokenType::PLUS ||
           currentToken().type == TokenType::MINUS) {
        advance();
        auto right = parseFactor();
    }
    
    return expr;
}

std::shared_ptr<ASTNode> Parser::parseFactor() {
    auto expr = parseUnary();
    
    while (currentToken().type == TokenType::MULTIPLY ||
           currentToken().type == TokenType::DIVIDE ||
           currentToken().type == TokenType::MODULO) {
        advance();
        auto right = parseUnary();
    }
    
    return expr;
}

std::shared_ptr<ASTNode> Parser::parseUnary() {
    if (currentToken().type == TokenType::LOGICAL_NOT ||
        currentToken().type == TokenType::MINUS ||
        currentToken().type == TokenType::MULTIPLY ||
        currentToken().type == TokenType::AMPERSAND ||
        currentToken().type == TokenType::SIZEOF) {
        advance();
        return parseUnary();
    }
    return parsePostfix();
}

std::shared_ptr<ASTNode> Parser::parsePostfix() {
    auto expr = parsePrimary();
    
    while (true) {
        if (match(TokenType::LEFT_BRACKET)) {
            parseExpression();
            expect(TokenType::RIGHT_BRACKET);
        } else if (match(TokenType::DOT)) {
            expect(TokenType::IDENTIFIER);
        } else if (match(TokenType::ARROW)) {
            expect(TokenType::IDENTIFIER);
        } else {
            break;
        }
    }
    
    return expr;
}

std::shared_ptr<ASTNode> Parser::parsePrimary() {
    if (currentToken().type == TokenType::IDENTIFIER ||
        currentToken().type == TokenType::INTEGER ||
        currentToken().type == TokenType::FLOAT ||
        currentToken().type == TokenType::STRING ||
        currentToken().type == TokenType::TRUE_KW ||
        currentToken().type == TokenType::FALSE_KW ||
        currentToken().type == TokenType::COUT ||
        currentToken().type == TokenType::CIN ||
        currentToken().type == TokenType::THIS) {
        advance();
        return std::make_shared<Expression>();
    }
    
    if (match(TokenType::NULL_KW, TokenType::NULLPTR)) {
        return std::make_shared<Expression>();
    }
    
    if (match(TokenType::LEFT_PAREN)) {
        auto expr = parseExpression();
        expect(TokenType::RIGHT_PAREN);
        return expr;
    }
    
    return std::make_shared<Expression>();
}

// Missing method implementations
const std::vector<std::string>& Parser::getErrors() const {
    return errors;
}

bool Parser::hasErrors() const {
    return !errors.empty();
}
