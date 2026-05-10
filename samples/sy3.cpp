#include <iostream>

using namespace std;

// 1. Missing semicolon
int a = 5

// 2. Wrong assignment operator
int b == 10;

// 3. Missing operator between values
int c = 10 20;

// 4. Expression starts with operator
int d = + 7;

// 5. Invalid identifier
int @value = 50;

// 6. Unmatched parentheses
int sum(int x, int y {
    return x + y;
}

// 7. Extra closing brace
void test() {
    int x = 10;
}}

// 8. Missing closing brace
void missingBrace() {
    int y = 20;

// 9. Double semicolon
int e = 100;;

// 10. Invalid function name
void 123func() {
    cout << "Hello";
}

// 11. Missing comma in parameters
int add(int a int b) {
    return a + b;
}

// 12. Empty assignment
int f = ;

// 13. Missing return type
main() {
    cout << "Hello";
    return 0;
}

// 14. Invalid keyword usage
integer z = 10;

// 15. Using undeclared variable (semantic + syntax combo case)
int result = x + y;

int main() {
    cout << "Test file for syntax analyzer" << endl;
    return 0;
}