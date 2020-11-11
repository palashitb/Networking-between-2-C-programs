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
    struct stat file;
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
    char chra[100];
    recv(new_socket, chra, 100, 0);
    printf("%s\n", chra);
    while(1){
        int i = 1;
        char filename[100];
        recv(new_socket, filename, 100, 0);
        int fd = open(filename, O_RDONLY, 00400);
        if( fd < 0 ){
            printf("fd = %d and filename was %s", fd, filename);
            send(new_socket, "No such file exists. Please try again!\n", 40, 0);
            continue;
        }
        else
            send(new_socket, "Ok. Initiating file transfer.\n", 31, 0);

        fstat(fd, &file);
        int size = 0, cur;
        while(1){
            cur = read(fd,  buffer, buf_size);
            size += cur;
            if( send(new_socket, buffer, cur, 0) < 0 ){
                perror("sharing failure:");
                exit(1);
            }
            if( size >= file.st_size){
                sleep(2);
                send(new_socket, "Palash", 7, 0);
                printf("\nSent the said file to the client completely!\n");
                break;
            }
            printf("\rDownloading..... %f %%", ((float)100 * (float)size) / (float)file.st_size);
            // printf("\r File sending ")
            // printf("%d %d\n", size, file.st_size);
            bzero(buffer, buf_size);
        }
    }

    send(new_socket , hello , strlen(hello) , 0 );  // use sendto() and recvfrom() for DGRAM
    return 0;
}
