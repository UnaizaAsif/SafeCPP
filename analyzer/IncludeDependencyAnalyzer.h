/**
 * IncludeDependencyAnalyzer.h
 * -----
 * Detects circular include dependencies in C++ header files.
 * 
 * Uses DFS-based cycle detection on a directed dependency graph.
 * Reports full cycle paths for diagnostic clarity.
 */

#ifndef INCLUDE_DEPENDENCY_ANALYZER_H
#define INCLUDE_DEPENDENCY_ANALYZER_H

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

// ================================================================
//  Cycle Information
// ================================================================
struct IncludeCycle {
    bool cycleFound = false;
    std::vector<std::string> cyclePath;  // e.g., ["a.h", "b.h", "c.h", "a.h"]
};

// ================================================================
//  Include Dependency Analyzer
// ================================================================
class IncludeDependencyAnalyzer {
public:
    IncludeDependencyAnalyzer() = default;

    /**
     * Add an include dependency
     * source includes target
     */
    void addDependency(const std::string& source, const std::string& target);

    /**
     * Detect cycles in the dependency graph using DFS
     * Returns cycle information if found
     */
    IncludeCycle detectCycles();

    /**
     * Get human-readable dependency graph
     */
    std::string getDependencyGraph() const;

    /**
     * Clear all dependencies (for new analysis)
     */
    void clear();

private:
    // Directed graph: key -> list of files it includes
    std::unordered_map<std::string, std::vector<std::string>> dependencyGraph;

    // DFS helpers
    bool dfs(const std::string& node,
             std::unordered_set<std::string>& visited,
             std::unordered_set<std::string>& recursionStack,
             std::vector<std::string>& currentPath,
             std::vector<std::string>& cycleResult);

    std::string formatCyclePath(const std::vector<std::string>& path) const;
};

#endif // INCLUDE_DEPENDENCY_ANALYZER_H
