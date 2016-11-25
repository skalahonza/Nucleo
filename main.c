#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>
#include <termios.h>

#define THREADS_COUNT 2

int hSerial;
struct termios o_tty;
bool quit = false;
pthread_mutex_t mtx;

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

void *thread1(void *v)
{
    bool q = false;
    while (!q)
    {
        //Read user input
        q = quit;
    }
    return 0;
}

void *thread2(void *v)
{
    bool q = false;
    while (!q)
    {
        //read serial data
        char chArrBuf[256];
        memset(&chArrBuf, '\0', sizeof(chArrBuf));
        int n = read(hSerial, &chArrBuf, sizeof(chArrBuf));
        if (n == -1)
            {/*fprintf(stderr, "Error while reding.\n");*/}
        else if (n == 0)
            {/*printf("End of file reached.\n");*/}
        else
        {
            printf("\r%s", chArrBuf);
        }
        usleep(100 * 1000);
        q = quit;
    }
    return 0;
}

void print_menu()
{
    printf("== program menu ==\n");
    printf("Item 1: Control LED\n");
    printf("Item 2: Read button state\n");
    printf("Item 3: Read joystick\n");
    printf("Item 4: Control display\n");
    printf("Item c: Enter a custom command\n");
    printf("Item e: Exit\n");
    printf("Selection:\n");
}

void handle_custom_command(char custom[100])
{
    printf("YOUR COMMAND %s\n", custom);
}

int main(int argc, char **args)
{
    if (argc <= 1)
    {
        fprintf(stderr, "No arguments given\n");
        return 100;
    }
    else
    {
        printf("Welcome! The input parameter is %s \n", args[1]);
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

    print_menu();
    call_stty(0);

    memset(&o_tty, 0, sizeof(o_tty));
    tcgetattr(hSerial, &o_tty);

    //create threads
    pthread_mutex_init(&mtx, NULL);
    pthread_t thrs[THREADS_COUNT];

    pthread_create(&thrs[0], NULL, thread1, NULL);
    pthread_create(&thrs[1], NULL, thread2, NULL);

    for (int i = 0; i < THREADS_COUNT; ++i)
    {
        pthread_join(thrs[i], NULL);
    }

    call_stty(1);
    close(hSerial);
    printf("\n");
    return 0;

    while (1)
    {

        char command;
        int result = scanf(" %c", &command);
        if (result < 0)
        {
            //Error handling
        }
        else
        {
            switch (command)
            {
            case '1':
                //switch led
                printf("Flash led\n");
                write(hSerial, "1", sizeof(char) * 2);
                break;
            case '2':
                //get button state
                write(hSerial, "2", sizeof(char) * 2);

                char chArrBuf[256];
                memset(&chArrBuf, '\0', sizeof(chArrBuf));
                int n = read(hSerial, &chArrBuf, sizeof(chArrBuf));
                if (n == -1)
                    fprintf(stderr, "Error while reding.\n");
                else if (n == 0)
                    printf("End of file reached.\n");
                else
                {
                    printf("%s", chArrBuf);
                }
                break;
            case '3':
            case '4':
                printf("Not implemented yet.\n");
                break;
            case 'c':
                ///custom command
                printf("Type custom command:\n");
                char custom[100];
                result = scanf("%100s", custom);
                handle_custom_command(custom);
                break;
            case 'e':
                exit(0);
                break;
            default:
                //Handle error
                fprintf(stderr, "Invalid command given\n");
                break;
            }
        }
    }
}
