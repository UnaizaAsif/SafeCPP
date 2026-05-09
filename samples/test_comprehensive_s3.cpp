// ============================================================
// SYSTEM 3 COMPREHENSIVE TEST
// ============================================================

// Test 1: Detect single memory leak
int* ptr1 = new int;
// ptr1 not freed → LEAK

// Test 2: Proper cleanup
int* ptr2 = new int;
delete ptr2;

// Test 3: Multiple allocations, partial cleanup
int* a = new int;
int* b = new int;
int* c = new int;
delete a;
// b and c leak

// Test 4: Array allocation and cleanup
int* arr = new int[100];
delete[] arr;

// Test 5: Complex variable names
int* complexVariableName = new int;
// complexVariableName leaks
