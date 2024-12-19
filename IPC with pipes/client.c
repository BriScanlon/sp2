// client.c
#include "communication.h"
#include "pipe_comm.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

void start_client(CommunicationContext *comm)
{
    char flag[BUFFER_SIZE];
    char input[BUFFER_SIZE];
    char output[BUFFER_SIZE];

    while (1)
    {
        printf("Enter option and string:\n");

        // Read flag
        fgets(flag, BUFFER_SIZE, stdin);
        flag[strcspn(flag, "\n")] = '\0'; // Remove trailing newline

        // Read input
        fgets(input, BUFFER_SIZE, stdin);
        input[strcspn(input, "\n")] = '\0'; // Remove trailing newline

        // Send flag
        printf("Client: Sending flag='%s'\n", flag);
        if (send_message(comm, flag, strlen(flag) + 1) <= 0)
        {
            perror("Failed to send flag");
            break;
        }

        fsync(((int *)comm->context)[1]); // Flush write end
        sleep(1);

        // Send input
        printf("Client: Sending input='%s'\n", input);
        if (send_message(comm, input, strlen(input) + 1) <= 0)
        {
            perror("Failed to send input");
            break;
        }

        // Receive response
        if (receive_message(comm, output, BUFFER_SIZE) > 0)
        {
            printf("Received from server: %s\n", output);
        }
        else
        {
            printf("No response from server\n");
            break;
        }
    }

    close_communication(comm); // Clean up
}
