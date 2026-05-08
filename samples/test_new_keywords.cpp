class Animal {
private:
    const int age;
public:
    virtual void speak() {
        try {
            throw true;
        } catch(bool error) {
            static auto result = false;
        }
    }
};

namespace myspace {
    using namespace std;
    template <typename T>
    struct Node {
        T data;
    };
}

function calc() {
    int x = 5;
    return x;
}
