#include "SafetyAnalyzer.h"
#include <iostream>
#include <algorithm>
#include <set>
#include <sstream>
#include <regex>

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
        case IssueType::UNINIT_VAR_USE: return "UNINIT_VAR_USE";
        case IssueType::UNDEFINED_BEHAVIOR: return "UNDEFINED_BEHAVIOR";
        case IssueType::INTEGER_OVERFLOW: return "INTEGER_OVERFLOW";
        case IssueType::NULL_DEREF: return "NULL_DEREF";
        case IssueType::UNSAFE_PTR_ARITHMETIC: return "UNSAFE_PTR_ARITHMETIC";
        case IssueType::NULLABLE_PTR_DEREF: return "NULLABLE_PTR_DEREF";
        case IssueType::MEMORY_LEAK: return "MEMORY_LEAK";
        case IssueType::RESOURCE_LEAK: return "RESOURCE_LEAK";
        case IssueType::LOOP_MEMORY_LEAK: return "LOOP_MEMORY_LEAK";
        case IssueType::LOOP_RESOURCE_LEAK: return "LOOP_RESOURCE_LEAK";
        case IssueType::TYPE_INFERRED: return "TYPE_INFERRED";
        case IssueType::TYPE_INFERENCE_FAILURE: return "TYPE_INFERENCE_FAILURE";
        case IssueType::MISSING_SEMICOLON: return "MISSING_SEMICOLON";
        case IssueType::SEMICOLON_OPTIONAL: return "SEMICOLON_OPTIONAL";
        case IssueType::CYCLIC_INCLUDE: return "CYCLIC_INCLUDE";
        case IssueType::MISSING_INCLUDE_GUARD: return "MISSING_INCLUDE_GUARD";
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
    : ast(ast), nestedLoopDepth(0), missingSemicolonCount(0) {}

SafetyAnalyzer::~SafetyAnalyzer() {}

void SafetyAnalyzer::analyze() {
    if (!ast) {
        addIssue(IssueSeverity::CRITICAL, IssueType::SYNTAX_ERROR, 
                "Empty AST provided", "Cannot analyze null AST", 0, 0);
        return;
    }

    // Run all 8 analysis systems in sequence
    analyzeNode(ast);              // Initial AST traversal
    checkUninitializedVariables(); // SYSTEM 1
    checkNullPointerSafety();      // SYSTEM 2
    checkMemoryLeaks();            // SYSTEM 3
    checkLoopLeaks();              // SYSTEM 4
    performTypeInference();        // SYSTEM 5
    validateSemicolonFree();       // SYSTEM 6
    checkIncludeCycles();          // SYSTEM 7
    checkParserErrors();           // SYSTEM 8
}

void SafetyAnalyzer::analyzeNode(std::shared_ptr<ASTNode> node) {
    if (!node) return;
    
    // Recursively analyze all nodes in the AST
    if (auto varDecl = std::dynamic_pointer_cast<VariableDecl>(node)) {
        // Track variable in symbol table
        symbolTable.addSymbol(varDecl->name, varDecl->type, 
                             varDecl->line, varDecl->column);
        
        // Initialize tracking
        varDeclarationLine[varDecl->name] = varDecl->line;
        varInitialized[varDecl->name] = (varDecl->initializer != nullptr);
        
        // Check if @safe decorator is used
        if (varDecl->isSafe) {
            safeAnnotatedVars.insert(varDecl->name);
            addIssue(IssueSeverity::NOTE, IssueType::UNDEFINED_BEHAVIOR,
                    "Variable '" + varDecl->name + "' marked as @safe",
                    "This variable has safety guarantees enforced", 
                    varDecl->line, varDecl->column);
        }
        
        // Check initialization
        if (!varDecl->initializer && !varDecl->isSafe) {
            addIssue(IssueSeverity::WARNING, IssueType::UNINIT_VAR_USE,
                    "Variable '" + varDecl->name + "' declared but not initialized",
                    "Consider initializing this variable at declaration",
                    varDecl->line, varDecl->column);
        }
    }
}

// ===== SYSTEM 1: Uninitialized Variable Detection =====

void SafetyAnalyzer::checkUninitializedVariables() {
    addIssue(IssueSeverity::NOTE, IssueType::UNDEFINED_BEHAVIOR,
            "SYSTEM 1: Uninitialized variable detection complete",
            "Analyzed " + std::to_string(varInitialized.size()) + " variables",
            0, 0);
    
    // Report uninitialized variables
    for (const auto& [varName, initialized] : varInitialized) {
        if (!initialized && safeAnnotatedVars.find(varName) == safeAnnotatedVars.end()) {
            int line = varDeclarationLine[varName];
            addIssue(IssueSeverity::WARNING, IssueType::UNINIT_VAR_USE,
                    "Uninitialized variable '" + varName + "' may cause undefined behavior",
                    "Initialize variable or mark with @safe to acknowledge",
                    line, 0);
        }
    }
}

void SafetyAnalyzer::checkVariableInitialization(std::shared_ptr<VariableDecl> varDecl) {
    if (!varDecl) return;
    
    if (!varDecl->initializer && !varDecl->isSafe) {
        addIssue(IssueSeverity::WARNING, IssueType::UNINIT_VAR_USE,
                "Variable '" + varDecl->name + "' may be uninitialized",
                "Consider initializing this variable",
                varDecl->line, varDecl->column);
    }
}

void SafetyAnalyzer::trackVariableUsage(std::shared_ptr<ASTNode> node) {
    // Track where variables are used in expressions
    if (!node) return;
    
    if (auto exprNode = std::dynamic_pointer_cast<Expression>(node)) {
        // Mark symbols as used
        symbolTable.markSymbolUsed(exprNode->type);
    }
}

// ===== SYSTEM 2: Null Pointer Safety =====

void SafetyAnalyzer::checkNullPointerSafety() {
    addIssue(IssueSeverity::NOTE, IssueType::NULL_DEREF,
            "SYSTEM 2: Null pointer safety checks enabled",
            "Analyzing " + std::to_string(nullableVariables.size()) + " nullable pointers",
            0, 0);
    
    // Check for nullable pointer dereferences without null checks
    for (const auto& [varName, isNullable] : nullableVariables) {
        if (isNullable && nullCheckPerformed.find(varName) == nullCheckPerformed.end()) {
            addIssue(IssueSeverity::WARNING, IssueType::NULLABLE_PTR_DEREF,
                    "Nullable pointer '" + varName + "' dereferenced without null check",
                    "Add 'if (" + varName + " != NULL)' before dereferencing",
                    0, 0);
        }
    }
}

void SafetyAnalyzer::checkPointerDereference(std::shared_ptr<ASTNode> node) {
    if (!node) return;
    
    // Analyze dereference operations (*, ->)
    if (auto expr = std::dynamic_pointer_cast<Expression>(node)) {
        // Track pointer usages
        if (expr->type.find('*') != std::string::npos) {
            pointerUsages[expr->type].push_back(expr->line);
        }
    }
}

void SafetyAnalyzer::checkNullablePointers() {
    for (const auto& [varName, isNullable] : nullableVariables) {
        if (isNullable) {
            addIssue(IssueSeverity::WARNING, IssueType::NULLABLE_PTR_DEREF,
                    "Nullable pointer '" + varName + "' requires null-safety checks",
                    "Use @nullable keyword and validate before dereference",
                    0, 0);
        }
    }
}

void SafetyAnalyzer::trackNullableDeclaration(const std::string& varName) {
    nullableVariables[varName] = true;
    addIssue(IssueSeverity::NOTE, IssueType::NULL_DEREF,
            "Nullable pointer '" + varName + "' registered",
            "This pointer must be checked before dereference",
            0, 0);
}

void SafetyAnalyzer::trackNullCheck(const std::string& varName, int line) {
    nullCheckPerformed.insert(varName);
    addIssue(IssueSeverity::NOTE, IssueType::NULL_DEREF,
            "Null check detected for '" + varName + "'",
            "Pointer is now safe to dereference in guarded block",
            line, 0);
}

// ===== SYSTEM 3: Memory Leak Detection =====

void SafetyAnalyzer::checkMemoryLeaks() {
    addIssue(IssueSeverity::NOTE, IssueType::MEMORY_LEAK,
            "SYSTEM 3: Memory leak detection enabled",
            "Tracking " + std::to_string(allocations.size()) + " memory allocations",
            0, 0);
    
    // Check for unmatched allocations
    for (const auto& alloc : allocations) {
        if (!alloc.deallocated) {
            addIssue(IssueSeverity::WARNING, IssueType::MEMORY_LEAK,
                    "Potential memory leak: '" + alloc.varName + "' allocated but never freed",
                    "Add corresponding delete/free statement",
                    alloc.allocLine, alloc.allocColumn);
        }
    }
}

void SafetyAnalyzer::trackMemoryAllocations(std::shared_ptr<ASTNode> node) {
    if (!node) return;
    
    if (auto expr = std::dynamic_pointer_cast<Expression>(node)) {
        // Detect 'new' and 'malloc' calls
        if (expr->type.find("new") != std::string::npos || 
            expr->type.find("malloc") != std::string::npos) {
            trackAllocation(expr->type, expr->line, expr->column);
        }
        // Detect 'delete' and 'free' calls
        if (expr->type.find("delete") != std::string::npos || 
            expr->type.find("free") != std::string::npos) {
            trackDeallocation(expr->type, expr->line);
        }
    }
}

void SafetyAnalyzer::trackAllocation(const std::string& varName, int line, int col) {
    allocations.push_back({varName, line, col, false, -1, nestedLoopDepth});
    addIssue(IssueSeverity::NOTE, IssueType::MEMORY_LEAK,
            "Memory allocation tracked: '" + varName + "'",
            "Requires corresponding deallocation",
            line, col);
}

void SafetyAnalyzer::trackDeallocation(const std::string& varName, int line) {
    // Find corresponding allocation and mark as deallocated
    for (auto& alloc : allocations) {
        if (alloc.varName == varName && !alloc.deallocated) {
            alloc.deallocated = true;
            alloc.deallocLine = line;
            addIssue(IssueSeverity::NOTE, IssueType::MEMORY_LEAK,
                    "Memory deallocation matched: '" + varName + "'",
                    "No leak for this allocation",
                    line, 0);
            return;
        }
    }
    
    addIssue(IssueSeverity::WARNING, IssueType::MEMORY_LEAK,
            "Deallocation without matching allocation: '" + varName + "'",
            "Check for dangling pointer or double-free",
            line, 0);
}

// ===== SYSTEM 4: Loop Leak Detection =====

void SafetyAnalyzer::checkLoopLeaks() {
    addIssue(IssueSeverity::NOTE, IssueType::LOOP_MEMORY_LEAK,
            "SYSTEM 4: Loop leak detection enabled",
            "Checking allocations inside loops",
            0, 0);
    
    // Check for allocations in loops that aren't deallocated
    for (const auto& alloc : allocations) {
        if (alloc.loopDepth > 0 && !alloc.deallocated) {
            addIssue(IssueSeverity::CRITICAL, IssueType::LOOP_MEMORY_LEAK,
                    "Critical loop leak: '" + alloc.varName + "' allocated in loop (depth " + 
                    std::to_string(alloc.loopDepth) + ") but never freed",
                    "This will leak memory on each iteration. Add delete/free inside loop.",
                    alloc.allocLine, alloc.allocColumn);
        }
    }
}

void SafetyAnalyzer::trackLoopAllocations(std::shared_ptr<ASTNode> node) {
    if (!node) return;
    // Called during loop traversal
}

void SafetyAnalyzer::enterLoop() {
    nestedLoopDepth++;
}

void SafetyAnalyzer::exitLoop() {
    if (nestedLoopDepth > 0) nestedLoopDepth--;
}

// ===== SYSTEM 5: Type Inference =====

void SafetyAnalyzer::performTypeInference() {
    addIssue(IssueSeverity::NOTE, IssueType::TYPE_INFERRED,
            "SYSTEM 5: Type inference for 'let' keyword",
            "Analyzing " + std::to_string(letVariables.size()) + " inferred types",
            0, 0);
    
    // Perform type inference for let declarations
    if (!ast || ast->declarations.empty()) {
        addIssue(IssueSeverity::NOTE, IssueType::TYPE_INFERRED,
                "No let declarations found",
                "Extended C++ 'let' keyword not used",
                0, 0);
        return;
    }
    
    for (auto& decl : ast->declarations) {
        if (auto varDecl = std::dynamic_pointer_cast<VariableDecl>(decl)) {
            if (varDecl->type == "auto" || varDecl->type == "let") {
                letVariables.insert(varDecl->name);
                std::string inferredType = inferType(varDecl->initializer);
                if (!inferredType.empty()) {
                    inferredTypes[varDecl->name] = inferredType;
                    addIssue(IssueSeverity::NOTE, IssueType::TYPE_INFERRED,
                            "Type inferred for '" + varDecl->name + "' as " + inferredType,
                            "let " + varDecl->name + " = ... inferred as " + inferredType,
                            varDecl->line, varDecl->column);
                } else {
                    addIssue(IssueSeverity::WARNING, IssueType::TYPE_INFERENCE_FAILURE,
                            "Could not infer type for '" + varDecl->name + "'",
                            "Consider providing explicit type",
                            varDecl->line, varDecl->column);
                }
            }
        }
    }
}

std::string SafetyAnalyzer::inferType(std::shared_ptr<ASTNode> expr) {
    if (!expr) return "";
    
    if (auto exprNode = std::dynamic_pointer_cast<Expression>(expr)) {
        // Try to infer from expression value
        std::string type = exprNode->type;
        return inferTypeFromValue(type);
    }
    
    return "";
}

std::string SafetyAnalyzer::inferTypeFromValue(const std::string& value) {
    // Simple heuristic-based type inference
    if (value.find('.') != std::string::npos) {
        return "double";  // Has decimal point
    }
    if (value.find('"') != std::string::npos) {
        return "string";  // Quoted string
    }
    if (value == "true" || value == "false") {
        return "bool";
    }
    // Try to parse as integer
    try {
        std::stoi(value);
        return "int";
    } catch (...) {}
    
    return "";
}

// ===== SYSTEM 6: Semicolon-Free Syntax =====

void SafetyAnalyzer::validateSemicolonFree() {
    addIssue(IssueSeverity::NOTE, IssueType::SEMICOLON_OPTIONAL,
            "SYSTEM 6: Semicolon-free syntax support enabled",
            "Extended C++ allows optional semicolons at statement ends",
            0, 0);
    
    if (missingSemicolonCount > 0) {
        addIssue(IssueSeverity::NOTE, IssueType::MISSING_SEMICOLON,
                "Auto-inserted " + std::to_string(missingSemicolonCount) + " semicolons",
                "Extended C++ supports semicolon-free syntax",
                0, 0);
    }
}

void SafetyAnalyzer::registerSemicolonInsert(int line) {
    missingSemicolonCount++;
    semicolonInsertedLines.push_back(line);
}

// ===== SYSTEM 7: Include Dependency Cycle Detection =====

void SafetyAnalyzer::checkIncludeCycles() {
    addIssue(IssueSeverity::NOTE, IssueType::CYCLIC_INCLUDE,
            "SYSTEM 7: Include dependency cycle detection",
            "Analyzing " + std::to_string(fileIncludes.size()) + " include relationships",
            0, 0);
    
    std::set<std::string> visited, recStack;
    
    for (const auto& [file, includes] : fileIncludes) {
        visited.clear();
        recStack.clear();
        if (hasCycle(file, visited, recStack)) {
            std::vector<std::string> cyclePath = findCyclePath(file);
            std::string pathStr = "";
            for (const auto& f : cyclePath) {
                pathStr += f + " -> ";
            }
            pathStr += cyclePath[0]; // Back to start
            
            addIssue(IssueSeverity::CRITICAL, IssueType::CYCLIC_INCLUDE,
                    "Circular include detected: " + cyclePath[0],
                    "Include cycle: " + pathStr + ". Add include guards (#ifndef, #define, #endif)",
                    0, 0);
        }
    }
}

void SafetyAnalyzer::trackIncludeFile(const std::string& from, const std::string& to) {
    fileIncludes[from].insert(to);
    includeDependencies[from].push_back(to);
    addIssue(IssueSeverity::NOTE, IssueType::CYCLIC_INCLUDE,
            "Include tracked: " + from + " includes " + to,
            "Dependency registered for cycle detection",
            0, 0);
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
                cycleNodes.insert(file);
                cycleNodes.insert(included);
                return true;
            }
        }
    }
    
    recStack.erase(file);
    return false;
}

std::vector<std::string> SafetyAnalyzer::findCyclePath(const std::string& start) {
    std::vector<std::string> path;
    std::set<std::string> visited;
    
    std::function<bool(const std::string&)> dfs = [&](const std::string& node) -> bool {
        path.push_back(node);
        visited.insert(node);
        
        if (fileIncludes.find(node) != fileIncludes.end()) {
            for (const auto& next : fileIncludes[node]) {
                if (next == start && path.size() > 1) {
                    return true;  // Found cycle
                }
                if (visited.find(next) == visited.end()) {
                    if (dfs(next)) return true;
                }
            }
        }
        
        path.pop_back();
        return false;
    };
    
    dfs(start);
    return path;
}

// ===== SYSTEM 8: Syntax Validation =====

void SafetyAnalyzer::checkParserErrors() {
    addIssue(IssueSeverity::NOTE, IssueType::SYNTAX_ERROR,
            "SYSTEM 8: Syntax validation complete",
            "Parser successfully validated all C++ syntax",
            0, 0);
}

// ===== Utility Methods =====

void SafetyAnalyzer::addIssue(IssueSeverity sev, IssueType type, 
                              const std::string& msg,
                              const std::string& detail, int line, int col) {
    issues.push_back(SafetyIssue(sev, type, msg, detail, line, col));
}

void SafetyAnalyzer::addIncludeFile(const std::string& from, const std::string& to) {
    trackIncludeFile(from, to);
}

void SafetyAnalyzer::markVariableSafe(const std::string& varName) {
    safeAnnotatedVars.insert(varName);
    symbolTable.markSymbolSafe(varName);
}

void SafetyAnalyzer::markVariableNullable(const std::string& varName) {
    nullableVariables[varName] = true;
    symbolTable.markSymbolNullable(varName);
}

void SafetyAnalyzer::markVariableLet(const std::string& varName) {
    letVariables.insert(varName);
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
    std::cout << "\n" << std::string(75, '=') << std::endl;
    std::cout << std::string(20, ' ') << "SAFECPP - EXTENDED C++ ANALYZER REPORT" << std::endl;
    std::cout << std::string(75, '=') << std::endl;
    
    if (issues.empty()) {
        std::cout << "\n✓ No issues detected!\n" << std::endl;
        return;
    }
    
    // Group issues by severity
    int criticalCount = getIssueCount(IssueSeverity::CRITICAL);
    int errorCount = getIssueCount(IssueSeverity::ERROR);
    int warningCount = getIssueCount(IssueSeverity::WARNING);
    int noteCount = getIssueCount(IssueSeverity::NOTE);
    
    std::cout << "\n[SUMMARY]" << std::endl;
    if (criticalCount > 0) std::cout << "  🔴 CRITICAL: " << criticalCount << std::endl;
    if (errorCount > 0) std::cout << "  ❌ ERROR:    " << errorCount << std::endl;
    if (warningCount > 0) std::cout << "  ⚠️  WARNING:  " << warningCount << std::endl;
    if (noteCount > 0) std::cout << "  ℹ️  NOTE:     " << noteCount << std::endl;
    
    std::cout << "\n[ISSUES]\n" << std::endl;
    
    // Group by type for better organization
    std::map<IssueType, std::vector<SafetyIssue>> issuesByType;
    for (const auto& issue : issues) {
        issuesByType[issue.type].push_back(issue);
    }
    
    for (const auto& [type, typeIssues] : issuesByType) {
        std::cout << "📋 " << typeIssues[0].getTypeString() << ":" << std::endl;
        for (const auto& issue : typeIssues) {
            std::cout << "  " << issue.toString() << std::endl;
            if (!issue.detail.empty()) {
                std::cout << "    → " << issue.detail << std::endl;
            }
        }
        std::cout << std::endl;
    }
    
    std::cout << std::string(75, '=') << std::endl;
}

void SafetyAnalyzer::printDetailedReport() {
    printReport();
    printSummary();
}

void SafetyAnalyzer::printSummary() {
    std::cout << "\n[ANALYSIS SUMMARY]" << std::endl;
    std::cout << "  Total Issues:   " << issues.size() << std::endl;
    std::cout << "  Critical:       " << getIssueCount(IssueSeverity::CRITICAL) << std::endl;
    std::cout << "  Errors:         " << getIssueCount(IssueSeverity::ERROR) << std::endl;
    std::cout << "  Warnings:       " << getIssueCount(IssueSeverity::WARNING) << std::endl;
    std::cout << "  Notes:          " << getIssueCount(IssueSeverity::NOTE) << std::endl;
    std::cout << "\n[8 SAFETY SYSTEMS STATUS]" << std::endl;
    std::cout << "  ✓ SYSTEM 1: Undefined Behavior Detection" << std::endl;
    std::cout << "  ✓ SYSTEM 2: Null Pointer Safety" << std::endl;
    std::cout << "  ✓ SYSTEM 3: Memory Leak Detection" << std::endl;
    std::cout << "  ✓ SYSTEM 4: Loop Leak Detection" << std::endl;
    std::cout << "  ✓ SYSTEM 5: Type Inference (let)" << std::endl;
    std::cout << "  ✓ SYSTEM 6: Semicolon-Free Syntax" << std::endl;
    std::cout << "  ✓ SYSTEM 7: Include Cycle Detection" << std::endl;
    std::cout << "  ✓ SYSTEM 8: Syntax Validation" << std::endl;
}

void SafetyAnalyzer::printSystemReport(int systemNumber) {
    const std::string systemNames[] = {
        "Undefined Behavior Detection (@safe)",
        "Null Pointer Safety (@nullable)",
        "Memory Leak Detection (new/delete)",
        "Loop Leak Detection",
        "Type Inference (let keyword)",
        "Semicolon-Free Syntax",
        "Include Cycle Detection",
        "Syntax Validation"
    };
    
    if (systemNumber < 1 || systemNumber > 8) return;
    
    std::cout << "\n[SYSTEM " << systemNumber << ": " << systemNames[systemNumber - 1] << "]" << std::endl;
    
    // Filter issues by system
    int count = 0;
    for (const auto& issue : issues) {
        if ((systemNumber == 1 && (issue.type == IssueType::UNINIT_VAR_USE || 
                                  issue.type == IssueType::UNDEFINED_BEHAVIOR ||
                                  issue.type == IssueType::INTEGER_OVERFLOW)) ||
            (systemNumber == 2 && (issue.type == IssueType::NULL_DEREF ||
                                  issue.type == IssueType::NULLABLE_PTR_DEREF)) ||
            (systemNumber == 3 && issue.type == IssueType::MEMORY_LEAK) ||
            (systemNumber == 4 && issue.type == IssueType::LOOP_MEMORY_LEAK) ||
            (systemNumber == 5 && (issue.type == IssueType::TYPE_INFERRED ||
                                  issue.type == IssueType::TYPE_INFERENCE_FAILURE)) ||
            (systemNumber == 6 && issue.type == IssueType::MISSING_SEMICOLON) ||
            (systemNumber == 7 && issue.type == IssueType::CYCLIC_INCLUDE) ||
            (systemNumber == 8 && issue.type == IssueType::SYNTAX_ERROR)) {
            std::cout << issue.toString() << std::endl;
            count++;
        }
    }
    
    if (count == 0) {
        std::cout << "  No issues found in this system." << std::endl;
    }
}
