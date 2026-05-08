// =========================================================================
// FULL DEMO: Complete Modern C++ with All Features
// =========================================================================
// Demonstrates proper C++ 17 code with OOP, templates, and best practices
// =========================================================================

#ifndef FULL_DEMO_H
#define FULL_DEMO_H

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <exception>

namespace FullApplication {
    // -------- Type Inference with auto --------
    class TypeInferenceDemo {
    public:
        void run() {
            auto x = 5;              // int
            auto y = 3.14;           // double
            auto z = "SafeCPP";      // const char*
            auto vec = std::vector<int>{};
            
            std::cout << "Type inference working" << std::endl;
        }
    };
    
    // -------- Proper Initialization --------
    class InitializedData {
    private:
        int value = 0;               // Always initialized
        std::string name = "";
        bool flag = false;
    public:
        InitializedData() = default;
        
        void setValue(int v) {
            value = v;
        }
        
        int getValue() const {
            return value;            // Safe: always initialized
        }
        
        virtual ~InitializedData() = default;
    };
    
    // -------- Null Pointer Safety with unique_ptr --------
    template <typename T>
    class SafePointer {
    private:
        std::unique_ptr<T> ptr;
    public:
        SafePointer() : ptr(nullptr) {}
        
        void allocate() {
            ptr = std::make_unique<T>();
        }
        
        T* get() const {
            return ptr.get();         // Safe: returns nullptr if not allocated
        }
        
        bool isValid() const {
            return ptr != nullptr;
        }
        
        virtual ~SafePointer() = default;
    };
    
    // -------- RAII for Memory Safety --------
    class ResourceManager {
    private:
        std::vector<int*> resources;
    public:
        void allocate(int count) {
            resources.clear();
            for (int i = 0; i < count; i++) {
                resources.push_back(new int(i));
            }
        }
        
        void printResources() {
            for (int i = 0; i < static_cast<int>(resources.size()); i++) {
                if (resources[i] != nullptr) {
                    std::cout << "Resource " << i << ": " 
                              << *resources[i] << std::endl;
                }
            }
        }
        
        ~ResourceManager() {
            for (auto ptr : resources) {
                delete ptr;
            }
            resources.clear();
        }
    };
    
    // -------- Template Classes --------
    template <typename T>
    class Container {
    private:
        std::vector<T> items;
    public:
        void add(const T& item) {
            items.push_back(item);
        }
        
        const T& at(int index) const {
            return items.at(index);
        }
        
        int size() const {
            return items.size();
        }
    };
    
    // -------- Exception Handling --------
    class SafeCalculator {
    public:
        static int divide(int a, int b) {
            try {
                if (b == 0) {
                    throw std::invalid_argument("Division by zero");
                }
                return a / b;
            } catch (const std::exception& e) {
                std::cout << "Error: " << e.what() << std::endl;
                return 0;
            }
        }
    };
    
    // -------- Inheritance and Polymorphism --------
    class Shape {
    public:
        virtual double area() const = 0;
        virtual void display() const = 0;
        virtual ~Shape() = default;
    };
    
    class Circle : public Shape {
    private:
        double radius;
    public:
        Circle(double r) : radius(r) {}
        
        double area() const override {
            const double PI = 3.14159;
            return PI * radius * radius;
        }
        
        void display() const override {
            std::cout << "Circle with radius: " << radius << std::endl;
        }
    };
    
    class Rectangle : public Shape {
    private:
        double width, height;
    public:
        Rectangle(double w, double h) : width(w), height(h) {}
        
        double area() const override {
            return width * height;
        }
        
        void display() const override {
            std::cout << "Rectangle: " << width << " x " << height << std::endl;
        }
    };
}

int main() {
    using namespace FullApplication;
    
    std::cout << "=== Full C++ Demo ===" << std::endl;
    
    // Type inference demo
    TypeInferenceDemo inferenceDemo;
    inferenceDemo.run();
    
    // Initialized data demo
    InitializedData data;
    data.setValue(42);
    std::cout << "Data value: " << data.getValue() << std::endl;
    
    // Safe pointer demo
    SafePointer<int> ptr;
    ptr.allocate();
    if (ptr.isValid()) {
        std::cout << "Pointer is valid" << std::endl;
    }
    
    // Resource management demo
    ResourceManager manager;
    manager.allocate(3);
    manager.printResources();
    
    // Container template demo
    Container<double> container;
    container.add(3.14);
    container.add(2.71);
    container.add(1.41);
    std::cout << "Container size: " << container.size() << std::endl;
    
    // Exception handling demo
    auto result = SafeCalculator::divide(10, 2);
    std::cout << "Division result: " << result << std::endl;
    
    // Polymorphism demo
    std::vector<std::unique_ptr<Shape>> shapes;
    shapes.push_back(std::make_unique<Circle>(5.0));
    shapes.push_back(std::make_unique<Rectangle>(4.0, 6.0));
    
    for (const auto& shape : shapes) {
        shape->display();
        std::cout << "Area: " << shape->area() << std::endl;
    }
    
    std::cout << "=== Demo Complete ===" << std::endl;
    return 0;
}

#endif  // FULL_DEMO_H
