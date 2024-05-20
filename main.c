
#define CWUTILS_IMPLEMENT_STRING
#include "cwstring.h"

#include <stdio.h>

int main() {
    CwString* file = cw_string_from_file("./cwstring.h");

    for (int i=0, col = 0; i<file -> size; i++, col++) {
        fprintf(stderr, "%c", file -> ptr[i]);
        // if (col == 16) {
        //     col = 0;
        //     fprintf(stderr, "\n");
        // }
    }
}
