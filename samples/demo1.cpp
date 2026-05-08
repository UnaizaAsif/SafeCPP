// =========================================================================
// DEMO 1: Proper C++ Code - Safe and Clean
// =========================================================================
// Demonstrates proper C++ with classes, templates, and type safety
// =========================================================================

#include <iostream>
#include <string>
#include <vector>

namespace SafeMath {
    template <typename T>
    class Calculator {
    private:
        T result;
    public:
        Calculator() : result(0) {}
        
        T add(T a, T b) {
            result = a + b;
            return result;
        }
        
        T multiply(T a, T b) {
            result = a * b;
            return result;
        }
        
        T getResult() const {
            return result;
        }
    };
}

class Person {
private:
    std::string name;
    int age;
public:
    Person(const std::string& n, int a) : name(n), age(a) {}
    
    virtual void display() const {
        std::cout << "Name: " << name << ", Age: " << age << std::endl;
    }
    
    virtual ~Person() {}
};

class Student : public Person {
private:
    float gpa;
public:
    Student(const std::string& n, int a, float g) 
        : Person(n, a), gpa(g) {}
    
    void display() const override {
        Person::display();
        std::cout << "GPA: " << gpa << std::endl;
    }
};

int main() {
    using namespace SafeMath;
    
    // Template usage with type inference
    auto calc = Calculator<int>();
    int sum = calc.add(5, 10);
    int product = calc.multiply(5, 10);
    
    // Object creation and polymorphism
    const Person* student = new Student("Alice", 20, 3.8f);
    student->display();
    delete student;
    
    // Vector with objects
    std::vector<int> numbers = {1, 2, 3, 4, 5};
    
    return 0;
}
