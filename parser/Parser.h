#ifndef PARSER_H
#define PARSER_H

#include "../lexer/Token.h"
#include <vector>
#include <memory>
#include <string>

struct ASTNode {
    int line = 0;
    int column = 0;
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
    std::string name, type;
    bool isSafe = false, isNullable = false;
    std::string getNodeType() const override { return "Declaration"; }
    void accept(class ASTVisitor* visitor) override;
};

struct VariableDecl : public Declaration {
    std::shared_ptr<ASTNode> initializer;
    std::string getNodeType() const override { return "VariableDecl"; }
    void accept(class ASTVisitor* visitor) override;
};

struct FunctionDecl : public Declaration {
    std::vector<std::pair<std::string,std::string>> parameters;
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

class Parser {
private:
    std::vector<Token> tokens;
    size_t position;
    std::vector<std::string> errors;

    Token currentToken();
    Token peekToken();
    void advance();
    void expect(TokenType type);
    bool match(TokenType type);
    bool match(TokenType type1, TokenType type2);
    void error(const std::string& message);
    void synchronize();

    std::shared_ptr<Program>  parseProgram();
    std::shared_ptr<ASTNode>  parseDeclaration();
    std::shared_ptr<ASTNode>  skipPreprocessor();
    std::shared_ptr<ASTNode>  parseClassDecl();
    std::shared_ptr<ASTNode>  parseNamespaceDecl();
    std::shared_ptr<ASTNode>  parseFunctionOrVariable();
    std::shared_ptr<ASTNode>  parseFunctionDecl();
    std::shared_ptr<ASTNode>  parseVariableDecl();
    std::shared_ptr<ASTNode>  parseLetDecl();
    std::shared_ptr<ASTNode>  parseIncludeStmt();
    std::shared_ptr<ASTNode>  parseStatement();
    std::shared_ptr<ASTNode>  parseBlock();
    std::shared_ptr<ASTNode>  parseIfStatement();
    std::shared_ptr<ASTNode>  parseWhileStatement();
    std::shared_ptr<ASTNode>  parseForStatement();
    std::shared_ptr<ASTNode>  parseSwitchStatement();
    std::shared_ptr<ASTNode>  parseGotoStatement();
    std::shared_ptr<ASTNode>  parseReturnStatement();
    std::shared_ptr<ASTNode>  parseExpressionStatement();
    std::shared_ptr<ASTNode>  parseExpression();
    std::shared_ptr<ASTNode>  parseAssignment();
    std::shared_ptr<ASTNode>  parseLogicalOr();
    std::shared_ptr<ASTNode>  parseLogicalAnd();
    std::shared_ptr<ASTNode>  parseEquality();
    std::shared_ptr<ASTNode>  parseComparison();
    std::shared_ptr<ASTNode>  parseShift();
    std::shared_ptr<ASTNode>  parseTerm();
    std::shared_ptr<ASTNode>  parseFactor();
    std::shared_ptr<ASTNode>  parseUnary();
    std::shared_ptr<ASTNode>  parsePostfix();
    std::shared_ptr<ASTNode>  parsePrimary();
    bool isStatementEnd();

public:
    Parser(const std::vector<Token>& tokens);
    ~Parser();
    std::shared_ptr<Program> parse();
    const std::vector<std::string>& getErrors() const;
    bool hasErrors() const;
};

class ASTVisitor {
public:
    virtual ~ASTVisitor() = default;
    virtual void visit(Program* node)      = 0;
    virtual void visit(Declaration* node)  = 0;
    virtual void visit(VariableDecl* node) = 0;
    virtual void visit(FunctionDecl* node) = 0;
    virtual void visit(Statement* node)    = 0;
    virtual void visit(Expression* node)   = 0;
};

#endif // PARSER_H