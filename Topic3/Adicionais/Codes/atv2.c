#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/wait.h>

#define MAX_CMD_LEN 1024
#define MAX_ARGS 100

int main()
{
    char command[MAX_CMD_LEN];

    while (1)
    {

        printf("mini-shell> ");
        fflush(stdout);

        if (fgets(command, sizeof(command), stdin) == NULL)
        {
            perror("fgets");
            continue;
        }

        command[strcspn(command, "\n")] = '\0';

        if (strcmp(command, "exit") == 0)
        {
            break;
        }

        if (fork() == 0)
        { // Processo filho
            char *args[MAX_ARGS];
            char *token = strtok(command, " ");
            int i;

            for (i = 0; token != NULL && i < MAX_ARGS - 1; i++)
            {
                args[i] = token;
                token = strtok(NULL, " ");
            }
            printf("i: %d\n", i);
            args[i] = NULL;

            execvp(args[0], args);
            perror("execvp");
            exit(EXIT_FAILURE);
        }
        else
        {               // Processo pai
            wait(NULL); // Espera o filho terminar
        }
    }

    return 0;
}
