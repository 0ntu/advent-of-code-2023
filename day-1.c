#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[]) {
    FILE* stream = NULL;
    stream = fopen("day-1-data.txt", "r");
    if (stream == NULL) {
        perror("Failed to open file");
        return EXIT_FAILURE;
    }

    #define BUF_SIZE 256
    char str[BUF_SIZE]= {0};

    int sum = 0;
    while (fgets(str, sizeof(str), stream)) {
        char num_str[3] = {0};
        num_str[2] = '\0';

        //1's place first
        size_t i = strlen(str) - 1;
        for (; i >= 0; --i) {
            if (isdigit(str[i])) {
                num_str[1] = str[i];
                break;
            }
        }
        int last_digit_index = i;



        //10's place last
        //if cannot find digit for 10's place, 0 inserted as default
        for (int j = 0; j <= last_digit_index; ++j) {
            if (isdigit(str[j])) {
                num_str[0] = str[j];
                break;
            }
        }

        printf("%s\n", num_str);
        sum += atoi(num_str);
    }

    printf("%d\n", sum);
    
    return EXIT_SUCCESS;
}
