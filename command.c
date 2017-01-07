#include "command.h"

Command *init_command(char *text)
{
    Command *tmp = malloc(sizeof(Command));
    tmp->string = text;
    tmp->lenght = strlen(text);
    tmp->next = NULL;
    //check if it is a wait response command
    tmp->response = (strcmp(text, "*IDN?") == 0) || strcmp(text, "BUTTON?");
    return tmp;
}
void free_command(Command **command)
{
    Command *tmp = *command;
    free(tmp->string);
    free(*command);
    *command = NULL;
}
void free_command_list(Command **list)
{
    Command *tmp = *list;
    while (tmp)
    {
        Command *next = tmp->next;
        free_command(&tmp);
        tmp = next;
    }
    *list = NULL;
}

void print_command(Command *cmnd)
{
    printf("%s, %d, ", cmnd->string, cmnd->lenght);
    if (cmnd->response)
        printf(" response:true\n");
    else
        printf(" response:true\n");
}