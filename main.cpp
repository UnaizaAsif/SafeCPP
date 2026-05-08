/**
 * =========================================================================
 * SafeCPP: Extended C++ Lexical and Syntax Analyzer
 * =========================================================================
 * 
 * A compiler front-end that analyzes Extended C++ code for:
 *  1. Lexical Correctness   (Token generation)
 *  2. Syntax Correctness    (Grammar validation)
 *  3. Safety Issues         (UB, Null, Leaks, etc.)
 * 
 * COMPILATION PIPELINE:
 * 
 *   Source Code
 *      ↓
 *   [LEXICAL ANALYSIS] → Tokenize input, recognize keywords & operators
 *      ↓
 *   Token Stream
 *      ↓
 *   [SYNTAX ANALYSIS] → Build AST, validate grammar, detect parse errors
 *      ↓
 *   Abstract Syntax Tree (AST)
 *      ↓
 *   [SEMANTIC ANALYSIS] → Perform 8 safety analysis systems
 *      ↓
 *   Analysis Report
 * 
 * =========================================================================
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include "lexer/Lexer.h"
#include "parser/Parser.h"
#include "analyzer/SafetyAnalyzer.h"

/**
 * Read source code from file
 */
std::string readSourceFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filename);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

/**
 * Print analysis header
 */
void printHeader() {
    std::cout << "\n" << std::string(75, '=') << std::endl;
    std::cout << std::string(15, ' ') << "SafeCPP: Extended C++ Analyzer" << std::endl;
    std::cout << std::string(10, ' ') << "Lexical + Syntax + Safety Analysis" << std::endl;
    std::cout << std::string(75, '=') << std::endl;
}

/**
 * Print phase header
 */
void printPhaseHeader(int phase, const std::string& name) {
    std::cout << "\n" << std::string(75, '-') << std::endl;
    std::cout << "[PHASE " << phase << "] " << name << std::endl;
    std::cout << std::string(75, '-') << std::endl;
}

/**
 * Main analysis function
 */
void analyzeFile(const std::string& filename) {
    try {
        printHeader();
        
        std::cout << "\nFile: " << filename << std::endl;
        
        // ===== PHASE 1: LEXICAL ANALYSIS =====
        printPhaseHeader(1, "LEXICAL ANALYSIS (Tokenization)");
        
        std::string source = readSourceFile(filename);
        std::cout << "Source code loaded: " << source.length() << " characters" << std::endl;
        
        Lexer lexer(source);
        auto tokens = lexer.tokenize();
        
        std::cout << "\n✓ Tokenization complete!" << std::endl;
        std::cout << "  Tokens generated: " << tokens.size() << std::endl;
        
        // Display all tokens in detailed format (ALL tokens including NEWLINE and EOF)
        std::cout << "\nToken stream (all " << tokens.size() << " tokens):" << std::endl;
        int tokenNumber = 1;
        for (const auto& token : tokens) {
            std::cout << token.getDetailedFormat(tokenNumber) << std::endl;
            tokenNumber++;
        }
        
        // ===== PHASE 2: SYNTAX ANALYSIS =====
        printPhaseHeader(2, "SYNTAX ANALYSIS (Parsing)");
        
        Parser parser(tokens);
        auto ast = parser.parse();
        
        if (parser.hasErrors()) {
            std::cout << "\n✗ Parser errors detected:" << std::endl;
            for (const auto& error : parser.getErrors()) {
                std::cout << "  • " << error << std::endl;
            }
        } else {
            std::cout << "\n✓ Syntax validation complete!" << std::endl;
            std::cout << "  AST generated successfully" << std::endl;
            
            if (ast && ast->declarations.size() > 0) {
                std::cout << "  Declarations found: " << ast->declarations.size() << std::endl;
                for (size_t i = 0; i < ast->declarations.size(); i++) {
                    std::cout << "    [" << i << "] " << ast->declarations[i]->getNodeType() << std::endl;
                }
            }
        }
        
        // ===== PHASE 3: SEMANTIC ANALYSIS (Safety Analysis) =====
        printPhaseHeader(3, "SEMANTIC ANALYSIS (Safety Analysis)");
        
        SafetyAnalyzer analyzer(ast);
        analyzer.analyze();
        
        std::cout << "\n✓ Analysis systems initialized:" << std::endl;
        std::cout << "  System 1: @safe UB Detection" << std::endl;
        std::cout << "  System 2: Null Pointer Safety" << std::endl;
        std::cout << "  System 3: Memory Leak Detection" << std::endl;
        std::cout << "  System 4: Loop Leak Detection" << std::endl;
        std::cout << "  System 5: let Type Inference" << std::endl;
        std::cout << "  System 6: Semicolon-Free Syntax" << std::endl;
        std::cout << "  System 7: Include Dependency Cycles" << std::endl;
        std::cout << "  System 8: Syntax Analyzer/Parser" << std::endl;
        
        // ===== ANALYSIS RESULTS =====
        printPhaseHeader(0, "ANALYSIS RESULTS");
        
        analyzer.printReport();
        analyzer.printSummary();
        
        // ===== FINAL STATUS =====
        std::cout << "\n" << std::string(75, '=') << std::endl;
        
        if (analyzer.hasCriticalIssues()) {
            std::cout << "ANALYSIS STATUS: ✗ CRITICAL ISSUES FOUND" << std::endl;
            std::cout << "Action: Review and fix critical issues before proceeding." << std::endl;
        } else if (analyzer.hasErrors()) {
            std::cout << "ANALYSIS STATUS: ✗ ERRORS FOUND" << std::endl;
            std::cout << "Action: Fix errors in source code." << std::endl;
        } else if (analyzer.hasWarnings()) {
            std::cout << "ANALYSIS STATUS: ⚠  WARNINGS DETECTED" << std::endl;
            std::cout << "Action: Review warnings and consider improvements." << std::endl;
        } else {
            std::cout << "ANALYSIS STATUS: ✓ PASSED" << std::endl;
            std::cout << "Action: Code is safe and ready for compilation." << std::endl;
        }
        
        std::cout << std::string(75, '=') << std::endl << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "\n[FATAL ERROR] " << e.what() << std::endl;
        std::cerr << std::string(75, '=') << std::endl << std::endl;
    }
}

/**
 * Main entry point
 */
int main(int argc, char* argv[]) {
    std::cout << std::endl;
    
    if (argc < 2) {
        std::cout << "SafeCPP: Extended C++ Lexical and Syntax Analyzer\n" << std::endl;
        std::cout << "Usage: " << argv[0] << " <source_file> [source_file2 ...]\n" << std::endl;
        
        std::cout << "Examples:\n" << std::endl;
        std::cout << "  " << argv[0] << " samples/demo1.cpp" << std::endl;
        std::cout << "  " << argv[0] << " samples/demo2.cpp" << std::endl;
        std::cout << "  " << argv[0] << " samples/full_demo.cpp\n" << std::endl;
        
        std::cout << "Features:\n" << std::endl;
        std::cout << "  • Lexical Analysis: Tokenizes source code\n"
                 << "  • Syntax Analysis: Validates grammar and builds AST\n"
                 << "  • Safety Analysis: 8 independent safety systems\n" << std::endl;
        
        std::cout << "8 Analysis Systems:\n" << std::endl;
        std::cout << "  1. @safe UB Detection - Detect undefined behavior\n"
                 << "  2. Null Pointer Safety - Find null pointer issues\n"
                 << "  3. Memory Leak Detection - Track memory allocations\n"
                 << "  4. Loop Leak Detection - Find leaks in loops\n"
                 << "  5. let Type Inference - Infer types automatically\n"
                 << "  6. Semicolon-Free Syntax - Support modern syntax\n"
                 << "  7. Include Dependency Cycles - Detect circular includes\n"
                 << "  8. Syntax Analyzer/Parser - Validate grammar\n" << std::endl;
        
        return 0;
    }
    
    // Process each file
    for (int i = 1; i < argc; i++) {
        analyzeFile(argv[i]);
        
        if (i < argc - 1) {
            std::cout << "\n" << std::endl;
        }
    }
    
    return 0;
}
