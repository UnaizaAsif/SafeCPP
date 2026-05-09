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
                          << "  Impact: Program may crash, produce wrong results, or seem to work\n"
                          << "          occasionally, making bugs very hard to track.\n"
                          << "  Location: Line " << line << ", Column " << column << "\n"
                          << "  Suggestion: " << suggestion << "\n";
                break;
            case ErrorKind::NULL_DEREF:
                std::cout << " SYSTEM 2: NULL POINTER SAFETY\n"
                          << "  Pointer '" << variable << "' is DEFINITELY NULL.\n"
                          << "  Attempting to dereference it (access *ptr) will cause immediate crash.\n"
                          << "  Risk: Segmentation fault / Memory access violation.\n"
                          << "  Impact: Program terminates abnormally. This is a CRITICAL bug.\n"
                          << "  Location: Line " << line << ", Column " << column << "\n"
                          << "  Suggestion: " << suggestion << "\n";
                break;
            case ErrorKind::MAYBE_NULL_DEREF:
                std::cout << "SYSTEM 2: NULL POINTER SAFETY\n"
                          << "  Pointer '" << variable << "' MAY BE NULL.\n"
                          << "  If null is dereferenced here, the program will crash.\n"
                          << "  Risk: Potential segmentation fault / Memory access violation.\n"
                          << "  Impact: Program may crash depending on runtime conditions.\n"
                          << "  Location: Line " << line << ", Column " << column << "\n"
                          << "  Suggestion: " << suggestion << "\n";
                break;
            case ErrorKind::UNKNOWN_SYMBOL:
                std::cout << "SYSTEM 1: UNDEFINED BEHAVIOR DETECTION\n"
                          << "  Symbol '" << variable << "' was never declared.\n"
                          << "  Risk: Using undefined symbols leads to undefined behavior.\n"
                          << "  Impact: Compiler or runtime error. Code is unsafe.\n"
                          << "  Location: Line " << line << ", Column " << column << "\n"
                          << "  Suggestion: " << suggestion << "\n";
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

#endif // SEMANTIC_ERROR_H