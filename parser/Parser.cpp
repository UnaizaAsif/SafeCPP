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
        // Skip newlines at top level
        while (match(TokenType::NEWLINE)) {}
        
        if (currentToken().type == TokenType::END_OF_FILE) break;
        
        try {
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

std::shared_ptr<ASTNode> Parser::parseDeclaration() {
    // Check for include statement
    if (match(TokenType::INCLUDE)) {
        return parseIncludeStmt();
    }
    
    // Check for let declaration
    if (currentToken().type == TokenType::LET) {
        return parseLetDecl();
    }
    
    // Check for function or variable declaration
    // Need to distinguish based on pattern
    
    // Look ahead to determine if it's a function or variable
    size_t lookAhead = position;
    
    if (currentToken().type == TokenType::SAFE) {
        lookAhead++;
    }
    if (lookAhead < tokens.size() && tokens[lookAhead].type == TokenType::NULLABLE) {
        lookAhead++;
    }
    
    // Skip type
    lookAhead++;
    
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
    
    // Parse function body
    expect(TokenType::LEFT_BRACE);
    while (currentToken().type != TokenType::RIGHT_BRACE &&
           currentToken().type != TokenType::END_OF_FILE) {
        while (match(TokenType::NEWLINE)) {}
        if (currentToken().type == TokenType::RIGHT_BRACE) break;
        auto stmt = parseStatement();
        if (stmt) {
            func->body.push_back(stmt);
        }
    }
    expect(TokenType::RIGHT_BRACE);
    
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
    
    // Parse type
    if (currentToken().type == TokenType::IDENTIFIER ||
        currentToken().type == TokenType::INT ||
        currentToken().type == TokenType::FLOAT_KW ||
        currentToken().type == TokenType::DOUBLE ||
        currentToken().type == TokenType::CHAR ||
        currentToken().type == TokenType::BOOL ||
        currentToken().type == TokenType::VOID) {
        varDecl->type = currentToken().value;
        advance();
    } else {
        error("Expected type in variable declaration");
    }
    
    // Parse variable name
    if (currentToken().type != TokenType::IDENTIFIER) {
        error("Expected variable name");
    }
    varDecl->name = currentToken().value;
    advance();
    
    // Parse optional initializer
    if (match(TokenType::ASSIGN)) {
        varDecl->initializer = parseExpression();
    }
    
    // Semicolon-free support: accept newline or semicolon or nothing
    if (!isStatementEnd()) {
        // In Extended C++, we can allow missing semicolon
        // Just continue
    } else {
        match(TokenType::SEMICOLON, TokenType::NEWLINE);
    }
    
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
    expect(TokenType::INCLUDE);
    
    if (currentToken().type != TokenType::STRING) {
        error("Expected string after #include");
    }
    
    std::string includePath = currentToken().value;
    advance();
    
    match(TokenType::SEMICOLON, TokenType::NEWLINE);
    
    auto stmt = std::make_shared<Statement>();
    stmt->line = currentToken().line;
    return stmt;
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
        case TokenType::RETURN:
            return parseReturnStatement();
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

std::shared_ptr<ASTNode> Parser::parseExpressionStatement() {
    auto expr = parseExpression();
    
    if (!isStatementEnd() && currentToken().type != TokenType::RIGHT_BRACE) {
        // Allow missing semicolon in Extended C++
    }
    match(TokenType::SEMICOLON, TokenType::NEWLINE);
    
    return expr;
}

std::shared_ptr<ASTNode> Parser::parseExpression() {
    return parseAssignment();
}

std::shared_ptr<ASTNode> Parser::parseAssignment() {
    auto expr = parseLogicalOr();
    
    if (currentToken().type == TokenType::ASSIGN) {
        advance();
        auto right = parseAssignment();
        return right;
    }
    
    return expr;
}

std::shared_ptr<ASTNode> Parser::parseLogicalOr() {
    auto expr = parseLogicalAnd();
    
    while (currentToken().type == TokenType::LOGICAL_OR) {
        advance();
        parseLogicalAnd();
    }
    
    return expr;
}

std::shared_ptr<ASTNode> Parser::parseLogicalAnd() {
    auto expr = parseEquality();
    
    while (currentToken().type == TokenType::LOGICAL_AND) {
        advance();
        parseEquality();
    }
    
    return expr;
}

std::shared_ptr<ASTNode> Parser::parseEquality() {
    auto expr = parseComparison();
    
    while (currentToken().type == TokenType::EQUAL || 
           currentToken().type == TokenType::NOT_EQUAL) {
        advance();
        parseComparison();
    }
    
    return expr;
}

std::shared_ptr<ASTNode> Parser::parseComparison() {
    auto expr = parseTerm();
    
    while (currentToken().type == TokenType::LESS_THAN ||
           currentToken().type == TokenType::GREATER_THAN ||
           currentToken().type == TokenType::LESS_EQUAL ||
           currentToken().type == TokenType::GREATER_EQUAL) {
        advance();
        parseTerm();
    }
    
    return expr;
}

std::shared_ptr<ASTNode> Parser::parseTerm() {
    auto expr = parseFactor();
    
    while (currentToken().type == TokenType::PLUS ||
           currentToken().type == TokenType::MINUS) {
        advance();
        parseFactor();
    }
    
    return expr;
}

std::shared_ptr<ASTNode> Parser::parseFactor() {
    auto expr = parseUnary();
    
    while (currentToken().type == TokenType::MULTIPLY ||
           currentToken().type == TokenType::DIVIDE ||
           currentToken().type == TokenType::MODULO) {
        advance();
        parseUnary();
    }
    
    return expr;
}

std::shared_ptr<ASTNode> Parser::parseUnary() {
    if (currentToken().type == TokenType::LOGICAL_NOT ||
        currentToken().type == TokenType::MINUS ||
        currentToken().type == TokenType::MULTIPLY ||
        currentToken().type == TokenType::AMPERSAND) {
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
        currentToken().type == TokenType::STRING) {
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
    
    error("Expected expression");
    return nullptr;
}

const std::vector<std::string>& Parser::getErrors() const {
    return errors;
}

bool Parser::hasErrors() const {
    return !errors.empty();
}
