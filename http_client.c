#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int main() {
    int sock;
    struct sockaddr_in server;
    char buffer[4096];

    char request[] =
        "GET / HTTP/1.1\r\n"
        "Host: example.com\r\n"
        "Connection: close\r\n\r\n";

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        return 1;
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(80);

    if (inet_pton(AF_INET, "104.21.48.1", &server.sin_addr) <= 0) {
        perror("inet_pton");
        return 1;
    }

    if (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
        perror("connect");
        return 1;
    }

    printf("Connected\n");

    if (send(sock, request, strlen(request), 0) < 0) {
        perror("send");
        return 1;
    }

    printf("Request sent\n");

    int n;
    while ((n = recv(sock, buffer, sizeof(buffer)-1, 0)) > 0) {
        buffer[n] = '\0';
        printf("%s", buffer);
    }

    printf("\nDone\n");

    close(sock);
    return 0;
}
/*
Create TCP socket
Connect to server
Send HTTP request
Receive response
Print HTML page
Close connection

*/