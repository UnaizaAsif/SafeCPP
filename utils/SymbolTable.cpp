#include "SymbolTable.h"
#include <iostream>
#include <algorithm>

// -------------------------------------------------------
//  Helpers
// -------------------------------------------------------
bool SymbolTable::isPointerType(const std::string& type) {
    return type.find('*') != std::string::npos;
}

// -------------------------------------------------------
//  Scope management
// -------------------------------------------------------
void SymbolTable::pushScope() {
    scopes.emplace_back();
}

void SymbolTable::popScope() {
    if (scopes.empty()) return;

    // Preserve scope entries so end-of-analysis leak reporting still sees
    // allocations from scopes that were already popped.
    for (const auto& entry : scopes.back()) {
        finalizedEntries.push_back(entry.second);
    }
    scopes.pop_back();
}

// -------------------------------------------------------
//  Declare
// -------------------------------------------------------
bool SymbolTable::declare(const std::string& name,
                          const std::string& type,
                          int line, int col) {
    if (scopes.empty()) pushScope();

    auto& current = scopes.back();
    if (current.count(name)) return false;   // already in this scope

    SymbolEntry e;
    e.name        = name;
    e.type        = type;
    e.declareLine = line;
    e.declareCol  = col;
    e.initState   = InitState::UNINITIALIZED;
    e.isPointer   = isPointerType(type);
    e.nullState   = e.isPointer ? NullState::UNKNOWN : NullState::NOT_A_POINTER;

    current[name] = e;
    return true;
}

// -------------------------------------------------------
//  Lookup (innermost scope first)
// -------------------------------------------------------
SymbolEntry* SymbolTable::lookup(const std::string& name) {
    for (int i = (int)scopes.size() - 1; i >= 0; --i) {
        auto it = scopes[i].find(name);
        if (it != scopes[i].end()) return &it->second;
    }
    return nullptr;
}

// -------------------------------------------------------
//  Initialization tracking (System 1)
// -------------------------------------------------------
bool SymbolTable::markInitialized(const std::string& name, int line) {
    SymbolEntry* e = lookup(name);
    if (!e) return false;
    e->initState  = InitState::INITIALIZED;
    e->assignLine = line;
    return true;
}

bool SymbolTable::checkInitialized(const std::string& name, int useLine) {
    SymbolEntry* e = lookup(name);
    if (!e) return true;   // unknown symbol – not our problem
    if (e->usageLine == -1) e->usageLine = useLine;
    return e->initState == InitState::INITIALIZED;
}

// -------------------------------------------------------
//  Null-state tracking (System 2)
// -------------------------------------------------------
bool SymbolTable::markNull(const std::string& name, int line) {
    SymbolEntry* e = lookup(name);
    if (!e || !e->isPointer) return false;
    e->nullState      = NullState::NULL_PTR;
    e->nullAssignLine = line;
    // Also treat as "initialized" for System 1
    e->initState      = InitState::INITIALIZED;
    e->assignLine     = line;
    return true;
}

bool SymbolTable::markNonNull(const std::string& name, int line) {
    SymbolEntry* e = lookup(name);
    if (!e || !e->isPointer) return false;
    e->nullState  = NullState::NONNULL;
    e->initState  = InitState::INITIALIZED;
    e->assignLine = line;
    return true;
}

bool SymbolTable::markMaybeNull(const std::string& name, int line) {
    SymbolEntry* e = lookup(name);
    if (!e || !e->isPointer) return false;
    e->nullState = NullState::MAYBE_NULL;
    return true;
}

bool SymbolTable::checkNullDereference(const std::string& name) {
    SymbolEntry* e = lookup(name);
    if (!e || !e->isPointer) return false;
    return e->nullState == NullState::NULL_PTR ||
           e->nullState == NullState::UNKNOWN;
}

// -------------------------------------------------------
//  Memory leak tracking (System 3)
// -------------------------------------------------------
bool SymbolTable::markAllocated(const std::string& name, int line, bool inLoop, int loopDepth) {
    SymbolEntry* e = lookup(name);
    if (!e) return false;
    e->allocated   = true;
    e->allocLine   = line;
    e->allocInLoop = inLoop;
    e->loopDepth   = loopDepth;
    // Mark as initialized for System 1
    e->initState   = InitState::INITIALIZED;
    e->assignLine  = line;
    return true;
}

bool SymbolTable::markFreed(const std::string& name, int line) {
    SymbolEntry* e = lookup(name);
    if (!e) return false;
    e->freed       = true;
    e->freeLine    = line;
    return true;
}

std::vector<SymbolTable::MemoryLeak> SymbolTable::getMemoryLeaks() const {
    std::vector<MemoryLeak> leaks;

    auto collectLeaks = [&leaks](const SymbolEntry& e) {
        if (!e.allocated || e.freed) return;
        MemoryLeak leak;
        leak.varName = e.name;
        leak.allocLine = e.allocLine;
        leak.errorType = e.allocInLoop ? 1 : 0;
        leaks.push_back(leak);
    };

    // Check active scopes
    for (const auto& scope : scopes) {
        for (const auto& entry : scope) {
            collectLeaks(entry.second);
        }
    }

    // Check finalized symbols from popped scopes
    for (const auto& e : finalizedEntries) {
        collectLeaks(e);
    }

    return leaks;
}

// -------------------------------------------------------
//  Debug dump
// -------------------------------------------------------
void SymbolTable::dump() const {
    std::cout << "\n=== Symbol Table Dump ===\n";
    for (int s = 0; s < (int)scopes.size(); ++s) {
        std::cout << "Scope " << s << ":\n";
        for (auto& entry : scopes[s]) {
            const std::string& name = entry.first;
            const SymbolEntry& e = entry.second;
            std::cout << "  " << e.type << " " << name
                      << "  init=";
            switch (e.initState) {
                case InitState::UNINITIALIZED: std::cout << "NO"; break;
                case InitState::INITIALIZED:   std::cout << "YES"; break;
                case InitState::MAYBE_INIT:    std::cout << "MAYBE"; break;
            }
            if (e.isPointer) {
                std::cout << "  null=";
                switch (e.nullState) {
                    case NullState::NULL_PTR:  std::cout << "NULL"; break;
                    case NullState::NONNULL:   std::cout << "SAFE"; break;
                    case NullState::MAYBE_NULL:std::cout << "MAYBE"; break;
                    case NullState::UNKNOWN:   std::cout << "UNKNOWN"; break;
                    default: break;
                }
            }
            std::cout << "  (line " << e.declareLine << ")\n";
        }
    }
    std::cout << "========================\n";
}