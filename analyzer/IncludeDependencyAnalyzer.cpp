/**
 * IncludeDependencyAnalyzer.cpp
 * -----
 * Implementation of include dependency cycle detection.
 */

#include "IncludeDependencyAnalyzer.h"
#include <sstream>
#include <algorithm>

// ================================================================
//  Add Dependency
// ================================================================
void IncludeDependencyAnalyzer::addDependency(const std::string& source,
                                               const std::string& target) {
    // Ensure both nodes exist in graph
    if (dependencyGraph.find(source) == dependencyGraph.end()) {
        dependencyGraph[source] = {};
    }
    if (dependencyGraph.find(target) == dependencyGraph.end()) {
        dependencyGraph[target] = {};
    }
    
    // Add edge: source includes target
    dependencyGraph[source].push_back(target);
}

// ================================================================
//  Detect Cycles Using DFS
// ================================================================
IncludeCycle IncludeDependencyAnalyzer::detectCycles() {
    IncludeCycle result;
    
    std::unordered_set<std::string> visited;
    std::unordered_set<std::string> recursionStack;
    std::vector<std::string> currentPath;
    
    // Try DFS from each node
    for (const auto& pair : dependencyGraph) {
        const std::string& node = pair.first;
        
        // Skip if already fully visited
        if (visited.find(node) != visited.end()) {
            continue;
        }
        
        // Start DFS from this node
        if (dfs(node, visited, recursionStack, currentPath, result.cyclePath)) {
            result.cycleFound = true;
            return result;
        }
    }
    
    return result;
}

// ================================================================
//  DFS Helper
// ================================================================
bool IncludeDependencyAnalyzer::dfs(
    const std::string& node,
    std::unordered_set<std::string>& visited,
    std::unordered_set<std::string>& recursionStack,
    std::vector<std::string>& currentPath,
    std::vector<std::string>& cycleResult) {
    
    // Mark as visited and add to recursion stack
    visited.insert(node);
    recursionStack.insert(node);
    currentPath.push_back(node);
    
    // Explore all neighbors
    for (const auto& neighbor : dependencyGraph[node]) {
        if (recursionStack.find(neighbor) != recursionStack.end()) {
            // Found cycle! Build the cycle path
            currentPath.push_back(neighbor);  // Add the node that completes the cycle
            cycleResult = currentPath;
            return true;
        }
        
        if (visited.find(neighbor) == visited.end()) {
            // Recursively explore
            if (dfs(neighbor, visited, recursionStack, currentPath, cycleResult)) {
                return true;
            }
        }
    }
    
    // Backtrack: remove from recursion stack and current path
    recursionStack.erase(node);
    currentPath.pop_back();
    
    return false;
}

// ================================================================
//  Format Cycle Path
// ================================================================
std::string IncludeDependencyAnalyzer::formatCyclePath(
    const std::vector<std::string>& path) const {
    
    if (path.empty()) {
        return "";
    }
    
    std::ostringstream oss;
    for (size_t i = 0; i < path.size(); ++i) {
        oss << path[i];
        if (i < path.size() - 1) {
            oss << " → ";
        }
    }
    
    return oss.str();
}

// ================================================================
//  Get Dependency Graph
// ================================================================
std::string IncludeDependencyAnalyzer::getDependencyGraph() const {
    std::ostringstream oss;
    
    for (const auto& pair : dependencyGraph) {
        const std::string& source = pair.first;
        const auto& targets = pair.second;
        
        if (targets.empty()) {
            oss << source << " (no includes)\n";
        } else {
            for (const auto& target : targets) {
                oss << source << " → " << target << "\n";
            }
        }
    }
    
    return oss.str();
}

// ================================================================
//  Clear
// ================================================================
void IncludeDependencyAnalyzer::clear() {
    dependencyGraph.clear();
}
