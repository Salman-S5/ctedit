#include <stdio.h>
#include "editor.h"


int main(int *argc, char *argv[]) {
    const char *filename = NULL;

    if (argc >= 2) {
        filename = argv[1];
    }

    editor_init(filename);
    return 0;
}