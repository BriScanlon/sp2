#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h> // For read and write

#define BUFFER_SIZE 1024

// Include the swapParts functionality
void swapTwoParts(char *input, char *output);
void processCOption(char *input);
void processUOption(char *input);
void defaultFunction(char *input);
void printUsage(char *output);

// Processes the -c option
void processCOptionServer(char *input, char *output) {
    size_t len = strlen(input);
    size_t half = len / 2;

    // Reverse the first half
    char temp[half + 1];
    strncpy(temp, input, half);
    temp[half] = '\0';

    for (size_t i = 0; i < half / 2; i++) {
        char swap = temp[i];
        temp[i] = temp[half - i - 1];
        temp[half - i - 1] = swap;
    }

    // Combine reversed half with the second half
    snprintf(output, BUFFER_SIZE, "%s%s", temp, input + half);
}

// Processes the -u option for the server
void processUOptionServer(char *input, char *output) {
    char swapped[BUFFER_SIZE];

    // Swap the two parts
    swapTwoParts(input, swapped);

    // Convert swapped result to uppercase
    for (size_t i = 0; swapped[i] != '\0'; i++) {
        swapped[i] = toupper(swapped[i]);
    }

    snprintf(output, BUFFER_SIZE, "%s", swapped);
}

// Default behavior for the server: swap two parts
void defaultFunctionServer(char *input, char *output) {
    char swapped[BUFFER_SIZE];

    // Swap the two parts
    swapTwoParts(input, swapped);

    snprintf(output, BUFFER_SIZE, "%s", swapped);
}

void swapTwoParts(char *input, char *output) {
    size_t len = strlen(input);
    size_t half = len / 2;

    char temp[len + 1];

    // Copy the second part to temp
    strcpy(temp, input + half);

    // Append the first part
    strncat(temp, input, half);

    // Copy the result to output
    strcpy(output, temp);
}

// Prints the usage options available to the user
void printUsage(char *output) {
    snprintf(output, BUFFER_SIZE,
             "Usage: program [options] <string>\n"
             "Options:\n"
             "  -h    Show this help message\n"
             "  -c    Reverse half of the string and append it to the other half\n"
             "  -u    Swap two parts of the string and convert to uppercase\n"
             "Default: Swap two parts of the string (split at the midpoint)\n");
}

// Main server loop
void start_server(int read_pipe, int write_pipe) {
    char input[BUFFER_SIZE];
    char output[BUFFER_SIZE];
    char option[BUFFER_SIZE];

    while (1) {
        // Read the option and input string from the client
        int optionRead = read(read_pipe, option, BUFFER_SIZE);
        if (optionRead <= 0) {
            printf("Server: No option received\n");
            continue;
        }

        int inputRead = read(read_pipe, input, BUFFER_SIZE);
        if (inputRead <= 0) {
            printf("Server: No input received\n");
            continue;
        }

        // Check for exit condition
        if (strcmp(input, "exit") == 0) {
            break;
        }

        // Process based on the option
        if (strcmp(option, "-h") == 0) {
            printUsage(output);
        } else if (strcmp(option, "-c") == 0) {
            processCOptionServer(input, output);
        } else if (strcmp(option, "-u") == 0) {
            processUOptionServer(input, output);
        } else {
            // Default behavior: swap two parts of the string
            defaultFunctionServer(input, output);
        }

        // Write the result back to the client
        write(write_pipe, output, strlen(output) + 1);
    }
}
