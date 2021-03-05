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

int main(int argc, char *argv[])
{
    printf("%s %s \n", argv[1], argv[2]);

    while (1)
    {
        char currentUserInputArray[2][MAX_STRING_WORD_SIZE];
        //constant loop for getting the user's input commands
        printf("ftp > ");
        scanf("%s %[^\n]s", currentUserInputArray[0], currentUserInputArray[1]);

        if (strcmp(currentUserInputArray[0], "USER") == 0 && currentUserInputArray[1][0] != '\0')
        {
            printf("Username command ---> %s --- %s \n", currentUserInputArray[0], currentUserInputArray[1]);
        }
        else if (strcmp(currentUserInputArray[0], "PASS") == 0 && currentUserInputArray[1][0] != '\0')
        {
            printf("Password command ---> %s --- %s \n", currentUserInputArray[0], currentUserInputArray[1]);
        }
        else
        {
            printf("Command not recognized. \n");
        }
    }
    return 0;
}