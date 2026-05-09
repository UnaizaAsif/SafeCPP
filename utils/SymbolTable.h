#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include <string>
#include <unordered_map>
#include <vector>

// ============================================================
//  Initialization state for System 1 (Undefined Behavior)
// ============================================================
enum class InitState {
    UNINITIALIZED,   // declared but never assigned
    INITIALIZED,     // assigned at least once
    MAYBE_INIT       // assigned inside a branch (conservative)
};

// ============================================================
//  Null state for System 2 (Null Pointer Safety)
// ============================================================
enum class NullState {
    NOT_A_POINTER,   // scalar type – not tracked
    NULL_PTR,        // assigned nullptr / NULL / 0
    NONNULL,         // assigned a non-null value (address, new, etc.)
    MAYBE_NULL,      // assigned in one branch but not another
    UNKNOWN          // pointer declared but never assigned
};

// ============================================================
//  One entry in the symbol table
// ============================================================
struct SymbolEntry {
    std::string name;
    std::string type;        // e.g. "int", "int*", "double*"
    int         declareLine;
    int         declareCol;

    // System 1
    InitState   initState   = InitState::UNINITIALIZED;
    int         assignLine  = -1;   // line of last assignment
    int         usageLine   = -1;   // line of first use

    // System 2
    bool        isPointer   = false;
    NullState   nullState   = NullState::NOT_A_POINTER;
    int         nullAssignLine = -1;
};

// ============================================================
//  Scoped symbol table (supports nested scopes)
// ============================================================
class SymbolTable {
public:
    // Push a new scope (entering { })
    void pushScope();

    // Pop current scope (leaving } )
    void popScope();

    // Declare a variable in the current scope.
    // Returns false if already declared in the same scope.
    bool declare(const std::string& name,
                 const std::string& type,
                 int line, int col);

    // Mark a variable as initialized/assigned.
    // Walks up scopes to find the symbol.
    bool markInitialized(const std::string& name, int line);

    // Mark a pointer as null.
    bool markNull(const std::string& name, int line);

    // Mark a pointer as non-null.
    bool markNonNull(const std::string& name, int line);

    // Mark a pointer as maybe-null (branch assignment).
    bool markMaybeNull(const std::string& name, int line);

    // Look up a symbol (searches from innermost scope outward).
    // Returns nullptr if not found.
    SymbolEntry* lookup(const std::string& name);

    // Is the variable initialized at point of use?
    // Also updates usageLine. Returns false → UB risk.
    bool checkInitialized(const std::string& name, int useLine);

    // Is this a pointer and in a null / unknown state?
    // Returns true → dereference is unsafe.
    bool checkNullDereference(const std::string& name);

    // Dump all symbols (for debug output)
    void dump() const;

private:
    // Each scope is a map from name → entry
    std::vector<std::unordered_map<std::string, SymbolEntry>> scopes;

    static bool isPointerType(const std::string& type);
};

#endif // SYMBOLTABLE_H