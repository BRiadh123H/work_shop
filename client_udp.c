#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

int main() {
    int sock;
    struct sockaddr_in server;
    char buffer[1024];

    sock = socket(AF_INET, SOCK_DGRAM, 0);

    server.sin_family = AF_INET;
    server.sin_port = htons(6000);
    inet_pton(AF_INET, "127.0.0.1", &server.sin_addr);

    while (1) {
        printf("You: ");
        fgets(buffer, sizeof(buffer), stdin);

        sendto(sock, buffer, strlen(buffer), 0,
               (struct sockaddr*)&server, sizeof(server));

        int n = recvfrom(sock, buffer, sizeof(buffer)-1, 0, NULL, NULL);
        buffer[n] = '\0';

        printf("Server: %s", buffer);
    }

    return 0;
}