#include <stdlib.h>
#include <stdbool.h>
#include <glib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <pwd.h>

#include "command.h"
#include "builtin.h"
#include "tests/syscall_mock.h"
#include "strextra.h"

struct internal_cmd
{
    char *nombre;
    void (*executor)(scommand cmd); // function pointer
};

static void sysc_exit(scommand cmd)
{
    close(STDIN_FILENO);
    // printf("Bye bye!\n");
    printf("╲╭━━━━╮╲╲ ╭━━━━━━━━━━━╮\n");
    printf("╲┃╭╮╭╮┃╲╲ ┃  Bye bye! ┃          \n");
    printf("┗┫┏━━┓┣┛╲ ╰┳━━━━━━━━━━╯\n");
    printf("╲┃╰━━╯┃ ━━━╯\n");
    printf("╲╰┳━━┳╯\n");
}

static void sysc_cd(scommand cmd) // función que ejecuta cd
{
    scommand_pop_front(cmd);
    if (!scommand_is_empty(cmd))
    {
        if (strncmp(scommand_front(cmd), "/", 1) == 0)
        {                               // usando la funcion strncmp que compara dos cadenas, me fijo si el segundo argumento del scommand empieza con "/". Esto quiere decir que se trata de un path absoluto.
            chdir(scommand_front(cmd)); // syscall chdir
        }
        else
        {
            char *cwd = NULL;
            cwd = getcwd(cwd, 0);                       // obtengo el current working directory
            if (!scommand_is_empty(cmd) && cwd != NULL) // chequea si getcwd funcionó correctamente
            {
                char *tmp = strmerge(cwd, "/");
                char *path = strmerge(tmp, scommand_front(cmd)); // concatena el path relativo al current working directory
                free(tmp);
                free(cwd);
                chdir(path); // syscall chdir
                free(path);
            }
            else
            {
                perror("Error obteniendo el current working directory.");
            }
        }
    }
    else
    {
        chdir(getpwuid(getuid())->pw_dir);
    }
}
static void sysc_help(scommand cmd) // función que ejecuta help
{
    printf("\nShell: MYBASH.\n");
    printf("\n");
    printf("\n──────────▄▌▐▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀​▀▀▀▀▀▀▌\n");
    printf("───────▄▄██▌█ ---> Te presentamos la MYBASH_NETA, subite! <--- \n");
    printf("────███████▌█▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄​▄▄▄▄▄​▄▄▄▄▄​▄▄▄▄▄​▄▄▄▄​▄​▄​▄▄▄▄▄▄▌\n");
    printf("────▀(@)▀▀▀▀▀▀▀(@)(@)▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀​▀▀▀▀(@)▀.......\n");
    printf("\n");
    printf("        Ejecuta desde comandos simples hasta multiples pipes!\n");
    printf("Los comandos simples admiten redirectores de entrada, salida y mas ;)\n");
    printf("   No te olvide de probar el modo de ejecucion en segundo plano :D\n\n");
    printf("Creadores:\n");
    printf("    - Victoria Lopez,\n");
    printf("    - Tomas Ponce Gessi,\n");
    printf("    - Jeremias Baez,\n");
    printf("    - Constanza Gomez\n\n");


    printf("Comandos internos:\n");
    printf("    - CD: Permiten mover el CWD (Current Working Directory) del proceso\n");
    printf("    - EXIT: Finaliza la ejecución de MyBash de manera limpia\n");
    printf("    - HELP: Imprime en pantalla un mensaje de ayuda de como usar la terminal y quienes son sus creadores\n\n");
}

static GList *internal_commands_list(void)
{
    // Creo una lista con los nombres de los comandos internos, usando Glist
    GList *list = NULL;
    struct internal_cmd *cd = malloc(sizeof(struct internal_cmd));
    cd->nombre = "cd";
    cd->executor = sysc_cd;
    list = g_list_append(list, cd);

    struct internal_cmd *exit = malloc(sizeof(struct internal_cmd));
    exit->nombre = "exit";
    exit->executor = sysc_exit;
    list = g_list_append(list, exit);

    struct internal_cmd *help = malloc(sizeof(struct internal_cmd));
    help->nombre = "help";
    help->executor = sysc_help;
    list = g_list_append(list, help);

    // Si se desea agregar un nuevo comando interno, aqui se añade su nombre y su función asociada,
    // luego se debe utilizar la función g_list_append para añadirla a la lista
    return list;
}

bool builtin_is_internal(scommand scmd)
{
    assert(scmd != NULL);
    bool internal = false; // variable que aloja si el comando es interno o no
    GList *list = internal_commands_list();
    int length = g_list_length(list);
    for (int i = 0; i < length; i++)
    {
        struct internal_cmd *data = (struct internal_cmd *)g_list_nth_data(list, i);
        if (strcmp(scommand_front(scmd), data->nombre) == 0) // comparo el comando pasado por argumento con mi lista de comandos
        {
            internal = true;
            break;
        }
    }

    while (g_list_length(list) != 0)
    {
        char *tmp = (char *)g_list_nth_data(list, 0);
        list = g_list_remove(list, tmp);
        free(tmp);
    }
    g_list_free(list);
    return internal;
}

/*
 * Indica si el comando alojado en `cmd` es un comando interno
 *
 * REQUIRES: cmd != NULL
 *
 */
void builtin_run(scommand scmd)
{
    assert(builtin_is_internal(scmd));
    GList *list = internal_commands_list();
    int length = g_list_length(list);

    for (int i = 0; i < length; i++)
    {
        struct internal_cmd *data = (struct internal_cmd *)g_list_nth_data(list, i);
        if (strcmp(scommand_front(scmd), data->nombre) == 0) // Comparo el comando pasado por argumento con mi lista de comandos
        {
            (data->executor)(scmd); // Si hay coincidencia, ejecuto la función asociada al comando
            break;
        }
    }
    while (g_list_length(list) != 0)
    {
        char *tmp = (char *)g_list_nth_data(list, 0);
        list = g_list_remove(list, tmp);
        free(tmp);
    }

    g_list_free(list);
}

/*
 * Ejecuta un comando interno
 *
 * REQUIRES: {builtin_is_internal(cmd)}
 *
 */

bool builtin_alone(pipeline p)
{
    assert(p != NULL);
    return pipeline_length(p) == 1 && builtin_is_internal(pipeline_front(p));
}
/*
 * Indica si el pipeline tiene solo un elemento y si este se corresponde a un
 * comando interno.
 *
 * REQUIRES: p != NULL
 *
 * ENSURES:
 *
 * builtin_alone(p) == pipeline_length(p) == 1 &&
 *                     builtin_is_internal(pipeline_front(p))
 *
 *
 */
