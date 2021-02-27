#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<sys/types.h>
#include <unistd.h>
#include<netinet/in.h> 
#include <arpa/inet.h>
#include <sys/select.h> 
#include <sys/time.h>



#define MAX_CONNECTIONS 10
#define N_USERS 5
#define USERNAME_LIMIT 20
#define PASSWORD_LIMIT 20
#define INPUT_FILE "users.txt"
#define COMMAND_LIMIT 100



typedef struct clients{
    int fd;
    int user_inited;
    int authenticated;
    char name[USERNAME_LIMIT];
    char pass[PASSWORD_LIMIT];
} client;


typedef struct{
    char username[USERNAME_LIMIT];
    char password[PASSWORD_LIMIT];
} user;




int create_bind_socket(int * server_sd, int * port, char * addr )
{
    *server_sd=socket(AF_INET,SOCK_STREAM,0);
    if (*server_sd<0){
        perror("Socket Failed");
        return -1;
    }

    printf("%d %d %s \n",*server_sd,*port,addr);


    struct sockaddr_in server_addr;

    memset(&server_addr,0,sizeof(server_addr));
    server_addr.sin_family=AF_INET;
    server_addr.sin_port=htons(*port);
    
    if (inet_aton(addr,&server_addr.sin_addr.s_addr)==0){
        perror("IP error");
        return -1;
    };


    if (bind(*server_sd,(struct sockaddr *)&server_addr, sizeof(server_addr))<0)
    {
        perror("Bind failed");
        return -1;
    }

    if (listen(*server_sd,10)<0){
        perror("Listen failed");
        return -1;
    }
} 

void initiaize_users(user* client_list,char * input_file){
    
    FILE* userfile=fopen(input_file,"r");
    char * line;
    size_t len=USERNAME_LIMIT+PASSWORD_LIMIT;

    line=(char *)malloc(len*sizeof(char));


    for (int i=0;i<N_USERS;i++){

        getline(&line,&len,userfile);
        char username[USERNAME_LIMIT], password[PASSWORD_LIMIT];
        sscanf(line,"%s %s",client_list[i].username,client_list[i].password);
    }
}


int check_username(user* client_list,char * username){

    for(int i=0;i<N_USERS;i++){
        if (strcmp(username,client_list[i].username)==0){
            return i;
        }        
    }
    
    return 0;       

}


void create_connection(client * client_list,int fd){

    for (int i=0;i<MAX_CONNECTIONS;i++){
        if (client_list[i].fd==-1){
            client_list[i].fd=fd;
        }
    }
    printf("Cannot add fd: %d; connection is full \n",fd);
}


int get_connection_index(client * client_list, int fd){
    for (int i=0;i<MAX_CONNECTIONS;i++){
        if (client_list[i].fd==fd){
            return i;
        }
    }
    
    return -1;

}

int main(int argc, char * argv[]){

    int socket_sd,port,high_sock;
    char * addr;
    struct timeval timeout;

    if (argc<3){
        printf("IP Address and Port not given \n");
        return -1;
    }

    addr=argv[1];
    port=atoi(argv[2]);

    user users[N_USERS];
    initiaize_users(users,INPUT_FILE);


    for (int i=0;i<N_USERS;i++){
        printf("%s %s \n" ,users[i].username,users[i].password);
    }


    if (create_bind_socket(&socket_sd,&port,addr)<0){
        return -1;
    };

    client clients[MAX_CONNECTIONS]={{-1,0, 0, "",""}};
    fd_set socks;
    FD_ZERO(&socks);
    FD_SET(socket_sd,&socks);
    high_sock=socket_sd;
    timeout.tv_sec=1;
    timeout.tv_usec=0;

    

    while (1){
        int readsocks=select(high_sock+1,&socks,NULL,NULL,&timeout);
        
        if (readsocks<0){
            perror("SELECT FAILED");
            return -1;
        }
        
        if (readsocks==0){
        }

        else{

            if (FD_ISSET(socket_sd,&socks)){

                int new_fd;
                new_fd=accept(socket_sd,NULL,NULL);
                
                if (accept<0){
                    perror("Accept failed");
                    return -1;
                }

                FD_SET(new_fd,&socks);

                if (new_fd>high_sock){
                    high_sock=new_fd;
                }

                create_connection(clients,new_fd);
            }
        
            else{

                for (int i=0;i<MAX_CONNECTIONS;i++){

                    if (FD_ISSET(clients[i].fd,&socks)){
                        
                        char buffer[COMMAND_LIMIT];

                        int buffer_size;
                        buffer_size=read(clients[i].fd,buffer,buffer_size);

                        if (buffer_size<0){
                            perror("Error reading from the socket");
                        }

                        else{
                            char command[COMMAND_LIMIT];
                            char param[COMMAND_LIMIT];
                            sscanf(buffer,"%s %s",command,param);

                            if (strcmp(command,"USER")==0){
                                
                                int userid=check_username(users,param);
                                if (userid<0){
                                    printf("USERNAME DOES NOT EXIST SEND AGAIN");
                                }
                                else{

                                    clients[i].user_inited=1;
                                    strcpy(clients[i].name,param);
                                    strcpy(clients[i].pass,users[userid].password);
                                }

                                
                            }

                            if (strcmp(command,"PASS")==0){
                                if (clients[i].user_inited==1){
                                    if (strcmp(clients[i].pass,param)==0){
                                        clients[i].authenticated=1;
                                    }

                                    else{

                                        printf("BAD AUTHENTICATION");
                                    }
                                }
                                else{
                                    printf("GIVE USERNAME FIRST");
                                }
                            }
                        }
                    }
                }
            
            }
        }
    }
}