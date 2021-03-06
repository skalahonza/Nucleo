#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdbool.h>
#include <pthread.h>
#include <termios.h>
#include "command.h"

#define BUFFER_SIZE 100
int THREADS_COUNT = 2;

int hSerial;
struct termios o_tty;
bool quit = false;
pthread_mutex_t mtx;

//COM BUFFER STRUCT
typedef struct
{
    int len;
    char message[BUFFER_SIZE];
} Com_Buffer_t;

typedef struct
{
    Com_Buffer_t *buffer;
    char *filename;
} Comm_Thread_Helper;

void clear_row()
{
    //clear row
    printf("%c[2K", 27);
    printf("\r");
}

void call_stty(int reset)
{
    if (reset)
    {
        reset = system("stty -raw");
    }
    else
    {
        reset = system("stty raw");
    }
}

void *ui_thread(void *v)
{
    Com_Buffer_t *buffer = ((Com_Buffer_t *)v);
    bool q = false;
    while (!q)
    {
        //Read user input
        char command;
        int result = scanf(" %c", &command);
        if (result != 1)
        {
            //Error handling
        }
        else
        {
            clear_row();
            switch (command)
            {
            case '1':
                //switch led
                strcpy(buffer->message, LED_COMMAND);
                buffer->len = LED_COMMAND_LEN;
                break;
            case '2':
                //get button state
                strcpy(buffer->message, BTN_COMMAND);
                buffer->len = BTN_COMMAND_LEN;
                break;
            case 'c':
            case 'C':
                ///custom command
                call_stty(1);
                printf("Type custom command:");
                char custom[CUSTOM_COM_LEN];
                //clear array
                for (int i = 0; i < CUSTOM_COM_LEN; ++i)
                    custom[i] = '\0';
                if (fgets(custom, CUSTOM_COM_LEN - 3, stdin) == NULL)
                {
                    //error handling
                    fprintf(stderr, "Error occurred, while reading custom command.\n");
                }
                // Remove trailing newline, if there.
                int len = strlen(custom);
                if ((len > 0) && (custom[len - 1] == '\n'))
                    custom[len-- - 1] = '\0';
                custom[len++] = '\r'; //add \r
                custom[len++] = '\n'; //add \n
                //len is now without null terminator
                len++; //add le nfor null terminator
                strcpy(buffer->message, custom);
                buffer->len = len;
                call_stty(0);
                break;
            case 'e':
            case 'E':
                quit = true;
                break;
            default:
                //not implemented key pressed
                break;
            }
        }
        q = quit;
    }
    return 0;
}

void *com_thread(void *v)
{
    Com_Buffer_t *buffer = ((Com_Buffer_t *)v);
    bool q = false;
    while (!q)
    {
        //read serial data
        pthread_mutex_lock(&mtx);
        char chArrBuf[256];
        memset(&chArrBuf, '\0', sizeof(chArrBuf));
        int n = read(hSerial, &chArrBuf, sizeof(chArrBuf));
        if (n == -1)
        {
            //Error while reading
        }
        else if (n == 0)
        {
            //EOF
        }
        else
        {
            ///clear_row();
            printf("%s", chArrBuf);
        }
        //writting
        if (buffer->len)
        {
            int result = write(hSerial, buffer->message, sizeof(char) * (buffer->len));
            //clear buffer
            memset(&buffer->message, '\0', sizeof(buffer->message));
            buffer->len = 0;
            if (result == -1)
            {
                fprintf(stderr, "Writting failed\n");
            }
        }
        pthread_mutex_unlock(&mtx);
        q = quit;
        usleep(100 * 100);
    }
    return 0;
}

void *commands_thread(void *v)
{
    Comm_Thread_Helper *helper = (Comm_Thread_Helper *)v;
    Com_Buffer_t *buffer = helper->buffer;
    char *filename = helper->filename;
    Command *list = read_commands_from_file(filename);
    Command *tmp = list;
    while (tmp)
    {
        strcpy(buffer->message, tmp->string);
        buffer->len = tmp->lenght + 1;
        tmp = tmp->next;
        usleep(100 * 1000);
    }
    free(filename);
    free_command_list(list);
    return 0;
}

void print_menu()
{
    printf("== program menu ==\n");
    printf("Item 1: Control LED\n");
    printf("Item 2: Read button state\n");
    printf("Item c: Enter a custom command\n");
    printf("Item e: Exit\n");
    printf("Selection:\n");
}

int main(int argc, char **args)
{
    //init mutex
    pthread_mutex_init(&mtx, NULL);
    Com_Buffer_t buffer;
    memset(&buffer.message, '\0', sizeof(buffer.message));

    //load input arguments
    if (argc <= 1)
    {
        fprintf(stderr, "No arguments given\n");
        return 100;
    }
    //parameter 1 exists
    if (argc >= 2)
    {
        printf("Welcome! The input parameter is %s \n", args[1]);
        THREADS_COUNT = 2; //UI and comm thread
    }

    //open serial
    hSerial = open(args[1], O_RDWR | O_NONBLOCK | O_NDELAY);
    if (hSerial < 0)
    {
        fprintf(stderr, "Cannot open serial.\n");
        fprintf(stderr, "%s \n", args[1]);
        fprintf(stderr, "return value %d \n", hSerial);
        return 100;
    }

    //serial opened
    print_menu();
    call_stty(0);

    memset(&o_tty, 0, sizeof(o_tty));
    tcgetattr(hSerial, &o_tty);

    //create threads
    pthread_t thrs[THREADS_COUNT];
    pthread_create(&thrs[0], NULL, ui_thread, &buffer);  //UI
    pthread_create(&thrs[1], NULL, com_thread, &buffer); //COMM THREAD

    //parameter 2 exists
    //perform the command list, then continue to the UI part
    if (argc == 3)
    {
        char *command_file = malloc(strlen(args[2]) + 1);
        strcpy(command_file, args[2]);
        printf("Command file path: %s\r\n", command_file);
        THREADS_COUNT = 3; //UI, comm thread, file thread
        Comm_Thread_Helper helper;
        helper.buffer = &buffer;
        helper.filename = command_file;
        pthread_create(&thrs[2], NULL, commands_thread, &helper); //commands file thread
    }

    //exit threads
    for (int i = 0; i < THREADS_COUNT; ++i)
    {
        pthread_join(thrs[i], NULL);
    }

    call_stty(1);
    close(hSerial);
    printf("\n");
    return 0;
}
