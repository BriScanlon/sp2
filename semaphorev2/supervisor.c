#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

#define SERVER_EXEC "./sem_server"
#define CLIENT_EXEC "./sem_client"

int main() {
    pid_t server_pid, client_pid;

    // Start the server process
    server_pid = fork();
    if (server_pid == 0) {
        execl(SERVER_EXEC, SERVER_EXEC, NULL);
        perror("Server failed to start");
        exit(1);
    }

    sleep(1); // Allow server to initialize

    // Start multiple clients
    for (int i = 0; i < 3; i++) {
        client_pid = fork();
        if (client_pid == 0) {
            execl(CLIENT_EXEC, CLIENT_EXEC, NULL);
            perror("Client failed to start");
            exit(1);
        }
        sleep(1); // Stagger client starts
    }

    // Wait for all child processes to complete
    while (wait(NULL) > 0);

    return 0;
}
