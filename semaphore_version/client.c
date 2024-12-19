#include "communication.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "sem_comm.h"

void start_client(CommunicationContext *comm)
{
    char flag[BUFFER_SIZE];
    char input[BUFFER_SIZE];
    char output[BUFFER_SIZE];

    while (1)
    {
        printf("*******************************************************************************************************\n");
        printf("* This application will take a flag and then a string from your input,                                *\n");
        printf("* send it to a server for processing, re-arrange parts of the string and then return it to you below. *\n");
        printf("*******************************************************************************************************\n");
        printf("Enter a flag (this is optional) the press enter.\n");

        // Read flag
        fgets(flag, BUFFER_SIZE, stdin);
        flag[strcspn(flag, "\n")] = '\0'; // Remove trailing newline

        printf("Now enter a string and press enter.\n");

        // Read input
        fgets(input, BUFFER_SIZE, stdin);
        input[strcspn(input, "\n")] = '\0'; // Remove trailing newline

        // Send flag
        if (send_message(comm, flag, strlen(flag) + 1) <= 0)
        {
            perror("Failed to send flag");
            break;
        }

        sleep(1);

        // Send input
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
