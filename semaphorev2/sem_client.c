#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include <fcntl.h>
#include <unistd.h>

#define SHMSZ 1024   // Shared memory size
#define SEM_NAME "ipc_semaphore"

int main() {
    int shmid;
    key_t key = 1000;  // Shared memory key
    char *shm, *s;
    sem_t *mutex;

    // Connect to the semaphore
    mutex = sem_open(SEM_NAME, 0);
    if (mutex == SEM_FAILED) {
        perror("Client: Unable to access semaphore");
        exit(EXIT_FAILURE);
    }

    // Connect to the shared memory segment
    if ((shmid = shmget(key, SHMSZ, 0666)) < 0) {
        perror("Client: Failure in shmget");
        exit(EXIT_FAILURE);
    }

    // Attach to the shared memory
    if ((shm = shmat(shmid, NULL, 0)) == (char *)-1) {
        perror("Client: Failure in shmat");
        exit(EXIT_FAILURE);
    }

    // Input flag and string from the user
    char flag[10], input_string[SHMSZ];
    printf("Enter flag: ");
    scanf("%s", flag);
    getchar(); // Consume newline left by scanf
    printf("Enter string: ");
    fgets(input_string, SHMSZ, stdin);
    input_string[strcspn(input_string, "\n")] = '\0';  // Remove newline character

    // Write flag and string to shared memory
    sem_wait(mutex);
    snprintf(shm, SHMSZ, "%s|%s", flag, input_string);  // Write in the format: flag|string
    sem_post(mutex);

    printf("Client: Data sent to server. Waiting for response...\n");

    // Wait for the server to process and provide the result
    while (*shm != '*') {
        sleep(1);
    }

    // Read the result from shared memory
    sem_wait(mutex);
    printf("Client: Server response: %s\n", shm + 1);  // Server writes the response after '*'
    sem_post(mutex);

    // Detach from shared memory
    if (shmdt(shm) == -1) {
        perror("Client: Failure in shmdt");
        exit(EXIT_FAILURE);
    }

    // Close the semaphore
    sem_close(mutex);

    return 0;
}
