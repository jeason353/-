/*server.c*/
#include<netinet/in.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>

#define SERVER_PORT                5558
#define LENGTH_OF_LISTEN_QUEUE     20
#define BUFFER_SIZE                1024
#define FILE_NAME_MAX_SIZE         512

int main(int argc, char **argv)
{
    // set socket's address information
    // 设置一个socket地址结构server_addr,代表服务器internet的地址和端口
    struct sockaddr_in   server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htons(INADDR_ANY);
    server_addr.sin_port = htons(SERVER_PORT);

    // create a stream socket
    // 创建用于internet的流协议(TCP)socket，用server_socket代表服务器向客户端提供服务的接口
    int server_socket = socket(PF_INET, SOCK_STREAM, 0);
    if (server_socket < 0)
    {
        perror("Create Socket");
        exit(1);
    }

    // int on = 1;
    // int tmp = setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR,&on, sizeof(on));
    // if(tmp < 0){
    //     perror("setsockopt");
    //     close(server_socket);
    //     exit(1);
    // }

    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)))
    {
        perror("bind 9220");
        close(server_socket);
        exit(1);
    }

    if (listen(server_socket, LENGTH_OF_LISTEN_QUEUE))
    {
        perror("Server Listen");
        close(server_socket);
        exit(1);
    }

    while(true)
    {
        // 定义客户端的socket地址结构client_addr，当收到来自客户端的请求后，调用accept
        // 接受此请求，同时将client端的地址和端口等信息写入client_addr中
        struct sockaddr_in client_addr;
        socklen_t length = sizeof(client_addr);

        int new_server_socket = accept(server_socket, (struct sockaddr*)&client_addr, &length);
        if (new_server_socket < 0)
        {
            printf("Server Accept Failed!\n");
            break;
        }

        // printf("count 1\n");
        char buffer[BUFFER_SIZE];
        bzero(buffer, sizeof(buffer));
        length = recv(new_server_socket, buffer, BUFFER_SIZE, 0);
        if (length < 0)
        {
            printf("Server Recieve Data Failed!\n");
            break;
        }

        char file_name[FILE_NAME_MAX_SIZE + 1];
        bzero(file_name, sizeof(file_name));
        strncpy(file_name, buffer,
                strlen(buffer) > FILE_NAME_MAX_SIZE ? FILE_NAME_MAX_SIZE : strlen(buffer));

        FILE *fp = fopen(file_name, "r");
        if (fp == NULL)
        {
            perror("find file");
            send(new_server_socket, buffer, 0, 0);
        }
        else
        {
            bzero(buffer, BUFFER_SIZE);
            int file_block_length = 0;
            while( (file_block_length = fread(buffer, sizeof(char), BUFFER_SIZE, fp)) > 0)
            {
                printf("file_block_length = %d\n", file_block_length);

                if (send(new_server_socket, buffer, file_block_length, 0) < 0)
                {
                    perror("send file");
                    break;
                }
                bzero(buffer, sizeof(buffer));
            }
            fclose(fp);
            printf("send %s successfully!\n", file_name);
        }

        close(new_server_socket);
    }

    close(server_socket);

    return 0;
}