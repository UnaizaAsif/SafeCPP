#ifndef PARSER_H
#define PARSER_H

#include "../lexer/Token.h"
#include <vector>
#include <memory>
#include <string>

// ========== AST NODE HIERARCHY ==========

struct ASTNode {
    int line;
    int column;
    virtual ~ASTNode() = default;
    virtual std::string getNodeType() const = 0;
    virtual void accept(class ASTVisitor* visitor) = 0;
};

struct Program : public ASTNode {
    std::vector<std::shared_ptr<ASTNode>> declarations;
    std::string getNodeType() const override { return "Program"; }
    void accept(class ASTVisitor* visitor) override;
};

struct Declaration : public ASTNode {
    std::string name;
    std::string type;
    bool isSafe;
    bool isNullable;
    std::string getNodeType() const override { return "Declaration"; }
    void accept(class ASTVisitor* visitor) override;
};

struct VariableDecl : public Declaration {
    std::shared_ptr<ASTNode> initializer;
    std::string getNodeType() const override { return "VariableDecl"; }
    void accept(class ASTVisitor* visitor) override;
};

struct FunctionDecl : public Declaration {
    std::vector<std::pair<std::string, std::string>> parameters;
    std::vector<std::shared_ptr<ASTNode>> body;
    std::string getNodeType() const override { return "FunctionDecl"; }
    void accept(class ASTVisitor* visitor) override;
};

struct Statement : public ASTNode {
    std::string getNodeType() const override { return "Statement"; }
    void accept(class ASTVisitor* visitor) override;
};

struct Expression : public ASTNode {
    std::string getNodeType() const override { return "Expression"; }
    void accept(class ASTVisitor* visitor) override;
};

/**
 * Syntax Analyzer (Parser)
 * Validates grammar and builds AST
 * 
 * Grammar Rules (Extended C++ - Updated for 65 Keywords):
 * 
 * program       → declaration*
 * declaration   → classDecl | funcDecl | varDecl | includeStmt | namespaceDec
 * 
 * classDecl     → [FINAL] CLASS IDENTIFIER [inheritance] LEFT_BRACE classMember* RIGHT_BRACE
 * inheritance   → COLON accessLevel TYPE IDENTIFIER (COMMA accessLevel TYPE IDENTIFIER)*
 * accessLevel   → PUBLIC | PRIVATE | PROTECTED
 * classMember   → accessLevel COLON (varDecl | funcDecl | constructorDecl | destructorDecl)*
 * constructorDecl → [EXPLICIT] IDENTIFIER LEFT_PAREN paramList RIGHT_PAREN block
 * destructorDecl  → TILDE IDENTIFIER LEFT_PAREN RIGHT_PAREN block
 * 
 * namespaceDec  → NAMESPACE IDENTIFIER LEFT_BRACE declaration* RIGHT_BRACE
 * 
 * funcDecl      → [VIRTUAL] type IDENTIFIER LEFT_PAREN paramList RIGHT_PAREN [OVERRIDE] [FINAL] block
 * varDecl       → [SAFE] [NULLABLE] type IDENTIFIER [ASSIGN expression] [NEWLINE|SEMICOLON]
 * letDecl       → LET IDENTIFIER ASSIGN expression [NEWLINE|SEMICOLON]
 * 
 * type          → [SIGNED|UNSIGNED] [SHORT|LONG|LONG LONG] baseType [MULTIPLY|AMPERSAND]*
 * baseType      → INT | FLOAT | DOUBLE | CHAR | WCHAR_T | CHAR16_T | CHAR32_T | CHAR8_T | BOOL | VOID | IDENTIFIER
 * 
 * statement     → exprStmt | ifStmt | whileStmt | forStmt | switchStmt | returnStmt | gotoStmt | jumpStmt | block
 * jumpStmt      → (BREAK | CONTINUE) [NEWLINE|SEMICOLON]
 * gotoStmt      → GOTO IDENTIFIER [NEWLINE|SEMICOLON]
 * switchStmt    → SWITCH LEFT_PAREN expression RIGHT_PAREN LEFT_BRACE caseBlock* RIGHT_BRACE
 * caseBlock     → (CASE expression | DEFAULT) COLON statement*
 * exprStmt      → expression [NEWLINE|SEMICOLON]
 * ifStmt        → IF LEFT_PAREN expression RIGHT_PAREN statement [ELSE statement]
 * whileStmt     → WHILE LEFT_PAREN expression RIGHT_PAREN statement
 * forStmt       → FOR LEFT_PAREN exprStmt exprStmt exprStmt RIGHT_PAREN statement
 * returnStmt    → RETURN [expression] [NEWLINE|SEMICOLON]
 * block         → LEFT_BRACE statement* RIGHT_BRACE
 * 
 * expression    → assignment
 * assignment    → logicalOr [ASSIGN assignment]
 * logicalOr     → logicalAnd (LOGICAL_OR logicalAnd)*
 * logicalAnd    → equality (LOGICAL_AND equality)*
 * equality      → comparison ((EQUAL | NOT_EQUAL) comparison)*
 * comparison    → bitwise ((LESS_THAN | GREATER_THAN | LESS_EQUAL | GREATER_EQUAL) bitwise)*
 * bitwise       → shift ((BITWISE_AND | BITWISE_OR | BITWISE_XOR) shift)*
 * shift         → term ((SHIFT_LEFT | SHIFT_RIGHT) term)*
 * term          → factor ((PLUS | MINUS) factor)*
 * factor        → unary ((MULTIPLY | DIVIDE | MODULO) unary)*
 * unary         → (LOGICAL_NOT | MINUS | MULTIPLY | AMPERSAND | SIZEOF) unary | postfix
 * postfix       → primary (LEFT_BRACKET expression RIGHT_BRACKET | DOT IDENTIFIER | ARROW IDENTIFIER | STREAM_OUT | STREAM_IN)*
 * primary       → IDENTIFIER | INTEGER | FLOAT | STRING | COUT | CIN | LEFT_PAREN expression RIGHT_PAREN | NULL | NULLPTR | TRUE | FALSE | THIS
 * 
 * Operators Added:
 *   STREAM_OUT (<<) - output stream operator (cout << value)
 *   STREAM_IN  (>>) - input stream operator (cin >> value)  [NOTE: conflicts with GREATER_THAN, resolved by lookahead]
 */
class Parser {
private:
    std::vector<Token> tokens;
    size_t position;
    std::vector<std::string> errors;

    // Token navigation
    Token currentToken();
    Token peekToken();
    void advance();
    void expect(TokenType type);
    bool match(TokenType type);
    bool match(TokenType type1, TokenType type2);

    // Error handling
    void error(const std::string& message);
    void synchronize();

    // Grammar rules
    std::shared_ptr<Program> parseProgram();
    std::shared_ptr<ASTNode> parseDeclaration();
    std::shared_ptr<ASTNode> parseFunctionDecl();
    std::shared_ptr<ASTNode> parseVariableDecl();
    std::shared_ptr<ASTNode> parseLetDecl();
    std::shared_ptr<ASTNode> parseIncludeStmt();
    std::shared_ptr<ASTNode> parseStatement();
    std::shared_ptr<ASTNode> parseBlock();
    std::shared_ptr<ASTNode> parseIfStatement();
    std::shared_ptr<ASTNode> parseWhileStatement();
    std::shared_ptr<ASTNode> parseForStatement();
    std::shared_ptr<ASTNode> parseSwitchStatement();
    std::shared_ptr<ASTNode> parseGotoStatement();
    std::shared_ptr<ASTNode> parseReturnStatement();
    std::shared_ptr<ASTNode> parseExpressionStatement();

    // Expression parsing (precedence climbing)
    std::shared_ptr<ASTNode> parseExpression();
    std::shared_ptr<ASTNode> parseAssignment();
    std::shared_ptr<ASTNode> parseLogicalOr();
    std::shared_ptr<ASTNode> parseLogicalAnd();
    std::shared_ptr<ASTNode> parseEquality();
    std::shared_ptr<ASTNode> parseComparison();
    std::shared_ptr<ASTNode> parseTerm();
    std::shared_ptr<ASTNode> parseFactor();
    std::shared_ptr<ASTNode> parseUnary();
    std::shared_ptr<ASTNode> parsePostfix();
    std::shared_ptr<ASTNode> parsePrimary();

    // Semicolon handling (Extended C++ feature)
    bool isStatementEnd();

public:
    Parser(const std::vector<Token>& tokens);
    ~Parser();

    std::shared_ptr<Program> parse();
    const std::vector<std::string>& getErrors() const;
    bool hasErrors() const;
};

// ========== VISITOR PATTERN ==========
class ASTVisitor {
public:
    virtual ~ASTVisitor() = default;
    virtual void visit(Program* node) = 0;
    virtual void visit(Declaration* node) = 0;
    virtual void visit(VariableDecl* node) = 0;
    virtual void visit(FunctionDecl* node) = 0;
    virtual void visit(Statement* node) = 0;
    virtual void visit(Expression* node) = 0;
};

#endif // PARSER_H
