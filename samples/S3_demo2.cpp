// Test case where memory is properly freed
int* p = new int;
int* q = new int;
delete p;
delete q;
