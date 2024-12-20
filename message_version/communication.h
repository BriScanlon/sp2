// communication.h
#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <stddef.h>

#define BUFFER_SIZE 1024

typedef struct {
    // Define function pointers for send, receive, and close operations
    int (*send)(void *context, const char *message, size_t size);
    int (*receive)(void *context, char *buffer, size_t size);
    void (*close)(void *context);
} CommunicationInterface;

// Define a generic context for IPC communication (e.g., pipes, messages, etc.)
typedef struct {
    void *context; // IPC-specific context (e.g., pipe file descriptors, socket, etc.)
    CommunicationInterface *interface; // Function pointer to the specific implementation
} CommunicationContext;

// Functions to abstract IPC communication
int send_message(CommunicationContext *comm, const char *message, size_t size);
int receive_message(CommunicationContext *comm, char *buffer, size_t size);
void close_communication(CommunicationContext *comm);

#endif // COMMUNICATION_H
