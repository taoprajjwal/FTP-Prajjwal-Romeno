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
#include <dirent.h>
#include <sys/sendfile.h>
#include <sys/stat.h>

#define MAX_CONNECTIONS 10
#define N_USERS 5
#define USERNAME_LIMIT 20
#define PASSWORD_LIMIT 20
#define INPUT_FILE "users.txt"
#define COMMAND_LIMIT 100
#define PATH_MAX 512
#define DEFAULT_PATH "/home/cn/"
#define MAX_RESPONSE 4096

typedef struct
{
	int fd;
	int user_inited;
	int authenticated;
	int user_id;
	char name[USERNAME_LIMIT];
	char pass[PASSWORD_LIMIT];
	char pwd[PATH_MAX];
} client;

typedef struct
{
	char username[USERNAME_LIMIT];
	char password[PASSWORD_LIMIT];
} user;

int create_bind_socket(int *server_sd, int *port, char *addr)
{
	*server_sd = socket(AF_INET, SOCK_STREAM, 0);
	if (*server_sd < 0)
	{
		perror("Socket Failed");
		return -1;
	}

	printf("%d %d %s \n", *server_sd, *port, addr);

	struct sockaddr_in server_addr;

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(*port);

	if (inet_aton(addr, &server_addr.sin_addr.s_addr) == 0)
	{
		perror("IP error");
		return -1;
	};

	if (bind(*server_sd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
	{
		perror("Bind failed");
		return -1;
	}

	if (listen(*server_sd, 10) < 0)
	{
		perror("Listen failed");
		return -1;
	}
}

void initiaize_users(user *client_list, char *input_file)
{

	FILE *userfile = fopen(input_file, "r");
	char *line;
	size_t len = USERNAME_LIMIT + PASSWORD_LIMIT;

	line = (char *)malloc(len * sizeof(char));

	for (int i = 0; i < N_USERS; i++)
	{

		getline(&line, &len, userfile);
		char username[USERNAME_LIMIT], password[PASSWORD_LIMIT];
		sscanf(line, "%s %s", client_list[i].username, client_list[i].password);
	}
}

int check_username(user *client_list, char *username)
{

	for (int i = 0; i < N_USERS; i++)
	{
		if (strcmp(username, client_list[i].username) == 0)
		{
			return i;
		}
	}

	return -1;
}

int create_connection(client *client_list, int fd)
{

	printf("New fd: %d \n", fd);

	for (int i = 0; i < MAX_CONNECTIONS; i++)
	{
		if (client_list[i].fd == -1)
		{
			client_list[i].fd = fd;
			return 1;
		}
	}

	return -1;
}

int get_connection_index(client *client_list, int fd)
{
	for (int i = 0; i < MAX_CONNECTIONS; i++)
	{
		if (client_list[i].fd == fd)
		{
			return i;
		}
	}

	return -1;
}

void send_file(int file_sd, char *file_path)
{
	while (1)
	{
		int file_descript = accept(file_sd, NULL, NULL);

		int pid = fork();
		if (pid < 0)
		{
			perror("Error in forking \n");
		}
		if (pid == 0)
		{

			int file = open(file_path);

			struct stat st;

			fstat(file, &st);

			int sent;
			sent = sendfile(file_descript, file, NULL, st.st_size);

			if (sent < st.st_size)
			{
				printf("File size and sent bytes not compatible !!! \n");
			}
			exit(0);
		}
		else
		{
			break;
		}
	}
}

int main(int argc, char *argv[])
{

	int socket_sd, port, high_sock, file_socket, file_port;
	char *addr;
	struct timeval timeout;

	if (argc < 3)
	{
		printf("IP Address and Port not given \n");
		return -1;
	}

	addr = argv[1];
	port = atoi(argv[2]);

	file_port = port + 1;

	user users[N_USERS];
	initiaize_users(users, INPUT_FILE);

	for (int i = 0; i < N_USERS; i++)
	{
		printf("%s %s \n", users[i].username, users[i].password);
	}

	if (create_bind_socket(&socket_sd, &port, addr) < 0)
	{
		return -1;
	};

	if (create_bind_socket(&file_socket, &file_port, addr) < 0)
	{
		perror("Failed in creating file transfer port");
		return -1;
	}

	client clients[MAX_CONNECTIONS];

	for (int i = 0; i < MAX_CONNECTIONS; i++)
	{
		clients[i].fd = -1;
		clients[i].user_inited = 0;
		clients[i].authenticated = 0;
		clients[i].user_id = -1;
		strcpy(clients[i].name, "");
		strcpy(clients[i].pass, "");
		strcpy(clients[i].pwd, DEFAULT_PATH);
	}

	for (int i = 0; i < MAX_CONNECTIONS; i++)
	{
		printf("%d %d \n", clients[i].fd, clients[i].authenticated);
	}

	fd_set socks, ready_socks;
	FD_ZERO(&socks);
	FD_SET(socket_sd, &socks);
	high_sock = socket_sd;

	//timeout.tv_sec = 1;

	timeout.tv_usec = 200;

	while (1)

		ready_socks = socks;
		int readsocks = select(high_sock + 1, &ready_socks, NULL, NULL, NULL);

		printf("Select() ran \n");

		if (readsocks < 0)
		{
			perror("SELECT FAILED");
			return -1;
		}

		if (readsocks == 0)
		{
			printf(".\n");
		}

		else
		{

			if (FD_ISSET(socket_sd, &ready_socks))
			{

				int new_fd;
				new_fd = accept(socket_sd, NULL, NULL);

				if (new_fd < 0)
				{
					perror("Accept failed");
					return -1;
				}

				FD_SET(new_fd, &socks);

				if (new_fd > high_sock)
				{
					high_sock = new_fd;
				}

				if (create_connection(clients, new_fd) < 0)
				{
					printf("Cannot add fd: %d; connection is full \n", new_fd);
				}
			}

			for (int i = 0; i < MAX_CONNECTIONS; i++)
			{
				printf("%d %d \n", clients[i].fd, clients[i].authenticated);
			}

			for (int i = 0; i < MAX_CONNECTIONS; i++)
			{

				if (FD_ISSET(clients[i].fd, &ready_socks))
				{

					printf("client %d has isset \n", i);

					char buffer[COMMAND_LIMIT];
					memset(buffer, 0, sizeof(buffer));

					int buffer_size;
					buffer_size = read(clients[i].fd, buffer, sizeof(buffer));

					printf("%d  %s \n", buffer_size, buffer);

					if (buffer_size < 0)
					{
						perror("Error reading from the socket");
					}

					else if (buffer_size == 0)
					{
						//TODO: REMOVE THE FD FROM THE FDSET AND the client struct
						printf("Removing %d from the client_list \n", clients[i].fd);
						FD_CLR(clients[i].fd, &socks);
						clients[i].fd = -1;
						close(clients[i].fd);
					}

					else
					{
						char command[COMMAND_LIMIT];
						char param[COMMAND_LIMIT];
						sscanf(buffer, "%s %s", command, param);

						if (strcmp(command, "USER") == 0)
						{

							char response[100];
							memset(response, 0, sizeof(response));

							int userid = check_username(users, param);

							if (userid < 0)
							{
								printf("USERNAME DOES NOT EXIST SEND AGAIN \n");
								strcpy(response, "430");
							}

							else
							{
								printf("User %s initatied \n", param);
								clients[i].user_inited = 1;
								strcpy(clients[i].name, param);
								strcpy(clients[i].pass, users[userid].password);

								strcpy(response, "331");
							}

							printf("    SENDING strlen %d\n", strlen(response));
							if (send(clients[i].fd, response, strlen(response), 0) < 1)
							{
								perror("Error in send");
								return -1;
							}
						}

						if (strcmp(command, "PASS") == 0)
						{

							char response[100];
							memset(response, 0, sizeof(response));

							if (clients[i].user_inited == 1)
							{

								if (strcmp(clients[i].pass, param) == 0)
								{
									clients[i].authenticated = 1;
									strcpy(response, "230");
								}

								else
								{
									strcpy(response, "430");
									printf("Wrong Password \n");
								}
							}
							else
							{
								strcpy(response, "503");
								printf("GIVE USERNAME FIRST \n");
							}

							if (send(clients[i].fd, response, strlen(response), 0) < 1)
							{
								perror("Error in send");
								return -1;
							}
						}

						if (strcmp(command, "LS") == 0)
						{

							char response_ls[MAX_RESPONSE];
							memset(response_ls, 0, sizeof(response_ls));

							if (clients[i].authenticated == 1)
							{

								DIR *d;
								struct dirent *dir;
								d = opendir(clients[i].pwd);
								if (d)
								{
									while ((dir = readdir(d)) != NULL)
									{
										if ((strlen(response_ls) + strlen(dir->d_name)) < MAX_RESPONSE)
										{
											strncat(response_ls, "%s \n", dir->d_name);
										}
										else
										{
											printf("MAXIMUM RESPONSE EXCEED IN LS!!!! \n");
										}
									}
								}
								if (send(clients[i].fd, response_ls, strlen(response_ls), 0) < 1)
								{
									perror("Error in send");
									return -1;
								}
							}

							else
							{
								strcpy(response_ls, "530");
								if (send(clients[i].fd, response_ls, strlen(response_ls), 0) < 1)
								{
									perror("Error in send");
									return -1;
								}
							}
						}

						if (strcmp(command, "PWD") == 0)
						{

							char response_pwd[PATH_MAX];
							memset(response_pwd, 0, sizeof(response_pwd));

							if (clients[i].authenticated == 1)
							{
								strcpy(response_pwd, clients[i].pwd);

								if (send(clients[i].fd, response_pwd, strlen(response_pwd), 0) < 1)
								{
									perror("Error in send");
									return -1;
								}
							}
							else
							{
								strcpy(response_pwd, "530");

								if (send(clients[i].fd, response_pwd, strlen(response_pwd), 0) < 1)
								{
									perror("Error in send");
									return -1;
								}
							}
						}

						if (strcmp(command, "CD") == 0)
						{

							// return changed path + success message
							char response[100];
							memset(response, 0, sizeof(response));

							if (clients[i].authenticated == 1)
							{

								// change directory to where the current connection thinks is the pwd
								chdir(clients[i].pwd);

								if (chdir(param) < 0)
								{
									printf("%s does not exists\n", param);
									strcpy(response, "Path does not exists\n");
								}

								else
								{
									strcpy(response, "200");

									char cwd[PATH_MAX];

									getcwd(cwd, sizeof(cwd));

									strcpy(clients[i].pwd, cwd);

									printf("%s \n", clients[i].pwd);
								}
							}

							else
							{
								strcpy(response, "503");
							}

							if (send(clients[i].fd, response, strlen(response), 0) < 1)
							{
								perror("Error in send");
								return -1;
							}
						}

						if (strcmp(command, "GET") == 0)
						{

							char response[20];

							if (clients[i].authenticated == 1)
							{

								//change to the pwd of the client, might have been changed by other connections
								chdir(clients[i].pwd);

								FILE *file;
								if (file = fopen(param, "r"))
								{
									fclose(file);
									strcpy(response, "150");
									send(clients[i].fd, response, strlen(response), 0);
									send_file(file_socket, param);
								}
								else
								{
									strcpy(response, "550");
									send(clients[i].fd, response, strlen(response), 0);
								}
							}

							else
							{
								strcpy(response, "503");
								send(clients[i].fd, response, strlen(response), 0);
							}
						}
					}
				}
			}
		}
	}
}