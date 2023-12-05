#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
typedef struct LinesArray {
    char** strs;
    size_t size;
    size_t capacity;
} LinesArray;

int linesArray_pushback(char* new_str, size_t len, LinesArray* lines_array) {
    // if capacity is full, realloc more space
    if (lines_array->size == lines_array->capacity) {
        #define GROWTH_MULTIPLIER 2
        char** tmp = realloc(lines_array->strs, lines_array->capacity * GROWTH_MULTIPLIER * sizeof(char*));
        if (tmp == NULL) {
            // on failure to realloc
            return -1;
        }
        lines_array->strs = tmp;
        lines_array->capacity *= GROWTH_MULTIPLIER;
    }
    lines_array->strs[lines_array->size] = new_str;
    ++lines_array->size;
    return 0;
}

int main(int argc, char** argv) {
    FILE* data = fopen("day-4-data.txt", "r");
    if (data == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    #define INITIAL_CAPACITY 50
    LinesArray lines_array;
    lines_array.strs = malloc(INITIAL_CAPACITY * sizeof(char*));
    lines_array.size = 0;
    lines_array.capacity = INITIAL_CAPACITY;


    char* str = NULL;
    size_t bytes;
    ssize_t nread;
    while ((nread = getline(&str, &bytes, data)) != -1) {
        if (linesArray_pushback(str, nread, &lines_array) == -1) {
            perror("linesarray_pushback");
            exit(EXIT_FAILURE);
        }
        str = NULL;
    }
    free(str);

    for(size_t i = 0; i < lines_array.size; ++i) {
        printf("%s", lines_array.strs[i]);
    }

    for(size_t i = 0; i < lines_array.size; ++i) {
        free(lines_array.strs[i]);
    }

    free(lines_array.strs);
    fclose(data);
}
