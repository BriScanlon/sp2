#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <mqueue.h>
#include "mq_common.h"

void process_message( const char *message)
{
    char command[MAX_SIZE + 50];
    FILE *fp;

    snprintf(command, sizeof(command), "./swapParts %s", message);

    // Debug log for the command
    printf("Executing command: %s\n", command);

    // Execute the command and capture output
    fp = popen(command, "r");
    if (fp == NULL)
    {
        return;
    }

    char result[MAX_SIZE + 50] = {0};
    int total_len = 0;
    // Read the command output
    while(fgets(command, sizeof(command), fp) != NULL) {
        if (total_len + strlen(command) < MAX_SIZE + 49) {
            strcat(result, command);
            total_len += strlen(command);
        } else {
            strncat(result, command, MAX_SIZE + 50 - total_len - 1);
            break;
        }
    }

    int status = pclose(fp);

    // Remove newline from the response
    printf("Result: %s", result);
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
	    else {
            strncpy(message, buffer, sizeof(message) - 1);
            process_message(buffer);
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
