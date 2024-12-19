#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#define BUFFER_SIZE 1024

void start_server(int read_pipe, int write_pipe);
void start_client(int write_pipe, int read_pipe);

int main() {
	// define pipes
    int client_to_server[2];
    int server_to_client[2];

    // Create the pipes
    if (pipe(client_to_server) == -1 || pipe(server_to_client) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // Fork the processes
    pid_t client_pid = fork();
    if (client_pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (client_pid == 0) {
        // Client process
        close(client_to_server[0]);
        close(server_to_client[1]);
        start_client(client_to_server[1], server_to_client[0]);
        exit(EXIT_SUCCESS);
    }

    pid_t server_pid = fork();
    if (server_pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (server_pid == 0) {
        // Server process
        close(client_to_server[1]);
        close(server_to_client[0]);
        start_server(client_to_server[0], server_to_client[1]);
        exit(EXIT_SUCCESS);
    }

    // Supervisor process: close unused pipe ends
    close(client_to_server[0]);
    close(client_to_server[1]);
    close(server_to_client[0]);
    close(server_to_client[1]);

    // Wait for child processes to finish
    wait(NULL);
    wait(NULL);

    return 0;
}
