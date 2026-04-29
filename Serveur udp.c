#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define TAILLE_BUFFER 256
#define NB_MESSAGES   60

int main(int argc, char *argv[]) {
    int    socket_fd;
    int    port;
    struct sockaddr_in adresse_serveur, adresse_client;
    socklen_t taille_adresse = sizeof(adresse_client);
    char   buffer[TAILLE_BUFFER];
    int    n;

    if (argc != 2) {
        fprintf(stderr, "Usage : %s <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    port = atoi(argv[1]);

    socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd < 0) { perror("socket"); exit(EXIT_FAILURE); }

    memset(&adresse_serveur, 0, sizeof(adresse_serveur));
    adresse_serveur.sin_family      = AF_INET;
    adresse_serveur.sin_addr.s_addr = htonl(INADDR_ANY);
    adresse_serveur.sin_port        = htons(port);

    if (bind(socket_fd, (struct sockaddr*)&adresse_serveur,
             sizeof(adresse_serveur)) < 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    printf("Serveur UDP en ecoute sur le port %d...\n", port);

    while (1) {

        n = recvfrom(socket_fd, buffer, sizeof(buffer) - 1, 0,
                     (struct sockaddr*)&adresse_client, &taille_adresse);
        if (n < 0) { perror("recvfrom"); continue; }
        buffer[n] = '\0';

        printf("\nNouveau client : %s:%d -- recu : %s",
               inet_ntoa(adresse_client.sin_addr),
               ntohs(adresse_client.sin_port),
               buffer);

        int nb_envoyes = 0;
        for (int i = 0; i < NB_MESSAGES; i++) {
            time_t maintenant = time(NULL);
            struct tm *t = localtime(&maintenant);
            char message[TAILLE_BUFFER];

            snprintf(message, sizeof(message),
                     "Il est %02d:%02d:%02d !\n",
                     t->tm_hour, t->tm_min, t->tm_sec);

            if (sendto(socket_fd, message, strlen(message), 0,
                       (struct sockaddr*)&adresse_client,
                       taille_adresse) < 0) {
                perror("sendto");
                break;
            }
            nb_envoyes++;
        }
        printf("=== %d messages envoyes au client ===\n", nb_envoyes);

        n = recvfrom(socket_fd, buffer, sizeof(buffer) - 1, 0,
                     (struct sockaddr*)&adresse_client, &taille_adresse);
        if (n > 0) {
            buffer[n] = '\0';
            printf("Recu : %s", buffer);
        }
    }

    close(socket_fd);
    return EXIT_SUCCESS;
}
