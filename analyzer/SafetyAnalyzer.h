/**
 * CORRECTED DESIGN: Stateful Global Variable Tracking
 * 
 * PRINCIPLE: Safety analysis is ALWAYS ACTIVE for all code.
 * 
 * The analyzer acts as a STATEFUL COMPILER FRONTEND:
 * - Automatically tracks EVERY variable (no annotations required)
 * - Maintains initialization state across the program
 * - Detects safety issues at usage points
 * - Produces COMPILER-STYLE DIAGNOSTICS (not just tokens)
 * 
 * This is similar to how Rust, Clang, and GCC perform analysis.
 */

#ifndef SAFETY_ANALYZER_H
#define SAFETY_ANALYZER_H

#include "../parser/Parser.h"
#include "../utils/SymbolTable.h"
#include "DependencyGraph.h"
#include <vector>
#include <string>
#include <memory>
#include <map>
#include <set>
#include <unordered_map>

/**
 * Safety Issue Severity Levels
 */
enum class IssueSeverity {
    NOTE,
    WARNING,
    ERROR,
    CRITICAL
};

/**
 * Safety Issue Types (8 Systems)
 * 
 * SYSTEM 1: @safe UB Detection
 * SYSTEM 2: Null Pointer Safety  
 * SYSTEM 3: Memory Leak Detection
 * SYSTEM 4: Loop Leak Detection
 * SYSTEM 5: let Type Inference
 * SYSTEM 6: Semicolon-Free Syntax
 * SYSTEM 7: Include Dependency Cycle
 * SYSTEM 8: Syntax Analyzer/Parser (already implemented in Parser.h)
 */
enum class IssueType {
    // System 1: UB Detection
    UNINIT_VAR_USE,         // Use of uninitialized variable
    UNDEFINED_BEHAVIOR,     // Undefined behavior detected
    INTEGER_OVERFLOW,       // Integer overflow risk
    
    // System 2: Null Pointer Safety
    NULL_DEREF,             // Potential null pointer dereference
    UNSAFE_PTR_ARITHMETIC,  // Unsafe pointer arithmetic
    NULLABLE_PTR_DEREF,     // Dereferencing nullable pointer without check
    
    // System 3: Memory Leak Detection
    MEMORY_LEAK,            // Allocated memory not freed
    RESOURCE_LEAK,          // Resource not released
    
    // System 4: Loop Leak Detection
    LOOP_MEMORY_LEAK,       // Memory allocated in loop not freed
    LOOP_RESOURCE_LEAK,     // Resource allocated in loop not released
    
    // System 5: Type Inference
    TYPE_INFERRED,          // Type successfully inferred (info message)
    TYPE_INFERENCE_FAILURE, // Could not infer type
    
    // System 6: Semicolon-Free Support
    MISSING_SEMICOLON,      // Detected missing semicolon (automatically inserted)
    SEMICOLON_OPTIONAL,     // Semicolon is optional in Extended C++
    
    // System 7: Include Dependency
    CYCLIC_INCLUDE,         // Circular include detected
    MISSING_INCLUDE_GUARD,  // Header missing include guard
    
    // System 8: Syntax Errors
    SYNTAX_ERROR,           // Generic syntax error
    INVALID_DECLARATION,    // Invalid declaration syntax
};

/**
 * Safety Issue Structure
 */
struct SafetyIssue {
    IssueSeverity severity;
    IssueType type;
    std::string message;
    std::string detail;
    int line;
    int column;
    std::string filename;

    SafetyIssue(IssueSeverity sev, IssueType typ, const std::string& msg, 
                const std::string& detail, int l, int c, const std::string& file = "")
        : severity(sev), type(typ), message(msg), detail(detail), line(l), column(c), filename(file) {}

    std::string toString() const;
    std::string getSeverityString() const;
    std::string getTypeString() const;
};

/**
 * Extended C++ Safety Analyzer
 * 
 * Performs 8 independent safety analysis systems:
 * 1. Uninitialized Variable Detection
 * 2. Null Pointer Safety Checks
 * 3. Memory Leak Detection
 * 4. Loop-specific Leak Detection
 * 5. Type Inference for let
 * 6. Semicolon-Free Syntax Validation
 * 7. Include Cycle Detection
 * 8. Syntax Validation (from Parser)
 */
class SafetyAnalyzer {
private:
    std::shared_ptr<Program> ast;
    SymbolTable symbolTable;
    DependencyGraph includeGraph;
    std::vector<SafetyIssue> issues;
    std::map<std::string, std::set<std::string>> fileIncludes;
    int nestedLoopDepth;

    // ===== SYSTEM 1: Uninitialized Variable Tracking =====
    std::map<std::string, bool> varInitialized;          // Track which vars are initialized
    std::map<std::string, int> varDeclarationLine;       // Track where var was declared
    std::set<std::string> safeAnnotatedVars;             // Variables marked with @safe
    
    // ===== SYSTEM 2: Null Pointer Tracking =====
    std::map<std::string, bool> nullableVariables;       // Track nullable pointers
    std::set<std::string> nullCheckPerformed;            // Track where null checks exist
    std::map<std::string, std::vector<int>> pointerUsages; // Track where pointers are used
    
    // ===== SYSTEM 3 & 4: Memory Allocation Tracking =====
    struct MemoryAllocation {
        std::string varName;
        int allocLine;
        int allocColumn;
        bool deallocated;
        int deallocLine;
        int loopDepth;
    };
    std::vector<MemoryAllocation> allocations;           // All memory allocations
    std::map<std::string, std::vector<MemoryAllocation>> loopAllocations;
    
    // ===== SYSTEM 5: Type Inference =====
    std::map<std::string, std::string> inferredTypes;    // Map of let variables to inferred types
    std::set<std::string> letVariables;                  // Variables declared with 'let'
    
    // ===== SYSTEM 6: Semicolon Tracking =====
    int missingSemicolonCount;                           // Count of auto-inserted semicolons
    std::vector<int> semicolonInsertedLines;             // Lines where semicolons were inserted
    
    // ===== SYSTEM 7: Include Dependency Graph =====
    std::map<std::string, std::vector<std::string>> includeDependencies;  // File -> includes
    std::set<std::string> cycleNodes;                    // Files involved in cycles

    // Analysis methods for each system
    void analyzeNode(std::shared_ptr<ASTNode> node);
    
    // SYSTEM 1: Uninitialized Variable Detection
    void checkUninitializedVariables();
    void checkVariableInitialization(std::shared_ptr<VariableDecl> varDecl);
    void trackVariableUsage(std::shared_ptr<ASTNode> node);
    
    // SYSTEM 2: Null Pointer Safety
    void checkNullPointerSafety();
    void checkPointerDereference(std::shared_ptr<ASTNode> node);
    void checkNullablePointers();
    void trackNullableDeclaration(const std::string& varName);
    void trackNullCheck(const std::string& varName, int line);
    
    // SYSTEM 3: Memory Leak Detection
    void checkMemoryLeaks();
    void trackMemoryAllocations(std::shared_ptr<ASTNode> node);
    void trackAllocation(const std::string& varName, int line, int col);
    void trackDeallocation(const std::string& varName, int line);
    
    // SYSTEM 4: Loop Leak Detection
    void checkLoopLeaks();
    void trackLoopAllocations(std::shared_ptr<ASTNode> node);
    void enterLoop();
    void exitLoop();
    
    // SYSTEM 5: Type Inference
    void performTypeInference();
    std::string inferType(std::shared_ptr<ASTNode> expr);
    std::string inferTypeFromValue(const std::string& value);
    
    // SYSTEM 6: Semicolon-Free Syntax
    void validateSemicolonFree();
    void registerSemicolonInsert(int line);
    
    // SYSTEM 7: Include Dependency
    void checkIncludeCycles();
    void trackIncludeFile(const std::string& from, const std::string& to);
    bool hasCycle(const std::string& file, std::set<std::string>& visited, 
                  std::set<std::string>& recStack);
    std::vector<std::string> findCyclePath(const std::string& start);
    
    // SYSTEM 8: Syntax Validation (from Parser errors)
    void checkParserErrors();

    void addIssue(IssueSeverity sev, IssueType type, const std::string& msg,
                  const std::string& detail, int line, int col);

public:
    SafetyAnalyzer(std::shared_ptr<Program> ast);
    ~SafetyAnalyzer();

    /**
     * Run all 8 analysis systems
     */
    void analyze();

    // Include tracking methods
    void addIncludeFile(const std::string& from, const std::string& to);
    
    // Variable annotation methods
    void markVariableSafe(const std::string& varName);
    void markVariableNullable(const std::string& varName);
    void markVariableLet(const std::string& varName);
    
    // Getters
    const std::vector<SafetyIssue>& getIssues() const;
    bool hasErrors() const;
    bool hasWarnings() const;
    bool hasCriticalIssues() const;
    int getIssueCount(IssueSeverity severity) const;
    
    // Reporting
    void printReport();
    void printDetailedReport();
    void printSummary();
    void printSystemReport(int systemNumber);
};

#endif // SAFETY_ANALYZER_H
