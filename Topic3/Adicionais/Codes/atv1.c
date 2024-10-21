#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <dirent.h> 
#include <ctype.h>  

int is_numeric(const char *str)
{
    for (int i = 0; str[i] != '\0'; i++)
    {
        if (!isdigit(str[i]))
        {
            return 0;
        }
    }
    return 1;
}

void print_process_status(const char *pid)
{
    char status_path[256];
    snprintf(status_path, sizeof(status_path), "/proc"
                                               "/%s"
                                               "/status",
             pid);

    FILE *status_file = fopen(status_path, "r");
    if (!status_file)
    {
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), status_file))
    {
        printf("%s", line);
    }

    fclose(status_file);
}

int main()
{
    char process_name[256];

    printf("Digite a string de busca: ");
    scanf("%s", process_name);

    DIR *proc_dir = opendir("/proc");

    if (!proc_dir)
    {
        perror("Erro ao abrir o diretorio /proc");
        return 1;
    }

    struct dirent *entry;
    while ((entry = readdir(proc_dir)) != NULL)
    {
        if (is_numeric(entry->d_name))
        {
            char status_path[256];
            snprintf(status_path, sizeof(status_path), "/proc"
                                                       "/%s"
                                                       "/status",
                     entry->d_name);

            FILE *status_file = fopen(status_path, "r");

            if (!status_file)
            {
                continue;
            }

            char line[256];
            int found = 0;
            while (fgets(line, sizeof(line), status_file))
            {
                if (strncmp(line, "Name:", strlen("Name:")) == 0)
                {
                    char *name = line + strlen("Name:") + 1;
                    if (strstr(name, process_name) != NULL)
                    {
                        printf("Processo com PID %s encontrado:\n", entry->d_name);
                        print_process_status(entry->d_name);
                        printf("\n");
                        found = 1;
                    }
                    break;
                }
            }

            fclose(status_file);

            if (!found)
            {
                continue;
            }
        }
    }

    closedir(proc_dir);
    return 0;
}
