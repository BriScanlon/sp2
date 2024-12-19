#include "sem_comm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>

#define SHARED_MEMORY_KEY 1234
#define SEMAPHORE_KEY 5678

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

    // Wait on semaphore
    struct sembuf sb = {0, -1, 0};
    if (semop(ctx->sem_id, &sb, 1) == -1)
    {
        perror("sem_send: semop wait failed");
        return -1;
    }

    // Copy message to shared memory
    strncpy(ctx->shared_memory, message, size);

    // Signal semaphore
    sb.sem_op = 1;
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

    // Wait on semaphore
    struct sembuf sb = {0, -1, 0};
    if (semop(ctx->sem_id, &sb, 1) == -1)
    {
        perror("sem_receive: semop wait failed");
        return -1;
    }

    // Copy from shared memory to buffer
    strncpy(buffer, ctx->shared_memory, size);
    buffer[size - 1] = '\0'; // Ensure null-termination

    // Signal semaphore
    sb.sem_op = 1;
    if (semop(ctx->sem_id, &sb, 1) == -1)
    {
        perror("sem_receive: semop signal failed");
        return -1;
    }

    return strlen(buffer);
}

// Function to close semaphore communication
void semaphore_close(void *context)
{
    SemaphoreContext *ctx = (SemaphoreContext *)context;

    // Detach from shared memory
    if (shmdt(ctx->shared_memory) == -1)
    {
        perror("semaphore_close: shmdt failed");
    }

    // Remove shared memory and semaphore if server
    shmctl(ctx->shm_id, IPC_RMID, NULL);
    semctl(ctx->sem_id, 0, IPC_RMID);

    free(ctx);
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

    key_t key = ftok("/tmp", 'S'); // Generate unique key
    if (key == -1)
    {
        perror("init_semaphore_comm: ftok failed");
        free(ctx);
        exit(EXIT_FAILURE);
    }

    // Create shared memory
    ctx->shm_id = shmget(key, BUFFER_SIZE, IPC_CREAT | 0666);
    if (ctx->shm_id == -1)
    {
        perror("init_semaphore_comm: shmget failed");
        free(ctx);
        exit(EXIT_FAILURE);
    }

    // Attach to shared memory
    ctx->shared_memory = shmat(ctx->shm_id, NULL, 0);
    if (ctx->shared_memory == (void *)-1)
    {
        perror("init_semaphore_comm: shmat failed");
        free(ctx);
        exit(EXIT_FAILURE);
    }

    // Create semaphore
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
