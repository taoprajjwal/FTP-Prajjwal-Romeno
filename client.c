/*
CLASS LAB CODE FOR REFERENCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[])
{

	int srv_socket = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in srv_addr;				  //structure to hold the type, address and port
	memset(&srv_addr, 0, sizeof(srv_addr));		  //set the Fill the structure with 0s
	srv_addr.sin_family = AF_INET;				  //Address family
	srv_addr.sin_port = htons(8080);			  //Port Number
	srv_addr.sin_addr.s_addr = htonl(INADDR_ANY); // intead of all local onterfaces you can also specify a single enterface e.g. inet_addr("127.0.0.1") for loopback address

	if (connect(srv_socket, (struct sockaddr *)&srv_addr, sizeof(srv_addr)) == -1)
	{
		perror("Connect: ");
		return -1;
	}

	char filename[50];
	if (argc < 2)
	{
		printf("Enter File name you want to get from server:");
		scanf("%s", filename);
	}
	else
		strcpy(filename, argv[1]);

	printf("requesting %s \n", filename);
	send(srv_socket, filename, strlen(filename), 0);
	char response[10000]; //string to hold the server esponse

	while (recv(srv_socket, response, sizeof(response), 0) > 0)
		printf("%s", response);
	fflush(stdout);
	close(srv_socket);
	return 0;
}
*/