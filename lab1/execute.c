#include <unistd.h> //for system calls
#include <stdio.h>  //for bool
#include <stdlib.h> //for EXIT_FAILURE, EXIT_SUCCESS
#include <fcntl.h>  //for open args
#include <errno.h>
#include <wait.h>
#include <assert.h>
#include <string.h>
#include <fcntl.h>

#include "glib.h"
#include "builtin.h"
#include "command.h"
#include "execute.h"
#include "tests/syscall_mock.h"

static char **scommand_to_array_pointer(scommand s)
{
    assert(!scommand_is_empty(s));
    unsigned int length = scommand_length(s);
    char **new_array = calloc(length + 1, sizeof(char *));
    for (unsigned int i = 0; i < length; ++i)
    {
        new_array[i] = strdup(scommand_front(s));
        scommand_pop_front(s);
    }
    new_array[length] = NULL;
    return new_array;
}
static void destroy_array_pointer(char **killme, unsigned int length)
{
    for (unsigned int i = 0; i < length; i++)
    {

        free(killme[i]);
        killme[i] = NULL;
    }

    free(killme);
    killme = NULL;
    assert(killme == NULL);
}

static bool init_scommand_redirects(scommand scmd)
{
    // Se obtiene las dos redirecciones, IN y OUT
    char *redir_out = scommand_get_redir_out(scmd);
    char *redir_in = scommand_get_redir_in(scmd);

    if (redir_in != NULL) // Si hay redireccion de entrada entonces:
    {
        // Abrimos el archivo en modo lectura.
        int fd_in = open(redir_in, O_RDONLY, 0);

        if (fd_in == -1) // Si ocurrio un error abriendo el archivo entonces print y retornamos true
        {
            printf("Error abriendo redireccion de entrada: %s\n", redir_in);
            return true;
        }
        // Vincluamos el File Descriptor del archivo a leer con el File Descriptor de STDIN.
        dup2(fd_in, STDIN_FILENO);
        // Cerramos el File Descriptor obtenido por open.
        close(fd_in);
    }
    if (redir_out != NULL) // Si hay redireccion de salida entonces:
    {
        // Abrimos el archivo en modo escritura
        int fd_out = open(redir_out, O_WRONLY, 0);
        if (fd_out == -1) // Si ocurrio un error abriendo el archivo entonces lo intentamos crear.
        {
            // Creamos el archivo con permisos 0644
            fd_out = creat(redir_out, 0644);
            if (fd_out == -1) // Si ocurrio un error creando el archivo entonces print y retornamos true.
            {
                printf("Error creando redireccion de salida: %s\n", redir_in);
                return true;
            }
        }
        // Vincluamos el File Descriptor del archivo a escribir con el File Descriptor de STDOUT.
        dup2(fd_out, STDOUT_FILENO);
        // Cerramos el File Descriptor obtenido por open.
        close(fd_out);
    }
    // Si estamos en este punto entonces se hicieron los redirectores y no ocurrieron errores,
    // retornamos false para indicarlo.
    return false;
}

static void execute_scommand(scommand scmd)
{
    unsigned int length = scommand_length(scmd);
    char **scmd_array = scommand_to_array_pointer(scmd);

    bool error = init_scommand_redirects(scmd);
    if (error)
    {
        printf("Error ejecutando el comando %s\n", scmd_array[0]);
        return;
    }

    execvp(scmd_array[0], scmd_array);
    printf("Error ejecutando el comando %s, talvez comando incorrecto?\n", scmd_array[0]);
    destroy_array_pointer(scmd_array, length);
}

static pid_t execute_pipe(int fd_in, int fd_out, scommand scmd, int *pipe_to_close)
{

    pid_t id = fork();

    if (id == 0)
    {
        if (fd_in != STDIN_FILENO)
        {

            dup2(fd_in, STDIN_FILENO);
        }
        if (fd_out != STDOUT_FILENO)
        {

            dup2(fd_out, STDOUT_FILENO);
        }
        if (pipe_to_close != NULL)
        {
            close(pipe_to_close[0]);
            close(pipe_to_close[1]);
        }
        execute_scommand(scmd);
        exit(EXIT_FAILURE);
    }

    return id;
}

void execute_pipeline(pipeline pipeline)
{
    assert(pipeline != NULL);
    if (pipeline_length(pipeline) <= 0)
    {
        return;
    }

    if (builtin_is_internal(pipeline_front(pipeline)))
    {
        if (pipeline_length(pipeline) > 1)
        {
            printf("No se permiten comandos internos con pipes.\n");
            return;
        }
        builtin_run(pipeline_front(pipeline));
        // pipeline_destroy(pipeline);
        return;
    }

    scommand scmd = pipeline_front(pipeline);
    bool foreground = pipeline_get_wait(pipeline);
    pid_t pid;

    if (pipeline_length(pipeline) == 1)
    {
        pid = execute_pipe(STDIN_FILENO, STDOUT_FILENO, scmd, NULL);
        if (pid < 0)
        {
            printf("Error creando el proceso para ejecutar el SCMD\n");
            return;
        }

        if (foreground)
        {
            waitpid(pid, NULL, 0);
        }
        return;
    }

    int fd_in = STDIN_FILENO;

    int p[2];

    GList *child_pid = NULL;

    int *tmp_pid = NULL;

    unsigned int length = pipeline_length(pipeline);
    for (unsigned int i = 0; i < length - 1; i++)
    {
        pipe(p);

        pid = execute_pipe(fd_in, p[1], scmd, p);

        if (i < length - 2)
        {
            close(p[1]);
        }

        if (1 <= i && i < length - 1)
        {
            close(fd_in);
        }

        if (pid < 0)
        {

            break;
        }

        tmp_pid = malloc(sizeof(pid_t));
        *tmp_pid = pid;
        child_pid = g_list_append(child_pid, tmp_pid);

        fd_in = p[0];

        pipeline_pop_front(pipeline);

        scmd = pipeline_front(pipeline);
    }
    if (pid > 0) // Esto solo pasa si no hubo un error creando algun proceso.
    {
        pid = execute_pipe(fd_in, STDOUT_FILENO, scmd, p);

        close(fd_in);
        close(p[1]);

        if (pid > 0) // Si no hubo un error entonces:
        {
            tmp_pid = malloc(sizeof(pid_t));
            *tmp_pid = pid;
            child_pid = g_list_append(child_pid, tmp_pid);

            if (foreground) // Si hay que esperar al ultimo SCMD entonces:
            {
                while (g_list_length(child_pid) != 0)
                {

                    tmp_pid = (pid_t *)g_list_nth_data(child_pid, 0);

                    waitpid(*tmp_pid, NULL, 0);

                    child_pid = g_list_remove(child_pid, tmp_pid);
                    free(tmp_pid);
                }
            }

            return;
        }
    }
    printf("Error creando el proceso para ejecutar la pipeline\n");
}