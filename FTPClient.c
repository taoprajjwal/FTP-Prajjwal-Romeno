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
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#define MAX_STRING_WORD_SIZE 30
#define MAX_STRING_SIZE 60
#define RESPONSE_SIZE 100
#define PATH_MAX 512
#define MAX_RESPONSE 4096


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
void authenticationMessage(int code)
{
    if (code == 530)
    {
        printf("Authentication required. Please enter username and password using the USER and PASS commands.\n");
    }
}
int connectToFileSocket(int *file_socket, char *ip, int *port)
{
    //file_socket = socket(AF_INET, SOCK_STREAM, 0);
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
            fflush(stdout);
            char response[RESPONSE_SIZE]; //string to hold the server esponse
            char *commandStringP1 = strcat(currentUserInputArray[0], " ");
            char *commandString = strcat(commandStringP1, currentUserInputArray[1]);
            send(srv_socket, commandString, strlen(commandString), 0);
            int n = recv(srv_socket, response, sizeof(response), 0);

            if (n > 0)
            {
                int responseCode = atoi(response);
                if (responseCode == 430)
                {
                    printf("Username does not exist, please try again.\n", response);
                }
                if (responseCode == 331)
                {
                    printf("Username found! Now send the password with the PASS command.\n", response);
                }
            }

            fflush(stdout);
        }
        else if (strcmp(currentUserInputArray[0], "PASS") == 0 && currentUserInputArray[1][0] != '\0')
        {
            fflush(stdout);
            char response[RESPONSE_SIZE]; //string to hold the server esponse
            char *commandStringP1 = strcat(currentUserInputArray[0], " ");
            char *commandString = strcat(commandStringP1, currentUserInputArray[1]);
            send(srv_socket, commandString, strlen(commandString), 0);
            int n = recv(srv_socket, response, sizeof(response), 0);
            if (n > 0)
            {
                int responseCode = atoi(response);
                if (responseCode == 430)
                {
                    printf("Wrong password. Try again.\n", response);
                }
                if (responseCode == 503)
                {
                    printf("Please enter username first using the USER command.\n", response);
                }
                if (responseCode == 230)
                {
                    printf("Authenticated!\n", response);
                }
            }

            fflush(stdout);
        }
        else if (strcmp(currentUserInputArray[0], "CD") == 0 && currentUserInputArray[1][0] != '\0')
        {
            printf("CD command ---> %s --- %s \n", currentUserInputArray[0], currentUserInputArray[1]);
            fflush(stdout);
            char response[PATH_MAX]; //string to hold the server esponse
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
            fflush(stdout);
            char response[PATH_MAX]; //string to hold the server esponse
            char *commandStringP1 = strcat(currentUserInputArray[0], " ");
            char *commandString = strcat(commandStringP1, currentUserInputArray[1]);
            send(srv_socket, commandString, strlen(commandString), 0);
            int n = recv(srv_socket, response, sizeof(response), 0);
            if (n > 0)
            {
                int responseCode = atoi(response);
                authenticationMessage(responseCode);
                if (responseCode == 200)
                {
                    printf("Changed server directory sucessfully.\n");
                }
            }

            fflush(stdout);
        }
        else if (strcmp(currentUserInputArray[0], "LS") == 0)
        {
            fflush(stdout);
            char response[MAX_RESPONSE]; //string to hold the server esponse
            char *commandStringP1 = strcat(currentUserInputArray[0], " ");
            char *commandString = strcat(commandStringP1, currentUserInputArray[1]);
            send(srv_socket, commandString, strlen(commandString), 0);
            int n = recv(srv_socket, response, sizeof(response), 0);
            if (n > 0)
            {
                printf("%s\n", response);
            }

            fflush(stdout);
        }
        else if (strcmp(currentUserInputArray[0], "!CD") == 0 && currentUserInputArray[1][0] != '\0')
        {
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
            file_socket = socket(AF_INET, SOCK_STREAM, 0);
            FILE *file;
            printf("currentUserInputArray %s \n", currentUserInputArray[1]);
            char *fileName = basename(currentUserInputArray[1]);
            printf("file is %s \n", fname(fileName));

            char response[RESPONSE_SIZE]; //string to hold the server esponse

			memset(response, 0, sizeof(response));
            
			char response2[20];

            char *commandStringP1 = strcat(currentUserInputArray[0], " ");
            char *commandString = strcat(commandStringP1, currentUserInputArray[1]);
            send(srv_socket, commandString, strlen(commandString), 0);
            
			int n = recv(srv_socket, response, sizeof(response), 0);

            printf("response is %s, int n:%d \n", response,n);
            
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
				memset(buffer, 0, sizeof(buffer));

                printf("Before file response \n");

                int remainingData = fileSize;
                int len;

				len = recv(file_socket, buffer, 512, 0);
				printf("Remaining data: %d, Len: %d", remainingData,len);

                while ((remainingData > 0) && (len > 0))
                {

					printf("Went into the main loop \n");
                    float percentage = (((float)fileSize - (float)remainingData) / ((float)fileSize)) * 100.0;
                    printf("Download %.2f%% complete\n", percentage);
                    fwrite(buffer, sizeof(char), len, file);
                    remainingData -= len;
                }


                //int n = recv(file_socket, fileResponse, sizeof(fileResponse), 0);
                printf("Download 100.00%% complete\n");
                printf("File %s successfully recieved! \n", fname(fileName));
                fclose(file);
            }
            //
        }
        else if (strcmp(currentUserInputArray[0], "PUT") == 0 && currentUserInputArray[1][0] != '\0')
        {
            file_socket = socket(AF_INET, SOCK_STREAM, 0);
            FILE *file;
            if (file = open(currentUserInputArray[1], O_RDONLY))
            {
                char *fileName = basename(currentUserInputArray[1]);
                struct stat st;
                fstat(file, &st);
                //(float)
                char fileSizeChar[512];
                sprintf(fileSizeChar, "%d", st.st_size);
                //printf("file is %s \n", fname(fileName));
                //(float)
                char *commandStringP1 = strcat(currentUserInputArray[0], " ");
                char *commandStringP2 = strcat(commandStringP1, fileSizeChar);
                char *commandStringP3 = strcat(commandStringP2, " ");
                char *commandString = strcat(commandStringP2, fname(fileName));

                //strcpy(response, "150");
                //sprintf(response2, "%d", st.st_size);
                //send(clients[i].fd, response, strlen(response), 0);
                //send(clients[i].fd, response2, sizeof(response2), 0);
                send(srv_socket, commandString, strlen(commandString), 0);

                int remainingData = st.st_size;
                off_t offset = 0;
                int sent;
                //char buf[512];
                /* Sending file data */
                int port = atoi(argv[2]) + 1;
                if (connectToFileSocket(file_socket, argv[1], port) < 0)
                {
                    perror("Connect: ");
                    return -1;
                }
                printf("remainingData: %d\n", remainingData);
                while (((sent = sendfile(file_socket, file, &offset, BUFSIZ)) > 0) && (remainingData > 0))
                {
                    int fileSize = st.st_size;
                    float percentage = (((float)fileSize - (float)remainingData) / ((float)fileSize)) * 100.0;
                    printf("Upload %.2f%% complete\n", percentage);
                    remainingData -= sent;
                }
                printf("Upload 100.00%% complete\n");
                // sent = sendfile(file_descript, file, NULL, st.st_size);

                // if (sent <= st.st_size)
                // {
                // 	printf("File size and sent bytes not compatible !!! \n");
                // }
                close(file_socket);
                close(file);
                //send_file(file_socket, param);
            }
            else
            {

                printf("File opening error.");
            }
        }
        else
        {
            printf("Invalid FTP command. \n");
        }
    }
    return 0;
}