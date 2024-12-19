#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Prints the usage options available to the user
void printUsage() {
    printf("Usage: program [options] <string>\n");
    printf("Options:\n");
    printf("  -h    Show this help message\n");
    printf("  -c    Reverse half of the string and append it to the other half\n");
    printf("  -u    Convert the input string to uppercase\n");
}

// Reverses a string in place
void reverseString(char *str) {
    int len = 0;
    while (str[len] != '\0') {
        len++;
    }

    for (int i = 0; i < len / 2; i++) {
        char temp = str[i];
        str[i] = str[len - i - 1];
        str[len - i - 1] = temp;
    }
}

// Processes the -c option
void processCOption(char *input) {
    size_t len = strlen(input);
    size_t half = len / 2;

    // Reverse the first half
    char temp[half + 1];
    strncpy(temp, input, half);
    temp[half] = '\0';
    reverseString(temp);

    // Combine reversed half with the second half
    printf("%s%s\n", temp, input + half);
}

// Processes the -u option
void processUOption(char *input) {
    for (size_t i = 0; input[i] != '\0'; i++) {
        input[i] = toupper(input[i]);
    }
    printf("%s\n", input);
}

int main(int userStringCount, char *userString[]) {
    if (userStringCount < 2) {
        fprintf(stderr, "Usage: ./reverse [options] <string>\n");
        return 1;
    }

    char *passThruString = userString[1];
    if (strcmp(passThruString, "-h") == 0) {
        printUsage();
    } else if (strcmp(passThruString, "-c") == 0) {
        if (userStringCount != 3) {
            fprintf(stderr, "Error: -c option requires a string argument\n");
            return EXIT_FAILURE;
        }
        processCOption(userString[2]);
    } else if (strcmp(passThruString, "-u") == 0) {
        if (userStringCount != 3) {
            fprintf(stderr, "Error: -u option requires a string argument\n");
            return EXIT_FAILURE;
        }
        processUOption(userString[2]);
    } else {
        // Default behavior: reverse the string
        reverseString(passThruString);
        printf("%s\n", passThruString);
    }

    return EXIT_SUCCESS;
}
