#include "Parser.h"
#include <stdexcept>
#include <iostream>

void Program::accept(ASTVisitor* v)      { v->visit(this); }
void Declaration::accept(ASTVisitor* v)  { v->visit(this); }
void VariableDecl::accept(ASTVisitor* v) { v->visit(this); }
void FunctionDecl::accept(ASTVisitor* v) { v->visit(this); }
void Statement::accept(ASTVisitor* v)    { v->visit(this); }
void Expression::accept(ASTVisitor* v)   { v->visit(this); }

Parser::Parser(const std::vector<Token>& tokens)
    : tokens(tokens), position(0) {}
Parser::~Parser() {}

Token Parser::currentToken() {
    if (position >= tokens.size()) return Token(TokenType::END_OF_FILE,"",0,0);
    return tokens[position];
}
Token Parser::peekToken() {
    if (position+1 >= tokens.size()) return Token(TokenType::END_OF_FILE,"",0,0);
    return tokens[position+1];
}
void Parser::advance() { if (position < tokens.size()) position++; }

void Parser::expect(TokenType type) {
    if (currentToken().type != type)
        error("Expected " + Token(type,"",0,0).getTokenTypeString() +
              " but got " + currentToken().getTokenTypeString());
    advance();
}
bool Parser::match(TokenType type) {
    if (currentToken().type == type) { advance(); return true; } return false;
}
bool Parser::match(TokenType t1, TokenType t2) {
    if (currentToken().type == t1 || currentToken().type == t2) { advance(); return true; } return false;
}
bool Parser::matchStatementEnd() {
    // System 6: Match any statement-ending token (semicolon, newline, or virtual STMT_END)
    if (currentToken().type == TokenType::SEMICOLON || 
        currentToken().type == TokenType::NEWLINE ||
        currentToken().type == TokenType::STMT_END) { 
        advance(); 
        return true; 
    } 
    return false;
}
void Parser::error(const std::string& message) {
    std::string msg = "Parse error at line " + std::to_string(currentToken().line) +
                      ", column " + std::to_string(currentToken().column) + ": " + message;
    errors.push_back(msg);
    throw std::runtime_error(msg);
}
void Parser::synchronize() {
    advance();
    while (currentToken().type != TokenType::END_OF_FILE) {
        if (currentToken().type == TokenType::SEMICOLON || 
            currentToken().type == TokenType::STMT_END) { 
            advance(); 
            return; 
        }
        switch (currentToken().type) {
            case TokenType::IF: case TokenType::WHILE:
            case TokenType::FOR: case TokenType::RETURN: return;
            default: break;
        }
        advance();
    }
}
bool Parser::isStatementEnd() {
    return currentToken().type == TokenType::SEMICOLON ||
           currentToken().type == TokenType::STMT_END   ||
           currentToken().type == TokenType::NEWLINE   ||
           currentToken().type == TokenType::RIGHT_BRACE||
           currentToken().type == TokenType::END_OF_FILE;
}
std::shared_ptr<Program> Parser::parse() { return parseProgram(); }

std::shared_ptr<Program> Parser::parseProgram() {
    auto program = std::make_shared<Program>();
    while (currentToken().type != TokenType::END_OF_FILE) {
        size_t beforeLoopPos = position;
        while (match(TokenType::NEWLINE) || match(TokenType::STMT_END)) {}
        if (currentToken().type == TokenType::END_OF_FILE) break;
        try {
            if (currentToken().type == TokenType::INCLUDE ||
                currentToken().type == TokenType::DEFINE  ||
                currentToken().type == TokenType::IFNDEF  ||
                currentToken().type == TokenType::ENDIF) {
                skipPreprocessor(); continue;
            }
            auto decl = parseDeclaration();
            if (decl) program->declarations.push_back(decl);
        } catch (const std::exception&) { synchronize(); }

        // Safety net: never allow non-advancing loops on malformed input.
        if (position == beforeLoopPos && currentToken().type != TokenType::END_OF_FILE) {
            errors.push_back("Parse recovery warning at line " +
                             std::to_string(currentToken().line) + ", column " +
                             std::to_string(currentToken().column) +
                             ": forced token advance to avoid parser stall.");
            advance();
        }
    }
    return program;
}

std::shared_ptr<ASTNode> Parser::skipPreprocessor() {
    while (currentToken().type != TokenType::NEWLINE &&
           currentToken().type != TokenType::END_OF_FILE) advance();
    match(TokenType::NEWLINE);
    return std::make_shared<Statement>();
}

std::shared_ptr<ASTNode> Parser::parseDeclaration() {
    while (match(TokenType::NEWLINE) || match(TokenType::STMT_END)) {}
    if (currentToken().type == TokenType::CLASS || currentToken().type == TokenType::STRUCT)
        return parseClassDecl();
    if (currentToken().type == TokenType::NAMESPACE)
        return parseNamespaceDecl();
    if (currentToken().type == TokenType::TEMPLATE) {
        advance();
        if (match(TokenType::LESS_THAN)) {
            int depth = 1;
            while (depth > 0 && currentToken().type != TokenType::END_OF_FILE) {
                if (currentToken().type == TokenType::LESS_THAN) depth++;
                if (currentToken().type == TokenType::GREATER_THAN) depth--;
                advance();
            }
        }
        return parseDeclaration();
    }
    if (currentToken().type == TokenType::LET) return parseLetDecl();
    if (currentToken().type == TokenType::PUBLIC ||
        currentToken().type == TokenType::PRIVATE ||
        currentToken().type == TokenType::PROTECTED) {
        advance();
        if (match(TokenType::COLON)) return std::make_shared<Statement>();
    }
    return parseFunctionOrVariable();
}

std::shared_ptr<ASTNode> Parser::parseClassDecl() {
    advance();
    auto cls = std::make_shared<Statement>();
    cls->line = currentToken().line;
    if (currentToken().type == TokenType::IDENTIFIER) advance();
    if (match(TokenType::COLON))
        while (currentToken().type != TokenType::LEFT_BRACE &&
               currentToken().type != TokenType::END_OF_FILE) advance();
    if (match(TokenType::LEFT_BRACE)) {
        int d = 1;
        while (d > 0 && currentToken().type != TokenType::END_OF_FILE) {
            if (currentToken().type == TokenType::LEFT_BRACE)  d++;
            if (currentToken().type == TokenType::RIGHT_BRACE) d--;
            advance();
        }
    }
    match(TokenType::SEMICOLON, TokenType::NEWLINE);
    return cls;
}

std::shared_ptr<ASTNode> Parser::parseNamespaceDecl() {
    expect(TokenType::NAMESPACE);
    auto ns = std::make_shared<Statement>();
    ns->line = currentToken().line;
    if (currentToken().type == TokenType::IDENTIFIER) advance();
    if (match(TokenType::LEFT_BRACE)) {
        int d = 1;
        while (d > 0 && currentToken().type != TokenType::END_OF_FILE) {
            if (currentToken().type == TokenType::LEFT_BRACE)  d++;
            if (currentToken().type == TokenType::RIGHT_BRACE) d--;
            advance();
        }
    }
    return ns;
}

std::shared_ptr<ASTNode> Parser::parseFunctionOrVariable() {
    while (currentToken().type == TokenType::VIRTUAL ||
           currentToken().type == TokenType::STATIC) advance();
    size_t la = position;
    if (la < tokens.size() && tokens[la].type == TokenType::SAFE) la++;
    if (la < tokens.size() && tokens[la].type == TokenType::NULLABLE) la++;
    while (la < tokens.size() &&
           (tokens[la].type == TokenType::SIGNED   ||
            tokens[la].type == TokenType::UNSIGNED ||
            tokens[la].type == TokenType::SHORT    ||
            tokens[la].type == TokenType::LONG     ||
            tokens[la].type == TokenType::CONST)) la++;
    if (la < tokens.size() &&
        (tokens[la].type == TokenType::IDENTIFIER ||
         tokens[la].type == TokenType::INT        ||
         tokens[la].type == TokenType::FLOAT_KW   ||
         tokens[la].type == TokenType::DOUBLE     ||
         tokens[la].type == TokenType::CHAR       ||
         tokens[la].type == TokenType::BOOL       ||
         tokens[la].type == TokenType::VOID       ||
         tokens[la].type == TokenType::AUTO)) {
        la++;
        while (la < tokens.size() && tokens[la].type == TokenType::COLON) {
            la++;
            if (la < tokens.size() && tokens[la].type == TokenType::COLON) la++;
            if (la < tokens.size() && tokens[la].type == TokenType::IDENTIFIER) la++;
        }
        if (la < tokens.size() && tokens[la].type == TokenType::LESS_THAN) {
            la++; int depth=1;
            while (depth>0 && la<tokens.size()) {
                if (tokens[la].type==TokenType::LESS_THAN) depth++;
                if (tokens[la].type==TokenType::GREATER_THAN) depth--;
                la++;
            }
        }
        while (la < tokens.size() &&
               (tokens[la].type==TokenType::MULTIPLY ||
                tokens[la].type==TokenType::AMPERSAND)) la++;
    }
    if (la < tokens.size() && tokens[la].type == TokenType::IDENTIFIER) {
        la++;
        if (la < tokens.size() && tokens[la].type == TokenType::LEFT_PAREN)
            return parseFunctionDecl();
    }
    return parseVariableDecl();
}

std::shared_ptr<ASTNode> Parser::parseFunctionDecl() {
    auto func = std::make_shared<FunctionDecl>();
    func->line   = currentToken().line;
    func->column = currentToken().column;
    func->isSafe = false; func->isNullable = false;
    match(TokenType::VIRTUAL);
    func->type = currentToken().value; advance();
    if (currentToken().type != TokenType::IDENTIFIER) error("Expected function name");
    func->name = currentToken().value; advance();
    expect(TokenType::LEFT_PAREN);
    while (currentToken().type != TokenType::RIGHT_PAREN &&
           currentToken().type != TokenType::END_OF_FILE) {
        std::string pt = currentToken().value; advance();
        if (currentToken().type != TokenType::IDENTIFIER) error("Expected parameter name");
        std::string pn = currentToken().value; advance();
        func->parameters.push_back({pt, pn});
        if (match(TokenType::COMMA)) {}
        else if (currentToken().type != TokenType::RIGHT_PAREN)
            error("Expected ',' or ')'");
    }
    expect(TokenType::RIGHT_PAREN);
    match(TokenType::OVERRIDE); match(TokenType::FINAL);
    if (match(TokenType::COLON))
        while (currentToken().type != TokenType::LEFT_BRACE &&
               currentToken().type != TokenType::END_OF_FILE) advance();
    if (match(TokenType::LEFT_BRACE)) {
        // Parse statements inside function body so syntax errors are reported
        // with accurate line/column instead of silently skipping the block.
        while (currentToken().type != TokenType::RIGHT_BRACE &&
               currentToken().type != TokenType::END_OF_FILE) {
            size_t beforeLoopPos = position;
            while (match(TokenType::NEWLINE) || match(TokenType::STMT_END)) {}
            if (currentToken().type == TokenType::RIGHT_BRACE ||
                currentToken().type == TokenType::END_OF_FILE) {
                break;
            }
            try {
                parseStatement();
            } catch (const std::exception&) {
                synchronize();
            }
            if (position == beforeLoopPos && currentToken().type != TokenType::END_OF_FILE) {
                errors.push_back("Parse recovery warning at line " +
                                 std::to_string(currentToken().line) + ", column " +
                                 std::to_string(currentToken().column) +
                                 ": forced token advance in function body to avoid parser stall.");
                advance();
            }
        }
        expect(TokenType::RIGHT_BRACE);
    } else {
        if (!matchStatementEnd()) {
            error("Expected function body or statement terminator after function declaration");
        }
    }
    return func;
}

std::shared_ptr<ASTNode> Parser::parseVariableDecl() {
    auto vd = std::make_shared<VariableDecl>();
    vd->line = currentToken().line; vd->column = currentToken().column;
    vd->isSafe    = match(TokenType::SAFE);
    vd->isNullable= match(TokenType::NULLABLE);
    while (currentToken().type==TokenType::SIGNED  ||
           currentToken().type==TokenType::UNSIGNED ||
           currentToken().type==TokenType::SHORT    ||
           currentToken().type==TokenType::LONG     ||
           currentToken().type==TokenType::CONST) advance();
    if (currentToken().type==TokenType::IDENTIFIER ||
        currentToken().type==TokenType::INT        ||
        currentToken().type==TokenType::FLOAT_KW   ||
        currentToken().type==TokenType::DOUBLE     ||
        currentToken().type==TokenType::CHAR       ||
        currentToken().type==TokenType::BOOL       ||
        currentToken().type==TokenType::VOID       ||
        currentToken().type==TokenType::AUTO       ||
        currentToken().type==TokenType::WCHAR_T    ||
        currentToken().type==TokenType::CHAR16_T   ||
        currentToken().type==TokenType::CHAR32_T   ||
        currentToken().type==TokenType::CHAR8_T) {
        vd->type = currentToken().value; advance();
        while (currentToken().type==TokenType::COLON) {
            advance();
            if (currentToken().type==TokenType::COLON) advance();
            if (currentToken().type==TokenType::IDENTIFIER) { vd->type += "::" + currentToken().value; advance(); }
        }
        if (currentToken().type==TokenType::LESS_THAN) {
            vd->type += "<"; advance(); int d=1;
            while (d>0 && currentToken().type!=TokenType::END_OF_FILE) {
                if (currentToken().type==TokenType::LESS_THAN) d++;
                if (currentToken().type==TokenType::GREATER_THAN) d--;
                vd->type += currentToken().value; advance();
            }
        }
        while (currentToken().type==TokenType::MULTIPLY ||
               currentToken().type==TokenType::AMPERSAND) { vd->type += currentToken().value; advance(); }
    } else { while (!isStatementEnd() && currentToken().type!=TokenType::END_OF_FILE) advance(); matchStatementEnd(); return vd; }
    if (currentToken().type==TokenType::IDENTIFIER) { vd->name=currentToken().value; advance(); }
    else { while (!isStatementEnd() && currentToken().type!=TokenType::END_OF_FILE) advance(); matchStatementEnd(); return vd; }
    if (match(TokenType::ASSIGN)) { vd->initializer = parseExpression(); }
    else if (match(TokenType::LEFT_BRACE)) { int d=1; while (d>0 && currentToken().type!=TokenType::END_OF_FILE) { if (currentToken().type==TokenType::LEFT_BRACE) d++; if (currentToken().type==TokenType::RIGHT_BRACE) d--; advance(); } }
    else if (match(TokenType::LEFT_PAREN)) { int d=1; while (d>0 && currentToken().type!=TokenType::END_OF_FILE) { if (currentToken().type==TokenType::LEFT_PAREN) d++; if (currentToken().type==TokenType::RIGHT_PAREN) d--; advance(); } }

    // Reject malformed declarations such as: int b,10;
    if (currentToken().type == TokenType::COMMA) {
        error("Invalid declarator after ',' in variable declaration");
    }

    if (!matchStatementEnd()) {
        error("Expected statement terminator ';' or newline after variable declaration");
    }
    return vd;
}

std::shared_ptr<ASTNode> Parser::parseLetDecl() {
    auto ld = std::make_shared<VariableDecl>();
    ld->line = currentToken().line; ld->column = currentToken().column;
    expect(TokenType::LET);
    if (currentToken().type != TokenType::IDENTIFIER) error("Expected identifier after 'let'");
    ld->name = currentToken().value; advance();
    expect(TokenType::ASSIGN);
    ld->initializer = parseExpression();
    ld->type = "auto";
    matchStatementEnd();  // System 6: Handle STMT_END, SEMICOLON, or NEWLINE
    return ld;
}

std::shared_ptr<ASTNode> Parser::parseIncludeStmt() { return std::make_shared<Statement>(); }

std::shared_ptr<ASTNode> Parser::parseStatement() {
    while (match(TokenType::NEWLINE) || match(TokenType::STMT_END)) {}

    // Allow declarations inside blocks/functions.
    if (currentToken().type == TokenType::LET ||
        currentToken().type == TokenType::INT ||
        currentToken().type == TokenType::FLOAT_KW ||
        currentToken().type == TokenType::DOUBLE ||
        currentToken().type == TokenType::CHAR ||
        currentToken().type == TokenType::BOOL ||
        currentToken().type == TokenType::VOID ||
        currentToken().type == TokenType::AUTO ||
        currentToken().type == TokenType::SIGNED ||
        currentToken().type == TokenType::UNSIGNED ||
        currentToken().type == TokenType::SHORT ||
        currentToken().type == TokenType::LONG ||
        currentToken().type == TokenType::CONST ||
        currentToken().type == TokenType::STATIC) {
        return parseDeclaration();
    }

    switch (currentToken().type) {
        case TokenType::LEFT_BRACE: return parseBlock();
        case TokenType::IF:         return parseIfStatement();
        case TokenType::WHILE:      return parseWhileStatement();
        case TokenType::FOR:        return parseForStatement();
        case TokenType::SWITCH:     return parseSwitchStatement();
        case TokenType::RETURN:     return parseReturnStatement();
        case TokenType::GOTO:       return parseGotoStatement();
        case TokenType::BREAK:      advance(); matchStatementEnd(); return std::make_shared<Statement>();
        case TokenType::CONTINUE:   advance(); matchStatementEnd(); return std::make_shared<Statement>();
        default: return parseExpressionStatement();
    }
}

std::shared_ptr<ASTNode> Parser::parseBlock() {
    expect(TokenType::LEFT_BRACE);
    auto blk = std::make_shared<Statement>(); blk->line = currentToken().line;
    while (currentToken().type != TokenType::RIGHT_BRACE && currentToken().type != TokenType::END_OF_FILE) {
        size_t beforeLoopPos = position;
        while (match(TokenType::NEWLINE)) {}
        if (currentToken().type == TokenType::RIGHT_BRACE) break;
        parseStatement();
        if (position == beforeLoopPos && currentToken().type != TokenType::END_OF_FILE) {
            errors.push_back("Parse recovery warning at line " +
                             std::to_string(currentToken().line) + ", column " +
                             std::to_string(currentToken().column) +
                             ": forced token advance inside block to avoid parser stall.");
            advance();
        }
    }
    expect(TokenType::RIGHT_BRACE);
    return blk;
}

std::shared_ptr<ASTNode> Parser::parseIfStatement() {
    expect(TokenType::IF); expect(TokenType::LEFT_PAREN);
    parseExpression(); expect(TokenType::RIGHT_PAREN);
    auto tb = parseStatement();
    if (match(TokenType::ELSE)) parseStatement();
    return tb;
}

std::shared_ptr<ASTNode> Parser::parseWhileStatement() {
    expect(TokenType::WHILE); expect(TokenType::LEFT_PAREN);
    parseExpression(); expect(TokenType::RIGHT_PAREN);
    return parseStatement();
}

std::shared_ptr<ASTNode> Parser::parseForStatement() {
    expect(TokenType::FOR); expect(TokenType::LEFT_PAREN);
    if (currentToken().type != TokenType::SEMICOLON) parseExpressionStatement();
    expect(TokenType::SEMICOLON);
    if (currentToken().type != TokenType::SEMICOLON) parseExpression();
    expect(TokenType::SEMICOLON);
    if (currentToken().type != TokenType::RIGHT_PAREN) parseExpression();
    expect(TokenType::RIGHT_PAREN);
    return parseStatement();
}

std::shared_ptr<ASTNode> Parser::parseReturnStatement() {
    expect(TokenType::RETURN);
    if (!isStatementEnd()) parseExpression();
    matchStatementEnd();
    return std::make_shared<Statement>();
}

std::shared_ptr<ASTNode> Parser::parseSwitchStatement() {
    expect(TokenType::SWITCH); expect(TokenType::LEFT_PAREN);
    parseExpression(); expect(TokenType::RIGHT_PAREN); expect(TokenType::LEFT_BRACE);
    auto sw = std::make_shared<Statement>(); sw->line = currentToken().line;
    while (currentToken().type != TokenType::RIGHT_BRACE && currentToken().type != TokenType::END_OF_FILE) {
        while (match(TokenType::NEWLINE)) {}
        if (currentToken().type == TokenType::CASE) {
            expect(TokenType::CASE); parseExpression(); expect(TokenType::COLON);
            while (currentToken().type != TokenType::CASE && currentToken().type != TokenType::DEFAULT &&
                   currentToken().type != TokenType::RIGHT_BRACE && currentToken().type != TokenType::END_OF_FILE) {
                while (match(TokenType::NEWLINE)) {}
                if (currentToken().type==TokenType::CASE||currentToken().type==TokenType::DEFAULT||currentToken().type==TokenType::RIGHT_BRACE) break;
                parseStatement();
            }
        } else if (currentToken().type == TokenType::DEFAULT) {
            expect(TokenType::DEFAULT); expect(TokenType::COLON);
            while (currentToken().type != TokenType::CASE && currentToken().type != TokenType::DEFAULT &&
                   currentToken().type != TokenType::RIGHT_BRACE && currentToken().type != TokenType::END_OF_FILE) {
                while (match(TokenType::NEWLINE)) {}
                if (currentToken().type==TokenType::CASE||currentToken().type==TokenType::DEFAULT||currentToken().type==TokenType::RIGHT_BRACE) break;
                parseStatement();
            }
        } else break;
    }
    expect(TokenType::RIGHT_BRACE);
    return sw;
}

std::shared_ptr<ASTNode> Parser::parseGotoStatement() {
    expect(TokenType::GOTO);
    if (currentToken().type != TokenType::IDENTIFIER) error("Expected label after 'goto'");
    advance(); matchStatementEnd();
    return std::make_shared<Statement>();
}

std::shared_ptr<ASTNode> Parser::parseExpressionStatement() {
    auto expr = parseExpression();
    if (!matchStatementEnd()) {
        error("Expected statement terminator ';' or newline after expression");
    }
    return std::make_shared<Statement>();
}

std::shared_ptr<ASTNode> Parser::parseExpression()  { return parseAssignment(); }

std::shared_ptr<ASTNode> Parser::parseAssignment() {
    auto e = parseLogicalOr();
    if (match(TokenType::ASSIGN)) parseAssignment();
    return e;
}

std::shared_ptr<ASTNode> Parser::parseLogicalOr() {
    auto e = parseLogicalAnd();
    while (match(TokenType::LOGICAL_OR)) { advance(); parseLogicalAnd(); }
    return e;
}

std::shared_ptr<ASTNode> Parser::parseLogicalAnd() {
    auto e = parseEquality();
    while (match(TokenType::LOGICAL_AND)) { advance(); parseEquality(); }
    return e;
}

std::shared_ptr<ASTNode> Parser::parseEquality() {
    auto e = parseComparison();
    while (currentToken().type==TokenType::EQUAL||currentToken().type==TokenType::NOT_EQUAL) { advance(); parseComparison(); }
    return e;
}

std::shared_ptr<ASTNode> Parser::parseComparison() {
    auto e = parseShift();
    while (currentToken().type==TokenType::LESS_THAN||currentToken().type==TokenType::GREATER_THAN||
           currentToken().type==TokenType::LESS_EQUAL||currentToken().type==TokenType::GREATER_EQUAL) { advance(); parseShift(); }
    return e;
}

std::shared_ptr<ASTNode> Parser::parseShift() {
    auto e = parseTerm();
    while (currentToken().type==TokenType::SHIFT_LEFT||currentToken().type==TokenType::SHIFT_RIGHT||
           currentToken().type==TokenType::STREAM_OUT||currentToken().type==TokenType::STREAM_IN) { advance(); parseTerm(); }
    return e;
}

std::shared_ptr<ASTNode> Parser::parseTerm() {
    auto e = parseFactor();
    while (currentToken().type==TokenType::PLUS||currentToken().type==TokenType::MINUS) { advance(); parseFactor(); }
    return e;
}

std::shared_ptr<ASTNode> Parser::parseFactor() {
    auto e = parseUnary();
    while (currentToken().type==TokenType::MULTIPLY||currentToken().type==TokenType::DIVIDE||currentToken().type==TokenType::MODULO) { advance(); parseUnary(); }
    return e;
}

std::shared_ptr<ASTNode> Parser::parseUnary() {
    if (currentToken().type==TokenType::LOGICAL_NOT||currentToken().type==TokenType::MINUS||
        currentToken().type==TokenType::MULTIPLY||currentToken().type==TokenType::AMPERSAND||
        currentToken().type==TokenType::SIZEOF) { advance(); return parseUnary(); }
    return parsePostfix();
}

std::shared_ptr<ASTNode> Parser::parsePostfix() {
    auto e = parsePrimary();
    while (true) {
        if (match(TokenType::LEFT_BRACKET)) { parseExpression(); expect(TokenType::RIGHT_BRACKET); }
        else if (match(TokenType::DOT))   { expect(TokenType::IDENTIFIER); }
        else if (match(TokenType::ARROW)) { expect(TokenType::IDENTIFIER); }
        else break;
    }
    return e;
}

std::shared_ptr<ASTNode> Parser::parsePrimary() {
    if (currentToken().type==TokenType::IDENTIFIER||currentToken().type==TokenType::INTEGER||
        currentToken().type==TokenType::FLOAT||currentToken().type==TokenType::STRING||
        currentToken().type==TokenType::CHAR_LITERAL||
        currentToken().type==TokenType::TRUE_KW||currentToken().type==TokenType::FALSE_KW||
        currentToken().type==TokenType::COUT||currentToken().type==TokenType::CIN||
        currentToken().type==TokenType::THIS) { advance(); return std::make_shared<Expression>(); }
    if (match(TokenType::NULL_KW, TokenType::NULLPTR)) return std::make_shared<Expression>();
    if (match(TokenType::LEFT_PAREN)) { auto e=parseExpression(); expect(TokenType::RIGHT_PAREN); return e; }
    return std::make_shared<Expression>();
}

const std::vector<std::string>& Parser::getErrors() const { return errors; }
bool Parser::hasErrors() const { return !errors.empty(); }