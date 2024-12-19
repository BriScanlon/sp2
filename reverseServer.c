#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h> // For read and write

#define BUFFER_SIZE 1024

// Prints the usage options available to the user
void printUsage(char *output) {
    snprintf(output, BUFFER_SIZE,
             "Usage: program [options] <string>\n"
             "Options:\n"
             "  -h    Show this help message\n"
             "  -c    Reverse half of the string and append it to the other half\n"
             "  -u    Convert the input string to uppercase\n");
}

// Reverses a string in place
void reverseString(char *str) {
    int len = strlen(str);
    for (int i = 0; i < len / 2; i++) {
        char temp = str[i];
        str[i] = str[len - i - 1];
        str[len - i - 1] = temp;
    }
}

// Processes the -c option
void processCOption(char *input, char *output) {
    size_t len = strlen(input);
    size_t half = len / 2;

    // Reverse the first half
    char temp[half + 1];
    strncpy(temp, input, half);
    temp[half] = '\0';
    reverseString(temp);

    // Combine reversed half with the second half
    snprintf(output, BUFFER_SIZE, "%s%s", temp, input + half);
}

// Processes the -u option
void processUOption(char *input, char *output) {
    for (size_t i = 0; input[i] != '\0'; i++) {
        input[i] = toupper(input[i]);
    }
    snprintf(output, BUFFER_SIZE, "%s", input);
}

// Main server loop
void start_server(int read_pipe, int write_pipe) {
    printf("Server: Server Starting\n");
    char input[BUFFER_SIZE];
    char output[BUFFER_SIZE];
    char option[BUFFER_SIZE];

    while (1) {
        // Read the option and input string from the client
	int optionRead = read(read_pipe, option, BUFFER_SIZE);
	if ( optionRead < 0) {
	    printf("Server: No option received\n");
	    continue;
	} else {
	    printf("Server: Option %s received\n", option);
	}

	int inputRead = read(read_pipe, input, BUFFER_SIZE);
	printf("Server: Received input: %s\n", input);
	if (inputRead < 0) {
	    printf("Server: No input received\n");
	    continue;
	} else {
	    printf("Server: Input %s received\n", input);
	}

        // Check for exit condition
        if (strcmp(input, "exit") == 0) {
            break;
        }

        // Process based on the option
        if (strcmp(option, "-h") == 0) {
            printUsage(output);
	    write(write_pipe, output, strlen(output) + 1);
        } else if (strcmp(option, "-c") == 0) {
            processCOption(input, output);
        } else if (strcmp(option, "-u") == 0) {
            processUOption(input, output);
        } else {
            // Default behavior: reverse the string
            reverseString(input);
            snprintf(output, BUFFER_SIZE, "%s", input);
        }

        // Write the result back to the client
	printf("Server: Write output to the pipe\n");
        write(write_pipe, output, strlen(output) + 1);
    }
}
