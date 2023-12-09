#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int word_to_i(char* str) {
    char* str_digits[] = {"zero", "one", "two", "three", "four", "five", "six", "seven", "eight", "nine"};

    for(int digit = 0; digit < 10; ++digit) {
        if (strncmp(str, str_digits[digit], strlen(str_digits[digit])) == 0) {
            return digit;
        }
    }

    return -1;
}

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
        int num_arr[3] = {0};
        num_arr[2] = '\0';

        //1's place first
        size_t i = strlen(str);
        for (; i >= 0; --i) {
            if (isdigit(str[i])) {
                num_arr[1] = str[i] - '0';
                break;
            }

            int word_digit = word_to_i(str + i);
            if (word_digit != -1) {
                num_arr[1] = word_digit;
                break;
            }
        }
        int last_digit_index = i;



        //10's place last
        //if cannot find digit for 10's place, 0 inserted as default
        for (int j = 0; j <= last_digit_index; ++j) {
            if (isdigit(str[j])) {
                num_arr[0] = str[j] - '0';
                break;
            }

            int word_digit = word_to_i(str + j);
            if (word_digit != -1) {
                num_arr[0] = word_digit;
                break;
            }
        }

        sum += (num_arr[0] * 10) + num_arr[1];
    }

    printf("%d\n", sum);
    
    return EXIT_SUCCESS;
}
