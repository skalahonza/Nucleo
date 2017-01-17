#include "command.h"
#include <errno.h>

Command *init_command(char *text)
{
    Command *tmp = malloc(sizeof(Command));
    tmp->string = text;
    tmp->lenght = strlen(text);
    tmp->next = NULL;
    //check if it is a wait response command
    if ((strstr(text, "?") != NULL) || (strstr(text, "BUTTON?") != NULL))
        tmp->response = true;
    else
        tmp->response = false;
    return tmp;
}
void free_command(Command *command)
{
    free(command->string);
    free(command);
}
void free_command_list(Command *list)
{
    Command *tmp = list;
    while (tmp)
    {
        Command *next = tmp->next;
        free_command(tmp);
        tmp = next;
    }
}

void print_command(Command *cmnd)
{
    printf("%s", cmnd->string);
    printf("%d, ", cmnd->lenght);
    if (cmnd->response)
        printf(" response: true\n");
    else
        printf(" response: false\n");
}

void print_command_list(Command *list)
{
    Command *tmp = list;
    while (tmp)
    {
        print_command(tmp);
        tmp = tmp->next;
    }
}

char *read_line(FILE *f)
{
    char *line = calloc(sizeof(char), 250);
    char tmp = fgetc(f);
    int i = 0;
    while (tmp != '\n')
    {
        if (tmp == EOF)
        {
            free(line);
            return NULL;
        }

        line[i] = tmp;
        i++;
        tmp = fgetc(f);
    }
    line[i++] = '\r';
    line[i++] = '\n';
    line[i++] = '\0';
    line = realloc(line, sizeof(char) * i);
    return line;
}

Command *read_commands_from_file(char *filename)
{
    Command *list = NULL;
    Command *tail = NULL;
    FILE *f = fopen(filename, "r");
    if (!f)
    {
        fprintf(stderr, "Error opening file: %s\n", strerror(errno));
        return NULL;
    }
    while (!feof(f))
    {
        char *line = read_line(f);
        //no more commands to read
        if (!line)
            break;
        Command *tmp = init_command(line);
        //first item
        if (!tail)
        {
            list = tail = tmp;
        }
        // not first item
        else
        {
            tail->next = tmp;
            tail = tmp;
        }
    }
    fclose(f);
    return list;
}

char *get_folder(char *filename)
{
    // test/file.txt     13     4
    int len = strlen(filename);
    int i;
    //escape filename
    for (i = len; i >= 0 || filename[i] == '/'; i--)
        ;
    //the slash and the null terminator must be included
    i += 2;
    char *folder = calloc(sizeof(char), i);
    memcpy(folder, filename, i);
    return folder;
}