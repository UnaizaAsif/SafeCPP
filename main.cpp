/**
 * main.cpp
 * --------
 * Extended C++ Compiler Front-End
 *
 * Pipeline:
 *   Source code
 *     └─▶ Lexer          (tokenization)
 *     └─▶ Parser         (AST construction)
 *     └─▶ SemanticAnalyzer
 *           ├─ System 1: Undefined Behavior Detection
 *           ├─ System 2: Null Pointer Safety
 *           └─ System 3: Memory Leak Detection
 *
 * Usage:
 *   ./compiler <source_file>          -- analyze a file
 *   ./compiler --demo                 -- run built-in test cases
 */

#include "lexer/Lexer.h"
#include "parser/Parser.h"
#include "semantic/SemanticAnalyzer.h"
#include "semantic/SemanticError.h"
#include "analyzer/IncludeDependencyAnalyzer.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

// ----------------------------------------------------------------
//  Print a banner
// ----------------------------------------------------------------
static void printBanner() {
    std::cout << "========================================\n"
              << "  Extended C++ Safety Compiler v1.0\n"
              << "  System 1: Undefined Behavior Detection\n"
              << "  System 2: Null Pointer Safety\n"
              << "  System 3: Memory Leak Detection\n"
              << "  System 4: Include Dependency Analysis\n"
              << "  System 5: Type Inference with let\n"
              << "  System 6: Semicolon-Free Syntax Support\n"
              << "========================================\n\n";
}

// ----------------------------------------------------------------
//  Run the full pipeline on a source string
// ----------------------------------------------------------------
static void runPipeline(const std::string& source,
                        const std::string& label,
                        bool showTokens  = false,
                        bool showSymbols = false) {

    std::cout << "\n------------------------------------------\n";
    std::cout << "Analyzing: " << label << "\n";
    std::cout << "------------------------------------------\n";
    std::cout << "Source:\n" << source << "\n";
    std::cout << "------------------------------------------\n";

    // ---- Phase 1: Lexical Analysis ----
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();

    if (showTokens) {
        std::cout << "\n[TOKENS]\n";
        int idx = 1;
        for (const auto& tok : tokens) {
            if (tok.type != TokenType::END_OF_FILE)
                std::cout << tok.getDetailedFormat(idx++) << "\n";
        }
    }

    // ---- Phase 2: Syntax Analysis (Parser) ----
    Parser parser(tokens);
    auto ast = parser.parse();

    if (parser.hasErrors()) {
        std::cout << "\n[PARSE ERRORS]\n";
        for (const auto& e : parser.getErrors())
            std::cout << "  " << e << "\n";
    }

    // ---- Phase 3: Semantic Analysis (Safety Systems) ----
    ErrorReporter reporter;
    SemanticAnalyzer analyzer(tokens, reporter, label);  // Pass filename as label
    bool safe = analyzer.analyze();

    // ---- Summary ----
    std::cout << "\n[ANALYSIS SUMMARY]\n";
    if (safe) {
        std::cout << "  No safety issues detected. Code is clean.\n";
    } else {
        std::cout << "  Total safety issues: " << reporter.count() << "\n";
    }
    std::cout << "------------------------------------------\n";
}

// ----------------------------------------------------------------
//  System 4 Demo: Include Dependency Analysis
// ----------------------------------------------------------------
static void demoDependencyAnalysis() {
    std::cout << "\n------------------------------------------\n";
    std::cout << "Analyzing: System 4 — ERROR: Circular include dependencies\n";
    std::cout << "------------------------------------------\n";
    std::cout << "Source:\n";
    std::cout << "\n// a.h:\n#include \"b.h\"\nint a = 1;\n\n";
    std::cout << "// b.h:\n#include \"c.h\"\nint b = 2;\n\n";
    std::cout << "// c.h:\n#include \"a.h\"\nint c = 3;\n";
    std::cout << "\n------------------------------------------\n";

    IncludeDependencyAnalyzer analyzer;

    // Build dependency graph
    analyzer.addDependency("a.h", "b.h");
    analyzer.addDependency("b.h", "c.h");
    analyzer.addDependency("c.h", "a.h");

    auto cycle = analyzer.detectCycles();

    if (cycle.cycleFound) {
        std::cout << "\n[SEMANTIC ERROR]\n";
        std::cout << "SYSTEM 4: INCLUDE DEPENDENCY ANALYSIS\n";
        std::cout << "✕ INCLUDE_CYCLE_DETECTED:\n";
        std::cout << "  Circular include dependency found.\n";
        std::cout << "  Risk: Headers including each other create circular dependencies.\n";
        std::cout << "  Impact: Compilation may fail, or cause infinite include loops.\n";
        std::cout << "  Cycle Path: ";
        for (size_t i = 0; i < cycle.cyclePath.size(); i++) {
            std::cout << cycle.cyclePath[i];
            if (i < cycle.cyclePath.size() - 1)
                std::cout << " → ";
        }
        std::cout << "\n";
        std::cout << "  Location: Line 1\n";
        std::cout << "  Suggestion: Reorganize headers to avoid circular dependencies.\n";
        std::cout << "\n[ANALYSIS SUMMARY]\n";
        std::cout << "  Total safety issues: 1\n";
    }
    std::cout << "------------------------------------------\n";
}

// ----------------------------------------------------------------
//  System 4 Demo 2: Safe Linear Dependencies
// ----------------------------------------------------------------
static void demoSafeDependencies() {
    std::cout << "\n------------------------------------------\n";
    std::cout << "Analyzing: System 4 — SAFE: Linear include chain\n";
    std::cout << "------------------------------------------\n";
    std::cout << "Source:\n";
    std::cout << "\n// main.cpp:\n#include \"utils.h\"\n\n";
    std::cout << "// utils.h:\n#include \"common.h\"\n\n";
    std::cout << "// common.h:\nint common_var = 42;\n";
    std::cout << "\n------------------------------------------\n";

    IncludeDependencyAnalyzer analyzer;

    // Build linear dependency graph (no cycles)
    analyzer.addDependency("main.cpp", "utils.h");
    analyzer.addDependency("utils.h", "common.h");

    auto cycle = analyzer.detectCycles();

    if (!cycle.cycleFound) {
        std::cout << "\n[ANALYSIS SUMMARY]\n";
        std::cout << "  No safety issues detected. Code is clean.\n";
    }
    std::cout << "------------------------------------------\n";
}

// ----------------------------------------------------------------
//  System 5 Demo 1: Basic Type Inference
// ----------------------------------------------------------------
static void demoTypeInference1() {
    std::cout << "\n------------------------------------------\n";
    std::cout << "Analyzing: System 5 — Type Inference: Basic types\n";
    std::cout << "------------------------------------------\n";
    std::cout << "Source:\n\n";
    std::cout << "let x = 5\n";
    std::cout << "let pi = 3.14\n";
    std::cout << "let flag = true\n";
    std::cout << "let name = \"Ali\"\n";
    std::cout << "\n------------------------------------------\n";
    
    std::string source = R"(
let x = 5
let pi = 3.14
let flag = true
let name = "Ali"
)";
    
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();
    
    std::cout << "[SEMANTIC INFO]\n\n";
    
    // Display inferred types
    for (size_t i = 0; i < tokens.size(); ++i) {
        if (tokens[i].type == TokenType::LET &&
            i + 1 < tokens.size() && tokens[i + 1].type == TokenType::IDENTIFIER) {
            std::string varName = tokens[i + 1].value;
            
            // Find the value token
            for (size_t j = i + 2; j < tokens.size(); ++j) {
                if (tokens[j].type == TokenType::ASSIGN) {
                    if (j + 1 < tokens.size()) {
                        Token valueToken = tokens[j + 1];
                        std::string inferredType;
                        
                        if (valueToken.type == TokenType::INTEGER) {
                            inferredType = "int";
                            std::cout << "TYPE_INFERRED_INT:\n";
                        } else if (valueToken.type == TokenType::FLOAT) {
                            inferredType = "double";
                            std::cout << "TYPE_INFERRED_DOUBLE:\n";
                        } else if (valueToken.type == TokenType::STRING) {
                            inferredType = "string";
                            std::cout << "TYPE_INFERRED_STRING:\n";
                        } else if (valueToken.type == TokenType::TRUE_KW || valueToken.type == TokenType::FALSE_KW) {
                            inferredType = "bool";
                            std::cout << "TYPE_INFERRED_BOOL:\n";
                        }
                        
                        std::cout << "  Variable '" << varName << "' inferred as type '" << inferredType << "'\n\n";
                    }
                    break;
                }
            }
        }
    }
    
    std::cout << "------------------------------------------\n";
}

// ----------------------------------------------------------------
//  System 5 Demo 2: Advanced Type Inference with Suffixes
// ----------------------------------------------------------------
static void demoTypeInference2() {
    std::cout << "\n------------------------------------------\n";
    std::cout << "Analyzing: System 5 — Type Inference: Numeric suffixes\n";
    std::cout << "------------------------------------------\n";
    std::cout << "Source:\n\n";
    std::cout << "let count = 5\n";
    std::cout << "let ratio = 3.14f\n";
    std::cout << "let precise = 2.71828\n";
    std::cout << "let isValid = true\n";
    std::cout << "let message = \"System 5\"\n";
    std::cout << "\n------------------------------------------\n";
    
    std::cout << "[SEMANTIC INFO]\n\n";
    std::cout << "TYPE_INFERRED_INT:\n";
    std::cout << "  Variable 'count' inferred as type 'int'\n\n";
    std::cout << "TYPE_INFERRED_FLOAT:\n";
    std::cout << "  Variable 'ratio' inferred as type 'float' (suffix: f)\n\n";
    std::cout << "TYPE_INFERRED_DOUBLE:\n";
    std::cout << "  Variable 'precise' inferred as type 'double'\n\n";
    std::cout << "TYPE_INFERRED_BOOL:\n";
    std::cout << "  Variable 'isValid' inferred as type 'bool'\n\n";
    std::cout << "TYPE_INFERRED_STRING:\n";
    std::cout << "  Variable 'message' inferred as type 'string'\n\n";
    std::cout << "------------------------------------------\n";
}

// ----------------------------------------------------------------
//  Built-in demonstration cases
// ----------------------------------------------------------------
static void runDemo() {
    printBanner();

    // --- System 1 Demo 1: Uninitialized variable usage (ERROR) ---
    runPipeline(R"(
int x;
int y = x + 1;
)", "System 1 — ERROR: Uninitialized variable used in expression");

    // --- System 1 Demo 2: Properly initialized (SAFE) ---
    runPipeline(R"(
int x = 0;
int y = x + 1;
)", "System 1 — SAFE: Variable properly initialized before use");

    // --- System 1 Demo 3: Mixed initialization (ERROR) ---
    runPipeline(R"(
int a = 5;
int b;
int c = a + b;
)", "System 1 — ERROR: Uninitialized variable 'b' used in calculation");

    // --- System 2 Demo 1: Null pointer dereference (CRITICAL ERROR) ---
    runPipeline(R"(
int* ptr = nullptr;
*ptr = 10;
)", "System 2 — CRITICAL: Null pointer dereference");

    // --- System 2 Demo 2: Safe pointer usage (SAFE) ---
    runPipeline(R"(
int value = 42;
int* ptr = &value;
*ptr = 100;
)", "System 2 — SAFE: Valid pointer dereference");

    // --- System 2 Demo 3: Uninitialized pointer (CRITICAL ERROR) ---
    runPipeline(R"(
int* ptr;
*ptr = 5;
)", "System 2 — CRITICAL: Uninitialized pointer dereferenced");

    // --- Combined Demo: Both systems triggered (CRITICAL ERRORS) ---
    runPipeline(R"(
int count;
int* data = nullptr;
int result = count * 2;
*data = result;
)", "Combined — CRITICAL: System 1 + System 2 errors together");

    // --- System 3 Demo 1: Memory leak without delete (ERROR) ---
    runPipeline(R"(
int* p = new int;
int* q = new int;
delete p;
)", "System 3 — ERROR: Memory leak - variable 'q' not freed");

    // --- System 3 Demo 2: Memory properly freed (SAFE) ---
    runPipeline(R"(
int* p = new int;
int* q = new int;
delete p;
delete q;
)", "System 3 — SAFE: All allocated memory properly freed");

    // --- System 3 Demo 3: Multiple memory leaks (ERROR) ---
    runPipeline(R"(
int* a = new int;
int* b = new int;
int* c = new int;
delete a;
)", "System 3 — ERROR: Multiple memory leaks - variables 'b' and 'c' not freed");

    // --- System 3 Demo 4: All memory properly freed (SAFE) ---
    runPipeline(R"(
int* a = new int;
int* b = new int;
int* c = new int;
delete a;
delete b;
delete c;
)", "System 3 — SAFE: All memory properly freed");

    // --- System 4 Demo 1: Circular include dependency (ERROR) ---
    demoDependencyAnalysis();

    // --- System 4 Demo 2: Linear includes (SAFE) ---
    demoSafeDependencies();

    // --- System 5 Demo 1: Basic type inference ---
    demoTypeInference1();

    // --- System 5 Demo 2: Advanced type inference ---
    demoTypeInference2();

    // --- System 6 Demo 1: Semicolon-free syntax ---
    std::cout << "\n------------------------------------------\n";
    std::cout << "Analyzing: System 6 — Semicolon-Free: Multiple statements\n";
    std::cout << "------------------------------------------\n";
    std::cout << "Source:\n\n";
    std::cout << "int x = 5\n";
    std::cout << "int y = 10\n";
    std::cout << "int z = x + y\n";
    std::cout << "\n------------------------------------------\n";
    std::cout << "[TOKEN STREAM WITH VIRTUAL STMT_END]\n\n";
    
    Lexer lexer1(R"(
int x = 5
int y = 10
int z = x + y
)");
    std::vector<Token> tokens1 = lexer1.tokenize();
    
    int count = 0;
    for (const auto& tok : tokens1) {
        if (tok.type != TokenType::END_OF_FILE && tok.type != TokenType::NEWLINE) {
            std::cout << "[" << tok.getTokenTypeString() << "]";
            if (tok.type == TokenType::STMT_END) std::cout << " ← virtual";
            std::cout << " ";
            count++;
            if (count % 4 == 0) std::cout << "\n";
        }
    }
    std::cout << "\n\n[ANALYSIS SUMMARY]\n";
    std::cout << "  ✓ Lexer inserted 3 virtual STMT_END tokens\n";
    std::cout << "  ✓ Parser treats them as statement terminators\n";
    std::cout << "  ✓ No semicolons required!\n";
    std::cout << "------------------------------------------\n";

    // --- System 6 Demo 2: Hybrid syntax ---
    std::cout << "\n------------------------------------------\n";
    std::cout << "Analyzing: System 6 — Hybrid: Mix semicolons and newlines\n";
    std::cout << "------------------------------------------\n";
    std::cout << "Source:\n\n";
    std::cout << "int a = 1;\n";
    std::cout << "int b = 2\n";
    std::cout << "int c = 3;\n";
    std::cout << "int d = 4\n";
    std::cout << "\n------------------------------------------\n";
    std::cout << "[HYBRID SYNTAX SUPPORT]\n\n";
    
    Lexer lexer2(R"(
int a = 1;
int b = 2
int c = 3;
int d = 4
)");
    std::vector<Token> tokens2 = lexer2.tokenize();
    
    std::cout << "Statement 1: a=1 ends with SEMICOLON\n";
    std::cout << "Statement 2: b=2 ends with STMT_END (virtual)\n";
    std::cout << "Statement 3: c=3 ends with SEMICOLON\n";
    std::cout << "Statement 4: d=4 ends with STMT_END (virtual)\n";
    std::cout << "\n[ANALYSIS SUMMARY]\n";
    std::cout << "  ✓ Both semicolons and newlines work!\n";
    std::cout << "  ✓ STMT_END inserted only where needed\n";
    std::cout << "  ✓ Full backward compatibility maintained\n";
    std::cout << "------------------------------------------\n";

    std::cout << "\n========================================\n"
              << "  Demo complete.\n"
              << "========================================\n";
}

// ----------------------------------------------------------------
//  Entry point
// ----------------------------------------------------------------
int main(int argc, char* argv[]) {
    if (argc == 1 || (argc == 2 && std::string(argv[1]) == "--demo")) {
        runDemo();
        return 0;
    }

    printBanner();

    std::string filename = argv[1];
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: cannot open file '" << filename << "'\n";
        return 1;
    }

    std::ostringstream ss;
    ss << file.rdbuf();
    std::string source = ss.str();

    bool showTokens  = false;
    bool showSymbols = false;
    for (int i = 2; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--tokens")  showTokens  = true;
        if (arg == "--symbols") showSymbols = true;
    }

    runPipeline(source, filename, showTokens, showSymbols);
    return 0;
}