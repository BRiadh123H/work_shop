#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

int main() {
    int sock;
    struct sockaddr_in server, client;
    char buffer[1024];
    socklen_t len = sizeof(client);

    sock = socket(AF_INET, SOCK_DGRAM, 0);

    server.sin_family = AF_INET;
    server.sin_port = htons(6000);
    server.sin_addr.s_addr = INADDR_ANY;

    bind(sock, (struct sockaddr*)&server, sizeof(server));

    while (1) {
        int n = recvfrom(sock, buffer, sizeof(buffer)-1, 0,
                         (struct sockaddr*)&client, &len);

        buffer[n] = '\0';
        printf("Client: %s", buffer);

        sendto(sock, "UDP received\n", 13, 0,
               (struct sockaddr*)&client, len);
    }

    return 0;
}