#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>

int main() {

    srand(time(NULL));   // Initialization, should only be called once.

    int MIN = 1;
    int MAX = 2047;
    int DOOMSIZE = 100000;

    int bufsizes[DOOMSIZE];
    uint8_t *bufs[DOOMSIZE];

    int min;

    // Do not comment anything above this out and expect it to work

    // TestSet 1

    for (int i = 0; i < DOOMSIZE; i++) {
        bufsizes[i] = (rand() % MAX) + MIN;
//        fprintf(stderr, "i: %d, size: %d ", i, bufsizes[i]);
        bufs[i] = malloc(bufsizes[i]);
//        fprintf(stderr, "p: %p\n", bufs[i]);
        for (int j = 0; j < bufsizes[i]; j++) {
            bufs[i][j] = bufsizes[i];
        }
        free(bufs[i]);
    }

    //TestSet 2

    for (int i = 0; i < DOOMSIZE; i++) {
        bufsizes[i] = (rand() % MAX) + MIN;
        //fprintf(stderr, "i: %d, size: %d ", i, bufsizes[i]);
        bufs[i] = malloc(bufsizes[i]);
        //fprintf(stderr, "p: %p\n", bufs[i]);
        for (int j = 0; j < bufsizes[i]; j++) {
            bufs[i][j] = bufsizes[i] % 240;
        }
    }

    for (int i = 0; i < DOOMSIZE - 1; i++) {
        //fprintf(stderr, "reallocing i: %d, size: %d -> %d\n", i, bufsizes[i], bufsizes[i+1]);

        if (bufsizes[i] >= bufsizes[i + 1]) {
            min = bufsizes[i + 1];
        } else {
            min = bufsizes[i];
        }

        bufs[i] = realloc(bufs[i], bufsizes[i + 1]);

        for (int j = 0; j < min; j++)
        {



            if(bufs[i][j] != bufsizes[i] % 240)
            {
                fprintf(stderr,"Bufsizes is: %d\nExpected answer of %d:\nActual: %d", bufsizes[i], bufsizes[i] % 240, bufs[i][j]);
                exit(0);
            }
        }
    }

    for (int i = 0; i < DOOMSIZE; i++) {
        //fprintf(stderr, "freeing i: %d\n", i);
        free(bufs[i]);
    }

    //test set 3
    //tests the edge point of large pages

    MIN = 4000;
    MAX = 200;

    DOOMSIZE = 10000;

    for (int i = 0; i < DOOMSIZE; i++) {
        bufsizes[i] = (rand() % MAX) + MIN;
//        fprintf(stderr, "i: %d, size: %d ", i, bufsizes[i]);
        bufs[i] = malloc(bufsizes[i]);
//        fprintf(stderr, "p: %p\n", bufs[i]);
        for (int j = 0; j < bufsizes[i]; j++) {
            bufs[i][j] = bufsizes[i] % 240;
        }
    }

    for (int i = 0; i < DOOMSIZE - 1; i++) {
//        fprintf(stderr, "reallocing i: %d, size: %d -> %d\n", i, bufsizes[i], bufsizes[i + 1]);

        if (bufsizes[i] >= bufsizes[i + 1]) {
            min = bufsizes[i + 1];
        } else {
            min = bufsizes[i];
        }

        bufs[i] = realloc(bufs[i], bufsizes[i + 1]);

        for (int j = 0; j < min; j++) {
            assert(bufs[i][j] == bufsizes[i] % 240);
        }
    }

    for (int i = 0; i < DOOMSIZE; i++) {
//        fprintf(stderr, "freeing i: %d\n", i);
        free(bufs[i]);
    }
}
