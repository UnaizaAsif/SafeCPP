================================================
SYSTEM 3 — MEMORY LEAK DETECTION
Extended C++ Safety Compiler v1.0
================================================

OVERVIEW:
--------
System 3 implements compile-time memory leak detection by tracking pointer
allocations (new) and deallocations (delete) throughout the program. It flags
any memory that is allocated but never freed.

KEY CAPABILITIES:
-----------------

✔ BASIC LEAK DETECTION
  - Identifies variables allocated with 'new'
  - Verifies each allocation has a corresponding 'delete'
  - Reports "MEMORY_LEAK_WARNING" for unreleased memory
  
✔ MULTIPLE LEAK DETECTION
  - Detects when multiple variables leak
  - Reports each leak individually
  - Helps identify systematic memory management issues
  
✔ SCOPE-AWARE TRACKING
  - Tracks allocations within nested scopes
  - Properly handles scope entry/exit via braces {}
  - Uses scope depth to identify nested contexts

IMPLEMENTATION DETAILS:
----------------------

1. SYMBOL TABLE EXTENSIONS (System 3 Fields):
   - allocated: bool            (was 'new' applied?)
   - freed: bool                (was 'delete' applied?)
   - allocLine: int             (line where 'new' occurred)
   - allocInLoop: bool          (allocated in nested scope?)
   - loopDepth: int             (scope nesting depth)

2. SEMANTIC ANALYZER LOGIC:
   
   a) Tracking Allocations:
      When "new" keyword is encountered in an assignment:
      - Mark variable as allocated
      - Record allocation line number
      - Note if allocation is in nested scope (scopeDepth > 1)
   
   b) Tracking Deallocations:
      When "delete" keyword is encountered:
      - Mark target variable as freed
      - Record deallocation line number
   
   c) Leak Detection:
      At end of analysis:
      - Iterate through all symbols
      - For each: if (allocated == true AND freed == false)
        → Report MEMORY_LEAK_WARNING

3. SCOPE TRACKING:
   - scopeDepth increments when entering { }
   - scopeDepth decrements when exiting } }
   - Uses scope depth to distinguish loop allocations
     (allocInLoop = scopeDepth > 1)

OUTPUT FORMAT:
--------------

✔ MEMORY_LEAK_WARNING:
  Variable '{varName}' allocated but not freed.
  
  Shows:
  - Variable name that leaked
  - Line where allocation occurred
  - Suggestion to add delete statement

✔ No Issues:
  If all memory is properly freed:
  "No memory leaks detected. Code is clean."

EXAMPLES:
---------

Example 1 – Simple Leak (DETECTED):
  int* p = new int;
  int* q = new int;
  delete p;
  // q is NOT freed → MEMORY_LEAK_WARNING for 'q'

Example 2 – Proper Cleanup (SAFE):
  int* p = new int;
  int* q = new int;
  delete p;
  delete q;
  // All memory freed → No issues detected

Example 3 – Multiple Leaks (ALL DETECTED):
  int* a = new int;
  int* b = new int;
  int* c = new int;
  delete a;
  // b and c both leak → Two MEMORY_LEAK_WARNINGs

TESTING:
--------

Test files provided:
  • test_simple.cpp        – Single memory leak
  • test_safe.cpp          – Proper cleanup (no issues)
  • test_multiple_leaks.cpp – Multiple leaks
  • test_system3.cpp       – Comprehensive test

Run tests:
  $ ./compiler test_simple.cpp
  $ ./compiler test_safe.cpp
  $ ./compiler test_multiple_leaks.cpp

Run full demo:
  $ ./compiler --demo

LIMITATIONS:
-----------

⚠ Currently does not detect:
  - Memory leaks in complex control flow (conditional branches)
  - Loop-based leaks in while/for loops (scope depth tracking)
  - Reachability analysis (allocated but logically unreachable code)
  - STL container leaks
  - Pointer arithmetic and aliasing

These are future enhancements that could be implemented.

INTEGRATION:
-----------

System 3 works alongside:
  • System 1: Undefined Behavior Detection
  • System 2: Null Pointer Safety
  
All three systems run in a single pass over the token stream, providing
comprehensive compile-time C++ safety analysis.

FILES MODIFIED:
---------------

New/Extended Components:
  • semantic/SemanticError.h
    - Added ErrorKind::MEMORY_LEAK
    - Added ErrorKind::LOOP_MEMORY_LEAK
    - Updated print() to handle memory leak messages

  • utils/SymbolTable.h
    - Extended SymbolEntry with memory tracking fields
    - Added markAllocated() method
    - Added markFreed() method
    - Added getMemoryLeaks() method

  • utils/SymbolTable.cpp
    - Implemented memory leak tracking methods

  • semantic/SemanticAnalyzer.h
    - Added scopeDepth tracking variable
    - Added processNew() method signature
    - Added processDelete() method signature
    - Added reportMemoryLeaks() method signature

  • semantic/SemanticAnalyzer.cpp
    - Implemented processNew() – handles 'new' keyword
    - Implemented processDelete() – handles 'delete' keyword
    - Updated processAssignment() – tracks allocations via 'new'
    - Updated analyze() – increments scopeDepth for scope tracking
    - Implemented reportMemoryLeaks() – reports all leaks
    - Integrated System 3 into main analysis loop

  • main.cpp
    - Updated banner to include System 3
    - Added System 3 demo test cases

COMPILATION:
-----------

Build with g++:
  $ g++ -std=c++17 -Wall -Wextra -I. -o compiler *.cpp lexer/*.cpp \
        parser/*.cpp utils/*.cpp semantic/*.cpp

Or using provided Makefile (Linux/macOS):
  $ make clean && make

CONCLUSION:
-----------

System 3 successfully implements basic compile-time memory leak detection.
It provides a solid foundation for identifying unreleased heap allocations
in C++ code, making it an essential component of the Extended C++ Safety
Compiler.
