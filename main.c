#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>

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

int read_serial(int hSerial, char chArrBuf[256])
{
    memset(&chArrBuf, '\0', sizeof(chArrBuf));
    return read(hSerial, &chArrBuf, sizeof(chArrBuf));
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
        //get serial
        printf("Welcome! The input parameter is %s \n", args[1]);
    }

    int hSerial = open(args[1], O_RDWR | O_NONBLOCK | O_NDELAY);
    if (hSerial < 0)
    {
        fprintf(stderr, "Cannot open serial.\n");
        fprintf(stderr, "%s \n", args[1]);
        fprintf(stderr, "return value %d \n", hSerial);
        return 100;
    }

    // h = handle
    struct termios o_tty;
    // o = object
    memset(&o_tty, 0, sizeof(o_tty));
    /*int iRetVal = */ tcgetattr(hSerial, &o_tty);
    print_menu();
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

    close(hSerial);
    return 0;
}
