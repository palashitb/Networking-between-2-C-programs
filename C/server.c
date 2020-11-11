#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#define PORT 8000

int main(int argc, char const *argv[]){
    int server_fd, new_socket, valread;
    struct sockaddr_in address;  
    int buf_size = 4000;
    int opt = 1;
    int addrlen = sizeof(address);
    char *hello = "Hello from server";
    char buffer[4000] = {0};

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)  // creates socket, SOCK_STREAM is for TCP. SOCK_DGRAM for UDP
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR,
                                                  &opt, sizeof(opt))) // SOL_SOCKET is the socket layer itself
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;  // Address family. For IPv6, it's AF_INET6. 29 others exist like AF_UNIX etc. 
    address.sin_addr.s_addr = INADDR_ANY;  // Accept connections from any IP address - listens from all interfaces.
    address.sin_port = htons( PORT );    // Server port to open. Htons converts to Big Endian - Left to Right. RTL is Little Endian

    if (bind(server_fd, (struct sockaddr *)&address,
                                 sizeof(address))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 10) < 0) // 3 is the maximum size of queue - connections you haven't accepted
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                       (socklen_t*)&addrlen))<0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    int i = 1, size = 0;
    int fd = open("out1.txt", O_CREAT | O_RDWR, 00400 | 00200);
    printf("waiting to recieve\n");
    int n = 0;
    while(1){  // read infromation received into the buffer
        n = recv(new_socket , buffer, buf_size, 0);
        if( n < buf_size && strcmp(buffer, "Palash") == 0 ){
            printf("helo hola\n");
            break;
        }
        size += n;
        printf("%d %d\n", size, n);
        write(fd, buffer, n);
        // if( buf_size > strlen(buffer))
        //     break;
        bzero(buffer, buf_size);
        i++;
    }
    send(new_socket , hello , strlen(hello) , 0 );  // use sendto() and recvfrom() for DGRAM
    return 0;
}
