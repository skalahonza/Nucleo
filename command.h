#ifndef COMMAND_H
#define COMMAND_H

#define CUSTOM_COM_LEN 16

#define LED_COMMAND "LED\r\n"
#define LED_COMMAND_LEN 6

#define BTN_COMMAND "BTN\r\n"
#define BTN_COMMAND_LEN 6

// Command types
// SIMPLE - IDN, LED - will be send directely no response expeted
// JOYSTICK - sends joystick request - waits for response
typedef enum { SIMPLE,
               JOYSTICK,
               BUTTON } CMD_TYPE;

//list of labels
typedef struct t_label
{
    struct t_label *next;
    char *id;
    int lenth;   //length of the identifier
    int jumpIdx; //index of the list that the label should jump to
} Labels;

//list of commands
typedef struct t_command
{
    struct t_command *next;
    char *string;  //command string
    int lenght;    //length of the command
    CMD_TYPE type; //type of the command
} Commands;

#endif /* COMMAND_H */