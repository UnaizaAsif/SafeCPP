// ============================================================
// SYSTEM 3 QUICK TEST - All Core Features
// ============================================================

// Leak 1: Single pointer not freed
int* ptr1 = new int;

// Leak 2: One of two pointers freed
int* a = new int;
int* b = new int;
delete a;

// Safe: Both freed
int* x = new int;
int* y = new int;
delete x;
delete y;

// Leak 3: Complex variable
int* complexName = new int;
