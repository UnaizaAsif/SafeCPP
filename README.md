# SafeCPP_2: Extended C++ Safety Compiler Front-End

SafeCPP_2 is a C++17 compiler front-end project that implements a full analysis pipeline: **lexical analysis**, **syntax analysis**, and **semantic safety analysis**. The project extends regular C++ with educational safety-focused features (`let` inference and semicolon-free statement support) and reports issues in a structured, phase-wise format suitable for academic demonstration.

---

## 1) Selected Language (Detailed Description)

The selected language is **C++ (extended subset)**, implemented in **C++17**. The compiler accepts standard C++-like constructs (types, pointers, control flow, functions, declarations, expressions, includes) and augments them with two custom language behaviors: `let`-based type inference and newline-based statement termination (`STMT_END` insertion). The purpose is to preserve familiar C++ syntax while adding static checks for unsafe patterns (uninitialized use, null dereference, memory leaks, include cycles) and improving usability/expressiveness for educational compiler design outcomes.

---

## 2) Selected Functionalities (List Only)

- Lexical tokenization of source code into typed tokens
- Keyword / identifier / literal / operator / delimiter recognition
- Comment and whitespace handling
- Virtual statement termination insertion (`STMT_END`) for semicolon-free syntax
- Syntax parsing with recursive-descent strategy
- Syntax error reporting with line/column locations
- Parser recovery and anti-hang progress guards
- System 1: uninitialized-variable usage detection
- System 2: null pointer dereference detection
- System 3: memory leak detection (including loop-aware leak classification)
- System 4: include dependency graph + cycle detection
- System 5: type inference for `let` declarations
- System 6: semicolon-free syntax reporting
- Phase-wise output formatting (Lexical / Syntax / Semantic / Summary)
- Optional visualization UI (`UI/`) using React + Express

---

## 3) Regular Expressions / Rules (Per Functionality)

> Note: Implementation uses character scanning + parser rules (not a pure regex engine).  
> The patterns below represent effective lexical/syntax rules used by the compiler.

### 3.1 Lexical Rules

- **Identifier**: `[A-Za-z_][A-Za-z0-9_]*`
- **Integer literal**: `[0-9]+`
- **Floating literal**: `[0-9]+\.[0-9]+`
- **String literal**: `"([^"\\]|\\.)*"`
- **Char literal**: `'([^'\\]|\\.)'`
- **Single-line comment**: `//.*`
- **Multi-line comment**: `/\*[\s\S]*?\*/`
- **Whitespace skip rule**: skip spaces/tabs; newline tokenized as `NEWLINE`
- **Multi-char operators first**: `==`, `!=`, `<=`, `>=`, `<<`, `>>`, `&&`, `||`, `->`
- **Single-char operators/delimiters**: `+ - * / % = ! < > & ( ) { } [ ] ; , . :`

### 3.2 Keyword Mapping Rule

If an identifier lexeme matches reserved words (`int`, `float`, `if`, `while`, `for`, `return`, `let`, `new`, `delete`, `nullptr`, `include`, etc.), assign keyword token type; otherwise assign `IDENTIFIER`.

### 3.3 System 6 (Semicolon-Free) Rule

Insert virtual `STMT_END` when:

- Current token can end a statement (identifier/literal/right delimiter/return/break/continue/delete), and
- Followed by one or more `NEWLINE`s, and
- Next non-newline token is a likely statement boundary (`int`, `let`, `if`, `for`, `while`, `return`, `class`, `struct`, `}`, EOF, etc.).

### 3.4 Syntax Grammar-Style Rules (Core)

- **Declaration**: `<type> <id> [= <expr>] <stmt_end>`
- **let declaration**: `let <id> = <expr> <stmt_end>`
- **Assignment**: `<id> = <expr> <stmt_end>`
- **If statement**: `if ( <expr> ) <statement> [else <statement>]`
- **While statement**: `while ( <expr> ) <statement>`
- **Function decl**: `<type> <id> ( [params] ) <body-or-end>`
- **Statement end**: `;` or `NEWLINE` or `STMT_END`

### 3.5 Syntax Error Rules

- Unexpected token after variable name (e.g. `==` instead of `=`)
- Missing expected delimiters (`)`, `(`, etc.)
- Invalid declarator forms (e.g. `int a,,b;`, `int b,10;`)
- Unexpected token in expression (e.g. `int f = ;`, invalid primary)
- Missing terminator after expression/declaration

### 3.6 Semantic Rules

- **System 1**: report use of declared but not initialized symbol
- **System 2**: report dereference when pointer is `NULL_PTR` or `UNKNOWN`
- **System 3**: report allocated-but-not-freed symbols; classify loop allocation leaks
- **System 4**: build include graph from `#include "..."`, DFS cycle detection
- **System 5**: infer `let` type from RHS first token:
  - `INTEGER -> int`
  - `FLOAT -> double`
  - `STRING -> string`
  - `CHAR_LITERAL -> char`
  - `TRUE/FALSE -> bool`
  - otherwise `unknown` + inference error
- **System 6**: print syntax info for each encountered virtual `STMT_END`

---

## 4) Source Code (How It Is Implemented)

### 4.1 Core Files

- `main.cpp`  
  Orchestrates all phases and prints:
  - `[LEXICAL ANALYZER PHASE]`
  - `[SYNTAX ANALYZER PHASE]`
  - `[SEMANTIC ANALYZER PHASE]`
  - `[ANALYSIS SUMMARY]`

- `lexer/Token.h`, `lexer/Token.cpp`  
  Token type system and token formatting.

- `lexer/Lexer.h`, `lexer/Lexer.cpp`  
  Character scanner, keyword mapping, literal parsing, operator tokenization, comment handling, and `STMT_END` insertion.

- `parser/Parser.h`, `parser/Parser.cpp`  
  Recursive-descent parser + AST node classes + syntax error collection + recovery guards (anti-hang forced advancement).

- `semantic/SemanticAnalyzer.h`, `semantic/SemanticAnalyzer.cpp`  
  Token-stream semantic checks for Systems 1–6; integrates symbol tracking and include cycle detection.

- `semantic/SemanticError.h`  
  Error and info reporting structures for semantic/syntax info messages.

- `utils/SymbolTable.h`, `utils/SymbolTable.cpp`  
  Scoped symbol tracking for initialization/null state/allocation state and leak extraction.

- `analyzer/IncludeDependencyAnalyzer.h`, `analyzer/IncludeDependencyAnalyzer.cpp`  
  Include dependency graph + DFS cycle detection.

### 4.2 Build Configuration

- `Makefile` builds:
  - `main.cpp`
  - lexer
  - parser
  - symbol table
  - semantic analyzer
  - include dependency analyzer

### 4.3 Optional UI

- `UI/server` (Express backend): runs `compiler_test2.exe` and returns JSON sections
- `UI/client` (React + Vite frontend): visualizes lexical/syntax/semantic/summary panels

---

## 5) Build and Run

## 5.1 Compile (Windows PowerShell)

```powershell
g++ -std=c++17 -Wall -Wextra -I. -o compiler_test2.exe main.cpp lexer/Token.cpp lexer/Lexer.cpp parser/Parser.cpp utils/SymbolTable.cpp semantic/SemanticAnalyzer.cpp analyzer/IncludeDependencyAnalyzer.cpp
```

## 5.2 Run Single File

```powershell
.\compiler_test2.exe "samples/sy1.cpp"
```

## 5.3 Run Useful Test Files

```powershell
.\compiler_test2.exe "samples/system1_ub.cpp"
.\compiler_test2.exe "samples/system2_null.cpp"
.\compiler_test2.exe "samples/system3_memory.cpp"
.\compiler_test2.exe "samples/system4_include.cpp"
.\compiler_test2.exe "samples/system5_let.cpp"
.\compiler_test2.exe "samples/system6_semicolon.cpp"
.\compiler_test2.exe "samples/all_systems_demo.cpp"
```

---

## 6) Output Format

The compiler prints phase-wise output:

1. **Lexical analyzer phase**: complete token stream with line/column
2. **Syntax analyzer phase**:
   - syntax issue lines with line/column (if invalid)
   - syntax summary (`VALID/INVALID`, total issues)
   - no hanging due to parser progress guards
3. **Semantic analyzer phase**:
   - semantic errors / info (`SYSTEM 1..6`)
4. **Analysis summary**:
   - total safety issue count

### Example (Invalid Syntax)

```text
[SYNTAX ANALYZER PHASE]
Syntax issues found:
  Parse error at line 2, column 13: Syntax Error: Unexpected token '+'. Expected: identifier or number

[SYNTAX SUMMARY]
SYNTAX STATUS: INVALID
TOTAL SYNTAX ISSUES: 1
```

### Example (Valid Syntax)

```text
[SYNTAX ANALYZER PHASE]
Syntax analysis completed successfully.

[SYNTAX SUMMARY]
SYNTAX STATUS: VALID
TOTAL SYNTAX ISSUES: 0
```

---

## 7) Limitations

- Parser supports a practical C++ subset, not the full standard language.
- Some syntax errors are recovered with warnings (`forced token advance`) to prevent hangs.
- Semantic analysis is mostly token-pattern based rather than full AST semantic traversal.
- `#include` cycle analysis currently targets quoted local includes (`"file.h"`), not full system-include resolution.
- Type inference (`let`) is first-token based; complex expressions are not deeply typed.
- Some diagnostics are intentionally conservative and may not cover all edge cases.
- `Makefile` includes Unix-style `rm` in `clean` target (PowerShell users may compile manually or use MinGW shell tools).

---

## 8) Project Structure (Current)

```text
SafeCCP_2/
├── analyzer/
├── documentation/
├── lexer/
├── parser/
├── semantic/
├── utils/
├── samples/
├── UI/
│   ├── server/
│   └── client/
├── main.cpp
├── Makefile
└── compiler_test2.exe
```

---

## 9) Suggested Viva / Demo Flow

1. Show lexical tokens on a simple input.
2. Show syntax invalid file (`sy1.cpp` / `sy2.cpp` / `sy3.cpp`) and line/column diagnostics.
3. Show semantic system files (`system1_ub.cpp` .. `system6_semicolon.cpp`).
4. Show integrated run (`all_systems_demo.cpp`).
5. Optionally show visualization from `UI/`.

