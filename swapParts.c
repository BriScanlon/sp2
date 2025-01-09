#include <stdio.h>

// Prints the usage options available to the user test
void printUsage() {
    printf("Usage: program [options] <string>\n");
    printf("Options:\n");
    printf("  -h    Show this help message\n");
    printf("  -c    Reverse half of the string and append it to the other half\n");
    printf("  -u    Swap two parts of the string and convert to uppercase\n");
    printf("Default: Swap two parts of the string (split at the midpoint)\n");
}

// Helper: Calculate the length of a string
size_t stringLength(const char *str) {
    size_t len = 0;
    while (str[len] != '\0') {
        len++;
    }
    return len;
}

// Helper: Copy a string
void stringCopy(char *dest, const char *src) {
    while (*src != '\0') {
        *dest++ = *src++;
    }
    *dest = '\0';
}

// Helper: Concatenate strings
void stringConcatenate(char *dest, const char *src) {
    while (*dest != '\0') {
        dest++;
    }
    while (*src != '\0') {
        *dest++ = *src++;
    }
    *dest = '\0';
}

// Helper: Convert string to uppercase
void toUppercase(char *str) {
    for (size_t i = 0; str[i] != '\0'; i++) {
        if (str[i] >= 'a' && str[i] <= 'z') {
            str[i] -= 'a' - 'A';
        }
    }
}

// Swaps two parts of the string
void swapTwoParts(char *input, char *output) {
    size_t len = stringLength(input);
    size_t half = len / 2;

    size_t i = 0;

    // Copy the second half to output
    for (; i < len - half; i++) {
        output[i] = input[half + i];
    }

    // Copy the first half to output
    for (size_t j = 0; j < half; j++, i++) {
        output[i] = input[j];
    }

    // Null-terminate the string
    output[i] = '\0';
}

// Processes the -c option
void processCOption(char *input) {
    size_t len = stringLength(input);
    size_t half = len / 2;

    char temp[half + 1];
    for (size_t i = 0; i < half; i++) {
        temp[i] = input[i];
    }
    temp[half] = '\0';

    // Reverse the first half
    for (size_t i = 0; i < half / 2; i++) {
        char swap = temp[i];
        temp[i] = temp[half - i - 1];
        temp[half - i - 1] = swap;
    }

    // Combine reversed half with the second half
    printf("%s%s\n", temp, input + half);
}

// Processes the -u option
void processUOption(char *input) {
    size_t len = stringLength(input);
    char swapped[len + 1];

    // Swap the two parts
    swapTwoParts(input, swapped);

    // Convert swapped result to uppercase
    toUppercase(swapped);

    printf("%s\n", swapped);
}

// Processes the swapping action (default behavior)
void defaultFunction(char *input) {
    size_t len = stringLength(input);
    char swapped[len + 1];

    // Swap two parts
    swapTwoParts(input, swapped);

    printf("%s\n", swapped);
}

int main(int userStringCount, char *userString[]) {
    if (userStringCount < 2) {
        printf("Usage: ./program [options] <string>\n");
        return 1;
    }

    char *firstArg = userString[1];

    // Check if the first argument is an option (starts with '-')
    if (firstArg[0] == '-' && firstArg[1] != '\0') {
        if (firstArg[1] == 'h' && firstArg[2] == '\0') {
            printUsage();
        } else if (firstArg[1] == 'c' && firstArg[2] == '\0') {
            if (userStringCount != 3) {
                printf("Error: -c option requires a string argument\n");
                return 1;
            }
            processCOption(userString[2]);
        } else if (firstArg[1] == 'u' && firstArg[2] == '\0') {
            if (userStringCount != 3) {
                printf("Error: -u option requires a string argument\n");
                return 1;
            }
            processUOption(userString[2]);
        } else {
            printf("Error: Unknown option '%s'\n", firstArg);
            return 1;
        }
    } else {
        // Default behavior: Swap two parts of the string
        defaultFunction(firstArg);
    }

    return 0;
}
