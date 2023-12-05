#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

# define NUM_HELD 25 
# define NUM_WINNING 10
# define POINT_VALUE 1

typedef struct LinesArray {
    char** strs;
    size_t size;
    size_t capacity;
} LinesArray;

typedef struct Game {
    size_t id;
    int held_nums[NUM_HELD];
    int winning_nums[NUM_WINNING];
} Game;

void parseGame(char* str, Game* game) {
    int currentOffset = 0;
    int incrementalOffset = 0;

    sscanf(str, "%*s %zu%*c %n", &game->id, &currentOffset);

    for (size_t i = 0; i < NUM_WINNING; ++i) {
        sscanf(str + currentOffset, "%i %n", &game->winning_nums[i], &incrementalOffset);

        currentOffset += incrementalOffset;
    }

    sscanf(str + currentOffset, "%*c %n", &incrementalOffset);
    currentOffset += incrementalOffset;


    for (size_t i = 0; i < NUM_HELD; ++i) {
        sscanf(str + currentOffset, "%i %n", &game->held_nums[i], &incrementalOffset);

        currentOffset += incrementalOffset;
    }
}

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

int doesNumOccurInArray(int value, int* arr, size_t size) {
    int occurences = 0;
    for (size_t i = 0; i < size; ++i) {
        if (arr[i] == value) {
            return 1;
        }
    }

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

    int total_points = 0;
    //change later
    for(size_t i = 0; i < lines_array.size; ++i) {
        Game game;
        parseGame(lines_array.strs[i], &game);
        

        int num_winning_held = 0;
        for (size_t i = 0; i < NUM_HELD; ++i) {
            if (doesNumOccurInArray(game.held_nums[i], game.winning_nums, NUM_WINNING)) {
                ++num_winning_held;
            }
        }

        int game_points = 0;
        if (num_winning_held > 0) {
            //exponent
            game_points = 1 << (num_winning_held - 1);
        }
        total_points += game_points;
    }
    printf("%i", total_points);

    for(size_t i = 0; i < lines_array.size; ++i) {
        free(lines_array.strs[i]);
    }

    free(lines_array.strs);
    fclose(data);
}
