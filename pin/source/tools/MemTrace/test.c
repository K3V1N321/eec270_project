#include <stdio.h>
#include <stdlib.h>

int main() {
    int size = 10;
    int* arr = (int*)malloc(size * sizeof(int));

    for (int i = 0; i < size; i++) {
        arr[i] = i * 2;
    }

    free(arr);
    return 0;
}