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
    int min_red;
    int min_green;
    int min_blue;
};

void parseGame(struct Game* game, const char* data) {
    int char_offset = 0;
    sscanf(data, "%*s %i %*c %n", &game->id, &char_offset);

    int value = 0;
    char color = '\0';

    game->result = Possible;
    game->min_red = 0;
    game->min_green = 0;
    game->min_blue = 0;
    
    const char* new_data = data + char_offset;
    int offset_increased = 0;
    //new_data[0] != 0 to stop when str is empty
    while (sscanf(new_data, "%i %c%*s %n", &value, &color, &offset_increased) && new_data[0] != '\0') {
        new_data += offset_increased;
        offset_increased = 0;

        if (color == 'r') {
            if (value > MAX_RED) {
                game->result = NotPossible;
            }

            if (value > game->min_red) {
                game->min_red = value;
            }
        }
        if (color == 'g') {
            if (value > MAX_GREEN) {
                game->result = NotPossible;
            }

            if (value > game->min_green) {
                game->min_green = value;
            }
        }
        if (color == 'b') {
            if (value > MAX_BLUE) {
                game->result = NotPossible;
            }

            if (value > game->min_blue) {
                game->min_blue = value;
            }
        }
    }
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

    int possibleid_sum = 0;
    int powerset_sum = 0;
    while ((nread = getline(&line, &len, stream)) != -1) {
        struct Game game;
        parseGame(&game, line);

        if (game.result == Possible) {
            possibleid_sum += game.id;
        }

        printf("r: %i g: %i b:%i\n", game.min_red, game.min_green, game.min_blue);
        powerset_sum += game.min_red * game.min_green * game.min_blue;
    }

    printf("Sum id of possible games: %i\n", possibleid_sum);
    printf("Power of min set of all games: %i\n", powerset_sum);

    free(line);
    fclose(stream);
    return EXIT_SUCCESS;
}
