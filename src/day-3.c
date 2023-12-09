#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

size_t find_beginning_digit_index(char* str, size_t initial_position) {
    // ensure first value is actually a digit
     assert(isdigit(str[initial_position]));

    // leftest_digit > 0 to prevent out of bounds access
    size_t leftest_digit = initial_position;
    for (; leftest_digit > 0; --leftest_digit) {
        if (!isdigit(str[leftest_digit - 1])) {
            return leftest_digit;
        }
    }

    // if here then number starts at beginning of string
    return leftest_digit;
}

int sumDigitsAtPart(size_t i, size_t j, LinesArray* txt) {
    // there are no parts at the end bounds of the LinesArray 
    int sum = 0;


    // sum left
    if (isdigit(txt->strs[i][j-1])) {
        size_t num_index = find_beginning_digit_index(txt->strs[i], j-1);
        int num = 0;
        assert(sscanf(txt->strs[i] + num_index, "%i", &num) == 1);

        printf("adding: %i\n", num);
        sum += num;
    }

    // sum right
    if (isdigit(txt->strs[i][j+1])) {
        size_t num_index = find_beginning_digit_index(txt->strs[i], j+1);
        int num = 0;
        assert(sscanf(txt->strs[i] + num_index, "%i", &num) == 1);

        printf("adding: %i\n", num);
        sum += num;
    }

    // sum up

    // if there is a digit middle up from the symbol, we only have to add to sum that digit
    // since any other digits placed left up or right up, will be contiguous and only
    // create 1 whole number
    //
    // .4.  34.  343
    // .*.  .*.  .*.
    // ...  ...  ...
    if (isdigit(txt->strs[i-1][j])) {
        //if middle up is empty, sum numbers top right and top left

        size_t num_index = find_beginning_digit_index(txt->strs[i-1], j);
        int num = 0;
        assert(sscanf(txt->strs[i-1] + num_index, "%i", &num) == 1);

        printf("adding: %i\n", num);
        sum += num;
    } else {
        if (isdigit(txt->strs[i-1][j-1])) {
            size_t num_index = find_beginning_digit_index(txt->strs[i-1], j-1);
            int num = 0;
            assert(sscanf(txt->strs[i-1] + num_index, "%i", &num) == 1);

            printf("adding: %i\n", num);
            sum += num;
        }

        if (isdigit(txt->strs[i-1][j+1])) {
            size_t num_index = find_beginning_digit_index(txt->strs[i-1], j+1);
            int num = 0;
            assert(sscanf(txt->strs[i-1] + num_index, "%i", &num) == 1);

            printf("adding: %i\n", num);
            sum += num;
        }
    }

    // sum down
    
    if (isdigit(txt->strs[i+1][j])) {
        //if middle up is empty, sum numbers top right and top left

        size_t num_index = find_beginning_digit_index(txt->strs[i+1], j);
        int num = 0;
        assert(sscanf(txt->strs[i+1] + num_index, "%i", &num) == 1);

        printf("adding: %i\n", num);
        sum += num;
    } else {
        if (isdigit(txt->strs[i+1][j-1])) {
            size_t num_index = find_beginning_digit_index(txt->strs[i+1], j-1);
            int num = 0;
            assert(sscanf(txt->strs[i+1] + num_index, "%i", &num) == 1);

            printf("adding: %i\n", num);
            sum += num;
        }

        if (isdigit(txt->strs[i+1][j+1])) {
            size_t num_index = find_beginning_digit_index(txt->strs[i+1], j+1);
            int num = 0;
            assert(sscanf(txt->strs[i+1] + num_index, "%i", &num) == 1);

            printf("adding: %i\n", num);
            sum += num;
        }
    }

    return sum;
}

int get_part_sums(LinesArray* txt) {
    int sum = 0;
    // there are no symbols at the borders of txt
    // iterate over lines
    for (size_t i = 1; i < txt->size - 1; ++i) {
        // iterate over characters to find symbol
        for (size_t j = 1; j < strlen(txt->strs[i]) - 1; ++j) {
            if (!isdigit(txt->strs[i][j]) && txt->strs[i][j] != '.') {
                sum += sumDigitsAtPart(i, j, txt);
            }
        }
    }
    return sum;
}


int main(int argc, char** argv) {
    FILE* data = fopen("day-3-data.txt", "r");
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
    printf("%i", get_part_sums(&lines_array));

    for(size_t i = 0; i < lines_array.size; ++i) {
        free(lines_array.strs[i]);
    }

    free(lines_array.strs);
    fclose(data);
}
