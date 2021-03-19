#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <string.h>

void serve_client(int client_sd);

int main()
{

	//1. socket
	int server_sd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_sd < 0)
	{
		perror("Socket Failed: ");
		return -1;
	}

	// Prepare Socket info
	struct sockaddr_in server_address;						 //structure to save IP address and port
	memset(&server_address, 0, sizeof(server_address));		 //Initialize/fill the server_address to 0
	server_address.sin_family = AF_INET;					 //address family
	server_address.sin_port = htons(8889);					 //port
	server_address.sin_addr.s_addr = inet_addr("127.0.0.1"); //htonl(INADDR_LOOPBACK); //inet_addr("127.0.0.1");

	if (setsockopt(server_sd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0)
	{
		perror("Setsockopt:");
		return -1;
	}
	//2. bind
	if (bind(server_sd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
	{
		perror("Bind failed..:");
		return -1;
	}

	//3. listen

	if (listen(server_sd, 5) < 0)
	{
		perror("Listen Error:");
		return -1;
	}

	// select() preparation
	fd_set full_fdset, ready_fdset;
	FD_ZERO(&full_fdset);
	FD_SET(server_sd, &full_fdset);

	int max_fd = server_sd;

	//4. accept
	while (1)
	{

		ready_fdset = full_fdset;
		if (select(max_fd + 1, &ready_fdset, NULL, NULL, NULL) < 0)
		{
			perror("Select");
			return -1;
		}

		for (int i = 0; i <= max_fd; i++) // In the end of the lab our loop terminating condition was i<max_fd that is why our server was not monitoring all sockets
		{
			if (FD_ISSET(i, &ready_fdset))
			{

				if (i == server_sd)
				{
					int client_sd = accept(server_sd, NULL, NULL);
					FD_SET(client_sd, &full_fdset);

					if (client_sd > max_fd)
						max_fd = client_sd;
				}
				else
				{
					serve_client(i);
					FD_CLR(i, &full_fdset);
				}
			}
		}
	}

	close(server_sd); //close the server socket
					  //6. close
}
//================================================
void serve_client(int client_sd)
{
	char filename[100];
	memset(filename, 0, sizeof(filename));

	if (recv(client_sd, filename, sizeof(filename) - 1, 0) > 0)
	{
		printf("sending file : %s \n", filename);
		char *result = "GOT IT!";
		send(client_sd, result, strlen(result), 0);
		// FILE* file;
		// if(!(file = fopen(filename,"r")))
		// {
		// 	perror("File cant be opened..");
		// }
		// else
		// {
		// 	char line[256];
		// 	while(fgets(line,sizeof(line),file)!=NULL)		//read the file until NULL
		// 	{
		// 		if(send(client_sd,line,strlen(line),0)==-1)		//send the server response to the client
		// 		{
		// 				perror("Error Sending file..\n");
		// 				break;
		// 		}
		// 		memset(line,0,sizeof(line));
		// 	}
		// 	fclose(file);
		//close(client_sd); //close the client socket
		close(client_sd);
	}
}