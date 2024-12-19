#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include "communication.h"
#include "pipe_comm.h"

// Function declarations
void start_client(CommunicationContext *comm);
void start_server(CommunicationContext *comm);

int main()
{
    int pipe_client_to_server[2]; // Pipe for client -> server
    int pipe_server_to_client[2]; // Pipe for server -> client

    // Create pipes
    if (pipe(pipe_client_to_server) == -1 || pipe(pipe_server_to_client) == -1)
    {
        perror("pipe failed");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();
    if (pid == 0)
    {
        // Client process
        close(pipe_client_to_server[0]); // Close read end of client -> server pipe
        close(pipe_server_to_client[1]); // Close write end of server -> client pipe

        // Initialize client communication context
        CommunicationContext *comm = init_pipe_comm((int[]){pipe_server_to_client[0], pipe_client_to_server[1]});
        start_client(comm);

        // Clean up
        close(pipe_client_to_server[1]);
        close(pipe_server_to_client[0]);
        exit(EXIT_SUCCESS);
    }
    else if (pid > 0)
    {
        // Server process
        close(pipe_client_to_server[1]); // Close write end of client -> server pipe
        close(pipe_server_to_client[0]); // Close read end of server -> client pipe

        // Initialize server communication context
        CommunicationContext *comm = init_pipe_comm((int[]){pipe_client_to_server[0], pipe_server_to_client[1]});
        start_server(comm);

        // Clean up
        close(pipe_client_to_server[0]);
        close(pipe_server_to_client[1]);
        wait(NULL); // Wait for client process to finish
    }
    else
    {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }

    return 0;
}
