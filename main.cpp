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

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

// ----------------------------------------------------------------
//  Print a banner
// ----------------------------------------------------------------
static void printBanner() {
    std::cout << "========================================\n"
              << "  Extended C++ Safety Compiler v1.0\n"
              << "  System 1: Undefined Behavior Detection\n"
              << "  System 2: Null Pointer Safety\n"
              << "  System 3: Memory Leak Detection\n"
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
    SemanticAnalyzer analyzer(tokens, reporter);
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