#include <stdlib.h>
#include <stdbool.h>

#include "parsing.h"
#include "parser.h"
#include "command.h"

static scommand parse_scommand(Parser p, bool *was_last_pipe, bool *was_last_bg)
{
    *was_last_pipe = false;
    *was_last_bg = false;
    parser_skip_blanks(p);
    parser_op_pipe(p, was_last_pipe);
    parser_op_background(p, was_last_bg);
    if (*was_last_pipe || *was_last_bg) // SCommand cant begin with | or &
    {
        printf("Error sintactico en el comando.\n");
        return NULL;
    }
    scommand scmd = scommand_new();
    arg_kind_t type;
    char *arg;
    while (!parser_at_eof(p))
    {
        parser_skip_blanks(p);
        parser_op_pipe(p, was_last_pipe);
        parser_op_background(p, was_last_bg);
        if (*was_last_pipe || *was_last_bg) // Pipe or & Reached. End of SCommand Parsing.
        {
            break;
        }
        arg = parser_next_argument(p, &type);
        if (arg == NULL)
        {
            // Se llego a un salto de linea \n
            break;
        }
        if (type == ARG_INPUT) // Se parseo un Redirector In <
        {
            if (scommand_get_redir_in(scmd) != NULL) // Si ya habia otro Redirector In entonces ERROR
            {
                scommand_destroy(scmd);
                free(arg);
                printf("No se permiten multiples redirectores de entrada.\n");
                return NULL;
            }
            if (arg == NULL)
            {
                scommand_destroy(scmd);
                printf("Error sintactico en el comando.\n");
                return NULL;
            }
            scommand_set_redir_in(scmd, arg);
            continue;
        }
        if (type == ARG_OUTPUT) // Se parseo un Redirector Out >
        {
            if (scommand_get_redir_out(scmd) != NULL) // Si ya habia otro Redirector Out entonces ERROR
            {
                scommand_destroy(scmd);
                free(arg);
                printf("No se permiten multiples redirectores de salida.\n");
                return NULL;
            }
            if (arg == NULL)
            {
                scommand_destroy(scmd);
                printf("Error sintactico en el comando.\n");
                return NULL;
            }
            scommand_set_redir_out(scmd, arg);
            continue;
        }
        scommand_push_back(scmd, arg);
    }
    if (scommand_length(scmd) == 0)
    {
        if (scommand_get_redir_in(scmd) != NULL || scommand_get_redir_out(scmd) != NULL)
        {
            printf("Error sintactico en el comando.\n");
        }
        scommand_destroy(scmd);
        scmd = NULL;
    }
    return scmd;
}

pipeline parse_pipeline(Parser p)
{

    pipeline result = pipeline_new();
    scommand scmd = NULL;

    bool error = false;
    bool garbage = false;
    bool was_last_pipe = true;
    bool was_last_bg = false;

    while (was_last_pipe)
    {
        scmd = parse_scommand(p, &was_last_pipe, &was_last_bg);
        error = (scmd == NULL);
        if (error)
        {
            parser_garbage(p, &garbage);
            pipeline_destroy(result);
            return NULL;
        }
        pipeline_push_back(result, scmd);
    }
    pipeline_set_wait(result, !was_last_bg);

    parser_garbage(p, &garbage);
    if (garbage)
    {
        printf("Error de parseo, se encontro basura al final de la pipeline: %s\n", parser_last_garbage(p));
        pipeline_destroy(result);
        return NULL;
    }

    return result;
}