# System 3: Memory Leak Detection - Design & Limitations

## Overview
System 3 implements compile-time memory leak detection for C++ code. It tracks `new` allocations and `delete` deallocations, reporting any unfreed memory at program end.

## What Works ✅

### Supported Features
- **Memory allocation tracking**: Detects all `new` operations with variable binding
- **Memory deallocation tracking**: Identifies corresponding `delete` operations
- **Leak detection**: Reports variables allocated but never deleted
- **Multiple leak reporting**: Identifies all leaked variables in a single pass
- **Proper cleanup detection**: Verifies all allocations have corresponding deallocations
- **Scope awareness**: Tracks which scope each allocation belongs to

### Supported Code Patterns
```cpp
// Pattern 1: Simple allocation without cleanup (ERROR)
int* ptr = new int;

// Pattern 2: Proper cleanup (SAFE)
int* p1 = new int;
delete p1;

// Pattern 3: Multiple allocations (ERROR if not all deleted)
int* x = new int;
int* y = new int;
delete x;
// y is leaked

// Pattern 4: Sequential scope management (SAFE)
int* p = new int;
delete p;
int* q = new int;
delete q;
```

## What Doesn't Work ❌

### Architectural Limitations

**1. Control Flow Structures**
- ❌ `while` loops
- ❌ `for` loops  
- ❌ `do-while` loops
- ❌ `if/else` conditionals
- ❌ `switch` statements

**Why**: The analyzer is single-pass token-based (not AST-based). Token stream navigation breaks when encountering control flow constructs, causing hangs during the lexing/parsing phase before semantic analysis even begins.

**2. Loop-Aware Analysis**
- ❌ Cannot detect repeated allocations inside loops
- ❌ Cannot distinguish loop-based leaks from regular leaks
- ❌ Cannot track allocations across loop iterations

**Why**: Proper loop handling requires AST-based analysis with control flow graphs. Single-pass token processing cannot reliably track state changes across multiple iterations.

**3. Complex Expressions**
- ⚠️ Function calls with new (unreliable)
- ⚠️ Pointer arithmetic
- ⚠️ Nested pointer operations
- ⚠️ Return value tracking

## Test Results

### Passing Tests (Sequential Code) ✅
- `test_no_loop.cpp` - Basic allocation and cleanup
- `test_safe.cpp` - Multiple allocations, all freed  
- `test_simple.cpp` - Single leak detection
- `test_multiple_leaks.cpp` - Multiple leak detection
- `test_quick_s3.cpp` - Mixed scenarios

### Failing Tests (Control Flow) ❌
- `test_while_simple.cpp` - Hangs during analysis
- `test_loop_leak.cpp` - Hangs during analysis
- Any code with `while`, `for`, `do` keywords - Causes hang

## Technical Details

### How System 3 Works

**Phase 1: Declaration Processing**
```
int* ptr = new int;
         ↓
      Mark ptr as:
      - Allocated: true
      - AllocationLine: 5
      - Freed: false
```

**Phase 2: Deletion Processing**
```
delete ptr;
   ↓
Find ptr in symbol table
Mark as:
- Freed: true
- FreeLine: 10
```

**Phase 3: Leak Reporting**
```
At end of analysis:
For each variable:
  if (allocated && !freed)
    → Report MEMORY_LEAK
```

### Symbol Table Extensions
```cpp
struct SymbolEntry {
    bool allocated;      // Was this variable assigned new?
    bool freed;          // Was this variable deleted?
    int allocLine;       // Line where allocation occurred
    bool allocInLoop;    // Was allocation inside a loop? (disabled)
    int loopDepth;       // Loop nesting level (disabled)
};
```

## Recommendations

### For Current Use
✅ Use System 3 for educational purposes with sequential code
✅ Demonstrate memory leak detection concepts  
✅ Test basic allocation/deallocation patterns
✅ Learn semantic analyzer architecture

### For Production Use
❌ Do NOT use for code with control flow structures
❌ Do NOT rely on loop-based analysis  
❌ Use static analysis tools (Clang Static Analyzer, Valgrind, AddressSanitizer)

### Future Improvements (Requires Redesign)
If loop support is needed:
1. Build proper AST from parser output (not token-based)
2. Implement control flow graph analysis
3. Track allocation sites across loop iterations
4. Handle pointer escaping and sharing

## Conclusion

System 3 successfully demonstrates **compile-time memory leak detection** for a specific subset of C++ patterns. Its limitations reflect fundamental design choices in the compiler architecture (single-pass, token-based analysis) rather than implementation gaps.

**Status**: ✅ **STABLE** for sequential code | ⚠️ **LIMITED** for control flow
