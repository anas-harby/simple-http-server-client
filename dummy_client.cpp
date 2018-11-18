// Client side C/C++ program to demonstrate Socket programming
#include <cstdio>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>

# define PORT 8080

int main(int argc, char const *argv[])
{
    struct sockaddr_in address;
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    // char *hello = const_cast<char *>("POST /hello.html HTTP/1.1\n"
    //         "User-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)\n"
    //         "Host: www.tutorialspoint.com\n"
    //         "Accept-Language: en-us\n"
    //         "Accept-Encoding: gzip, deflate\n"
    //         "Connection: Keep-Alive\n\n"
    //         "<!DOCTYPE html>\n"
    //         "<html>\n"
    //         "  <head>\n"
    //         "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n"
    //         "    <link rel=\"stylesheet\" href=\"/style/style.css\">\n"
    //         "    <link rel=\"stylesheet\" href=\"https://cdn.rawgit.com/twbs/bootstrap/48938155eb24b4ccdde09426066869504c6dab3c/dist/css/bootstrap.min.css\">\n"
    //         "    <!-- <link rel=\"stylesheet\" href=\"vendor/bootstrap/css/bootstrap.min.css\" type=\"text/css\"> -->\n"
    //         "    <link rel=\"stylesheet\" href=\"/css/business-casual.css\" type=\"text/css\">\n"
    //         "    <script src=\"https://maps.googleapis.com/maps/api/js?key=AIzaSyAq06l5RUVfib62IYRQacLc-KAy0XIWAVs\"></script>\n"
    //         "  </head>\n"
    //         "  <body>\n"
    //         "    <div class=\"container\"></div>\n"
    //         "  </body>\n"
    //         "  <script src=\"/bundle.js\"></script>\n"
    //         "</html>");
    char *hello = const_cast<char *>("GET hello.html HTTP/1.1\n"
            "User-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)\n"
            "Host: www.tutorialspoint.com\n"
            "Accept-Language: en-us\n"
            "Accept-Encoding: gzip, deflate\n"
            "Connection: Keep-Alive\n\n");
    char buffer[1024] = {0};
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
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

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }

    send(sock , hello, strlen(hello), 0);
    printf("Hello kitty\n");
//    valread = static_cast<int>(read(sock , buffer, 1024));
    printf("%s\n", buffer);
    return 0;
}
