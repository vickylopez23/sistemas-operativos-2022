#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <wait.h>
#include <unistd.h>

#include "command.h"
#include "execute.h"
#include "parser.h"
#include "parsing.h"
#include "builtin.h"

static void show_prompt(void)
{
    char *hostname = malloc(sizeof(char) * 30); // 30 caracteres del hostname maximo.
    gethostname(hostname, 30);

    char *username = malloc(sizeof(char) * 30);
    getlogin_r(username, 30);

    char *path = malloc(sizeof(char) * 120);
    getcwd(path, 120);
    printf("MyBash|%s@%s:%s>>", username, hostname, path);
    fflush(stdout);

    free(hostname);
    free(username);
    free(path);
}

int main(int argc, char *argv[])
{
    pipeline pipe;
    Parser input;
    bool quit = false;

    input = parser_new(stdin);
    while (!quit)
    {
        show_prompt();
        pipe = parse_pipeline(input);
        if (pipe != NULL)
        {
            execute_pipeline(pipe);
            pipeline_destroy(pipe);
        }
        // Finalizamos los procesos zombies.
        while (waitpid(-1, NULL, WNOHANG) > 0);
        quit = parser_at_eof(input);
    }
    parser_destroy(input);
    input = NULL;
    return EXIT_SUCCESS;
}
