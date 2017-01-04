#ifndef COMMAND_H
#define COMMAND_H

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
} Labels_list;

// list of commands
typedef struct t_command
{
    struct t_command *next;
    char *string;  //command string
    int lenght;    //length of the command
    CMD_TYPE type; //type of the command
    bool response+ //if the command awaits response
} Commands_list;

// the nucelo board will validate the commands


#endif /* COMMAND_H */