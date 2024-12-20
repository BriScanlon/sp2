#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <mqueue.h>
#include "mq_common.h"

void process_message(const char *flag, const char *message, char *response)
{
    char command[MAX_SIZE + 50];
    FILE *fp;

    // Handle specific cases for command construction
    if (strcmp(flag, "-h") == 0)
    {
        // `-h` flag: Ignore the message and display help
        snprintf(command, sizeof(command), "./swapParts %s", flag);
    }
    else if (strlen(flag) > 0 && strlen(message) > 0)
    {
        // Both flag and message are provided
        snprintf(command, sizeof(command), "./swapParts %s \"%s\"", flag, message);
    }
    else if (strlen(flag) > 0)
    {
        // Flag only
        snprintf(command, sizeof(command), "./swapParts %s", flag);
    }
    else if (strlen(message) > 0)
    {
        // Message only
        snprintf(command, sizeof(command), "./swapParts \"%s\"", message);
    }
    else
    {
        // No flag, no message: Call `swapParts` with no arguments
        snprintf(command, sizeof(command), "./swapParts");
    }

    // Debug log for the command
    printf("Executing command: %s\n", command);

    // Execute the command and capture output
    fp = popen(command, "r");
    if (fp == NULL)
    {
        snprintf(response, MAX_SIZE, "Error executing swapParts");
        return;
    }

    // Read the command output
    if (fgets(response, MAX_SIZE, fp) == NULL)
    {
        snprintf(response, MAX_SIZE, "No response from swapParts");
    }
    pclose(fp);

    // Remove newline from the response
    response[strcspn(response, "\n")] = '\0';
}

int main(int argc, char **argv)
{
    mqd_t mq;
    struct mq_attr attr;
    char buffer[MAX_SIZE];
    char flag[10] = {0};
    char message[MAX_SIZE] = {0};
    char response[MAX_SIZE];
    int must_stop = 0;

    // Initialize the queue attributes
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_SIZE;
    attr.mq_curmsgs = 0;

    // Create the message queue
    mq = mq_open(QUEUE_NAME, O_CREAT | O_RDONLY, 0644, &attr);
    CHECK((mqd_t)-1 != mq);

    printf("Server running... Waiting for messages.\n");

    do
    {
        ssize_t bytes_read;

        // Receive the message
        bytes_read = mq_receive(mq, buffer, MAX_SIZE, NULL);
        CHECK(bytes_read >= 0);

        buffer[bytes_read] = '\0';

        if (!strncmp(buffer, MSG_STOP, strlen(MSG_STOP)))
        {
            must_stop = 1;
        }
        else if (strncmp(buffer, "FLAG:", 5) == 0)
        {
            // Extract the flag
            strncpy(flag, buffer + 5, sizeof(flag) - 1);
            flag[strcspn(flag, "\n")] = '\0'; // Trim newline
            printf("Received flag: %s\n", flag);
        }
        else if (strncmp(buffer, "MESSAGE:", 8) == 0)
        {
            // Extract the message
            strncpy(message, buffer + 8, sizeof(message) - 1);
            message[strcspn(message, "\n")] = '\0'; // Trim newline
            printf("Received message: %s\n", message);
        }

        // Process if both flag and message are ready
        if (strlen(flag) > 0 && (strlen(message) > 0 || strcmp(flag, "-h") == 0))
        {
            process_message(flag, message, response);
            printf("Processed: %s\n", response);

            // Reset state after processing
            memset(flag, 0, sizeof(flag));
            memset(message, 0, sizeof(message));
        }
    } while (!must_stop);

    // Cleanup
    CHECK((mqd_t)-1 != mq_close(mq));
    CHECK((mqd_t)-1 != mq_unlink(QUEUE_NAME));

    return 0;
}
