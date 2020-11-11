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

    // Converts an IP address in numbers-and-dots notation into either a 
    // struct in_addr or a struct in6_addr depending on whether you specify AF_INET or AF_INET6.
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

    int fd = open("temp.txt", O_RDONLY, 00400);
    int i = 1;
    fstat(fd, &file);
    printf("starting to send\n");
    int size = 0, cur;
    while(1){
        cur = read(fd,  buffer, buf_size);
        size += cur;
        if( send(sock, buffer, cur, 0) < 0 ){
            perror("sharing failure:");
            exit(1);
        }
        if( size >= file.st_size){
            sleep(2);
            send(sock, "Palash", 7, 0);
            break;
        }
        i++;
        bzero(buffer, buf_size);
    }
    bzero(buffer, buf_size);
    valread = recv( sock , buffer, buf_size, 0);  // receive message back from server, into the buffer
    printf("%s\n",buffer);
    return 0;
}
