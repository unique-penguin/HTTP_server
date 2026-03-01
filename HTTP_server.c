#include <stdio.h>
// perror()
// printf()
#include <sys/socket.h>
// socket()
// bind()
// listen()
// accept()
// send()
#include <netinet/in.h>
// sockaddr_in
#include <string.h>
// memset()
#include <arpa/inet.h>
// inet_addr()
#include <unistd.h>
// close()

#define PORT 8080
#define LOCAL_IP "192.168.1.153"

int main(int argc, char const *argv[])
{
    // html and http to send
    FILE *hmtl_file;
    if((hmtl_file = fopen("index.html", "r")) == NULL){
        perror("\x1b[1;33;41mSTDERR fopen()\x1b[0m");
        return -1;
    }

    char content[1024];
    size_t content_len = fread(content, 1, sizeof(content)-1, hmtl_file);
    content[content_len] = '\0';

    char http_header[8192] = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
    strcat(http_header, content);
    strcat(http_header, "\r\n\r\n");
    printf("HTTP TO BE SENT:\n%s", http_header);

    // create a socket for the server
    int server_socket_fd;
    if((server_socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        perror("\x1b[1;33;41mSTDERR socket()\x1b[0m");
        return -1;
    }
    //printf("Server socket fd: %d\n", server_socket_fd);

    // define the address
    struct sockaddr_in server_socket_address;
    memset(&server_socket_address, 0, sizeof(server_socket_address));
    server_socket_address.sin_family = AF_INET;
    server_socket_address.sin_port = htons(PORT);
    server_socket_address.sin_addr.s_addr = INADDR_ANY; //inet_addr(LOCAL_IP);

    if(bind(server_socket_fd, (struct sockaddr *) &server_socket_address, sizeof(server_socket_address)) == -1){
        perror("\x1b[1;33;41mSTDERR bind()\x1b[0m");
        return -1;
    }

    if(listen(server_socket_fd, 5) == -1){
        perror("\x1b[1;33;41mSTDERR listen()\x1b[0m");
        return -1;
    }

    
    while (1)
    {
        int client_socket_fd;

        struct sockaddr_in client_socket_address;
        memset(&client_socket_address, 0, sizeof(client_socket_address));
        socklen_t client_socket_address_len = sizeof(client_socket_address);

        if((client_socket_fd = accept(server_socket_fd, (struct sockaddr *) &client_socket_address, &client_socket_address_len)) == -1){
            //perror("\x1b[1;33;41;mSTDERR accept()\x1b[0m");
            continue;
        }
        printf("Client connected: %s\n", inet_ntoa(client_socket_address.sin_addr));
        if(send(client_socket_fd, http_header, sizeof(http_header), 0) < 0){
            perror("\x1b[1;33;41;mSTDERR send()\x1b[0m");
            printf("HTTP TO BE SENT:\n%s", http_header);
        }
        close(client_socket_fd);
    }

    return 0;
}
