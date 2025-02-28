// communication.c
#include "communication.h"
#include <unistd.h> // For read and write
#include <stdio.h>

// Function to send a message
int send_message(CommunicationContext *comm, const char *message, size_t size)
{
    return comm->interface->send(comm->context, message, size);
}

int receive_message(CommunicationContext *comm, char *buffer, size_t size)
{
    int bytesRead = comm->interface->receive(comm->context, buffer, size);
    if (bytesRead > 0)
    {
        buffer[bytesRead] = '\0'; // Null-terminate the received data
    }
    return bytesRead;
}

// Function to close communication
void close_communication(CommunicationContext *comm)
{
    comm->interface->close(comm->context); // Will now call semaphore_close
}
