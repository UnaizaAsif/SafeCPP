# SafeCPP Project Report

## 1) Language Description

SafeCPP is a C++17-based compiler front-end project that analyzes an extended subset of C++ and adds safety-oriented behavior. It combines lexical scanning, recursive-descent syntax parsing, and semantic safety checks in a single pipeline. The implementation keeps standard C++-like constructs (types, pointers, control flow, functions, includes) while introducing custom compiler behavior such as `let` type inference and semicolon-free statement termination using virtual `STMT_END` tokens.

---

## 2) Selected Functionalities (List Only + Location)

- System 1: Undefined Behavior Detection  
  📍 `semantic/SemanticAnalyzer.cpp -> processUse()`, `processDeclaration()`, `analyze()`

- System 2: Null Pointer Safety  
  📍 `semantic/SemanticAnalyzer.cpp -> processDereference()`, `processAssignment()`  
  📍 `utils/SymbolTable.cpp -> markNull()`, `markNonNull()`, `checkNullDereference()`

- System 3: Memory Leak Detection  
  📍 `semantic/SemanticAnalyzer.cpp -> processAssignment()`, `processDelete()`, `reportMemoryLeaks()`  
  📍 `utils/SymbolTable.cpp -> markAllocated()`, `markFreed()`, `getMemoryLeaks()`

- System 4: Include Dependency Analysis  
  📍 `semantic/SemanticAnalyzer.cpp -> analyzeIncludeDependencies()`, `extractIncludeDirectives()`, `reportIncludeCycles()`  
  📍 `analyzer/IncludeDependencyAnalyzer.cpp -> addDependency()`, `detectCycles()`, `dfs()`

- System 5: Type Inference with `let`  
  📍 `semantic/SemanticAnalyzer.cpp -> inferTypeFromToken()`, `analyze()` (LET block)  
  📍 `semantic/SemanticError.h -> System5Info::print()`

- System 6: Semicolon-Free Syntax Support  
  📍 `lexer/Lexer.cpp -> tokenize()`, `canEndStatement()`, `isStatementBoundary()`  
  📍 `parser/Parser.cpp -> matchStatementEnd()`  
  📍 `semantic/SemanticAnalyzer.cpp -> analyze()` (`STMT_END` reporting)

- Lexer, Parser, Symbol Table, Error Reporter integration  
  📍 `main.cpp -> runPipeline()`

---

## 3) Regular Expressions / Rules (Per Functionality + Location)

## 3.1 Lexical Rules

- Identifier: `[A-Za-z_][A-Za-z0-9_]*`  
  📍 `lexer/Lexer.cpp -> readIdentifier()`

- Integer literal: `[0-9]+`  
  📍 `lexer/Lexer.cpp -> readNumber()`

- Float literal: `[0-9]+\.[0-9]+`  
  📍 `lexer/Lexer.cpp -> readNumber()`

- String literal: `"([^"\\]|\\.)*"`  
  📍 `lexer/Lexer.cpp -> readString()`

- Char literal: `'([^'\\]|\\.)'`  
  📍 `lexer/Lexer.cpp -> readString()` (returns `TokenType::CHAR_LITERAL`)

- Comments:  
  - Single-line: `//.*`  
  - Multi-line: `/\* ... \*/`  
  📍 `lexer/Lexer.cpp -> skipComment()`

- Operators and delimiters tokenization (including multi-char operators like `==`, `!=`, `<=`, `>=`, `<<`, `>>`, `&&`, `||`, `->`)  
  📍 `lexer/Lexer.cpp -> nextToken()`

## 3.2 System 6 Token-Insertion Rules

- Insert virtual `STMT_END` if:
  1) current token can end statement,  
  2) next token(s) include newline,  
  3) next non-newline token is a statement boundary.  
  📍 `lexer/Lexer.cpp -> tokenize()`, `canEndStatement()`, `isStatementBoundary()`

## 3.3 Parser / Syntax Rules

- Statement terminator is any of: `SEMICOLON | NEWLINE | STMT_END`  
  📍 `parser/Parser.cpp -> matchStatementEnd()`

- `if` syntax requires parentheses: `if (expr) statement`  
  📍 `parser/Parser.cpp -> parseIfStatement()`

- `while` syntax requires parentheses: `while (expr) statement`  
  📍 `parser/Parser.cpp -> parseWhileStatement()`

- Declaration validation examples:
  - reject invalid declarator after comma (e.g., `int a,,b`)  
  - reject `==` after identifier in declaration context  
  📍 `parser/Parser.cpp -> parseVariableDecl()`

- Anti-hang parser recovery: forced token advancement when parser makes no progress  
  📍 `parser/Parser.cpp -> parseProgram()`, `parseFunctionDecl()`, `parseBlock()`

## 3.4 Semantic Rules

- System 1 (Uninitialized use): if symbol exists but not initialized at use site -> report  
  📍 `semantic/SemanticAnalyzer.cpp -> processUse()`

- System 2 (Null dereference): dereference pointer in `NULL_PTR` or `UNKNOWN` state -> report  
  📍 `semantic/SemanticAnalyzer.cpp -> processDereference()`  
  📍 `utils/SymbolTable.cpp -> checkNullDereference()`

- System 3 (Leak): symbol marked allocated but not freed by end -> report  
  📍 `semantic/SemanticAnalyzer.cpp -> reportMemoryLeaks()`  
  📍 `utils/SymbolTable.cpp -> getMemoryLeaks()`

- System 4 (Include cycle): build include graph + DFS cycle detection  
  📍 `semantic/SemanticAnalyzer.cpp -> extractIncludeDirectives()`, `reportIncludeCycles()`  
  📍 `analyzer/IncludeDependencyAnalyzer.cpp -> detectCycles()`, `dfs()`

- System 5 (`let` inference): infer from first RHS token type and print info token  
  📍 `semantic/SemanticAnalyzer.cpp -> inferTypeFromToken()`, LET block in `analyze()`

- System 6 syntax info output for inserted terminators  
  📍 `semantic/SemanticAnalyzer.cpp -> analyze()` (`if (at(TokenType::STMT_END))`)

---

## 4) Source Code Snippets (Relevant Only)

### Lexer
📍 `lexer/Lexer.cpp -> tokenize()`

```cpp
// SYSTEM 6: Insert STMT_END tokens for semicolon-free syntax
if (tokens[i + 1].type == TokenType::NEWLINE &&
    canEndStatement(current) &&
    isStatementBoundary(*nextNonNewline)) {
    processedTokens.push_back(Token(TokenType::STMT_END, "", current.line, current.column));
}
```

### Parser
📍 `parser/Parser.cpp -> parseVariableDecl()`

```cpp
if (currentToken().type == TokenType::COMMA) {
    error("Invalid declarator after ',' in variable declaration");
}
if (!matchStatementEnd()) {
    error("Expected statement terminator ';' or newline after variable declaration");
}
```

📍 `parser/Parser.cpp -> parseProgram()` (anti-hang recovery)

```cpp
if (position == beforeLoopPos && currentToken().type != TokenType::END_OF_FILE) {
    errors.push_back("Parse recovery warning ... forced token advance ...");
    advance();
}
```

### Semantic Analyzer
📍 `semantic/SemanticAnalyzer.cpp -> processUse()`

```cpp
if (!symTable.checkInitialized(name, line)) {
    SemanticError err;
    err.kind = ErrorKind::UNINITIALIZED_USE;
    reporter.report(err);
}
```

📍 `semantic/SemanticAnalyzer.cpp -> processDereference()`

```cpp
if (symTable.checkNullDereference(name)) {
    err.kind = (e->nullState == NullState::NULL_PTR)
                 ? ErrorKind::NULL_DEREF
                 : ErrorKind::MAYBE_NULL_DEREF;
    reporter.report(err);
}
```

📍 `semantic/SemanticAnalyzer.cpp -> reportIncludeCycles()`

```cpp
IncludeCycle cycle = dependencyAnalyzer.detectCycles();
if (cycle.cycleFound && !cycle.cyclePath.empty()) {
    err.kind = ErrorKind::INCLUDE_CYCLE;
    reporter.report(err);
}
```

### Symbol Table
📍 `utils/SymbolTable.cpp -> markAllocated(), getMemoryLeaks()`

```cpp
e->allocated = true;
e->allocInLoop = inLoop;
...
if (!e.allocated || e.freed) return;
```

### Include Dependency Analyzer
📍 `analyzer/IncludeDependencyAnalyzer.cpp -> dfs()`

```cpp
if (recursionStack.find(neighbor) != recursionStack.end()) {
    currentPath.push_back(neighbor);
    cycleResult = currentPath;
    return true;
}
```

### Error Reporting
📍 `semantic/SemanticError.h -> SemanticError::print()`

```cpp
case ErrorKind::INCLUDE_CYCLE:
    std::cout << "SYSTEM 4: INCLUDE DEPENDENCY ANALYSIS\n"
              << "  Circular include dependency found.\n";
```

---

## 5) Output Examples (Real Format)

### System 1 (Uninitialized Use)

```text
[SEMANTIC ERROR]
SYSTEM 1: UNDEFINED BEHAVIOR DETECTION
  Variable 'x' was never initialized.
  Risk: Reading garbage memory leads to unpredictable behavior.
  Location: Line 6, Column 13
```

### System 2 (Null Dereference)

```text
[SEMANTIC ERROR]
 SYSTEM 2: NULL POINTER SAFETY
  Pointer 'ptr' dereferenced without null check..
  Risk: System Crash / Segmentation fault / Memory access violation.
  Location: Line 6, Column 6
```

### System 3 (Memory Leak)

```text
[SEMANTIC ERROR]
SYSTEM 3: MEMORY LEAK DETECTION
  Memory allocated to 'data' is never released..
  Risk: Memory allocated with 'new' is never released.
  Location: Line 5, Column 0
```

### System 4 (Include Cycle)

```text
[SEMANTIC ERROR]
SYSTEM 4: INCLUDE DEPENDENCY ANALYSIS
  Circular include dependency found.
  Cycle Path: c.h → a.h → b.h → c.h
```

### System 5 (`let` Type Inference)

```text
[SEMANTIC INFO]
TYPE_INFERRED_DOUBLE:
Variable 'pi' inferred as type 'double'
```

### System 6 (Semicolon-Free Syntax)

```text
[SYNTAX INFO]
Implicit statement terminator inserted.

Virtual Token:
STMT_END
Location: Line 8, Column 12
```

---

## 6) Limitations (Code-Based Only)

- Parsing is subset-oriented; not full ISO C++ grammar coverage.  
  📍 Complex constructs are simplified in `parser/Parser.cpp`.

- Parser uses recovery warnings with forced token advance to prevent hangs; this can reduce diagnostic precision after severe syntax corruption.  
  📍 `parser/Parser.cpp -> parseProgram()`, `parseFunctionDecl()`, `parseBlock()`.

- Semantic analysis is largely token-stream driven rather than deep AST semantic analysis.  
  📍 `semantic/SemanticAnalyzer.cpp` mostly processes tokens directly in `analyze()`.

- `let` inference is first-token based and does not perform full expression type checking/inference.  
  📍 `semantic/SemanticAnalyzer.cpp -> inferTypeFromToken()`.

- Include analysis focuses on local quoted includes (`#include "..."`) and file traversal from current source path; system include path resolution is limited.  
  📍 `semantic/SemanticAnalyzer.cpp -> extractIncludeDirectives()`.

- Symbol table’s unknown symbols are tolerated in some checks (e.g., init check returns true for missing symbol), so undefined-symbol policy is partial.  
  📍 `utils/SymbolTable.cpp -> checkInitialized()`.

---

## 7) Verification Note

This report is implementation-traceable: each listed feature/rule is mapped to concrete files and functions in the current codebase (`main.cpp`, `lexer/`, `parser/`, `semantic/`, `utils/`, `analyzer/`).

