// Test: Loop-based memory leak
int i = 0;
while (i < 3) {
    int* x = new int;
    i = i + 1;
}
