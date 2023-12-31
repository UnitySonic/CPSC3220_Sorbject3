#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

int main() {
    uint8_t * ptr;
    uint8_t * ptr2;

    ptr = malloc(1);
    ptr[0] = 5;
    ptr2 = realloc(ptr, 0);

    fprintf(stderr, "%p\n", ptr);
    fprintf(stderr, "%p\n", ptr2);
}
