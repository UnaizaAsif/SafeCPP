#include "SemanticAnalyzer.h"
#include <iostream>
#include <set>
#include <filesystem>
#include <fstream>
#include <functional>
#include <unordered_set>

// ----------------------------------------------------------------
//  Construction
// ----------------------------------------------------------------
SemanticAnalyzer::SemanticAnalyzer(const std::vector<Token>& tokens,
                                   ErrorReporter&            reporter,
                                   const std::string&       filename)
    : tokens(tokens), reporter(reporter), pos(0), currentFileName(filename) {
    symTable.pushScope(); // global scope
}

// ----------------------------------------------------------------
//  Token navigation
// ----------------------------------------------------------------
const Token& SemanticAnalyzer::cur() const {
    static Token eof(TokenType::END_OF_FILE, "", 0, 0);
    if (pos >= tokens.size()) return eof;
    return tokens[pos];
}

const Token& SemanticAnalyzer::peek(int offset) const {
    static Token eof(TokenType::END_OF_FILE, "", 0, 0);
    size_t idx = pos + offset;
    if (idx >= tokens.size()) return eof;
    return tokens[idx];
}

bool SemanticAnalyzer::at(TokenType t) const { return cur().type == t; }

bool SemanticAnalyzer::atEnd() const {
    return pos >= tokens.size() || at(TokenType::END_OF_FILE);
}

void SemanticAnalyzer::advance() {
    if (!atEnd()) ++pos;
}

bool SemanticAnalyzer::match(TokenType t) {
    if (at(t)) { advance(); return true; }
    return false;
}

// ----------------------------------------------------------------
//  Classifier helpers
// ----------------------------------------------------------------
bool SemanticAnalyzer::isTypeKeyword(TokenType t) const {
    switch (t) {
        case TokenType::INT:      case TokenType::FLOAT_KW:
        case TokenType::DOUBLE:   case TokenType::CHAR:
        case TokenType::BOOL:     case TokenType::VOID:
        case TokenType::SHORT:    case TokenType::LONG:
        case TokenType::UNSIGNED: case TokenType::SIGNED:
        case TokenType::AUTO:     case TokenType::WCHAR_T:
        case TokenType::CHAR16_T: case TokenType::CHAR32_T:
        case TokenType::CHAR8_T:
            return true;
        default:
            return false;
    }
}

bool SemanticAnalyzer::isNullLiteral(TokenType t) const {
    return t == TokenType::NULLPTR || t == TokenType::NULL_KW;
}

bool SemanticAnalyzer::isPointerType(const std::string& raw) const {
    return raw.find('*') != std::string::npos;
}

// ================================================================
//  System 5: Infer type from token
// ================================================================
std::string SemanticAnalyzer::inferTypeFromToken(const Token& tok) {
    switch (tok.type) {
        case TokenType::INTEGER:
            return "int";
        case TokenType::FLOAT:
            return "double";
        case TokenType::STRING:
            return "string";
        case TokenType::CHAR_LITERAL:
            return "char";
        case TokenType::TRUE_KW:
        case TokenType::FALSE_KW:
            return "bool";
        default:
            return "unknown";
    }
}

// ----------------------------------------------------------------
//  Collect a type string (e.g. "int", "int*", "double**")
// ----------------------------------------------------------------
std::string SemanticAnalyzer::collectType() {
    std::string result;

    // Optional modifiers: const, signed, unsigned, short, long
    while (isTypeKeyword(cur().type) &&
           (cur().type == TokenType::CONST   ||
            cur().type == TokenType::SIGNED  ||
            cur().type == TokenType::UNSIGNED||
            cur().type == TokenType::SHORT   ||
            cur().type == TokenType::LONG)) {
        result += cur().value + " ";
        advance();
    }

    // Base type
    if (isTypeKeyword(cur().type) || at(TokenType::IDENTIFIER)) {
        result += cur().value;
        advance();
    }

    // Skip template parameters: vector<int>
    if (at(TokenType::LESS_THAN)) {
        int depth = 1;
        advance();
        while (depth > 0 && !atEnd()) {
            if (at(TokenType::LESS_THAN))    depth++;
            if (at(TokenType::GREATER_THAN)) depth--;
            advance();
        }
        result += "<...>";
    }

    // Pointer / reference markers
    while (at(TokenType::MULTIPLY) || at(TokenType::AMPERSAND)) {
        result += cur().value;
        advance();
    }

    return result;
}

// ----------------------------------------------------------------
//  Skip to end of statement (;  or newline at top level)
// ----------------------------------------------------------------
void SemanticAnalyzer::skipToStatementEnd() {
    while (!atEnd() &&
           !at(TokenType::SEMICOLON) &&
           !at(TokenType::NEWLINE)   &&
           !at(TokenType::RIGHT_BRACE)) {
        advance();
    }
    match(TokenType::SEMICOLON);
    match(TokenType::NEWLINE);
}

// ----------------------------------------------------------------
//  Process 'new' keyword (System 3)
// ----------------------------------------------------------------
void SemanticAnalyzer::processNew() {
    // cur() is NEW
    advance();  // consume NEW

    // Skip the type being allocated (e.g., "int", "char*", etc.)
    if (isTypeKeyword(cur().type) || at(TokenType::IDENTIFIER)) {
        advance();
    }

    // Skip any array brackets: new int[10]
    if (at(TokenType::LEFT_BRACKET)) {
        int depth = 1;
        advance();
        while (depth > 0 && !atEnd()) {
            if (at(TokenType::LEFT_BRACKET))   depth++;
            if (at(TokenType::RIGHT_BRACKET)) depth--;
            advance();
        }
    }

    // Skip any parentheses for constructor: new Type(args)
    if (at(TokenType::LEFT_PAREN)) {
        int depth = 1;
        advance();
        while (depth > 0 && !atEnd()) {
            if (at(TokenType::LEFT_PAREN))   depth++;
            if (at(TokenType::RIGHT_PAREN)) depth--;
            advance();
        }
    }
}

// ----------------------------------------------------------------
//  Process 'delete' keyword (System 3)
// ----------------------------------------------------------------
void SemanticAnalyzer::processDelete() {
    // cur() is DELETE
    int deleteLine = cur().line;
    advance();

    // Next should be the pointer name or *name
    if (at(TokenType::MULTIPLY)) {
        advance();
    }

    if (at(TokenType::IDENTIFIER)) {
        std::string varName = cur().value;
        symTable.markFreed(varName, deleteLine);
        advance();
    }

    // Skip to end of statement
    skipToStatementEnd();
}

// ----------------------------------------------------------------
//  Report all memory leaks at end of analysis (System 3)
// ----------------------------------------------------------------
void SemanticAnalyzer::reportMemoryLeaks() {
    std::vector<SymbolTable::MemoryLeak> leaks = symTable.getMemoryLeaks();

    for (const auto& leak : leaks) {
        SemanticError err;
        err.line = leak.allocLine;
        err.column = 0;  // we don't have exact column info
        err.variable = leak.varName;

        if (leak.errorType == 0) {
            // Regular memory leak
            err.kind = ErrorKind::MEMORY_LEAK;
            err.suggestion = "Add 'delete " + leak.varName + ";' to free this memory.";
        } else {
            // Loop memory leak
            err.kind = ErrorKind::LOOP_MEMORY_LEAK;
            err.suggestion = "Add 'delete " + leak.varName + ";' inside the loop to prevent accumulation.";
        }

        reporter.report(err);
    }
}

// ================================================================
//  System 4: Include Dependency Analysis
// ================================================================
void SemanticAnalyzer::analyzeIncludeDependencies() {
    // Extract all #include directives from the token stream
    extractIncludeDirectives();
    
    // Check for cycles
    reportIncludeCycles();
}

// ================================================================
//  Extract Include Directives
// ================================================================
void SemanticAnalyzer::extractIncludeDirectives() {
    auto trim = [](const std::string& s) {
        const std::string ws = " \t\r\n";
        size_t start = s.find_first_not_of(ws);
        if (start == std::string::npos) return std::string();
        size_t end = s.find_last_not_of(ws);
        return s.substr(start, end - start + 1);
    };

    auto basename = [](const std::string& path) {
        return std::filesystem::path(path).filename().string();
    };

    std::unordered_set<std::string> visitedFiles;
    std::function<void(const std::filesystem::path&)> scanFile =
        [&](const std::filesystem::path& filePath) {
            std::string canonicalKey = filePath.lexically_normal().string();
            if (visitedFiles.count(canonicalKey)) return;
            visitedFiles.insert(canonicalKey);

            std::ifstream in(filePath);
            if (!in.is_open()) return;

            std::string sourceName = basename(canonicalKey);
            std::string line;
            while (std::getline(in, line)) {
                std::string t = trim(line);
                if (t.rfind("#include", 0) != 0) continue;

                size_t q1 = t.find('"');
                size_t q2 = (q1 == std::string::npos) ? std::string::npos : t.find('"', q1 + 1);
                if (q1 == std::string::npos || q2 == std::string::npos || q2 <= q1 + 1) continue;

                std::string includeRel = t.substr(q1 + 1, q2 - q1 - 1);
                std::filesystem::path includePath = filePath.parent_path() / includeRel;
                includePath = includePath.lexically_normal();

                dependencyAnalyzer.addDependency(sourceName, basename(includePath.string()));
                scanFile(includePath);
            }
        };

    if (!currentFileName.empty()) {
        std::filesystem::path rootFile(currentFileName);
        if (std::filesystem::exists(rootFile)) {
            scanFile(rootFile);
            return;
        }
    }

    // Fallback to token-stream extraction when source file path is unavailable.
    for (size_t i = 0; i < tokens.size(); ++i) {
        const Token& tok = tokens[i];
        if (tok.type == TokenType::INCLUDE && i + 1 < tokens.size() &&
            tokens[i + 1].type == TokenType::STRING) {
            dependencyAnalyzer.addDependency(currentFileName, tokens[i + 1].value);
        }
    }
}

// ================================================================
//  Report Include Cycles
// ================================================================
void SemanticAnalyzer::reportIncludeCycles() {
    IncludeCycle cycle = dependencyAnalyzer.detectCycles();
    
    if (cycle.cycleFound && !cycle.cyclePath.empty()) {
        // Format cycle path
        std::string cyclePath;
        for (size_t i = 0; i < cycle.cyclePath.size(); ++i) {
            cyclePath += cycle.cyclePath[i];
            if (i < cycle.cyclePath.size() - 1) {
                cyclePath += " → ";
            }
        }
        
        SemanticError err;
        err.kind = ErrorKind::INCLUDE_CYCLE;
        err.line = 1;  // Report at line 1 since it's a global issue
        err.column = 0;
        err.variable = cyclePath;
        err.suggestion = cyclePath;
        
        reporter.report(err);
    }
}

// ----------------------------------------------------------------
//  Process an assignment (already past the '=')
// ----------------------------------------------------------------
void SemanticAnalyzer::processAssignment(const std::string& name,
                                         int line, int col) {
    // Decide null vs non-null for System 2
    if (isNullLiteral(cur().type) ||
        (at(TokenType::INTEGER) && cur().value == "0")) {
        symTable.markNull(name, line);
    } else if (at(TokenType::NEW)) {
        // System 3: Track memory allocation
        // Note: Loop-aware tracking disabled due to token stream issues
        symTable.markAllocated(name, cur().line, insideLoop, loopDepth);
        advance(); // consume NEW
        // Skip type
        if (isTypeKeyword(cur().type) || at(TokenType::IDENTIFIER)) {
            advance();
        }
        // Skip array brackets: new int[10]
        if (at(TokenType::LEFT_BRACKET)) {
            int depth = 1;
            advance();
            while (depth > 0 && !atEnd()) {
                if (at(TokenType::LEFT_BRACKET))  depth++;
                if (at(TokenType::RIGHT_BRACKET)) depth--;
                advance();
            }
        }
        return;  // don't process further
    } else {
        symTable.markNonNull(name, line);
    }

    // Always mark initialized for System 1
    symTable.markInitialized(name, line);
}

// ----------------------------------------------------------------
//  Process a use of an identifier (not as assignment target)
// ----------------------------------------------------------------
void SemanticAnalyzer::processUse(const std::string& name,
                                  int line, int col) {
    // System 1 – check initialization
    if (!symTable.checkInitialized(name, line)) {
        SymbolEntry* e = symTable.lookup(name);
        if (e) { // declared but not initialized
            SemanticError err;
            err.kind       = ErrorKind::UNINITIALIZED_USE;
            err.variable   = name;
            err.line       = line;
            err.column     = col;
            err.suggestion = "Initialize variable before use (e.g., " +
                             e->type + " " + name + " = 0;)";
            reporter.report(err);
        }
    }
}

// ----------------------------------------------------------------
//  Process a dereference  *<id>
// ----------------------------------------------------------------
void SemanticAnalyzer::processDereference() {
    // cur() is MULTIPLY at entry
    advance(); // consume *

    if (!at(TokenType::IDENTIFIER)) return;

    std::string name = cur().value;
    int         line = cur().line;
    int         col  = cur().column;
    advance();

    SymbolEntry* e = symTable.lookup(name);
    if (!e || !e->isPointer) return;

    // System 1 – pointer itself uninitialized?
    if (e->initState == InitState::UNINITIALIZED) {
        SemanticError err;
        err.kind       = ErrorKind::UNINITIALIZED_USE;
        err.variable   = name;
        err.line       = line;
        err.column     = col;
        err.suggestion = "Initialize pointer before dereferencing (e.g., " +
                         e->type + " " + name + " = nullptr;)";
        reporter.report(err);
        return; // no need to also report null deref
    }

    // System 2 – null / unknown state?
    if (symTable.checkNullDereference(name)) {
        ErrorKind kind = (e->nullState == NullState::NULL_PTR)
                             ? ErrorKind::NULL_DEREF
                             : ErrorKind::MAYBE_NULL_DEREF;
        SemanticError err;
        err.kind     = kind;
        err.variable = name;
        err.line     = line;
        err.column   = col;
        if (kind == ErrorKind::NULL_DEREF) {
            err.suggestion = "Check that '" + name +
                             "' is not null before dereferencing. "
                             "Consider: if (" + name + " != nullptr) { ... }";
        } else {
            err.suggestion = "Ensure '" + name +
                             "' is assigned a valid address before use. "
                             "Consider adding a null check.";
        }
        reporter.report(err);
    }
}

// ----------------------------------------------------------------
//  Process a declaration statement
//  Pattern: <type> [*] <id> [ = <expr> ] [;]
// ----------------------------------------------------------------
void SemanticAnalyzer::processDeclaration() {
    std::string typeName = collectType();
    if (typeName.empty()) { skipToStatementEnd(); return; }

    // identifier
    if (!at(TokenType::IDENTIFIER)) { skipToStatementEnd(); return; }
    std::string varName = cur().value;
    int         declLine = cur().line;
    int         declCol  = cur().column;
    advance();

    symTable.declare(varName, typeName, declLine, declCol);

    // optional initializer
    if (match(TokenType::ASSIGN)) {
        // Determine null/non-null for System 2 from the first RHS token
        processAssignment(varName, declLine, declCol);

        // Scan the RHS expression for identifier uses (System 1 check)
        int depth = 0;
        while (!atEnd()) {
            if (at(TokenType::LEFT_PAREN) || at(TokenType::LEFT_BRACE))  depth++;
            if (at(TokenType::RIGHT_PAREN)|| at(TokenType::RIGHT_BRACE)) { if (depth == 0) break; depth--; }
            if ((at(TokenType::SEMICOLON) || at(TokenType::NEWLINE)) && depth == 0) break;

            // Check any identifiers on the RHS (but not function-call names)
            if (at(TokenType::IDENTIFIER)) {
                std::string rhsName = cur().value;
                int rhsLine = cur().line, rhsCol = cur().column;
                advance();
                // If not immediately followed by '(' (function call), check init
                if (!at(TokenType::LEFT_PAREN)) {
                    processUse(rhsName, rhsLine, rhsCol);
                }
                continue;
            }
            // Handle dereference on RHS
            if (at(TokenType::MULTIPLY) && peek().type == TokenType::IDENTIFIER) {
                processDereference();
                continue;
            }
            advance();
        }
    }

    match(TokenType::SEMICOLON);
    match(TokenType::NEWLINE);
}

// ----------------------------------------------------------------
//  Main analysis loop
// ----------------------------------------------------------------
bool SemanticAnalyzer::analyze() {
    // ---- System 4: Analyze include dependencies ----
    analyzeIncludeDependencies();
    
    while (!atEnd()) {
        if (at(TokenType::STMT_END)) {
            System6Info info;
            info.message = "Implicit statement terminator inserted";
            info.token = "STMT_END";
            info.line = cur().line;
            info.column = cur().column;
            info.print();
            advance();
            continue;
        }

        // Skip preprocessor lines, newlines, access modifiers
        if (at(TokenType::NEWLINE)   ||
            at(TokenType::SEMICOLON) ||
            at(TokenType::PUBLIC)    ||
            at(TokenType::PRIVATE)   ||
            at(TokenType::PROTECTED) ||
            at(TokenType::INCLUDE)   ||
            at(TokenType::DEFINE)    ||
            at(TokenType::IFNDEF)    ||
            at(TokenType::ENDIF)) {
            advance(); continue;
        }

        // Scope management
        if (at(TokenType::LEFT_BRACE)) {
            symTable.pushScope();
            scopeDepth++;
            advance(); continue;
        }
        if (at(TokenType::RIGHT_BRACE)) {
            symTable.popScope();

            if (scopeDepth > 0)
                scopeDepth--;

            if (insideLoop) {
                loopDepth--;

                if (loopDepth <= 0) {
                    insideLoop = false;
                    loopDepth = 0;
                }
            }

            advance();
            continue;
        }

        // ---- Loop detection for FOR, WHILE ----
        if (at(TokenType::FOR) ||
            at(TokenType::WHILE)) {

            insideLoop = true;
            loopDepth++;
            advance();
            continue;
        }

        // ---- System 5: LET declarations (type inference) ----
        if (at(TokenType::LET)) {
            advance(); // consume LET

            if (!at(TokenType::IDENTIFIER)) {
                advance();
                continue;
            }

            std::string name = cur().value;
            int line = cur().line;
            int col = cur().column;
            advance(); // consume identifier

            if (!match(TokenType::ASSIGN)) {
                SemanticError err;
                err.kind = ErrorKind::TYPE_INFERENCE_ERROR;
                err.variable = name;
                err.line = line;
                err.column = col;
                reporter.report(err);
                continue;
            }

            Token rhs = cur();
            std::string inferredType = inferTypeFromToken(rhs);

            if (inferredType == "unknown") {
                SemanticError err;
                err.kind = ErrorKind::TYPE_INFERENCE_ERROR;
                err.variable = name;
                err.line = line;
                err.column = col;
                reporter.report(err);
            }

            symTable.declare(name, inferredType, line, col);
            symTable.markInitialized(name, line);

            // Print System 5 type inference info
            System5Info info;
            info.variable = name;
            info.inferredType = inferredType;

            if (inferredType == "int")
                info.typeToken = "TYPE_INFERRED_INT";
            else if (inferredType == "float")
                info.typeToken = "TYPE_INFERRED_FLOAT";
            else if (inferredType == "double")
                info.typeToken = "TYPE_INFERRED_DOUBLE";
            else if (inferredType == "string")
                info.typeToken = "TYPE_INFERRED_STRING";
            else if (inferredType == "char")
                info.typeToken = "TYPE_INFERRED_CHAR";
            else if (inferredType == "bool")
                info.typeToken = "TYPE_INFERRED_BOOL";
            else
                info.typeToken = "TYPE_INFERRED_UNKNOWN";

            info.print();

            // Process RHS for identifier uses
            while (!atEnd() &&
                   !at(TokenType::SEMICOLON) &&
                   !at(TokenType::NEWLINE) &&
                   !at(TokenType::STMT_END)) {

                if (at(TokenType::IDENTIFIER)) {
                    processUse(cur().value, cur().line, cur().column);
                }

                advance();
            }

            match(TokenType::SEMICOLON);
            match(TokenType::NEWLINE);
            match(TokenType::STMT_END);

            continue;
        }

        // ---- Dereference: unary *  ----
        // Distinguish from multiply: unary * follows operator, (, or start
        if (at(TokenType::MULTIPLY)) {
            // Peek back: if previous meaningful token was an identifier or )
            // this is multiply, not dereference. Simplistic heuristic:
            // if next token is IDENTIFIER, treat as dereference.
            if (peek().type == TokenType::IDENTIFIER) {
                processDereference();
                continue;
            }
            advance(); continue;
        }

        // ---- Declaration: type keyword or IDENTIFIER followed by IDENTIFIER ----
        if (isTypeKeyword(cur().type)) {
            // Save position – if collectType leaves us at IDENTIFIER, it's a decl
            size_t saved = pos;
            std::string t = collectType();
            if (at(TokenType::IDENTIFIER)) {
                // It's a declaration (possibly a function – check for '(')
                if (peek().type != TokenType::LEFT_PAREN) {
                    // Rewind and let processDeclaration handle it cleanly
                    pos = saved;
                    processDeclaration();
                    continue;
                }
                // Function declaration – skip body
                pos = saved;
                skipToStatementEnd();
                continue;
            }
            // Not a decl after all – restore and continue
            pos = saved;
            advance();
            continue;
        }

        // ---- IDENTIFIER: assignment or use ----
        if (at(TokenType::IDENTIFIER)) {
            std::string name = cur().value;
            int         line = cur().line;
            int         col  = cur().column;
            advance();

            if (match(TokenType::ASSIGN)) {
                // Assignment: mark initialized / null state from first token
                processAssignment(name, line, col);
                // Scan RHS for identifier uses
                int depth = 0;
                while (!atEnd()) {
                    if (at(TokenType::LEFT_PAREN)||at(TokenType::LEFT_BRACE)) depth++;
                    if (at(TokenType::RIGHT_PAREN)||at(TokenType::RIGHT_BRACE)) { if(depth==0) break; depth--; }
                    if ((at(TokenType::SEMICOLON)||at(TokenType::NEWLINE))&&depth==0) break;
                    if (at(TokenType::IDENTIFIER)) {
                        std::string rn=cur().value; int rl=cur().line,rc=cur().column; advance();
                        if (!at(TokenType::LEFT_PAREN)) processUse(rn,rl,rc);
                        continue;
                    }
                    if (at(TokenType::MULTIPLY)&&peek().type==TokenType::IDENTIFIER) { processDereference(); continue; }
                    advance();
                }
                match(TokenType::SEMICOLON); match(TokenType::NEWLINE);
            } else {
                // Use
                processUse(name, line, col);
            }
            continue;
        }

        // ---- NEW keyword (System 3) ----
        if (at(TokenType::NEW)) {
            processNew();
            continue;
        }

        // ---- DELETE keyword (System 3) ----
        if (at(TokenType::DELETE)) {
            processDelete();
            continue;
        }

        advance(); // consume anything else
    }

    // ---- System 3: Report all memory leaks at end of analysis ----
    reportMemoryLeaks();

    return !reporter.hasErrors();
}