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

#define MAX_STRING_WORD_SIZE 30
#define MAX_STRING_SIZE 60
#define RESPONSE_SIZE 10
#define PATH_MAX 512

int main(int argc, char *argv[])
{
    printf("%s %s \n", argv[1], argv[2]); // port number is second argv 2
    int srv_socket = socket(AF_INET, SOCK_STREAM, 0);
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

                char cwd[PATH_MAX];

                getcwd(cwd, sizeof(cwd));

                strcpy(CWD, cwd);

                printf("Changed directory. \n");
            }

            fflush(stdout);
        }
        else
        {
            printf("Command not recognized. \n");
        }
    }
    return 0;
}