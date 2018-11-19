// Client side C/C++ program to demonstrate Socket programming
#include <cstdio>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>

# define PORT 8080

int main(int argc, char const *argv[])
{
    struct sockaddr_in address;
    int sock1 = 0, sock2 = 0, valread;
    struct sockaddr_in serv_addr;
    char *hello1 = const_cast<char *>("GET hello.html HTTP/1.1\r\n"
            "User-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)\r\n"
            "Host: www.tutorialspoint.com\r\n"
            "Accept-Language: en-us\r\n"
            "Accept-Encoding: gzip, deflate\r\n"
            "Connection: Keep-Alive\r\n"
            "\r\n");
    char *hello2 = const_cast<char *>("GET hello.html HTTP/1.1\r\n"
            "User-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)\r\n"
            "Host: www.tutorialspoint.com\r\n"
            "Accept-Language: en-us\r\n"
            "Accept-Encoding: gzip, deflate\r\n"
            "Connection: Keep-Alive\r\n"
            "\r\n");

    char buffer[1024] = {0}, buff2[1024] = {0};
    if ((sock1 = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock1, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }
    send(sock1 , hello1, strlen(hello1), 0);
    valread = static_cast<int>(read(sock1 , buffer, 1024));
    std::cout << buffer << std::endl;

    if ((sock2 = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    if (connect(sock2, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }
    send(sock2, hello2, strlen(hello2), 0);
    valread = static_cast<int>(read(sock2 , buff2, 1024));
    std::cout << buff2 << std::endl;
    return 0;
}
