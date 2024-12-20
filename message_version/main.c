#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include "communication.h"
#include "msg_comm.h"

// Function declarations
void start_client(CommunicationContext *comm);
void start_server(CommunicationContext *comm);

int main()
{
    pid_t pid = fork();

    if (pid == 0)
    {
        // Client process
        CommunicationContext *comm = init_msg_comm(); // Initialize semaphore-based context
        start_client(comm);
        close_communication(comm); // Clean up communication resources
        exit(EXIT_SUCCESS);
    }
    else if (pid > 0)
    {
        // Server process
        CommunicationContext *comm = init_msg_comm(); // Initialize semaphore-based context
        start_server(comm);
        close_communication(comm); // Clean up communication resources
        wait(NULL);                // Wait for client process to finish
    }
    else
    {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }

    return 0;
}
