/**
 * CORRECTED SAFECPP: Stateful Compiler Frontend with Global Variable Tracking
 * 
 * Design Principle: Safety analysis is ALWAYS ACTIVE for all code.
 * Every variable is tracked automatically, independent of special keywords.
 * 
 * This file demonstrates the CORRECT approach to undefined behavior detection.
 */

#include "SafetyAnalyzer.h"
#include <iostream>
#include <algorithm>
#include <set>
#include <sstream>
#include <regex>
#include <iomanip>

// ========== CORRECTED: Diagnostic Message Builder ==========

class DiagnosticMessage {
private:
    std::string severity;      // [ERROR], [WARNING], [NOTE]
    int line;
    std::string rule;
    std::string problem;
    std::string impact;
    std::string suggestion;

public:
    DiagnosticMessage(const std::string& sev, int l, const std::string& r,
                      const std::string& p, const std::string& i, const std::string& s)
        : severity(sev), line(l), rule(r), problem(p), impact(i), suggestion(s) {}

    std::string format() const {
        std::ostringstream oss;
        
        oss << "\n" << std::string(70, '=') << "\n";
        oss << "[" << severity << "] Line " << line << ": " << problem << "\n";
        oss << std::string(70, '-') << "\n";
        
        oss << "Rule: " << rule << "\n\n";
        
        oss << "Impact:\n  " << impact << "\n\n";
        
        oss << "Suggestion:\n  " << suggestion << "\n";
        
        oss << std::string(70, '=') << "\n";
        
        return oss.str();
    }
};

// ========== CORRECTED: Automatic Undefined Behavior Detection ==========

/**
 * SYSTEM 1: AUTOMATIC UNDEFINED BEHAVIOR DETECTION
 * 
 * NO SPECIAL KEYWORD REQUIRED - Every variable is tracked.
 */
void performUndefinedBehaviorAnalysis(const std::map<std::string, std::shared_ptr<Symbol>>& symbols) {
    
    for (const auto& [varName, symbol] : symbols) {
        
        // ❌ Case 1: Variable declared but not initialized, then used
        if (!symbol->initialized && symbol->isUsed) {
            
            DiagnosticMessage diag(
                "ERROR",
                symbol->usageLine,
                "Uninitialized variable must not be read",
                "Variable '" + varName + "' is used before initialization",
                "Reading uninitialized memory results in undefined behavior. "
                "Values are unpredictable, may differ across runs, "
                "or lead to system-dependent behavior and crashes.",
                "Initialize '" + varName + "' at declaration: "
                "int " + varName + " = 0;"
            );
            
            std::cout << diag.format();
        }
    }
}

/**
 * SYSTEM 2: AUTOMATIC NULL POINTER DEREFERENCE DETECTION
 * 
 * NO SPECIAL KEYWORD REQUIRED - Pointer types are detected automatically.
 */
void performNullPointerAnalysis(const std::map<std::string, std::shared_ptr<Symbol>>& symbols) {
    
    for (const auto& [varName, symbol] : symbols) {
        
        // ❌ Case 1: Pointer dereferenced without null check
        if (symbol->isPointer && symbol->isPotentiallyNull && 
            symbol->isUsed && !symbol->nullCheckPerformed) {
            
            DiagnosticMessage diag(
                "ERROR",
                symbol->usageLine,
                "Pointer must be validated before dereference",
                "Pointer '" + varName + "' is dereferenced without null check",
                "Invalid pointer dereference causes segmentation fault "
                "or memory corruption, crashing the program immediately.",
                "Add null check before dereferencing: "
                "if (" + varName + " != nullptr) { ... }"
            );
            
            std::cout << diag.format();
        }
    }
}

/**
 * SYSTEM 3: AUTOMATIC MEMORY LEAK DETECTION
 * 
 * NO SPECIAL KEYWORD REQUIRED - new/malloc are detected automatically.
 */
void performMemoryLeakAnalysis(const std::map<std::string, std::shared_ptr<Symbol>>& symbols) {
    
    for (const auto& [varName, symbol] : symbols) {
        
        // ⚠️ Case 1: Memory allocated but never freed
        if (symbol->isAllocated && !symbol->isDeallocated) {
            
            DiagnosticMessage diag(
                "WARNING",
                symbol->allocationLine,
                "All allocated memory must be deallocated",
                "Memory allocated to '" + varName + "' is never freed",
                "Continuous memory leaks exhaust system resources, "
                "causing performance degradation, application slowdown, "
                "or eventual out-of-memory crash in long-running programs.",
                "Add deallocation: delete " + varName + "; "
                "or use smart pointers (unique_ptr, shared_ptr)"
            );
            
            std::cout << diag.format();
        }
    }
}

/**
 * SYSTEM 4: AUTOMATIC LOOP-BASED MEMORY LEAK DETECTION
 * 
 * NO SPECIAL KEYWORD REQUIRED - Loop context is tracked automatically.
 */
void performLoopLeakAnalysis(const std::map<std::string, std::shared_ptr<Symbol>>& symbols,
                             int loopDepth) {
    
    if (loopDepth == 0) return;  // Not in a loop
    
    for (const auto& [varName, symbol] : symbols) {
        
        // ⚠️ Case 1: Memory allocated in loop without deallocation
        if (symbol->isAllocated && !symbol->isDeallocated && loopDepth > 0) {
            
            int estimatedLeaks = loopDepth * 1000;  // Heuristic: assume 1000 iterations
            
            DiagnosticMessage diag(
                "CRITICAL",
                symbol->allocationLine,
                "Memory allocated in loops must be freed in each iteration",
                "Memory allocated to '" + varName + "' inside loop (depth " + 
                std::to_string(loopDepth) + ") is never freed",
                "Loop-based memory leaks multiply rapidly. "
                "Each iteration allocates memory without cleanup, "
                "causing exponential memory growth (~" + std::to_string(estimatedLeaks) + 
                " leaks if loop runs 1000x), leading to system crash.",
                "Add deallocation inside loop: "
                "delete " + varName + "; at loop end, "
                "or restructure to avoid repeated allocations."
            );
            
            std::cout << diag.format();
        }
    }
}

/**
 * SYSTEM 5: AUTOMATIC TYPE INFERENCE (let keyword)
 * 
 * Advanced feature: Automatically infer types from initializers.
 */
void performTypeInferenceAnalysis(const std::map<std::string, std::shared_ptr<Symbol>>& symbols) {
    
    for (const auto& [varName, symbol] : symbols) {
        if (symbol->type == "auto" || symbol->type == "let") {
            
            std::cout << "\n[NOTE] Line " << symbol->line 
                      << ": Type inferred for '" << varName << "'\n";
        }
    }
}

// ========== Main Corrected Analysis ==========

void runCorrectedSafetyAnalysis(const std::map<std::string, std::shared_ptr<Symbol>>& symbols,
                                int loopDepth = 0) {
    
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "SAFECPP: STATEFUL COMPILER FRONTEND ANALYSIS\n";
    std::cout << "Principle: All variables tracked automatically, always-on analysis\n";
    std::cout << std::string(70, '=') << "\n";
    
    std::cout << "\n[SYSTEM 1] Undefined Behavior Detection\n";
    performUndefinedBehaviorAnalysis(symbols);
    
    std::cout << "\n[SYSTEM 2] Null Pointer Safety\n";
    performNullPointerAnalysis(symbols);
    
    std::cout << "\n[SYSTEM 3] Memory Leak Detection\n";
    performMemoryLeakAnalysis(symbols);
    
    std::cout << "\n[SYSTEM 4] Loop-Based Leak Detection\n";
    performLoopLeakAnalysis(symbols, loopDepth);
    
    std::cout << "\n[SYSTEM 5] Type Inference\n";
    performTypeInferenceAnalysis(symbols);
    
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "Analysis complete. All variables checked, all safety rules enforced.\n";
    std::cout << std::string(70, '=') << "\n\n";
}
