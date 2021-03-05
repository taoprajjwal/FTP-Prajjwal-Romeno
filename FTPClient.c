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

#define MAX_STRING_SIZE 30

int main(int argc, char *argv[])
{
    printf("%s %s \n", argv[1], argv[2]);
    while (1)
    {
        char currentInputString[MAX_STRING_SIZE];
        //constant loop for user input
        printf("ftp >");
        scanf("%s", currentInputString);
        printf("%s\n", currentInputString);
    }
    return 0;
}