#ifndef MSG_COMM_H
#define MSG_COMM_H

#include "communication.h"

// Function prototypes
int msg_send(void *context, const char *message, size_t size);
int msg_receive(void *context, char *buffer, size_t size);
void msg_close(void *context); // Reverted to original signature
CommunicationContext *init_msg_comm();

#endif // MSG_COMM_H
