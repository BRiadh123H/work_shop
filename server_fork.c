#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server, client;
    socklen_t len = sizeof(client);
    char buffer[1024];

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    server.sin_family = AF_INET;
    server.sin_port = htons(7000);
    server.sin_addr.s_addr = INADDR_ANY;

    bind(server_fd, (struct sockaddr*)&server, sizeof(server));
    listen(server_fd, 5);

    printf("Concurrent server running...\n");

    while (1) {
        client_fd = accept(server_fd, (struct sockaddr*)&client, &len);

        if (fork() == 0) {
            // child process
            close(server_fd);

            while (1) {
                int n = recv(client_fd, buffer, sizeof(buffer)-1, 0);
                if (n <= 0) break;

                buffer[n] = '\0';
                send(client_fd, buffer, n, 0);
            }

            close(client_fd);
            return 0;
        }

        close(client_fd);
    }

    return 0;
}