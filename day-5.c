#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

# define MAX_TRANSLATIONS 200
# define MAX_MAPS 7
# define MAX_SEEDS 200

typedef struct Translation {
    long source;
    long destination;
    long range;
} Translation;

// a map is an array of translations
typedef struct Map {
    size_t size;
    Translation* translations;
} Map;

long remap(long in, Map* map) {

    for (size_t i = 0; i < map->size; ++i) {
        // check if in range of mapping
        long offset_from_source = in - map->translations[i].source;

        //in must be greater than source
        if (offset_from_source < 0) continue;

        if (offset_from_source < map->translations[i].range) {
            return map->translations[i].destination + offset_from_source;
        }
    }

    //if cannot be remapped
    return in;
}

void getSeeds(FILE* stream, long* out, size_t buf_size, size_t* num_seeds) {
    //ignore "seeds:"
    fscanf(stream, "%*s");

    long seed = -1;
    size_t i;
    for (i = 0; (i < buf_size) && (fscanf(stream, "%ld", &seed) == 1); ++i) {
        assert(seed != -1);
        out[i] = seed;
    }

    //return num of seeds read
    *num_seeds = i;
}

void getMapping(FILE* stream, Map* map) {
    map->translations = malloc(MAX_TRANSLATIONS * sizeof(Translation));
    
    long source, destination, range;
    size_t map_size;
    for (map_size = 0; (map_size < MAX_TRANSLATIONS) && (fscanf(stream, "%ld %ld %ld", &destination, &source, &range) && feof(stream) == 0); ++map_size) {
        map->translations[map_size].destination = destination;
        map->translations[map_size].source = source;
        map->translations[map_size].range = range;
    }

    map->size = map_size;
}

void freeMapping(Map* map) {
    free(map->translations);
}

long find_lowest(long* arr, size_t size) {
    long lowest = arr[0];
    
    for (size_t i = 0; i < size; ++i) {
        if (arr[i] < lowest) {
            lowest = arr[i];
        }
    }

    return lowest;
}

int main(int argc, char** argv) {

    FILE* stream = NULL;
    stream = fopen("day-5-data.txt", "r");
    if (stream == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    // get input seeds
    long seeds[MAX_SEEDS];
    size_t num_seeds;
    getSeeds(stream, seeds, MAX_SEEDS, &num_seeds);

    // create maps
    Map mappings[MAX_MAPS];
    size_t num_maps;
    for (num_maps = 0; (num_maps < MAX_MAPS) && feof(stream) == 0; ++num_maps) {
        //ignore mapping header: "... map"
        fscanf(stream, "%*s %*s ");
        getMapping(stream, &mappings[num_maps]);
    }
    
    for (size_t i = 0; i < num_maps; ++i) {
        for (size_t j = 0; j < mappings[i].size; ++j) {
            fprintf(stderr, "%ld, %ld, %ld\n", mappings[i].translations[j].source, mappings[i].translations[j].destination, mappings[i].translations[j].range);
        }
        fprintf(stderr, "\n\n");
    }
    
    // print initial seeds
    printf("---- initial -----\n");
    for (size_t i = 0; i < num_seeds; ++i) {
        printf("%ld\n", seeds[i]);
    }

    for (size_t j = 0; j < num_maps; ++j) {
        //perform remap
        for (size_t i = 0; i < num_seeds; ++i) {
            seeds[i] = remap(seeds[i], &mappings[j]);
        }

        // print remapped seeds
        printf("---- remapped -----\n");
        for (size_t i = 0; i < num_seeds; ++i) {
            printf("%ld\n", seeds[i]);
        }
    }

    printf("lowest: %ld\n", find_lowest(seeds, num_seeds));

    //cleanup
    fclose(stream);
    for (size_t i = 0; i < num_maps; ++i) {
        freeMapping(&mappings[i]);
    }
}
