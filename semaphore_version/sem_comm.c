#include "sem_comm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>

#define SHARED_MEMORY_KEY 10014635
#define SEMAPHORE_KEY 10014630
#define BUFFER_SIZE 1024

typedef struct
{
    int shm_id;
    int sem_id;
    char *shared_memory;
} SemaphoreContext;

// Function to send a message
int sem_send(void *context, const char *message, size_t size)
{
    SemaphoreContext *ctx = (SemaphoreContext *)context;

    if (!ctx || !ctx->shared_memory)
    {
        fprintf(stderr, "sem_send: Invalid context or shared memory\n");
        return -1;
    }

    // Wait for the semaphore (sem_wait)
    struct sembuf sb = {0, -1, 0}; // sem_wait: Wait for the semaphore
    printf("sem_send: Waiting for semaphore...\n");
    if (semop(ctx->sem_id, &sb, 1) == -1)
    {
        perror("sem_send: semop wait failed");
        return -1;
    }

    // Copy the message to shared memory
    printf("sem_send: Writing to shared memory: '%s'\n", message);
    strncpy(ctx->shared_memory, message, size);

    // Signal the semaphore (sem_post)
    sb.sem_op = 1; // sem_post: Signal the semaphore
    if (semop(ctx->sem_id, &sb, 1) == -1)
    {
        perror("sem_send: semop signal failed");
        return -1;
    }

    return size;
}

// Function to receive a message
int sem_receive(void *context, char *buffer, size_t size)
{
    SemaphoreContext *ctx = (SemaphoreContext *)context;

    if (!ctx || !ctx->shared_memory)
    {
        fprintf(stderr, "sem_receive: Invalid context or shared memory\n");
        return -1;
    }

    // Wait for the semaphore (sem_wait)
    struct sembuf sb = {0, -1, 0}; // sem_wait: Wait for the semaphore
    printf("sem_receive: Waiting for semaphore...\n");
    if (semop(ctx->sem_id, &sb, 1) == -1)
    {
        perror("sem_receive: semop wait failed");
        return -1;
    }

    // Copy from shared memory to the buffer
    printf("sem_receive: Read from shared memory: '%s'\n", ctx->shared_memory);
    strncpy(buffer, ctx->shared_memory, size);
    buffer[size - 1] = '\0'; // Ensure null-termination

    // Signal the semaphore (sem_post)
    sb.sem_op = 1; // sem_post: Signal the semaphore
    if (semop(ctx->sem_id, &sb, 1) == -1)
    {
        perror("sem_receive: semop signal failed");
        return -1;
    }

    return strlen(buffer);
}

// Function to close semaphore communication
// Function to close semaphore communication
void semaphore_close(void *context)
{
    SemaphoreContext *ctx = (SemaphoreContext *)context;

    if (ctx)
    {
        // Detach shared memory if it is still attached
        if (ctx->shared_memory && ctx->shared_memory != (void *)-1)
        {
            printf("semaphore_close: Detaching shared memory\n");
            if (shmdt(ctx->shared_memory) == -1)
            {
                perror("semaphore_close: shmdt failed");
            }
            else
            {
                ctx->shared_memory = NULL; // Prevent further detachments
            }
        }

        // Only remove shared memory if it's not already removed
        if (ctx->shm_id >= 0 && ctx->shared_memory == NULL)
        {
            printf("semaphore_close: Removing shared memory segment\n");
            if (shmctl(ctx->shm_id, IPC_RMID, NULL) == -1)
            {
                perror("semaphore_close: shmctl failed");
            }
            ctx->shm_id = -1; // Prevent further attempts to remove
        }

        // Remove semaphore if it is valid and not already removed
        if (ctx->sem_id >= 0)
        {
            printf("semaphore_close: Removing semaphore\n");
            if (semctl(ctx->sem_id, 0, IPC_RMID) == -1)
            {
                perror("semaphore_close: semctl failed");
            }
            ctx->sem_id = -1; // Prevent further attempts to remove
        }

        // Free allocated memory (only once)
        if (ctx != NULL)
        {
            printf("semaphore_close: Freeing allocated memory\n");
            free(ctx);
            ctx = NULL; // Prevent double free
        }
    }
    else
    {
        printf("semaphore_close: Context was NULL\n");
    }
}

// Function to initialize semaphore communication
CommunicationContext *init_semaphore_comm()
{
    SemaphoreContext *ctx = malloc(sizeof(SemaphoreContext));
    if (!ctx)
    {
        perror("init_semaphore_comm: malloc failed");
        exit(EXIT_FAILURE);
    }

    ctx->shm_id = -1; // Initialize to invalid ID
    ctx->sem_id = -1;
    ctx->shared_memory = (void *)-1;

    // Create shared memory
    printf("init_semaphore_comm: Creating shared memory\n");
    ctx->shm_id = shmget(SHARED_MEMORY_KEY, BUFFER_SIZE, IPC_CREAT | 0666);
    if (ctx->shm_id == -1)
    {
        perror("init_semaphore_comm: shmget failed");
        free(ctx);
        exit(EXIT_FAILURE);
    }

    // Attach to shared memory
    printf("init_semaphore_comm: Attaching to shared memory\n");
    ctx->shared_memory = shmat(ctx->shm_id, NULL, 0);
    if (ctx->shared_memory == (void *)-1)
    {
        perror("init_semaphore_comm: shmat failed");
        shmctl(ctx->shm_id, IPC_RMID, NULL);
        free(ctx);
        exit(EXIT_FAILURE);
    }

    // Create semaphore
    printf("init_semaphore_comm: Creating semaphore\n");
    ctx->sem_id = semget(SEMAPHORE_KEY, 1, IPC_CREAT | 0666);
    if (ctx->sem_id == -1)
    {
        perror("init_semaphore_comm: semget failed");
        shmdt(ctx->shared_memory);
        shmctl(ctx->shm_id, IPC_RMID, NULL);
        free(ctx);
        exit(EXIT_FAILURE);
    }

    // Initialize semaphore value to 1
    printf("init_semaphore_comm: Initializing semaphore value\n");
    if (semctl(ctx->sem_id, 0, SETVAL, 1) == -1)
    {
        perror("init_semaphore_comm: semctl failed");
        shmdt(ctx->shared_memory);
        shmctl(ctx->shm_id, IPC_RMID, NULL);
        semctl(ctx->sem_id, 0, IPC_RMID);
        free(ctx);
        exit(EXIT_FAILURE);
    }

    CommunicationContext *comm = malloc(sizeof(CommunicationContext));
    static CommunicationInterface sem_interface = {sem_send, sem_receive, semaphore_close};

    comm->context = ctx;
    comm->interface = &sem_interface;

    return comm;
}
