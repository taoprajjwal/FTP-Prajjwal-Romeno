#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/time.h>
#include <libgen.h>

#define MAX_STRING_WORD_SIZE 30
#define MAX_STRING_SIZE 60
#define RESPONSE_SIZE 10
#define PATH_MAX 512

char *fname(char *path)
{
    // taken from https://stackoverflow.com/a/32478632
    char *aux = path;

    /* Go to end of string, so you don't need strlen */
    while (*path++)
        ;

    /* Find the last occurence of \ */
    while (*path-- != '\\' && path != aux)
        ;

    /* It must ignore the \ */
    return (aux == path) ? path : path + 2;
}

int connectToFileSocket(int *file_socket, char *ip, int *port)
{
    //int file_socket = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in srv_addr;              //structure to hold the type, address and port
    memset(&srv_addr, 0, sizeof(srv_addr));   //set the Fill the structure with 0s
    srv_addr.sin_family = AF_INET;            //Address family
    srv_addr.sin_port = htons((port));        //Port Number - check if arg exists or display error msg
    srv_addr.sin_addr.s_addr = inet_addr(ip); // intead of all local onterfaces you can also specify a single enterface e.g. inet_addr("127.0.0.1") for loopback address
    char CWD[PATH_MAX];
    return connect(file_socket, (struct sockaddr *)&srv_addr, sizeof(srv_addr));
}

int main(int argc, char *argv[])
{
    printf("%s %s \n", argv[1], argv[2]); // port number is second argv 2
    int srv_socket = socket(AF_INET, SOCK_STREAM, 0);
    int file_socket = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in srv_addr;                   //structure to hold the type, address and port
    memset(&srv_addr, 0, sizeof(srv_addr));        //set the Fill the structure with 0s
    srv_addr.sin_family = AF_INET;                 //Address family
    srv_addr.sin_port = htons(atoi(argv[2]));      //Port Number - check if arg exists or display error msg
    srv_addr.sin_addr.s_addr = inet_addr(argv[1]); // intead of all local onterfaces you can also specify a single enterface e.g. inet_addr("127.0.0.1") for loopback address
    char CWD[PATH_MAX];
    if (connect(srv_socket, (struct sockaddr *)&srv_addr, sizeof(srv_addr)) == -1)
    {
        perror("Connect: ");
        return -1;
    }

    while (1)
    {
        char currentUserInputArray[2][MAX_STRING_WORD_SIZE];
        //constant loop for getting the user's input commands
        printf("ftp > ");
        //scanf("%[^\n]", buffer);
        scanf("%s %[^\n]s", currentUserInputArray[0], currentUserInputArray[1]);

        if (strcmp(currentUserInputArray[0], "USER") == 0 && currentUserInputArray[1][0] != '\0')
        {
            printf("Username command ---> %s --- %s \n", currentUserInputArray[0], currentUserInputArray[1]);
            fflush(stdout);
            char response[RESPONSE_SIZE]; //string to hold the server esponse
            char *commandStringP1 = strcat(currentUserInputArray[0], " ");
            char *commandString = strcat(commandStringP1, currentUserInputArray[1]);
            printf("Command sent -> %s \n", commandString);
            send(srv_socket, commandString, strlen(commandString), 0);
            int n = recv(srv_socket, response, sizeof(response), 0);
            if (n > 0)
            {
                printf("%s", response);
            }

            fflush(stdout);
        }
        else if (strcmp(currentUserInputArray[0], "PASS") == 0 && currentUserInputArray[1][0] != '\0')
        {
            printf("Password command ---> %s --- %s \n", currentUserInputArray[0], currentUserInputArray[1]);
            fflush(stdout);
            char response[RESPONSE_SIZE]; //string to hold the server esponse
            char *commandStringP1 = strcat(currentUserInputArray[0], " ");
            char *commandString = strcat(commandStringP1, currentUserInputArray[1]);
            printf("Command sent -> %s \n", commandString);
            send(srv_socket, commandString, strlen(commandString), 0);
            int n = recv(srv_socket, response, sizeof(response), 0);
            if (n > 0)
            {
                printf("%s", response);
            }

            fflush(stdout);
        }
        else if (strcmp(currentUserInputArray[0], "CD") == 0 && currentUserInputArray[1][0] != '\0')
        {
            printf("CD command ---> %s --- %s \n", currentUserInputArray[0], currentUserInputArray[1]);
            fflush(stdout);
            char response[RESPONSE_SIZE]; //string to hold the server esponse
            char *commandStringP1 = strcat(currentUserInputArray[0], " ");
            char *commandString = strcat(commandStringP1, currentUserInputArray[1]);
            printf("Command sent -> %s \n", commandString);
            send(srv_socket, commandString, strlen(commandString), 0);
            int n = recv(srv_socket, response, sizeof(response), 0);
            if (n > 0)
            {
                printf("%s", response);
            }

            fflush(stdout);
        }
        else if (strcmp(currentUserInputArray[0], "PWD") == 0)
        {
            printf("PWD command ---> %s --- %s \n", currentUserInputArray[0], currentUserInputArray[1]);
            fflush(stdout);
            char response[RESPONSE_SIZE]; //string to hold the server esponse
            char *commandStringP1 = strcat(currentUserInputArray[0], " ");
            char *commandString = strcat(commandStringP1, currentUserInputArray[1]);
            printf("Command sent -> %s \n", commandString);
            send(srv_socket, commandString, strlen(commandString), 0);
            int n = recv(srv_socket, response, sizeof(response), 0);
            if (n > 0)
            {
                printf("%s", response);
            }

            fflush(stdout);
        }
        else if (strcmp(currentUserInputArray[0], "LS") == 0)
        {
            printf("LS command ---> %s --- %s \n", currentUserInputArray[0], currentUserInputArray[1]);
            fflush(stdout);
            char response[RESPONSE_SIZE]; //string to hold the server esponse
            char *commandStringP1 = strcat(currentUserInputArray[0], " ");
            char *commandString = strcat(commandStringP1, currentUserInputArray[1]);
            printf("Command sent -> %s \n", commandString);
            send(srv_socket, commandString, strlen(commandString), 0);
            int n = recv(srv_socket, response, sizeof(response), 0);
            if (n > 0)
            {
                printf("%s", response);
            }

            fflush(stdout);
        }
        else if (strcmp(currentUserInputArray[0], "!CD") == 0 && currentUserInputArray[1][0] != '\0')
        {
            printf("!CD command ---> %s --- %s \n", currentUserInputArray[0], currentUserInputArray[1]);
            fflush(stdout);

            if (chdir(currentUserInputArray[1]) < 0)
            {
                printf("Path %s does not exist.\n", currentUserInputArray[1]);
            }

            else
            {
                getcwd(CWD, sizeof(CWD));
                printf("Changed directory. \n");
            }

            fflush(stdout);
        }
        else if (strcmp(currentUserInputArray[0], "!PWD") == 0 && currentUserInputArray[1][0] == '\0')
        {
            printf("%s \n", CWD);
        }
        else if (strcmp(currentUserInputArray[0], "GET") == 0 && currentUserInputArray[1][0] != '\0')
        {
            FILE *file;
            printf("currentUserInputArray %s \n", currentUserInputArray[1]);
            char *fileName = basename(currentUserInputArray[1]);
            printf("file is %s \n", fname(fileName));

            char response[10]; //string to hold the server esponse
            char response2[20];
            char *commandStringP1 = strcat(currentUserInputArray[0], " ");
            char *commandString = strcat(commandStringP1, currentUserInputArray[1]);
            send(srv_socket, commandString, strlen(commandString), 0);
            int n = recv(srv_socket, response, sizeof(response), 0);
            printf("response is %s\n", response);
            if (n > 0 && atoi(response) != 550 && atoi(response) != 503)
            {
                int fileSize = atoi(response);

                file = fopen(fname(fileName), "w");
                if (file == NULL)
                {
                    perror("Open file error: ");
                    return -1;
                }
                int port = atoi(argv[2]) + 1;
                if (connectToFileSocket(file_socket, argv[1], port) < 0)
                {
                    perror("Connect: ");
                    return -1;
                }
                char buffer[512]; //string to hold the server esponse
                printf("Before file response \n");

                int remainingData = fileSize;
                int len;
                while ((remainingData > 0) && ((len = recv(file_socket, buffer, 512, 0)) > 0))
                {
                    printf("buf %s\n", buffer);
                    fwrite(buffer, sizeof(char), len, file);
                    remainingData -= len;
                }
                //int n = recv(file_socket, fileResponse, sizeof(fileResponse), 0);
                printf("passed while loop \n");
                fclose(file);
                close(file_socket);
            }
            //
        }
        else
        {
            printf("Invalid FTP command. \n");
        }
    }
    return 0;
}