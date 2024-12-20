#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int start_process(const char *program) {
    pid_t pid = fork();
    if (pid == 0) {
        execlp(program, program, NULL);
        perror("Failed to start process");
        exit(EXIT_FAILURE);
    }
    return pid;
}

int main() {
    pid_t server_pid, client_pid;

    printf("Starting server...\n");
    server_pid = start_process("./server");

    sleep(2); // Give the server time to initialize

    printf("Starting client...\n");
    client_pid = start_process("./client");

    // Wait for the client to finish
    waitpid(client_pid, NULL, 0);

    printf("Client finished. Stopping server...\n");
    kill(server_pid, SIGTERM);

    // Wait for the server to stop
    waitpid(server_pid, NULL, 0);

    printf("Supervisor finished.\n");
    return 0;
}
