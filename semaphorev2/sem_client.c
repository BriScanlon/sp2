#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include <fcntl.h>
#include <unistd.h>

#define SHMSZ 2048
char SEM_NAME[] = "vik";

int main(int argc, char *argv[])
{
    char *flag, *input_string;
    int shmid;
    key_t key = 1000;
    char *shm;
    sem_t *mutex;

    // Validate arguments
    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s <flag> <string>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    flag = argv[1];
    input_string = argv[2];

    // Connect to the semaphore
    mutex = sem_open(SEM_NAME, 0);
    if (mutex == SEM_FAILED)
    {
        perror("Client: Unable to access semaphore");
        exit(EXIT_FAILURE);
    }

    // Connect to the shared memory segment
    if ((shmid = shmget(key, SHMSZ, 0666)) < 0)
    {
        perror("Client: Failure in shmget");
        exit(EXIT_FAILURE);
    }

    // Attach to the shared memory
    if ((shm = shmat(shmid, NULL, 0)) == (char *)-1)
    {
        perror("Client: Failure in shmat");
        exit(EXIT_FAILURE);
    }

    // Write flag and string to shared memory
    sem_wait(mutex);
    if (snprintf(shm, SHMSZ, "%s|%s", flag, input_string) >= SHMSZ)
    {
        fprintf(stderr, "Client: Data exceeds shared memory size.\n");
        sem_post(mutex);
        exit(EXIT_FAILURE);
    }
    sem_post(mutex);

    printf("Client: Data sent to server. Waiting for response...\n");

    // Wait for the server to process and provide the result
    while (*shm != '*')
    {
        sleep(1);
    }

    // Read the result from shared memory
    sem_wait(mutex);
    printf("Client: Server response: %s\n", shm + 1); // Response starts after '*'
    sem_post(mutex);

    // Detach from shared memory
    if (shmdt(shm) == -1)
    {
        perror("Client: Failure in shmdt");
        exit(EXIT_FAILURE);
    }

    // Close the semaphore
    sem_close(mutex);

    return 0;
}
