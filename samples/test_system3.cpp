// Test System 3 – Memory Leak Detection

// Test Case 1: Simple memory leak
int* p = new int;
int* q = new int;
delete p;
// q is not freed – MEMORY LEAK

// Test Case 2: Proper cleanup
int* x = new int;
delete x;

// Test Case 3: Loop memory leak
int i = 0;
while (i < 5) {
    int* temp = new int;
    // temp not deleted in loop – LOOP LEAK
    i = i + 1;
}
