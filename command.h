#ifndef COMMAND_H
#define COMMAND_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define CUSTOM_COM_LEN 50

//switches led on or off
#define LED_COMMAND "LED\r\n"
#define LED_COMMAND_LEN 6

#define BTN_COMMAND "BUTTON?\r\n"
#define BTN_COMMAND_LEN 10

#include <stdbool.h>

// list of commands
typedef struct t_command
{
    struct t_command *next;
    char *string; //command string
    int lenght;   //length of the command
    bool response; //if the command awaits response
} Command;

// the nucelo board will validate the commands

//init memory space
Command *init_command(char *text);
void free_command(Command *command);
void free_command_list(Command *list);
void print_command(Command *cmnd);
void print_command_list(Command *list);
//Reads commands from file and returns list of them
Command *read_commands_from_file(char *filename);
//finds out in which folder the file is
char *get_folder(char *filename);

#endif /* COMMAND_H */