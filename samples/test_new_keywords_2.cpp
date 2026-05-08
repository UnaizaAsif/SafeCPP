// =========================================================================
// TEST: All New Keywords Added in Extended Phase
// =========================================================================

#include <iostream>
#include <vector>

namespace TestKeywords {
    
    // Test: short, long, signed, unsigned, wchar_t, char16_t, char32_t, char8_t
    class TypeDemo {
    private:
        short s = 10;
        long l = 1000;
        signed int si = -50;
        unsigned int ui = 100;
        wchar_t wc = L'A';
        char16_t c16 = u'B';
        char32_t c32 = U'C';
        char8_t c8 = u8'D';
        
        // Test: mutable, explicit, final, override
        mutable int counter = 0;
        
    public:
        // Test: explicit constructor
        explicit TypeDemo(int x) {}
        
        // Test: virtual and override
        virtual void display() override {
            std::cout << "TypeDemo" << std::endl;
        }
        
        // Test: const and override
        void updateCounter() const {
            counter++;
        }
    };
    
    // Test: final - prevents further inheritance
    class FinalClass final {
    public:
        void doSomething() {
            std::cout << "FinalClass" << std::endl;
        }
    };
    
    // Test: goto and default
    void controlFlowDemo() {
        int x = 5;
        
        switch(x) {
            case 1:
                std::cout << "One" << std::endl;
                break;
            default:
                std::cout << "Other" << std::endl;
                break;
        }
        
        // goto example
        if (x < 0) {
            goto error_handler;
        }
        
        error_handler:
            std::cout << "Error occurred" << std::endl;
    }
    
    // Test: sizeof operator
    void sizeofDemo() {
        auto size_int = sizeof(int);
        auto size_long = sizeof(long);
        auto size_char = sizeof(char);
        auto size_pointer = sizeof(void*);
        
        std::cout << "sizeof(int): " << size_int << std::endl;
        std::cout << "sizeof(long): " << size_long << std::endl;
    }
    
    // Test: cout and cin (from iostream)
    void ioDemo() {
        int value;
        std::cout << "Enter a value: ";
        std::cin >> value;
        std::cout << "You entered: " << value << std::endl;
    }
}

int main() {
    using namespace TestKeywords;
    
    // Test type keywords
    short s = 5;
    long l = 10000;
    signed int si = -100;
    unsigned int ui = 200;
    
    wchar_t wc = L'X';
    char16_t c16 = u'Y';
    char32_t c32 = U'Z';
    char8_t c8 = u8'W';
    
    // Test sizeof
    auto sz = sizeof(long);
    
    // Test TypeDemo with explicit constructor
    TypeDemo demo(42);
    demo.display();
    
    // Test control flow
    int choice = 2;
    switch(choice) {
        case 1:
            std::cout << "Choice 1" << std::endl;
            break;
        default:
            std::cout << "Default choice" << std::endl;
            break;
    }
    
    // Demonstrate goto (not recommended in practice)
    if (false) {
        goto skip;
    }
    std::cout << "This is printed" << std::endl;
    
    skip:
    std::cout << "Jump completed" << std::endl;
    
    // Test I/O
    std::cout << "Testing cout" << std::endl;
    
    return 0;
}
