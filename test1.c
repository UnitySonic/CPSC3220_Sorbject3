#include <stdlib.h>
#include <assert.h>

#define NUM_MALLOCS 1000

int main() {
    int* test_array = malloc(sizeof(int) * NUM_MALLOCS);

    for(int i = 0; i < NUM_MALLOCS; i++) {
        test_array[i] = i + NUM_MALLOCS;
    }

    for(int i = 0; i < NUM_MALLOCS; i++){
        assert(test_array[i] == i + NUM_MALLOCS);
    }
    return 0;
}
