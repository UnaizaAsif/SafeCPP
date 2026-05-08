#include <iostream>
#include <vector>

safe int globalCounter = 0

nullable char* message = nullptr

let age = 20
let pi = 3.14

namespace Demo {

class Person {

public:

    int id
    char grade

    void show() {
        cout << id
    }
}

}

int add(int a, int b) {
    return a + b
}

int multiply(int a, int b) {
    int result = a * b
    return result
}

int main() {

    int x = 10
    float y = 5.5
    bool flag = true

    int arr[5] = {1,2,3,4,5}

    cout << x
    cin >> x

    x = x + 1
    y = y * 2

    if(x > 5) {
        cout << x
    }
    else {
        cout << 0
    }

    while(x > 0) {
        x = x - 1
    }

    for(i = 0; i < 5; i = i + 1) {
        cout << arr[i]
    }

    switch(x) {

        case 1:
            cout << 1
            break

        case 2:
            cout << 2
            break

        default:
            cout << 0
    }

    return 0
}