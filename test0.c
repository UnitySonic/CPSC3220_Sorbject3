#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int main() {
    int* i = (int*) malloc(sizeof(int));
    *i = 5;
    i = realloc(i, sizeof(int) * 2);
    fprintf(stdout, "i[0]: %d\n", i[0]);
    fprintf(stdout, "i[1]: %d\n", i[1]);
    assert(i[0] == 5);
    free(i);
}
