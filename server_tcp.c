#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server, client;
    socklen_t len = sizeof(client);
    char buffer[1024];

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    server.sin_family = AF_INET;
    server.sin_port = htons(5000);
    server.sin_addr.s_addr = INADDR_ANY;

    bind(server_fd, (struct sockaddr*)&server, sizeof(server));
    listen(server_fd, 5);

    printf("Server waiting...\n");

    client_fd = accept(server_fd, (struct sockaddr*)&client, &len);

    while (1) {
        int n = recv(client_fd, buffer, sizeof(buffer)-1, 0);
        if (n <= 0) break;

        buffer[n] = '\0';
        printf("Client: %s", buffer);

        send(client_fd, "Message received\n", 18, 0);
    }

    close(client_fd);
    close(server_fd);
    return 0;
}