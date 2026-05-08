#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <string>
#include <map>
#include <vector>
#include <memory>
#include <set>

/**
 * Symbol Structure
 * Represents a declared identifier with type and scope information
 */
struct Symbol {
    std::string name;
    std::string type;
    std::string scope;
    int line;
    int column;
    bool isUsed;
    bool isSafe;
    bool isNullable;

    Symbol(const std::string& n, const std::string& t, const std::string& s, int l, int c)
        : name(n), type(t), scope(s), line(l), column(c), isUsed(false), isSafe(false), isNullable(false) {}

    std::string toString() const;
};

/**
 * Symbol Table
 * Manages scoped symbol information for semantic analysis
 */
class SymbolTable {
private:
    std::vector<std::map<std::string, std::shared_ptr<Symbol>>> scopeStack;
    std::string currentScope;

public:
    SymbolTable();
    ~SymbolTable();

    // Scope management
    void pushScope(const std::string& scopeName);
    void popScope();

    // Symbol operations
    void addSymbol(const std::string& name, const std::string& type, int line, int column);
    bool findSymbol(const std::string& name) const;
    std::shared_ptr<Symbol> getSymbol(const std::string& name);
    
    // Usage tracking
    void markSymbolUsed(const std::string& name);
    
    // Safety attributes
    void markSymbolSafe(const std::string& name);
    void markSymbolNullable(const std::string& name);
    
    // Reporting
    void printSymbols();
    void printUnusedSymbols();
    void printScope(const std::string& scopeName);
};

#endif // SYMBOL_TABLE_H
