#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

void swapPartsUsingExecf(char *option, char *input, char *output)
{
    int pipe_fd[2];
    if (pipe(pipe_fd) == -1)
    {
        perror("pipe failed");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();
    if (pid == 0)
    {
        // Child process
        close(pipe_fd[0]);               // Close read end
        dup2(pipe_fd[1], STDOUT_FILENO); // Redirect stdout to write end of pipe
        close(pipe_fd[1]);

        // If the flag is empty, do not pass any option to swapParts
        if (option == NULL || strlen(option) == 0)
        {
            // Pass only the input to swapParts, without a flag
            char *args[] = {"./swapParts", input, NULL};
            execvp(args[0], args);
        }
        else
        {
            // Pass the flag as well if it's not empty
            char *args[] = {"./swapParts", option, input, NULL};
            execvp(args[0], args);
        }

        // If execvp fails, exit with an error
        perror("execvp failed");
        exit(EXIT_FAILURE);
    }
    else if (pid > 0)
    {
        // Parent process
        close(pipe_fd[1]); // Close write end
        wait(NULL);        // Wait for child process to complete

        // Read the output from swapParts
        int bytesRead = read(pipe_fd[0], output, 1024);
        if (bytesRead > 0)
        {
            output[bytesRead] = '\0'; // Null-terminate the string
        }
        else
        {
            perror("Failed to read output from swapParts");
            strcpy(output, "Error processing input");
        }
        close(pipe_fd[0]);
    }
    else
    {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }
}
