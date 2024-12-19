#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "communication.h"
#include "swap_parts_exec.h"

void start_server(CommunicationContext *comm)
{
    char flag[BUFFER_SIZE];
    char input[BUFFER_SIZE];
    char output[BUFFER_SIZE];

    while (1)
    {
        // Receive flag
        printf("Server: Waiting for flag...\n");
        if (receive_message(comm, flag, BUFFER_SIZE) <= 0)
        {
            perror("Failed to receive flag");
            break; // Exit if no more data or pipe closed
        }

        if (strlen(flag) == 0)
        {
            flag[0] = '\0';
        }

        printf("Server: Received flag='%s'\n", flag);

        // Receive input
        printf("Server: Waiting for input...\n");
        if (receive_message(comm, input, BUFFER_SIZE) <= 0)
        {
            perror("Failed to receive input");
            break; // Exit if no more data or pipe closed
        }
        printf("Server: Received input='%s'\n", input);

        printf("Server: Calling swapPartsUsingExecf...\n");
        swapPartsUsingExecf(flag, input, output);

        printf("Server: Sending output='%s'\n", output);
        if (send_message(comm, output, strlen(output) + 1) <= 0)
        {
            perror("Failed to send output");
            break;
        }
    }

    close_communication(comm); // Clean up
}