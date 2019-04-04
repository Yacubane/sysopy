#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>

#define MAX_COMMAND_LENGTH 64
#define MAX_COMMAND_COUNT 64
#define MAX_PROGRAM_COUNT 64
#define MAX_LINE_SIZE 512

typedef struct command_t
{
    pid_t pid;
    int args_size;
    char **args;
} command_t;

static int create_error(char *message)
{
    fprintf(stderr, "%s\n", message);
    return 1;
}
static int create_error_and_close(FILE *file, char *message)
{
    fclose(file);
    return create_error(message);
}
int main(int argc, char *argv[])
{
    if (argc != 2)
        return create_error("Please provide 1 argument (path)");

    FILE *fd;
    if ((fd = fopen(argv[1], "r")) == NULL)
        return create_error_and_close(fd, "Cannot open list file");

    char line[MAX_LINE_SIZE];

    command_t **commands = malloc(sizeof(command_t *) * MAX_PROGRAM_COUNT);
    for (int i = 0; i < MAX_PROGRAM_COUNT; i++)
    {
        commands[i] = malloc(sizeof(command_t));
        commands[i]->args = malloc(sizeof(char *) * MAX_COMMAND_COUNT);
        for (int j = 0; j < MAX_COMMAND_COUNT; j++)
        {
            commands[i]->args[j] = malloc(sizeof(char) * MAX_COMMAND_LENGTH);
        }
    }

    while (fgets(line, MAX_LINE_SIZE, fd) != NULL)
    {
        int program_index = 0;
        int arg_index = 0;

        char *tok;
        tok = strtok(line, " \n");
        while (tok != NULL)
        {
            if (tok[0] == '|' && tok[1] == '\0')
            {
                commands[program_index]->args_size = arg_index;
                arg_index = 0;
            }
            else
            {
                if (arg_index == 0)
                    program_index++;
                strncpy(commands[program_index - 1]->args[arg_index], tok, MAX_COMMAND_LENGTH);
                arg_index++;
                commands[program_index - 1]->args_size = arg_index;
            }
            tok = strtok(NULL, " \n");
        }

        int old_pipe[2];
        int new_pipe[2];
        int is_old_pipe = 0;

        for (int i = 0; i < program_index; i++)
        {

            if (i < (program_index - 1) && pipe(new_pipe) < 0)
                return create_error("Cannot make pipe");

            pid_t pid = fork();
            if (pid == 0)
            {
                if (i > 0)
                {
                    dup2(old_pipe[0], STDIN_FILENO);
                    close(old_pipe[0]);
                    close(old_pipe[1]);
                }
                if (i < (program_index - 1))
                {
                    dup2(new_pipe[1], STDOUT_FILENO);
                    close(new_pipe[0]);
                    close(new_pipe[1]);
                }

                commands[i]->args[commands[i]->args_size] = NULL;
                execvp(commands[i]->args[0], commands[i]->args);
                exit(0);
            }
            else if (pid > 0)
            {
                commands[i]->pid = pid;
                if (i > 0)
                {
                    close(old_pipe[0]);
                    close(old_pipe[1]);
                }
                if (i < (program_index - 1))
                {
                    old_pipe[0] = new_pipe[0];
                    old_pipe[1] = new_pipe[1];
                    is_old_pipe = 1;
                }
            }
            else
            {
                return create_error("Cannot make fork");
            }
        }
        if (is_old_pipe)
        {
            close(old_pipe[0]);
            close(old_pipe[1]);
        }

        for (int i = 0; i < program_index; i++)
            waitpid(commands[i]->pid, NULL, 0);
    }
    for (int i = 0; i < MAX_PROGRAM_COUNT; i++)
    {
        free(commands[i]);
        for (int j = 0; j < MAX_COMMAND_COUNT; j++)
        {
            free(commands[i]->args[j]);
        }
    }
    free(commands);
    return 0;
}