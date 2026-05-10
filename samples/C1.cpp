#include "samples/headers/a.h"
#include <iostream>

using namespace std;

int main() {

    int uninitialized;

    cout << uninitialized;

    int* ptr = nullptr;

    *ptr = 5;

    int* leak = new int(50);

    let pi = 3.14
    let name = "SafeCPP"

    cout << pi
    cout << name

    return 0
}