#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include <fcntl.h>
#include <unistd.h>

#define SHMSZ 2048       // Increased size to handle larger strings
char SEM_NAME[] = "vik"; // Semaphore name

int main()
{
    char input_data[SHMSZ], flag[64], input_string[1024], result[SHMSZ];
    int shmid;
    key_t key = 1000; // Shared memory key
    char *shm;
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
        // Wait for client input
        sem_wait(mutex);
        if (*shm == '\0')
        { // No new data
            sem_post(mutex);
            sleep(1);
            continue;
        }

        // Copy client data from shared memory
        strncpy(input_data, shm, SHMSZ);
        *shm = '\0'; // Clear shared memory to avoid reprocessing old data
        sem_post(mutex);

        // Parse client input
        if (sscanf(input_data, "%63[^|]|%1023[^\n]", flag, input_string) != 2)
        {
            fprintf(stderr, "Server: Invalid input format.\n");
            continue;
        }

        // Debug logging
        printf("Server: Received flag='%s', string='%s'\n", flag, input_string);

        // Sanitize inputs
        if (strlen(flag) > 50 || strlen(input_string) > 500)
        {
            fprintf(stderr, "Server: Input exceeds allowed length.\n");
            continue;
        }

        // Construct command for `swapParts`
        char command[SHMSZ];
        if (snprintf(command, sizeof(command), "./swapParts '%s' '%s'", flag, input_string) >= sizeof(command))
        {
            fprintf(stderr, "Server: Command exceeds buffer size. Truncating input.\n");
            continue;
        }

        // Execute command
        FILE *fp = popen(command, "r");
        if (fp == NULL)
        {
            perror("Server: Error running swapParts");
            snprintf(result, sizeof(result), "Error running swapParts");
        }
        else
        {
            if (fgets(result, sizeof(result), fp) == NULL)
            {
                perror("Server: No output from swapParts");
                snprintf(result, sizeof(result), "No output from swapParts");
            }
            pclose(fp);
        }

        // Send the result back to the client
        sem_wait(mutex);
        if (snprintf(shm, SHMSZ, "*%s", result) >= SHMSZ)
        {
            fprintf(stderr, "Server: Result exceeds shared memory size. Truncating output.\n");
        }
        sem_post(mutex);

        printf("Server: Sent result: %s\n", result);
    }

    // Clean up
    if (shmdt(shm) == -1)
    {
        perror("Server: Failure in shmdt");
    }
    shmctl(shmid, IPC_RMID, 0);
    sem_close(mutex);
    sem_unlink(SEM_NAME);

    return 0;
}
