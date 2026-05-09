#ifndef SEMANTIC_ANALYZER_H
#define SEMANTIC_ANALYZER_H

#include "../lexer/Token.h"
#include "../utils/SymbolTable.h"
#include "SemanticError.h"
#include <vector>

/**
 * SemanticAnalyzer
 * ----------------
 * Performs two safety checks by making a single pass over the token stream
 * produced by the Lexer, augmented with the symbol table maintained during
 * lexical analysis.
 *
 * System 1 – Undefined Behavior Detection
 *   Tracks every variable declaration and flags any use that occurs before
 *   an assignment.
 *
 * System 2 – Null Pointer Safety
 *   Tracks pointer declarations and their null / non-null state; flags any
 *   dereference ( *ptr ) when the pointer is null or in an unknown state.
 *
 * The class operates on the flat token vector.  It does not need the full
 * AST – recognising the simple patterns below is sufficient:
 *
 *   Declaration:   <type> <id> [= <expr>] ;
 *   Assignment:    <id> = <expr> ;
 *   Dereference:   * <id>          (as an expression)
 *   Use:           any IDENTIFIER that is not the target of = or a decl name
 */
class SemanticAnalyzer {
public:
    SemanticAnalyzer(const std::vector<Token>& tokens,
                     ErrorReporter&            reporter);

    // Run both safety systems. Returns true if no errors were found.
    bool analyze();

private:
    const std::vector<Token>& tokens;
    ErrorReporter&            reporter;
    SymbolTable               symTable;
    size_t                    pos;

    // ---- token navigation ----
    const Token& cur()  const;
    const Token& peek(int offset = 1) const;
    bool   at(TokenType t)   const;
    bool   atEnd()           const;
    void   advance();
    bool   match(TokenType t);

    // ---- helpers ----
    bool isTypeKeyword(TokenType t) const;
    bool isNullLiteral(TokenType t) const;
    bool isPointerType(const std::string& raw) const;

    // ---- analysis passes ----
    void processDeclaration();   // handles <type> [*] <id> [= ...] ;
    void processAssignment(const std::string& name, int line, int col);
    void processUse(const std::string& name, int line, int col);
    void processDereference();   // handles * <id>
    void skipToStatementEnd();

    // Collect the full type string (e.g. "int*", "double*")
    std::string collectType();
};

#endif // SEMANTIC_ANALYZER_H