#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define TAILLE_BUFFER 1024

int main(int argc, char *argv[]) {
    int    socket_fd;
    int    port;
    struct sockaddr_in adresse_serveur;
    struct hostent    *info_serveur;
    char   buffer[TAILLE_BUFFER];
    int    n;
    int    nb_recus = 0;

    if (argc != 3) {
        fprintf(stderr, "Usage : %s <serveur> <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    port = atoi(argv[2]);

    info_serveur = gethostbyname(argv[1]);
    if (info_serveur == NULL) {
        fprintf(stderr, "Erreur : impossible de resoudre '%s'\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) { perror("socket"); exit(EXIT_FAILURE); }

    memset(&adresse_serveur, 0, sizeof(adresse_serveur));
    adresse_serveur.sin_family = AF_INET;
    adresse_serveur.sin_port   = htons(port);
    memcpy(&adresse_serveur.sin_addr,
           info_serveur->h_addr_list[0],
           info_serveur->h_length);

    if (connect(socket_fd, (struct sockaddr*)&adresse_serveur,
                sizeof(adresse_serveur)) < 0) {
        perror("connect");
        exit(EXIT_FAILURE);
    }
    printf("Connecte au serveur %s:%d\n", argv[1], port);

    const char *bonjour = "Bonjour\n";
    if (write(socket_fd, bonjour, strlen(bonjour)) < 0) {
        perror("write Bonjour"); exit(EXIT_FAILURE);
    }
    printf("Envoye au serveur : %s", bonjour);

    printf("\n=== Reception des messages ===\n");
    while ((n = read(socket_fd, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[n] = '\0';
        printf("%s", buffer);

        for (int i = 0; i < n; i++) {
            if (buffer[i] == '\n') nb_recus++;
        }

        if (nb_recus >= 60) break;
    }

    const char *au_revoir = "Au revoir\n";
    if (write(socket_fd, au_revoir, strlen(au_revoir)) < 0) {
        perror("write Au revoir");
    } else {
        printf("Envoye au serveur : %s", au_revoir);
    }

    printf("\n=== Bilan client : %d messages d'heure recus ===\n", nb_recus);
    close(socket_fd);
    return EXIT_SUCCESS;
}
