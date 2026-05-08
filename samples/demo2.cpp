// =========================================================================
// DEMO 2: C++ with Memory Management and Pointers
// =========================================================================
// Shows proper C++ memory handling, smart pointers, and error conditions
// =========================================================================

#include <iostream>
#include <memory>
#include <vector>
#include <stdexcept>

class DataBuffer {
private:
    int* data;
    int size;
public:
    DataBuffer(int s) : size(s) {
        data = new int[size];
    }
    
    void setValue(int index, int value) {
        if (index >= 0 && index < size) {
            data[index] = value;
        }
    }
    
    int getValue(int index) const {
        if (index >= 0 && index < size) {
            return data[index];
        }
        throw std::out_of_range("Index out of bounds");
    }
    
    ~DataBuffer() {
        delete[] data;
        data = nullptr;
    }
};

template <typename T>
class SmartContainer {
private:
    std::vector<std::unique_ptr<T>> items;
public:
    void add(T* item) {
        if (item != nullptr) {
            items.push_back(std::unique_ptr<T>(item));
        }
    }
    
    T* get(int index) const {
        if (index >= 0 && index < static_cast<int>(items.size())) {
            return items[index].get();
        }
        return nullptr;
    }
    
    int size() const {
        return items.size();
    }
};

class DataProcessor {
private:
    std::vector<int*> tempBuffer;
public:
    void processData(int count) {
        try {
            for (int i = 0; i < count; i++) {
                int* temp = new int(i);
                if (temp == nullptr) {
                    throw std::bad_alloc();
                }
                tempBuffer.push_back(temp);
            }
        } catch (const std::exception& e) {
            std::cout << "Error: " << e.what() << std::endl;
            cleanup();
        }
    }
    
    void cleanup() {
        for (auto ptr : tempBuffer) {
            delete ptr;
        }
        tempBuffer.clear();
    }
    
    ~DataProcessor() {
        cleanup();
    }
};

int main() {
    try {
        // RAII: Resource Acquisition Is Initialization
        std::unique_ptr<DataBuffer> buffer = 
            std::make_unique<DataBuffer>(10);
        
        buffer->setValue(0, 42);
        buffer->setValue(1, 100);
        buffer->setValue(5, 555);
        
        std::cout << "Value at index 0: " << buffer->getValue(0) << std::endl;
        
        // Smart container for automatic cleanup
        SmartContainer<int> container;
        container.add(new int(10));
        container.add(new int(20));
        container.add(new int(30));
        
        // Safe iteration
        for (int i = 0; i < container.size(); i++) {
            auto value = container.get(i);
            if (value != nullptr) {
                std::cout << "Item " << i << ": " << *value << std::endl;
            }
        }
        
        // Data processor with exception handling
        DataProcessor processor;
        processor.processData(5);
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
}
