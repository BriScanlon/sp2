// server.c
#include "communication.h"

void start_server(CommunicationContext *comm) {
    char input[BUFFER_SIZE];
    char output[BUFFER_SIZE];
    char option[BUFFER_SIZE];

    while (1) {
        // Receive option and input
        receive_message(comm, option, BUFFER_SIZE);
        receive_message(comm, input, BUFFER_SIZE);

        // Check for exit condition
        if (strcmp(input, "exit") == 0) {
            break;
        }

        // Process the input and generate the output
        snprintf(output, BUFFER_SIZE, "Processed: %s", input); // Example processing

        // Send the result back to the client
        send_message(comm, output, strlen(output) + 1);
    }

    close_communication(comm);
}

// Example usage:
int main() {
    // Example for pipe communication
    int pipe_fd[2];
    pipe(pipe_fd);
    CommunicationContext *comm = init_pipe_comm(pipe_fd);

    start_server(comm);
    free(comm);

    return 0;
}
