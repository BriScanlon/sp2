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

    struct mq_attr attributes;
    attributes.mq_flags = 0;
    attributes.mq_maxmsg = 10;
    attributes.mq_msgsize = MAX_SIZE;
    attributes.mq_curmsgs = 0;

    // Open the message queue for writing
    mq = mq_open(QUEUE_NAME, O_CREAT | O_WRONLY, 0644, &attributes);
    CHECK((mqd_t)-1 != mq);

    printf("Send to server (please type in \"exit\" to exit the program):\n");

    do
    {
        sleep(1);
        printf("Message: ");
        fflush(stdout);
        memset(message, 0, sizeof(message));
        fgets(message, MAX_SIZE, stdin);

        // Remove trailing newline
        message[strcspn(message, "\n")] = '\0';

        // Send the message to the server
        mq_send(mq, message, strlen(message) + 1, 0);

        if (strncmp(message, "exit", 4) == 0)
        {
            break;
        }
    } while (1);

    // Cleanup
    CHECK((mqd_t)-1 != mq_close(mq));

    printf("Client finished.\n");
    return 0;
}
