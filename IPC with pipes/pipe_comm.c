// pipe_comm.c
#include "communication.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int pipe_send(void *context, const char *message, size_t size)
{
    int *pipe_fd = (int *)context;
    printf("pipe_send: Writing to FD=%d, message='%s'\n", pipe_fd[1], message); // Debug log
    int bytesWritten = write(pipe_fd[1], message, size);
    if (bytesWritten <= 0)
    {
        perror("pipe_send failed");
    }
    return bytesWritten;
}

int pipe_receive(void *context, char *buffer, size_t size)
{
    int *pipe_fd = (int *)context;
    printf("pipe_receive: Reading from FD=%d\n", pipe_fd[0]); // Debug log
    int bytesRead = read(pipe_fd[0], buffer, size);
    if (bytesRead > 0)
    {
        buffer[bytesRead] = '\0';                        // Null-terminate the buffer
        printf("pipe_receive: Received '%s'\n", buffer); // Debug log
    }
    else
    {
        perror("pipe_receive failed");
    }
    return bytesRead;
}

void pipe_close(void *context)
{
    int *pipe_fd = (int *)context;
    if (pipe_fd[0] != -1)
        close(pipe_fd[0]);
    if (pipe_fd[1] != -1)
        close(pipe_fd[1]);
}

// Function to initialize pipe communication context
CommunicationContext *init_pipe_comm(int *pipe_fd)
{
    CommunicationContext *comm = malloc(sizeof(CommunicationContext));
    static CommunicationInterface pipe_interface = {pipe_send, pipe_receive, pipe_close};

    comm->context = pipe_fd;
    comm->interface = &pipe_interface;

    // Debug log for pipe initialization
    printf("init_pipe_comm: Read FD=%d, Write FD=%d\n", pipe_fd[0], pipe_fd[1]);

    return comm;
}
