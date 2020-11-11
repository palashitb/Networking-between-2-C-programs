// Client side C/C++ program to demonstrate Socket programming
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <string.h>
#define PORT 8000

void exhit(int socket){
    printf("Client closing down\n");
    close(socket);
    exit(1);
}

int main(int argc, char const *argv[])
{
    struct stat file;
    struct sockaddr_in address;
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char *hello = "Hello from client";
    int buf_size = 4000;
    char buffer[4000] = {0};
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    memset(&serv_addr, '0', sizeof(serv_addr)); // to make sure the struct is empty. Essentially sets sin_zero as 0
                                                // which is meant to be, and rest is defined below

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)  // connect to the server address
    {
        printf("\nConnection Failed \n");
        return -1;
    }
    
    send(sock, "Connected to cliend successfully!\n", 35, 0);
    printf("Connected to server successfully!\n");
    while(1){

        char inp[1000];
        char files[100];
        scanf("%s", inp);
        char *in = strtok(inp, " \t");
        int ctr = 0;
        int ctrl = 1;
        while(in != NULL){
            strcpy(files, in);
            // printf("%s ctr is %d haha\n", files, ctrl);
            ctrl++;
            in = strtok(NULL, " \t");
            if( strcmp(files, "exit") == 0 )
                exhit(sock);
            else if( strcmp(files, "get") == 0 )
                continue;
        
            send(sock, files, 100, 0);
            char response[100];
            recv(sock, response, 100, 0);
            if( response[0] == 'O' && response[1] == 'k' )
                printf("%s\n", response);
            else{
                printf("%s\n", response);
                continue; 
            }   
            int fd = open("out.txt", O_RDWR | O_CREAT, 00400 | 00200);
            int i = 1;
            int size = 0;
            int n = 0;
            while(1){  // read infromation received into the buffer
                n = recv(sock , buffer, buf_size, 0);
                if( n < buf_size && strcmp(buffer, "Palash") == 0 ){
                    printf("File has been recieved completely!\n");
                    break;
                }
                size += n;
                write(fd, buffer, n);
                bzero(buffer, buf_size);
                i++;
            }
            bzero(buffer, buf_size);
        }
    }
    valread = recv(sock, buffer, buf_size, 0);  // receive message back from server, into the buffer
    printf("%s\n",buffer);
    return 0;
}
