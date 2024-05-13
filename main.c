#include <stdio.h>


#define CW_IMPL
#include "cwutils.h"
#undef CW_IMPL

int main() {
    CwArray* array = cw_array_new(sizeof(int));

    for (int i=0;i<4;i++) {
        void* next = cw_array_push(array);
        *((int*)(next)) = 4;
    }

    int* arr = (int*)(array -> ptr);

    fprintf(stderr, "%d\n", arr[3]);
}
