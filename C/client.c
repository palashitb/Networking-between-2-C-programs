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
    int i = 0;
    send(sock, "Connected to client successfully!\n", 35, 0);
    printf("Connected to server successfully!\n");
    while(1){
        char inp[1000];
        char files[100];
        scanf("%s", inp);
        char *in = strtok(inp, " \t");
        int j = 0;
        while(in != NULL){
            strcpy(files, in);
            in = strtok(NULL, " \t");
            // continue;
            if( strcmp(files, "exit") == 0 )
                exhit(sock);
            else if( strcmp(files, "get") == 0 ){
                continue;
            }
            char fname[100];
            int fd = 0;
            sprintf(fname, "output%d", i);  
            fd = open(fname, O_RDWR | O_CREAT | O_TRUNC, 00400 | 00200);
            if( send(sock, files, 100, 0) < 0 ){
                perror("Sending failure:");
                exit(1);
            }
            char response[100];
            if( recv(sock, response, 100, 0) < 0 ){
                perror("receiving failure:");
                exit(1);
            }
            if( response[0] == 'O' && response[1] == 'k' ){
                printf("%s for %s\n", response, files);
            }
            else{
                printf("%s\n", response);
                continue; 
            }
            int size = 0;
            int n = 0;
            while(1){  // read infromation received into the buffer
                n = recv(sock , buffer, buf_size, 0);
                if( n < 0 ){
                    perror("receiving error:");
                    exit(1);
                }
                if( n < buf_size && strcmp(buffer, "Palash") == 0 ){
                    printf("File has been recieved completely!\n");
                    break;
                }
                size += n;
                write(fd, buffer, n);
                bzero(buffer, buf_size);
            }
            i++;
            bzero(buffer, buf_size);
        }
    }
    valread = recv(sock, buffer, buf_size, 0);  // receive message back from server, into the buffer
    printf("%s\n",buffer);
    return 0;
}
