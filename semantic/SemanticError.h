#ifndef SEMANTIC_ERROR_H
#define SEMANTIC_ERROR_H

#include <string>
#include <vector>
#include <iostream>

enum class ErrorKind {
    UNINITIALIZED_USE,      // System 1 – undefined behavior
    NULL_DEREF,             // System 2 – null pointer dereference
    MAYBE_NULL_DEREF,       // System 2 – possible null dereference
    UNKNOWN_SYMBOL,         // used but never declared
    MEMORY_LEAK,            // System 3 – memory not freed
    LOOP_MEMORY_LEAK,       // System 3 – memory allocated in loop without delete
    INCLUDE_CYCLE,          // System 4 – circular include dependency
    TYPE_INFERENCE_ERROR,   // System 5 – type inference issue
    SYNTAX_ERROR,           // System 6 – syntax error
};

struct SemanticError {
    ErrorKind   kind;
    std::string variable;
    int         line;
    int         column;
    std::string suggestion;

    // Pretty-print in the required compiler-style format
    void print() const {
        std::cout << "\n[SEMANTIC ERROR]\n";
        switch (kind) {
            case ErrorKind::UNINITIALIZED_USE:
                std::cout << "SYSTEM 1: UNDEFINED BEHAVIOR DETECTION\n"
                          << "  Variable '" << variable << "' was never initialized.\n"
                          << "  Risk: Reading garbage memory leads to unpredictable behavior.\n"
                          << "  Location: Line " << line << ", Column " << column << "\n";
                break;
            case ErrorKind::NULL_DEREF:
                std::cout << " SYSTEM 2: NULL POINTER SAFETY\n"
                          << "  Pointer '" << variable << "' dereferenced without null check..\n"
                          << "  Risk: System Crash / Segmentation fault / Memory access violation.\n"
                          << "  Location: Line " << line << ", Column " << column << "\n";
                break;
            case ErrorKind::MAYBE_NULL_DEREF:
                std::cout << "SYSTEM 2: NULL POINTER SAFETY\n"
                          << "  Pointer '" << variable << "' may be null.\n"
                          << "  If null is dereferenced here, the program will crash.\n"
                          << "  Risk: System Crash / Potential segmentation fault / Memory access violation.\n"
                          << "  Location: Line " << line << ", Column " << column << "\n";
                break;
            case ErrorKind::UNKNOWN_SYMBOL:
                std::cout << "SYSTEM 1: UNDEFINED BEHAVIOR DETECTION\n"
                          << "  Symbol '" << variable << "' was never declared.\n"
                          << "  Risk: Using undefined symbols leads to undefined behavior.\n"
                          << "  Location: Line " << line << ", Column " << column << "\n";
                break;
            case ErrorKind::MEMORY_LEAK:
                std::cout << "SYSTEM 3: MEMORY LEAK DETECTION\n"
                          << "  Memory allocated to '" << variable << "' is never released..\n"
                          << "  Risk: Memory allocated with 'new' is never released.\n"
                          << "  Location: Line " << line << ", Column " << column << "\n";
                break;
            case ErrorKind::LOOP_MEMORY_LEAK:
                std::cout << "SYSTEM 3: MEMORY LEAK DETECTION\n"
                          << "  Memory allocated inside loop without deletion in same scope.\n"
                          << "  Risk: Memory allocated in loop without corresponding 'delete'.\n"
                          << "  Location: Line " << line << ", Column " << column << "\n";
                break;
            case ErrorKind::INCLUDE_CYCLE:
                std::cout << "SYSTEM 4: INCLUDE DEPENDENCY ANALYSIS\n"
                          << "  Circular include dependency found.\n"
                          << "  Risk: Headers including each other create circular dependencies.\n"
                          << "  Impact: Compilation may fail or create infinite include chains.\n"
                          << "  Cycle Path: " << variable << "\n"
                          << "  Location: Line " << line << ", Column " << column << "\n";
                break;
            case ErrorKind::TYPE_INFERENCE_ERROR:
                std::cout << "SYSTEM 5: TYPE INFERENCE WITH let\n"
                          << "  Failed to infer type for variable '" << variable << "'.\n"
                          << "  Risk: Compiler cannot determine correct datatype.\n"
                          << "  Location: Line " << line << ", Column " << column << "\n";
                break;
            case ErrorKind::SYNTAX_ERROR:
                std::cout << "SYSTEM 6: SEMICOLON-FREE SYNTAX\n"
                          << "  Invalid statement structure near '" << variable << "'.\n"
                          << "  Risk: Statement boundary could not be determined.\n"
                          << "  Location: Line " << line << ", Column " << column << "\n";
                break;
        }
    }
};

// Collector passed around the analysis phases
class ErrorReporter {
public:
    void report(SemanticError err) {
        errors.push_back(err);
        err.print();
    }

    bool hasErrors() const { return !errors.empty(); }
    int  count()     const { return (int)errors.size(); }

    const std::vector<SemanticError>& all() const { return errors; }

private:
    std::vector<SemanticError> errors;
};

// ============================================================
//  System 5 Type Inference Info Message
// ============================================================
struct System5Info {
    std::string inferredType;  // e.g., "int", "double", "string"
    std::string variable;      // variable name
    std::string typeToken;     // e.g., "TYPE_INFERRED_INT"

    void print() const {
        std::cout << "\n[SEMANTIC INFO]\n";
        std::cout << typeToken << ":\n";
        std::cout << "Variable '" << variable << "' inferred as type '" << inferredType << "'\n";
    }
};

// ============================================================
//  System 6 Syntax Info Message
// ============================================================
struct System6Info {
    std::string message;  // e.g., "Implicit statement terminator inserted"
    std::string token;    // e.g., "STMT_END"

    void print() const {
        std::cout << "\n[SYNTAX INFO]\n";
        std::cout << message << ".\n\n";
        std::cout << "Virtual Token:\n";
        std::cout << token << "\n";
    }
};

#endif // SEMANTIC_ERROR_H