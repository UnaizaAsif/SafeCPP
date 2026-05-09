// Test multiple memory leaks
int* a = new int;
int* b = new int;
int* c = new int;
delete a;
// b and c are both leaking
