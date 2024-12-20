#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include <fcntl.h>
#include <unistd.h>

#define SHMSZ 1024 // Shared memory size
#define SEM_NAME "ipc_semaphore"

int main()
{
    int shmid;
    key_t key = 1000; // Shared memory key
    char *shm, *s;
    sem_t *mutex;

    // Create and initialize the semaphore
    mutex = sem_open(SEM_NAME, O_CREAT, 0644, 1);
    if (mutex == SEM_FAILED)
    {
        perror("Server: Unable to create semaphore");
        sem_unlink(SEM_NAME);
        exit(EXIT_FAILURE);
    }

    // Create the shared memory segment
    if ((shmid = shmget(key, SHMSZ, IPC_CREAT | 0666)) < 0)
    {
        perror("Server: Failure in shmget");
        exit(EXIT_FAILURE);
    }

    // Attach to the shared memory
    if ((shm = shmat(shmid, NULL, 0)) == (char *)-1)
    {
        perror("Server: Failure in shmat");
        exit(EXIT_FAILURE);
    }

    printf("Server: Waiting for client data...\n");

    while (1)
    {
        // Wait for client data
        if (*shm == '\0')
        {
            sleep(1);
            continue;
        }

        sem_wait(mutex);

        // Read client input
        char client_data[SHMSZ];
        strncpy(client_data, shm, SHMSZ);

        // Reset shared memory to avoid stale data
        *shm = '\0';

        sem_post(mutex);

        // Parse the input data
        char flag[10], input_string[SHMSZ];
        sscanf(client_data, "%[^|]|%s", flag, input_string);
        printf("Server: Received flag='%s', string='%s'\n", flag, input_string);

        // Process the input using the `swapParts` binary
        char command[SHMSZ];
        snprintf(command, SHMSZ, "./swapParts %s \"%s\"", flag, input_string);
        printf("Server: Executing command: %s\n", command);

        FILE *fp = popen(command, "r");
        if (fp == NULL)
        {
            perror("Server: Failed to execute swapParts");
            continue;
        }

        char result[SHMSZ];
        if (fgets(result, SHMSZ, fp) == NULL)
        {
            perror("Server: Error reading swapParts output");
            strcpy(result, "Error processing input");
        }
        pclose(fp);

        // Send the result back to the client
        sem_wait(mutex);

        snprintf(shm, SHMSZ, "*%s", result); // Prefix '*' indicates the response is ready

        sem_post(mutex);

        printf("Server: Response sent to client: %s\n", result);
    }

    // Detach and clean up shared memory
    if (shmdt(shm) == -1)
    {
        perror("Server: Failure in shmdt");
        exit(EXIT_FAILURE);
    }

    shmctl(shmid, IPC_RMID, 0);

    // Unlink the semaphore
    sem_close(mutex);
    sem_unlink(SEM_NAME);

    return 0;
}
