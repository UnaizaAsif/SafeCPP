// Simple memory leak test
int* p = new int;
int* q = new int;
delete p;
