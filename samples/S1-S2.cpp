#include <iostream>
using namespace std;

int main() {

    int* ptr = nullptr;

    *ptr = 5;

    int* leak = new int(100);

    return 0;
}