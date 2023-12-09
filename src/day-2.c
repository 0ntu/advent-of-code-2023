#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const int MAX_RED = 12;
const int MAX_GREEN = 13;
const int MAX_BLUE = 14;

enum Result {
    Possible,
    NotPossible
};

struct Game {
    int id;
    enum Result result;
};

void parseGame(struct Game* game, const char* data) {
    int char_offset = 0;
    sscanf(data, "%*s %i %*c %n", &game->id, &char_offset);

    int value = 0;
    char color = 'a';

    
    
    const char* new_data = data + char_offset;
    int offset_increased = 0;
    //new_data[0] != 0 to stop when str is empty
    while (sscanf(new_data, "%i %c%*s %n", &value, &color, &offset_increased) && new_data[0] != '\0') {
        new_data += offset_increased;
        offset_increased = 0;

        if (color == 'r') {
            if (value > MAX_RED) {
                game->result = NotPossible;
                return;
            }
        }
        if (color == 'g') {
            if (value > MAX_GREEN) {
                game->result = NotPossible;
                return;
            }
        }
        if (color == 'b') {
            if (value > MAX_BLUE) {
                game->result = NotPossible;
                return;
            }
        }

        if (color == 'a') {
            perror("BAD COLOR");
        }
    }

    game->result = Possible;
}

int main(int argc, char* argv[]) {
    FILE* stream = NULL;
    char* line = NULL;
    size_t len = 0;
    ssize_t nread;
    
    stream = fopen("day-2-data.txt", "r");
    if (stream == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    int sum = 0;
    while ((nread = getline(&line, &len, stream)) != -1) {
        struct Game game;
        parseGame(&game, line);

        if (game.result == Possible) {
            printf("Game: %i Possible\n", game.id);
            sum += game.id;
        }
    }

    printf("%i\n", sum);

    free(line);
    fclose(stream);
    return EXIT_SUCCESS;
}
