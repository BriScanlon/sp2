// pipe_comm.h
#ifndef PIPE_COMM_H
#define PIPE_COMM_H

#include "communication.h"

// Function to initialize pipe communication context
CommunicationContext *init_pipe_comm(int *pipe_fd);

#endif // PIPE_COMM_H