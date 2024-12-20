#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <mqueue.h>
#include "mq_common.h"

int main(int argc, char **argv)
{
    mqd_t mq;
    char buffer[MAX_SIZE];
    char flag[10];
    char message[MAX_SIZE];

    // Open the message queue for writing
    mq = mq_open(QUEUE_NAME, O_WRONLY);
    CHECK((mqd_t)-1 != mq);

    printf("Send to server (enter \"exit\" for flag to stop):\n");

    do
    {
        // Get the flag from the user
        printf("Flag (e.g., -f): ");
        fflush(stdout);
        memset(flag, 0, sizeof(flag));
        fgets(flag, sizeof(flag), stdin);

        // Remove trailing newline
        flag[strcspn(flag, "\n")] = '\0';

        // Stop if the user enters "exit"
        if (strncmp(flag, "exit", 4) == 0)
        {
            break;
        }

        // Send the flag to the server
        snprintf(buffer, sizeof(buffer), "FLAG:%s", flag);
        CHECK(0 <= mq_send(mq, buffer, MAX_SIZE, 0));

        // Get the message from the user (can be blank if flag is -h)
        printf("Message: ");
        fflush(stdout);
        memset(message, 0, sizeof(message));
        fgets(message, MAX_SIZE, stdin);

        // Remove trailing newline
        message[strcspn(message, "\n")] = '\0';

        // Send the message to the server
        snprintf(buffer, sizeof(buffer), "MESSAGE:%.1015s", message);
        CHECK(0 <= mq_send(mq, buffer, MAX_SIZE, 0));

    } while (1);

    // Cleanup
    CHECK((mqd_t)-1 != mq_close(mq));

    printf("Client finished.\n");
    return 0;
}
