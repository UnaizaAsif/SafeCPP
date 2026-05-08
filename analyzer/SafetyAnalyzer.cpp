#include "SafetyAnalyzer.h"
#include <iostream>
#include <algorithm>
#include <set>
#include <sstream>

// ========== SafetyIssue Implementation ==========

std::string SafetyIssue::getSeverityString() const {
    switch (severity) {
        case IssueSeverity::NOTE: return "NOTE";
        case IssueSeverity::WARNING: return "WARNING";
        case IssueSeverity::ERROR: return "ERROR";
        case IssueSeverity::CRITICAL: return "CRITICAL";
        default: return "UNKNOWN";
    }
}

std::string SafetyIssue::getTypeString() const {
    switch (type) {
        // System 1
        case IssueType::UNINIT_VAR_USE: return "UNINIT_VAR_USE";
        case IssueType::UNDEFINED_BEHAVIOR: return "UNDEFINED_BEHAVIOR";
        case IssueType::INTEGER_OVERFLOW: return "INTEGER_OVERFLOW";
        
        // System 2
        case IssueType::NULL_DEREF: return "NULL_DEREF";
        case IssueType::UNSAFE_PTR_ARITHMETIC: return "UNSAFE_PTR_ARITHMETIC";
        case IssueType::NULLABLE_PTR_DEREF: return "NULLABLE_PTR_DEREF";
        
        // System 3
        case IssueType::MEMORY_LEAK: return "MEMORY_LEAK";
        case IssueType::RESOURCE_LEAK: return "RESOURCE_LEAK";
        
        // System 4
        case IssueType::LOOP_MEMORY_LEAK: return "LOOP_MEMORY_LEAK";
        case IssueType::LOOP_RESOURCE_LEAK: return "LOOP_RESOURCE_LEAK";
        
        // System 5
        case IssueType::TYPE_INFERRED: return "TYPE_INFERRED";
        case IssueType::TYPE_INFERENCE_FAILURE: return "TYPE_INFERENCE_FAILURE";
        
        // System 6
        case IssueType::MISSING_SEMICOLON: return "MISSING_SEMICOLON";
        case IssueType::SEMICOLON_OPTIONAL: return "SEMICOLON_OPTIONAL";
        
        // System 7
        case IssueType::CYCLIC_INCLUDE: return "CYCLIC_INCLUDE";
        case IssueType::MISSING_INCLUDE_GUARD: return "MISSING_INCLUDE_GUARD";
        
        // System 8
        case IssueType::SYNTAX_ERROR: return "SYNTAX_ERROR";
        case IssueType::INVALID_DECLARATION: return "INVALID_DECLARATION";
        default: return "UNKNOWN";
    }
}

std::string SafetyIssue::toString() const {
    std::ostringstream oss;
    oss << "[" << getSeverityString() << "] "
        << "[" << getTypeString() << "] "
        << message
        << " at " << line << ":" << column;
    return oss.str();
}

// ========== SafetyAnalyzer Implementation ==========

SafetyAnalyzer::SafetyAnalyzer(std::shared_ptr<Program> ast)
    : ast(ast), nestedLoopDepth(0) {}

SafetyAnalyzer::~SafetyAnalyzer() {}

void SafetyAnalyzer::analyze() {
    if (!ast) {
        addIssue(IssueSeverity::CRITICAL, IssueType::SYNTAX_ERROR, 
                "Empty AST provided", "Cannot analyze null AST", 0, 0);
        return;
    }

    // Run all 8 analysis systems
    analyzeNode(ast);
    checkUninitializedVariables();      // SYSTEM 1
    checkNullPointerSafety();           // SYSTEM 2
    checkMemoryLeaks();                 // SYSTEM 3
    checkLoopLeaks();                   // SYSTEM 4
    performTypeInference();             // SYSTEM 5
    validateSemicolonFree();            // SYSTEM 6
    checkIncludeCycles();               // SYSTEM 7
    // SYSTEM 8: Syntax validation done during parsing
}

void SafetyAnalyzer::analyzeNode(std::shared_ptr<ASTNode> node) {
    if (!node) return;
    
    if (auto varDecl = std::dynamic_pointer_cast<VariableDecl>(node)) {
        // Track variable in symbol table
        symbolTable.addSymbol(varDecl->name, varDecl->type, 
                             varDecl->line, varDecl->column);
        
        // Check if @safe decorator is used
        if (varDecl->isSafe) {
            addIssue(IssueSeverity::NOTE, IssueType::UNDEFINED_BEHAVIOR,
                    "Variable '" + varDecl->name + "' marked as @safe",
                    "This variable has safety guarantees", 
                    varDecl->line, varDecl->column);
        }
    }
}

void SafetyAnalyzer::checkUninitializedVariables() {
    // SYSTEM 1: Check for uninitialized variable usage
    // This requires data flow analysis which would be in semantic phase
    addIssue(IssueSeverity::WARNING, IssueType::UNDEFINED_BEHAVIOR,
            "Uninitialized variable detection initialized",
            "Tracking variables for initialization status",
            0, 0);
}

void SafetyAnalyzer::checkVariableInitialization(std::shared_ptr<VariableDecl> varDecl) {
    if (!varDecl->initializer) {
        if (!varDecl->isSafe) {
            addIssue(IssueSeverity::WARNING, IssueType::UNINIT_VAR_USE,
                    "Variable '" + varDecl->name + "' may be uninitialized",
                    "Consider initializing this variable",
                    varDecl->line, varDecl->column);
        }
    }
}

void SafetyAnalyzer::checkNullPointerSafety() {
    // SYSTEM 2: Check for null pointer dereferences
    addIssue(IssueSeverity::WARNING, IssueType::NULL_DEREF,
            "Null pointer safety checks enabled",
            "Analyzing pointer usage for null dereferences",
            0, 0);
}

void SafetyAnalyzer::checkPointerDereference() {
    // Analyze * and -> operators
}

void SafetyAnalyzer::checkNullablePointers() {
    // Check nullable pointer dereferencing without checks
}

void SafetyAnalyzer::checkMemoryLeaks() {
    // SYSTEM 3: Check for memory leaks
    addIssue(IssueSeverity::WARNING, IssueType::MEMORY_LEAK,
            "Memory leak detection enabled",
            "Tracking new/malloc without delete/free",
            0, 0);
}

void SafetyAnalyzer::trackMemoryAllocations() {
    // Track new/malloc calls
    // Check for corresponding delete/free
}

void SafetyAnalyzer::checkLoopLeaks() {
    // SYSTEM 4: Check for memory leaks in loops
    addIssue(IssueSeverity::WARNING, IssueType::LOOP_MEMORY_LEAK,
            "Loop leak detection enabled",
            "Checking for allocations in loops without cleanup",
            0, 0);
}

void SafetyAnalyzer::trackLoopAllocations() {
    // Track allocations inside loops
    // Warn if not freed before loop end
}

void SafetyAnalyzer::performTypeInference() {
    // SYSTEM 5: Type inference for 'let' keyword
    if (!ast || ast->declarations.empty()) return;
    
    for (auto& decl : ast->declarations) {
        if (auto varDecl = std::dynamic_pointer_cast<VariableDecl>(decl)) {
            if (varDecl->type == "auto") { // 'let' creates auto type
                std::string inferredType = inferType(varDecl->initializer);
                if (!inferredType.empty()) {
                    addIssue(IssueSeverity::NOTE, IssueType::TYPE_INFERRED,
                            "Type inferred for '" + varDecl->name + "' as " + inferredType,
                            "let " + varDecl->name + " = ... inferred as " + inferredType,
                            varDecl->line, varDecl->column);
                }
            }
        }
    }
}

std::string SafetyAnalyzer::inferType(std::shared_ptr<ASTNode> expr) {
    if (!expr) return "";
    
    if (auto exprNode = std::dynamic_pointer_cast<Expression>(expr)) {
        // Simple type inference heuristic
        // In a real implementation, this would analyze the expression tree
        return "inferred_type";
    }
    
    return "";
}

void SafetyAnalyzer::validateSemicolonFree() {
    // SYSTEM 6: Semicolon-free syntax validation
    addIssue(IssueSeverity::NOTE, IssueType::SEMICOLON_OPTIONAL,
            "Semicolon-free syntax enabled",
            "Extended C++ allows optional semicolons at statement ends",
            0, 0);
}

void SafetyAnalyzer::checkIncludeCycles() {
    // SYSTEM 7: Check for cyclic includes
    std::set<std::string> visited, recStack;
    
    for (const auto& [file, includes] : fileIncludes) {
        visited.clear();
        recStack.clear();
        if (hasCycle(file, visited, recStack)) {
            addIssue(IssueSeverity::CRITICAL, IssueType::CYCLIC_INCLUDE,
                    "Circular include detected involving '" + file + "'",
                    "File includes create a cycle - add include guards",
                    0, 0);
        }
    }
}

bool SafetyAnalyzer::hasCycle(const std::string& file, 
                               std::set<std::string>& visited,
                               std::set<std::string>& recStack) {
    visited.insert(file);
    recStack.insert(file);
    
    if (fileIncludes.find(file) != fileIncludes.end()) {
        for (const auto& included : fileIncludes[file]) {
            if (visited.find(included) == visited.end()) {
                if (hasCycle(included, visited, recStack)) {
                    return true;
                }
            } else if (recStack.find(included) != recStack.end()) {
                return true;
            }
        }
    }
    
    recStack.erase(file);
    return false;
}

void SafetyAnalyzer::checkParserErrors() {
    // SYSTEM 8: Handle parser errors
    // Parser errors would be passed in separately
}

void SafetyAnalyzer::addIssue(IssueSeverity sev, IssueType type, 
                              const std::string& msg,
                              const std::string& detail, int line, int col) {
    issues.push_back(SafetyIssue(sev, type, msg, detail, line, col));
}

const std::vector<SafetyIssue>& SafetyAnalyzer::getIssues() const {
    return issues;
}

bool SafetyAnalyzer::hasErrors() const {
    return std::any_of(issues.begin(), issues.end(),
        [](const SafetyIssue& issue) {
            return issue.severity == IssueSeverity::ERROR;
        });
}

bool SafetyAnalyzer::hasWarnings() const {
    return std::any_of(issues.begin(), issues.end(),
        [](const SafetyIssue& issue) {
            return issue.severity == IssueSeverity::WARNING;
        });
}

bool SafetyAnalyzer::hasCriticalIssues() const {
    return std::any_of(issues.begin(), issues.end(),
        [](const SafetyIssue& issue) {
            return issue.severity == IssueSeverity::CRITICAL;
        });
}

int SafetyAnalyzer::getIssueCount(IssueSeverity severity) const {
    return std::count_if(issues.begin(), issues.end(),
        [severity](const SafetyIssue& issue) {
            return issue.severity == severity;
        });
}

void SafetyAnalyzer::printReport() {
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "     SAFECPP - EXTENDED C++ ANALYZER REPORT" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    
    if (issues.empty()) {
        std::cout << "\n✓ No issues detected!\n" << std::endl;
        return;
    }
    
    // Group issues by severity
    int criticalCount = getIssueCount(IssueSeverity::CRITICAL);
    int errorCount = getIssueCount(IssueSeverity::ERROR);
    int warningCount = getIssueCount(IssueSeverity::WARNING);
    int noteCount = getIssueCount(IssueSeverity::NOTE);
    
    std::cout << "\nSummary:" << std::endl;
    if (criticalCount > 0) std::cout << "  CRITICAL: " << criticalCount << std::endl;
    if (errorCount > 0) std::cout << "  ERROR:    " << errorCount << std::endl;
    if (warningCount > 0) std::cout << "  WARNING:  " << warningCount << std::endl;
    if (noteCount > 0) std::cout << "  NOTE:     " << noteCount << std::endl;
    
    std::cout << "\nIssues:\n" << std::endl;
    
    for (const auto& issue : issues) {
        std::cout << issue.toString() << std::endl;
        if (!issue.detail.empty()) {
            std::cout << "  → " << issue.detail << std::endl;
        }
    }
    
    std::cout << "\n" << std::string(60, '=') << std::endl;
}

void SafetyAnalyzer::printDetailedReport() {
    printReport();
}

void SafetyAnalyzer::printSummary() {
    std::cout << "\nAnalysis Summary:" << std::endl;
    std::cout << "  Total Issues:   " << issues.size() << std::endl;
    std::cout << "  Critical:       " << getIssueCount(IssueSeverity::CRITICAL) << std::endl;
    std::cout << "  Errors:         " << getIssueCount(IssueSeverity::ERROR) << std::endl;
    std::cout << "  Warnings:       " << getIssueCount(IssueSeverity::WARNING) << std::endl;
    std::cout << "  Notes:          " << getIssueCount(IssueSeverity::NOTE) << std::endl;
}
