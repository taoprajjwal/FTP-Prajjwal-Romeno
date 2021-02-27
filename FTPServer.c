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


#define N_USERS 5
#define USERNAME_LIMIT 20
#define PASSWORD_LIMIT 20
#define INPUT_FILE "users.txt"



typedef struct clients{

    int user_inited;
    int authenticated;
    char name[USERNAME_LIMIT];
    char pass[PASSWORD_LIMIT];
} client;


int create_bind_socket(int * server_sd, int * port, char * addr )
{
    *server_sd=socket(AF_INET,SOCK_STREAM,0);
    if (*server_sd<0){
        perror("Socket Failed");
        return -1;
    }

    struct sockaddr_in server_addr;

    memset(&server_addr,0,sizeof(server_addr));
    server_addr.sin_family=AF_INET;
    server_addr.sin_port=htons(*port);
    inet_aton(addr,&server_addr.sin_addr.s_addr);


    if (bind(*server_sd,(struct sockaddr *)&server_addr, sizeof(server_addr))<0)
    {
        perror("Bind failed");
        return -1;
    }
} 

void initiaize_users(client* client_list,char * input_file){
    
    FILE* userfile=fopen(input_file,"r");
    char * line;
    size_t len=USERNAME_LIMIT+PASSWORD_LIMIT;

    line=(char *)malloc(len*sizeof(char));


    for (int i=0;i<N_USERS;i++){

        getline(&line,&len,userfile);
        char username[USERNAME_LIMIT], password[PASSWORD_LIMIT];
        sscanf(line,"%s %s",client_list[i].name,client_list[i].pass);
        client_list[i].authenticated=0;
        client_list[i].user_inited=-1;
    }
}


int check_username(client * client_list,char * username){

    for(int i=0;i<N_USERS;i++){
        if (strcmp(username,client_list[i].name)==0){
            client_list[i].user_inited=1;
            return i;
        }        
    }
    
    return 0;       

}


int check_password(client * client_list,int username_index, char * password){

    if (strcmp(password,client_list[username_index].pass)==0){
        return 1;
    }

    return 0;
}

int main(int argc, char * argv[])
{


    int socket_sd,port;
    char * addr;

    if (argc<2){
        printf("IP Address and Port not given \n");
        return -1;
    }

    addr=argv[0];
    port=atoi(argv[1]);

    client users[N_USERS];
    initiaize_users(users,INPUT_FILE);


    for (int i=0;i<N_USERS;i++){
        printf("%s %s \n" ,users[i].name,users[i].pass);
    }

    int username_id;


}