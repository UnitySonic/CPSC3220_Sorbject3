#define _GNU_SOURCE

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

int main(void) {
    printf("zero: %d\n", open("/dev/zero", O_RDWR));
    int test1AnswerKey = 0;
    int *test1[200000];
    for (size_t i = 0; i < 200000; i++) {
        test1[i] = (int *) malloc(16);
        *test1[i] = 200 + i;
        memset(test1[i], 0, 16);
        assert(*test1[i] == test1AnswerKey);
        //printf("test1[%ld]:  %d\n", i, *test1[i]);
    }
    printf("malloc 16 checks PASS \n");

    int* test2[100000];
    for (size_t i = 0; i < 100000; i++) {
        test2[i] = (int *) malloc(1024);
        memset(test2[i], 1, 1024);
        *test1[i] = 200 + i;
        //printf("test2[%ld]:  %d\n", i, *test2[i]);
        assert(*test2[i] == 16843009);
        free(test2[i]);
    }
    printf("malloc and free 1024 checks PASS \n");

    int *test3 = (int *) malloc(1030);
    memset(test3, 1, 1030);
    printf("large alloc: %d\n", *test3);

    u_int64_t reallocTestKey = (1 << 16) - 1;
    int *reallocTest = (int *) malloc(2);
    memset(reallocTest, 0b11111111, 2);
    printf("reallocTest: %d\n", *reallocTest);
    int *realloc2 = (int *) realloc(reallocTest, 8);
    printf("realloc2: %d\n", *realloc2);
    assert(*realloc2 == reallocTestKey);
    printf("Realloc form small to large alloc PASS\n");
}
