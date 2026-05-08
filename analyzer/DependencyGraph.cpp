#include "DependencyGraph.h"
#include <iostream>
#include <algorithm>

DependencyGraphNode::DependencyGraphNode(const std::string& id, const std::string& type)
    : id(id), type(type) {}

DependencyGraphNode::~DependencyGraphNode() {}

void DependencyGraphNode::addDependency(const std::string& depId) {
    if (std::find(dependencies.begin(), dependencies.end(), depId) == dependencies.end()) {
        dependencies.push_back(depId);
    }
}

bool DependencyGraphNode::hasDependency(const std::string& depId) const {
    return std::find(dependencies.begin(), dependencies.end(), depId) != dependencies.end();
}

DependencyGraph::DependencyGraph() {}

DependencyGraph::~DependencyGraph() {}

void DependencyGraph::addNode(const std::string& id, const std::string& type) {
    if (nodes.find(id) == nodes.end()) {
        nodes[id] = std::make_shared<DependencyGraphNode>(id, type);
    }
}

void DependencyGraph::addDependency(const std::string& from, const std::string& to) {
    if (nodes.find(from) != nodes.end()) {
        nodes[from]->addDependency(to);
    }
}

bool DependencyGraph::hasCyclicDependencyHelper(const std::string& nodeId,
                                               std::map<std::string, bool>& visited,
                                               std::map<std::string, bool>& recStack) {
    visited[nodeId] = true;
    recStack[nodeId] = true;

    if (nodes.find(nodeId) != nodes.end()) {
        for (const auto& dep : nodes[nodeId]->dependencies) {
            if (visited.find(dep) == visited.end() || !visited[dep]) {
                if (hasCyclicDependencyHelper(dep, visited, recStack)) {
                    return true;
                }
            } else if (recStack[dep]) {
                return true;
            }
        }
    }

    recStack[nodeId] = false;
    return false;
}

bool DependencyGraph::hasCyclicDependency() const {
    std::map<std::string, bool> visited;
    std::map<std::string, bool> recStack;

    for (const auto& [id, node] : nodes) {
        if (visited.find(id) == visited.end() || !visited[id]) {
            if (const_cast<DependencyGraph*>(this)->hasCyclicDependencyHelper(id, visited, recStack)) {
                return true;
            }
        }
    }

    return false;
}

std::vector<std::string> DependencyGraph::getTopologicalOrder() {
    std::vector<std::string> order;
    std::map<std::string, bool> visited;

    // Simple topological sort (placeholder)
    for (const auto& [id, node] : nodes) {
        if (visited.find(id) == visited.end() || !visited[id]) {
            order.push_back(id);
            visited[id] = true;
        }
    }

    return order;
}

void DependencyGraph::print() {
    std::cout << "=== Dependency Graph ===" << std::endl;
    for (const auto& [id, node] : nodes) {
        std::cout << node->id << " (" << node->type << ") -> ";
        for (const auto& dep : node->dependencies) {
            std::cout << dep << " ";
        }
        std::cout << std::endl;
    }
    std::cout << "=======================" << std::endl;
}
