# SafeCPP: Extended C++ Lexical and Syntax Analyzer

## Project Overview

**SafeCPP** is an advanced compiler front-end that analyzes an **Extended C++** language—a superset of standard C++ that introduces safety-aware features and modern syntax enhancements. Unlike a completely new language, Extended C++ extends C++ similarly to how:
- Objective-C extends C
- CUDA extends C++
- C++/CLI extends C++
- TypeScript extends JavaScript

This project combines **lexical analysis**, **syntax analysis**, and **static safety analysis** into a unified compiler pipeline that detects vulnerabilities before compilation.

## Technical Significance

### Why This Matters (CLO-3: No Obvious Solution)

**Problem**: Modern C++ code contains numerous latent vulnerabilities:
- Uninitialized variable usage (System 1)
- Null pointer dereferences (System 2)
- Memory leaks and resource leaks (System 3-4)
- Circular include dependencies (System 7)

**Solution**: SafeCPP proactively identifies these issues using compiler-level analysis during the lexical, syntactic, and semantic phases—**before** runtime errors occur.

**Innovation**: By integrating extended syntax features (let type inference, semicolon-free syntax) with static analysis, SafeCPP demonstrates how modern language design can improve both productivity and safety.

## The 8 Analysis Systems

SafeCPP implements **8 independent analysis systems** that work together in the compilation pipeline:

### System 1: @safe UB Detection
Detects undefined behavior and uninitialized variable usage.

**Example:**
```cpp
@safe int x;        // Marked for safety analysis
printf(x);          // WARNING: Uninitialized variable
```

### System 2: Null Pointer Safety
Detects null pointer dereferences without proper checks.

**Example:**
```cpp
@safe nullable int* ptr = NULL;
*ptr = 5;           // CRITICAL: Potential null dereference
```

**Safe variant:**
```cpp
if (ptr != NULL) {
    *ptr = 5;       // OK: Protected by check
}
```

### System 3: Memory Leak Detection
Tracks `new`/`delete` and `malloc`/`free` operations.

**Example:**
```cpp
int* data = new int[100];  // WARNING: Allocated but never freed
```

**Safe variant:**
```cpp
@safe int* data = new int[100];
// use data...
delete[] data;      // Properly released
```

### System 4: Loop Leak Detection
Detects memory/resource leaks specifically in loops.

**Example:**
```cpp
for (int i = 0; i < 1000; i++) {
    int* temp = new int;   // CRITICAL: Loop leak - allocated 1000 times
    // Missing: delete temp
}
```

**Safe variant:**
```cpp
for (int i = 0; i < 1000; i++) {
    int* temp = new int;
    // ... use temp ...
    delete temp;    // Freed each iteration
}
```

### System 5: let Type Inference
Automatically infers types for `let` declarations (Extended C++ feature).

**Example:**
```cpp
let pi = 3.14;              // Inferred: double
let message = "SafeCPP";    // Inferred: string
let count = 42;             // Inferred: int
```

**Output:**
```
NOTE [5:5]: Type inferred for 'pi' as double
NOTE [6:5]: Type inferred for 'message' as string
NOTE [7:5]: Type inferred for 'count' as int
```

### System 6: Semicolon-Free Syntax
Supports optional semicolons at statement boundaries (Extended C++ feature).

**Example:**
```cpp
int x = 5           // No semicolon - valid in Extended C++
let y = 10
if (x > 0) {
    printf("positive")
}
```

**Note:** Semicolons are still valid; they're just optional before newlines or `}`.

### System 7: Include Dependency Cycle Detection
Detects circular include dependencies using DFS.

**Example:**
```cpp
// file_a.h
#include "file_b.h"  // OK

// file_b.h
#include "file_a.h"  // CRITICAL: Cyclic include detected!
```

**Solution:** Use include guards:
```cpp
#ifndef FILE_A_H
#define FILE_A_H
// ... content ...
#endif
```

### System 8: Syntax Analyzer/Parser
**Built-in to the parser** - validates Extended C++ grammar:
- Variable declarations with safety modifiers
- Function declarations and calls
- Control flow statements (if, while, for, switch)
- Expression precedence and associativity
- Extended syntax (let, @safe, nullable, nonnull)

## Project Architecture

### Compilation Pipeline

```
Source Code
    ↓
[PHASE 1] LEXICAL ANALYSIS
    └→ Tokenize input
    └→ Recognize keywords, operators, literals
    ↓
Token Stream
    ↓
[PHASE 2] SYNTAX ANALYSIS
    └→ Build Abstract Syntax Tree (AST)
    └→ Validate grammar rules
    └→ Detect parse errors
    ↓
Abstract Syntax Tree (AST)
    ↓
[PHASE 3] SEMANTIC ANALYSIS
    └→ System 1: UB Detection
    └→ System 2: Null Safety
    └→ System 3: Leak Detection
    └→ System 4: Loop Leaks
    └→ System 5: Type Inference
    └→ System 6: Semicolon-Free Validation
    └→ System 7: Include Cycles
    └→ System 8: Syntax Validation
    ↓
Analysis Report with Issues & Warnings
```

### File Structure

```
SafeCPP/
├── main.cpp                    # Entry point
│
├── lexer/
│   ├── Lexer.h                # Lexer interface
│   ├── Lexer.cpp              # Pure tokenization
│   ├── Token.h                # Token types & structure
│   └── Token.cpp              # Token implementation
│
├── parser/
│   ├── Parser.h               # Parser/AST interface
│   └── Parser.cpp             # Extended C++ grammar validation
│
├── analyzer/
│   ├── SafetyAnalyzer.h       # 8 safety systems
│   ├── SafetyAnalyzer.cpp     # Analysis implementation
│   ├── DependencyGraph.h      # Include cycle detection
│   └── DependencyGraph.cpp    # Graph algorithms (DFS)
│
├── utils/
│   ├── SymbolTable.h          # Symbol tracking
│   └── SymbolTable.cpp        # Scope management
│
├── samples/
│   ├── demo1.cpp              # Systems 5 & 6: Type inference & syntax
│   ├── demo2.cpp              # Systems 1-4, 7: Safety issues
│   └── full_demo.cpp          # All systems demonstrated
│
├── output/
│   └── token_output.txt       # Analysis output placeholder
│
├── Makefile                   # Build configuration
└── README.md                  # This file
```

## Building & Running

### Prerequisites
- C++17 compatible compiler (g++, clang, MSVC)
- Make or CMake

### Build (Linux/macOS)
```bash
make
```

### Build (Windows with MinGW)
```bash
g++ main.cpp lexer/Lexer.cpp lexer/Token.cpp parser/Parser.cpp \
    analyzer/SafetyAnalyzer.cpp analyzer/DependencyGraph.cpp \
    utils/SymbolTable.cpp -std=c++17 -o safecpp.exe
```

### Run
```bash
./safecpp samples/demo1.cpp
./safecpp samples/demo2.cpp
./safecpp samples/full_demo.cpp
```

### Clean
```bash
make clean
```

## Test Cases (8 Systems)

### Test 1: Type Inference (System 5)
**Input:**
```cpp
let x = 42
```
**Expected Output:**
```
NOTE [1:1]: Type inferred for 'x' as int
```

### Test 2: Null Pointer Safety (System 2)
**Input:**
```cpp
@safe nullable int* ptr = NULL;
*ptr = 5;
```
**Expected Output:**
```
CRITICAL [2:1]: Potential null pointer dereference
```

### Test 3: Memory Leak (System 3)
**Input:**
```cpp
int* data = new int[100];
```
**Expected Output:**
```
WARNING [1:1]: Memory leak detected - allocated memory not freed
```

### Test 4: Loop Leak (System 4)
**Input:**
```cpp
for (int i = 0; i < 10; i++) {
    int* x = new int;
}
```
**Expected Output:**
```
CRITICAL [2:5]: Loop leak - memory allocated in loop without cleanup
```

### Test 5: Include Cycle (System 7)
**Input files:**
- a.h: `#include "b.h"`
- b.h: `#include "a.h"`

**Expected Output:**
```
CRITICAL: Cyclic include detected: a.h → b.h → a.h
```

### Test 6: Uninitialized Variable (System 1)
**Input:**
```cpp
int x;
printf(x);
```
**Expected Output:**
```
WARNING [2:1]: Use of uninitialized variable 'x'
```

### Test 7: Semicolon-Free (System 6)
**Input:**
```cpp
let x = 5
let y = 10
```
**Expected Output:**
```
NOTE: Semicolon-free syntax validated
```

### Test 8: Parser Validation (System 8)
**Input (invalid syntax):**
```cpp
int x = ;
```
**Expected Output:**
```
SYNTAX_ERROR [1:8]: Expected expression
```

## Key Assumptions & Limitations

### Assumptions
1. **C++17 Compliance**: Code uses C++17 or newer syntax
2. **Extended C++ Dialect**: Code may use Extended C++ features (let, @safe, etc.)
3. **Single-Pass Analysis**: Analysis is performed in one pass after parsing
4. **No Execution**: Analysis is purely static; no code execution
5. **Include Files Available**: All included files are accessible during analysis

### Limitations
1. **Template Analysis**: Limited support for C++ templates
2. **Macro Expansion**: Preprocessor macros not fully expanded
3. **Type System**: Simplified type inference (does not cover all C++ types)
4. **Data Flow**: Limited interprocedural data flow analysis
5. **External Libraries**: Cannot analyze code in external libraries
6. **Performance**: Worst-case analysis may be slow for very large codebases

## Academic Justification (CPA)

### CPA-1: Non-Obvious Solution
Combining lexical analysis, syntax validation, and static safety analysis into one tool requires:
- Deep understanding of compiler design
- Knowledge of graph algorithms (cycle detection)
- Expertise in static analysis techniques
- No straightforward off-the-shelf solution exists

### CPA-2: Technical Complexity
Implementation demonstrates:
- **Compiler theory**: Lexing, parsing, AST construction
- **Formal languages**: Grammar rules, precedence analysis
- **Graph theory**: DFS for cycle detection
- **Data structures**: Symbol tables, scope management
- **Software engineering**: Multi-phase architecture, visitor pattern, error handling

### CPA-3: Real-World Applicability
Solves actual problems in modern C++ development:
- **Safety**: Prevents entire classes of runtime errors
- **Developer Productivity**: Automatic type inference, flexible syntax
- **Code Quality**: Identifies latent bugs before runtime
- **Dependency Management**: Detects circular includes early

## Future Enhancements

- [ ] Full C++20 support
- [ ] Advanced data flow analysis
- [ ] Taint analysis for security
- [ ] IDE integration (VS Code, IntelliJ)
- [ ] Configuration files for analysis rules
- [ ] CI/CD pipeline integration
- [ ] Performance profiling and optimization
- [ ] Support for custom analyzer plugins

## References & Resources

- **Compilers: Principles, Techniques, and Tools** (Dragon Book) - Aho, Lam, Sethi, Ullman
- **Engineering a Compiler** - Cooper & Torczon
- **Static Program Analysis** - Nielsen, Nielsen, Hankin
- **C++ Standard Library Reference**

## License

[Add license information]

## Author

[Your Name/Organization]

## Contact & Support

For issues, questions, or contributions, please contact [your-contact-info].

---

**Last Updated:** May 2026

**Status:** Academic Project - CT-367 Compiler Design Course

