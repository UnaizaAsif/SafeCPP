# SafeCPP: C++ Safety Analyzer

## Project Overview

**SafeCPP** is a compiler front-end that performs **static safety analysis** on C++ code to detect critical bugs **before runtime**. It focuses on two core systems that catch the most dangerous undefined behavior and memory safety violations:

- **System 1: Undefined Behavior Detection** — Detects uninitialized variable usage
- **System 2: Null Pointer Safety** — Detects null pointer dereferences

Unlike traditional compilers that only check syntax, SafeCPP uses semantic analysis to understand variable lifetimes, initialization states, and pointer safety.

## Why This Matters

**Problem**: Modern C++ code contains numerous latent vulnerabilities:
- **Uninitialized variables** — Reading garbage memory causes unpredictable behavior
- **Null pointer dereferences** — Segmentation faults and memory corruption

**Solution**: SafeCPP proactively identifies these issues during **static analysis** before your code compiles or runs.

**Innovation**: By integrating semantic analysis with lexical and syntactic parsing, SafeCPP demonstrates how compiler-level analysis prevents the most common C++ bugs.

## The Two Core Safety Systems

### System 1: Undefined Behavior Detection

Detects when variables are used before initialization.

**❌ Unsafe Code:**
```cpp
int x;              // Declared but not initialized
int y = x + 1;      // ERROR: Using garbage value in x
```

**✅ Safe Code:**
```cpp
int x = 0;          // Properly initialized
int y = x + 1;      // OK: x has a known value
```

**What SafeCPP Will Report:**
```
[SEMANTIC ERROR]
  ❌ SYSTEM 1: UNDEFINED BEHAVIOR DETECTION
  ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
  Variable 'x' was never initialized.
  Risk: Reading garbage memory leads to unpredictable behavior.
  Impact: Program may crash, produce wrong results, or seem to work
          occasionally, making bugs very hard to track.
  Location: Line 2, Column 11
  Suggestion: Initialize variable before use (e.g., int x = 0;)
```

---

### System 2: Null Pointer Safety

Detects when null pointers are dereferenced.

**❌ Unsafe Code:**
```cpp
int* ptr = nullptr;     // Explicitly set to null
*ptr = 10;              // ERROR: Dereferencing null pointer!
```

**✅ Safe Code:**
```cpp
int value = 42;
int* ptr = &value;      // Points to valid memory
*ptr = 100;             // OK: Dereferencing valid pointer
```

**What SafeCPP Will Report:**
```
[SEMANTIC ERROR]
  ❌ SYSTEM 2: NULL POINTER SAFETY
  ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
  Pointer 'ptr' is DEFINITELY NULL.
  Attempting to dereference it (access *ptr) will cause immediate crash.
  Risk: Segmentation fault / Memory access violation.
  Impact: Program terminates abnormally. This is a CRITICAL bug.
  Location: Line 2, Column 1
  Suggestion: Check that 'ptr' is not null before dereferencing.
              Consider: if (ptr != nullptr) { ... }
```

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
    └→ Validate C++ grammar rules
    ↓
Abstract Syntax Tree (AST)
    ↓
[PHASE 3] SEMANTIC ANALYSIS
    └→ System 1: Undefined Behavior Detection
    └→ System 2: Null Pointer Safety
    ↓
Analysis Report with Issues & Warnings
```

### File Structure

```
SafeCPP/
├── main.cpp                    # Entry point - runs demo or analyzes files
│
├── lexer/
│   ├── Lexer.h                # Tokenizer interface
│   ├── Lexer.cpp              # Tokenization implementation
│   ├── Token.h                # Token types & definitions
│   └── Token.cpp              # Token implementation
│
├── parser/
│   ├── Parser.h               # Parser/AST interface
│   └── Parser.cpp             # C++ grammar validation
│
├── semantic/
│   ├── SemanticAnalyzer.h     # Safety analysis interface
│   ├── SemanticAnalyzer.cpp   # Systems 1 & 2 implementation
│   └── SemanticError.h        # Error reporting
│
├── utils/
│   ├── SymbolTable.h          # Symbol tracking
│   └── SymbolTable.cpp        # Scope management & initialization tracking
│
├── samples/
│   └── demo3.cpp              # Example test case
│
├── Makefile                   # Build configuration
└── README.md                  # This file
```

## Building & Running

### Prerequisites
- C++17 compatible compiler (g++, clang, MSVC)
- Make or CMake

**Linux/macOS:**
```bash
make
```

**Windows (with MinGW or MSVC):**
```bash
g++ main.cpp lexer/Lexer.cpp lexer/Token.cpp parser/Parser.cpp \
    semantic/SemanticAnalyzer.cpp utils/SymbolTable.cpp -std=c++17 -o safecpp.exe
```

### Run

**Demo (shows all test cases):**
```bash
./safecpp --demo
```

**Analyze a specific file:**
```bash
./safecpp samples/demo3.cpp
./safecpp your_code.cpp
```

**Show detailed token information:**
```bash
./safecpp samples/demo3.cpp --tokens
```

### Clean
```bash
make clean
```

## Test Cases

### Test 1: Uninitialized Variable (System 1 — ERROR)
**File:** `samples/demo3.cpp`
```cpp
int x;              // Not initialized
std::cout << x;     // ERROR: Using uninitialized x
```
**Expected Output:**
```
[SEMANTIC ERROR]
  ❌ SYSTEM 1: UNDEFINED BEHAVIOR DETECTION
  Variable 'x' was never initialized...
```

---

### Test 2: Null Pointer Dereference (System 2 — CRITICAL ERROR)
```cpp
int* ptr = nullptr;
*ptr = 10;          // ERROR: Dereferencing null pointer
```
**Expected Output:**
```
[SEMANTIC ERROR]
  ❌ SYSTEM 2: NULL POINTER SAFETY
  Pointer 'ptr' is DEFINITELY NULL...
```

---

### Test 3: Safe Code (NO ERRORS)
```cpp
int x = 0;          // Properly initialized
int y = x + 1;      // OK
```
**Expected Output:**
```
[ANALYSIS SUMMARY]
  No safety issues detected. Code is clean.
```

---

### Test 4: Safe Pointer (NO ERRORS)
```cpp
int value = 42;
int* ptr = &value;
*ptr = 100;         // OK: Valid dereference
```
**Expected Output:**
```
[ANALYSIS SUMMARY]
  No safety issues detected. Code is clean.
```

## How It Works

### System 1: Undefined Behavior Detection

SafeCPP maintains a **symbol table** that tracks:
- Variables declared in the current scope
- Whether each variable has been initialized
- The initialization location

When it encounters a variable use (e.g., `x + 1`), it checks:
1. Is the variable declared?
2. Has it been initialized?
3. If not → Report `UNINITIALIZED_USE` error

### System 2: Null Pointer Safety

SafeCPP tracks pointer states:
- Is the pointer initialized?
- Is it definitely null? (assigned `nullptr` or `0`)
- Is it definitely non-null? (assigned a valid address)
- Is it unknown? (assigned from another variable)

When it encounters a dereference (`*ptr`), it checks:
1. Is the pointer initialized?
2. What is its null state?
3. If null → Report `NULL_DEREF` error
4. If unknown → Report `MAYBE_NULL_DEREF` warning

---

## Error Messages Explained

Each error message includes:

| Part | Purpose |
|------|----------|
| **System Identifier** | Which safety system detected the issue |
| **Variable Name** | The problematic variable or pointer |
| **Risk Description** | What bad thing will happen |
| **Impact** | How the program will be affected |
| **Location** | Exact line and column in source code |
| **Suggestion** | How to fix the problem |

**Example:**
```
❌ SYSTEM 1: UNDEFINED BEHAVIOR DETECTION
Variable 'x' was never initialized.
Risk: Reading garbage memory leads to unpredictable behavior.
Impact: Program may crash, produce wrong results, or seem to work
        occasionally, making bugs very hard to track.
Location: Line 5, Column 10
Suggestion: Initialize variable before use (e.g., int x = 0;)
```

---

## Limitations & Future Work

**Current Limitations:**
- Only analyzes simple variable declarations and assignments
- Does not perform inter-procedural analysis (across function calls)
- Does not handle complex control flow (loops with conditional assignments)
- Does not track pointer ownership or lifetime

**Future Enhancements:**
- Control flow analysis for conditional initialization
- Function-level data flow analysis
- Smart pointer support (std::unique_ptr, std::shared_ptr)
- Reference tracking
- Use-after-free detection

