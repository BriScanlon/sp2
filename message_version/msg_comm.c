#include "msg_comm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>

#define MSG_KEY 10014630
#define BUFFER_SIZE 1024

typedef struct
{
    long msg_type;
    char msg_text[BUFFER_SIZE];
} Message;

typedef struct
{
    int msg_id;
} MessageContext;

// Function to send a message
int msg_send(void *context, const char *message, size_t size)
{
    MessageContext *ctx = (MessageContext *)context;
    if (!ctx)
    {
        fprintf(stderr, "msg_send: Invalid context\n");
        return -1;
    }

    Message msg;
    msg.msg_type = 1; // Set message type
    strncpy(msg.msg_text, message, size);
    msg.msg_text[size - 1] = '\0'; // Ensure null-termination

    if (msgsnd(ctx->msg_id, &msg, sizeof(msg.msg_text), 0) == -1)
    {
        perror("msg_send: msgsnd failed");
        return -1;
    }

    printf("msg_send: Sent message: '%s'\n", msg.msg_text);
    return size;
}

// Function to receive a message
int msg_receive(void *context, char *buffer, size_t size)
{
    MessageContext *ctx = (MessageContext *)context;
    if (!ctx)
    {
        fprintf(stderr, "msg_receive: Invalid context\n");
        return -1;
    }

    Message msg;
    if (msgrcv(ctx->msg_id, &msg, sizeof(msg.msg_text), 0, 0) == -1)
    {
        perror("msg_receive: msgrcv failed");
        return -1;
    }

    strncpy(buffer, msg.msg_text, size);
    buffer[size - 1] = '\0'; // Ensure null-termination

    printf("msg_receive: Received message: '%s'\n", buffer);
    return strlen(buffer);
}

// Function to close message communication
void msg_close(void *context)
{
    MessageContext *ctx = (MessageContext *)context;
    if (ctx)
    {
        printf("msg_close: Removing message queue\n");
        if (msgctl(ctx->msg_id, IPC_RMID, NULL) == -1)
        {
            perror("msg_close: msgctl failed");
        }
        free(ctx);
    }
}

// Function to initialize message communication
CommunicationContext *init_msg_comm()
{
    MessageContext *ctx = malloc(sizeof(MessageContext));
    if (!ctx)
    {
        perror("init_msg_comm: malloc failed");
        exit(EXIT_FAILURE);
    }

    // Create or access message queue
    ctx->msg_id = msgget(MSG_KEY, IPC_CREAT | 0666);
    if (ctx->msg_id == -1)
    {
        perror("init_msg_comm: msgget failed");
        free(ctx);
        exit(EXIT_FAILURE);
    }

    printf("init_msg_comm: Message queue created with ID: %d\n", ctx->msg_id);

    CommunicationContext *comm = malloc(sizeof(CommunicationContext));
    static CommunicationInterface msg_interface = {msg_send, msg_receive, msg_close};

    comm->context = ctx;
    comm->interface = &msg_interface;

    return comm;
}
