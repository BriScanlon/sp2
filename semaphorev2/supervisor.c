#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    pid_t server_pid, client_pid;

    // Start the server process
    server_pid = fork();
    if (server_pid == 0) {
        execl("./sem_server", "./sem_server", NULL);
        perror("Supervisor: Failed to start server");
        exit(EXIT_FAILURE);
    } else if (server_pid < 0) {
        perror("Supervisor: Failed to fork for server");
        exit(EXIT_FAILURE);
    }

    // Wait for the server to initialize
    sleep(2); // Allow server to initialize

    // Start a single client process
    client_pid = fork();
    if (client_pid == 0) {
        execl("./sem_client", "./sem_client", "Flag1", "Input1", NULL);  // Replace "Flag1" and "Input1" as needed
        perror("Supervisor: Failed to start client");
        exit(EXIT_FAILURE);
    } else if (client_pid < 0) {
        perror("Supervisor: Failed to fork for client");
    }

    // Wait for the client to complete
    waitpid(client_pid, NULL, 0);

    // Optionally wait for the server to complete
    kill(server_pid, SIGINT); // Send interrupt signal to stop server
    waitpid(server_pid, NULL, 0);

    return 0;
}
