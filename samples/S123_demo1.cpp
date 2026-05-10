// ===============================
// sample.cpp
// Comprehensive SafeCPP Test File
// ===============================

int count = 10;

int x;
printf("%d", x);     // Undefined Behavior (uninitialized variable)

int* ptr = nullptr;
*ptr = 5;            // Null pointer dereference

int* leak = new int;
*leak = 100;         // Memory allocated but never deleted

float pi = 3.14;
string name = "SafeCPP";

int y = 20;          // Syntax error: incomplete assignment

print(count);
print(pi);
print(name);

return 0