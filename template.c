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

int initialize_linesArray(LinesArray* lines_array, size_t initial_capacity) {
    lines_array->strs = malloc(initial_capacity * sizeof(char*));
    
    if (lines_array == NULL) {
        return -1;
    }
    lines_array->size = 0;
    lines_array->capacity = initial_capacity;

    return 0;
}

int main(int argc, char** argv) {
     // --- init stuff ----
    FILE* data = fopen("day-5-data.txt", "r");
    if (data == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    LinesArray lines_array;
    if (initialize_linesArray(&lines_array, 50) == -1) {
        perror("malloc error");
        exit(EXIT_FAILURE);
    }

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

    // --------------------


    // ------ CLEANUP -----
    for(size_t i = 0; i < lines_array.size; ++i) {
        free(lines_array.strs[i]);
    }

    free(lines_array.strs);
    fclose(data);
}
