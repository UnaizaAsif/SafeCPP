int i = 0;
while (i < 5) {
    int* x = new int;
    delete x;
    i = i + 1;
}
