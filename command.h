#ifndef COMMAND_H
#define COMMAND_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define CUSTOM_COM_LEN 16

#define LED_COMMAND "LED\r\n"
#define LED_COMMAND_LEN 6

#define LED_ON_COMMAND "LED ON\r\n"
#define LED_ON_COMMAND_LEN 9

#define LED_OFF_COMMAND "LED OFF\r\n"
#define LED_OFF_COMMAND_LEN 10

#define BTN_COMMAND "BTN\r\n"
#define BTN_COMMAND_LEN 6

#include <stdbool.h>

// Command types
// IDN, LED - will be send directely no response expeted
// JOYSTICK - sends joystick request - waits for response
typedef enum { LED,
               IDN,
               JOYSTICK,
               BUTTON,
               DRAW_SETTEXTCOLOR,
               DRAW_CLEAR,
               DRAW_PIXEL,
               DRAW_LINE,
               DRAW_CIRCLE,
               DRAW_SETFONT,
               DRAW_TEXT } CMD_TYPE;

// list of labels
typedef struct t_label
{
    struct t_label *next;
    char *id;
    int lenth;   //length of the identifier
    int jumpIdx; //index of the list that the label should jump to
} Label;

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

//Reads commands from file and returns list of them
Command *read_commands_from_file(char *filename);

#endif /* COMMAND_H */