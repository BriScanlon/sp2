#ifndef SEM_COMM_H
#define SEM_COMM_H

#include "communication.h"

// Function to initialize semaphore communication context
CommunicationContext *init_semaphore_comm();

// Rename the custom semaphore close function
void semaphore_close(void *context);

#endif