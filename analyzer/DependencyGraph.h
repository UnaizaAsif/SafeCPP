#ifndef DEPENDENCY_GRAPH_H
#define DEPENDENCY_GRAPH_H

#include <string>
#include <vector>
#include <map>
#include <memory>

class DependencyGraphNode {
public:
    std::string id;
    std::string type; // "function", "variable", etc.
    std::vector<std::string> dependencies;

    DependencyGraphNode(const std::string& id, const std::string& type);
    ~DependencyGraphNode();

    void addDependency(const std::string& depId);
    bool hasDependency(const std::string& depId) const;
};

class DependencyGraph {
private:
    std::map<std::string, std::shared_ptr<DependencyGraphNode>> nodes;

    bool hasCyclicDependencyHelper(const std::string& nodeId, 
                                   std::map<std::string, bool>& visited,
                                   std::map<std::string, bool>& recStack);

public:
    DependencyGraph();
    ~DependencyGraph();

    void addNode(const std::string& id, const std::string& type);
    void addDependency(const std::string& from, const std::string& to);
    bool hasCyclicDependency() const;
    std::vector<std::string> getTopologicalOrder();
    void print();
};

#endif // DEPENDENCY_GRAPH_H
