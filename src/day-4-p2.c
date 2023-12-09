#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

# define NUM_HELD 25
# define NUM_WINNING 10

typedef struct LinesArray {
    char** strs;
    size_t size;
    size_t capacity;
} LinesArray;

typedef struct Card {
    size_t id;
    int held_nums[NUM_HELD];
    int winning_nums[NUM_WINNING];
} Card;

void parseCard(char* str, Card* game) {
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

int doesNumOccurInArray(int value, const int* arr, size_t size) {
    int occurences = 0;
    for (size_t i = 0; i < size; ++i) {
        if (arr[i] == value) {
            return 1;
        }
    }

    return 0;
}

int getCardMatches(const Card card) {
    int num_copies = 0;
    for (size_t i = 0; i < NUM_HELD; ++i) {
        if (doesNumOccurInArray(card.held_nums[i], card.winning_nums, NUM_WINNING)) {
            ++num_copies;
        }
    }
    return num_copies;
}

int sumRecurse(int* card_matches, size_t position) {
    const int num_copies = card_matches[position];
    int sum = 1;
    for (size_t i = 1; i <= num_copies; ++i) {
        sum += sumRecurse(card_matches, position + i);
    }
    return sum;
}

int getTotalCards(LinesArray* txt)  {
    int sum = 0;
    
    //get num of matches for each card
    //and put them into array num_card_copies
    int* card_matches = malloc(txt->size * sizeof(int));
    for (size_t i = 0; i < txt->size; ++i) {
        Card card;
        parseCard(txt->strs[i], &card);

        *(card_matches + i) = getCardMatches(card);
    }

    for (size_t i = 0; i < txt->size; ++i) {
        fprintf(stderr, "%i\n", card_matches[i]);
    }

    for (size_t i = 0; i < txt->size; ++i) {
        const int num_copies = card_matches[i];
        fprintf(stderr, "MAJOR: copies - %i, position - %zu\n", num_copies, i);
        sum += sumRecurse(card_matches, i);
    }

    free(card_matches);
    return sum;
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

    int total_cards = getTotalCards(&lines_array);
    printf("total cards: %i", total_cards);

    for(size_t i = 0; i < lines_array.size; ++i) {
        free(lines_array.strs[i]);
    }


    free(lines_array.strs);
    fclose(data);
}
