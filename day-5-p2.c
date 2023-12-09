// Brute Force lol
// ~ 6min runtime

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

typedef struct SeedRange {
    long source;
    long range;
} SeedRange;

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

void getSeedRanges(FILE* stream, SeedRange* out, size_t max_out, size_t* num_seed_ranges) {
    //ignore "seeds:"
    fscanf(stream, "%*s");

    long seed_start = -1;
    long range = -1;
    size_t i;
    
    for (i = 0; (i < max_out) && (fscanf(stream, "%ld %ld", &seed_start, &range) == 2); ++i) {
        assert(seed_start != -1 && range != -1);
        out[i].source = seed_start;
        out[i].range = range;
    }

    //return num of seeds ranges read
    *num_seed_ranges = i;
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

void fill_increasing(long* arr, size_t size, long start) {
    for (size_t i = 0; i < size; ++i) {
        arr[i] = i + start;
    }
}

int main(int argc, char** argv) {

    FILE* stream = NULL;
    stream = fopen("day-5-data.txt", "r");
    if (stream == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    // get input seeds
    SeedRange seed_ranges[MAX_SEEDS];
    size_t num_seed_ranges;
    getSeedRanges(stream, seed_ranges, MAX_SEEDS, &num_seed_ranges);

    // create maps
    Map mappings[MAX_MAPS];
    size_t num_maps;
    for (num_maps = 0; (num_maps < MAX_MAPS) && feof(stream) == 0; ++num_maps) {
        //ignore mapping header: "... map"
        fscanf(stream, "%*s %*s ");
        getMapping(stream, &mappings[num_maps]);
    }
    
    // for (size_t i = 0; i < num_maps; ++i) {
    //     for (size_t j = 0; j < mappings[i].size; ++j) {
    //         fprintf(stderr, "%ld, %ld, %ld\n", mappings[i].translations[j].source, mappings[i].translations[j].destination, mappings[i].translations[j].range);
    //     }
    //     fprintf(stderr, "\n\n");
    // }
    

    long* range_results = malloc(num_seed_ranges * sizeof(long));

    for (size_t k = 0; k < num_seed_ranges; ++k) {

        long* seeds = malloc(seed_ranges[k].range * sizeof(long));
        long num_seeds = seed_ranges[k].range;
        fill_increasing(seeds, num_seeds, seed_ranges[k].source);

        //keep remapping for all maps
        for (size_t j = 0; j < num_maps; ++j) {
            //perform remap
            // for each seed, replace with result from remap
            for (size_t i = 0; i < num_seeds; ++i) {
                seeds[i] = remap(seeds[i], &mappings[j]);
            }

            // print remapped seeds
            // printf("---- remapped -----\n");
            // for (size_t i = 0; i < num_seeds; ++i) {
            //     printf("%ld\n", seeds[i]);
            // }
        }
        long lowest = find_lowest(seeds, num_seeds);
        printf("lowest for range: %ld\n", lowest);
        range_results[k] = lowest;

        free(seeds);
    }

    printf("absolute lowest for all ranges: %ld\n", find_lowest(range_results, num_seed_ranges));



    //cleanup
    fclose(stream);

    free(range_results);

    for (size_t i = 0; i < num_maps; ++i) {
        freeMapping(&mappings[i]);
    }
}
