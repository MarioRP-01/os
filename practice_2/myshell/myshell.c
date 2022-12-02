#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "parser.h"

// -----
// ARRAY
// -----
typedef struct Array {
    void * array;
    size_t size;
} Array;

Array new_array(void * array, size_t size) {
    Array new_array;
    new_array.array = array;
    new_array.size = size;
    return new_array;
}

// -----
// MAIN
// -----
int main(int argc, char ** argv) {

    // TODO: Error handling
    if (argc != 1) return 1;

    char * pwd = getcwd(NULL, (size_t) 0); // TODO: Free memory
    
    tline * line;
    Array buffer = new_array(NULL, 1024);
    while (getline((char **) &buffer.array, &buffer.size, stdin)) {
        // TODO: Check if line is null
        // TODO: Check if line->commands->filename is null
        line = tokenize(buffer.array);
        printf("%s\n", line->commands->filename);
    }


    free(pwd);
    if (buffer.array != NULL) free(buffer.array);
    return 0;
}
