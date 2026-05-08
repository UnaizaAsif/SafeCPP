#include "SymbolTable.h"
#include <iostream>
#include <algorithm>

std::string Symbol::toString() const {
    std::string attrs;
    if (isSafe) attrs += "[SAFE]";
    if (isNullable) attrs += "[NULLABLE]";
    
    return "Symbol: " + name + " (" + type + ") in " + scope + 
           " " + attrs +
           " [" + std::to_string(line) + ":" + std::to_string(column) + "]";
}

SymbolTable::SymbolTable() {
    pushScope("global");
}

SymbolTable::~SymbolTable() {}

void SymbolTable::pushScope(const std::string& scopeName) {
    scopeStack.push_back(std::map<std::string, std::shared_ptr<Symbol>>());
    currentScope = scopeName;
}

void SymbolTable::popScope() {
    if (!scopeStack.empty()) {
        scopeStack.pop_back();
    }
    currentScope = "global";
}

void SymbolTable::addSymbol(const std::string& name, const std::string& type, int line, int column) {
    if (!scopeStack.empty()) {
        auto symbol = std::make_shared<Symbol>(name, type, currentScope, line, column);
        scopeStack.back()[name] = symbol;
    }
}

bool SymbolTable::findSymbol(const std::string& name) const {
    for (auto it = scopeStack.rbegin(); it != scopeStack.rend(); ++it) {
        if (it->find(name) != it->end()) {
            return true;
        }
    }
    return false;
}

std::shared_ptr<Symbol> SymbolTable::getSymbol(const std::string& name) {
    for (auto it = scopeStack.rbegin(); it != scopeStack.rend(); ++it) {
        if (it->find(name) != it->end()) {
            return (*it)[name];
        }
    }
    return nullptr;
}

void SymbolTable::markSymbolUsed(const std::string& name) {
    auto symbol = getSymbol(name);
    if (symbol) {
        symbol->isUsed = true;
    }
}

void SymbolTable::markSymbolSafe(const std::string& name) {
    auto symbol = getSymbol(name);
    if (symbol) {
        symbol->isSafe = true;
    }
}

void SymbolTable::markSymbolNullable(const std::string& name) {
    auto symbol = getSymbol(name);
    if (symbol) {
        symbol->isNullable = true;
    }
}

void SymbolTable::printSymbols() {
    std::cout << "\n=== Symbol Table ===" << std::endl;
    for (size_t i = 0; i < scopeStack.size(); i++) {
        std::cout << "Scope [" << i << "]:" << std::endl;
        for (const auto& [name, symbol] : scopeStack[i]) {
            std::cout << "  " << symbol->toString() << std::endl;
        }
    }
    std::cout << "==================" << std::endl;
}

void SymbolTable::printUnusedSymbols() {
    std::cout << "\n=== Unused Symbols ===" << std::endl;
    bool found = false;
    for (const auto& scope : scopeStack) {
        for (const auto& [name, symbol] : scope) {
            if (!symbol->isUsed) {
                std::cout << "  " << symbol->toString() << std::endl;
                found = true;
            }
        }
    }
    if (!found) {
        std::cout << "  (None)" << std::endl;
    }
    std::cout << "=====================" << std::endl;
}

void SymbolTable::printScope(const std::string& scopeName) {
    std::cout << "\n=== Scope: " << scopeName << " ===" << std::endl;
    for (const auto& scope : scopeStack) {
        for (const auto& [name, symbol] : scope) {
            if (symbol->scope == scopeName) {
                std::cout << "  " << symbol->toString() << std::endl;
            }
        }
    }
    std::cout << std::string(20, '=') << std::endl;
}
