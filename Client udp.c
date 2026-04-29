#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define TAILLE_BUFFER 256
#define NB_MESSAGES   60

int main(int argc, char *argv[]) {
    int    socket_fd;
    int    port;
    struct sockaddr_in adresse_serveur;
    struct hostent    *info_serveur;
    socklen_t taille_adresse = sizeof(adresse_serveur);
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

    socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd < 0) { perror("socket"); exit(EXIT_FAILURE); }

    memset(&adresse_serveur, 0, sizeof(adresse_serveur));
    adresse_serveur.sin_family = AF_INET;
    adresse_serveur.sin_port   = htons(port);
    memcpy(&adresse_serveur.sin_addr,
           info_serveur->h_addr_list[0],
           info_serveur->h_length);

    const char *bonjour = "Bonjour\n";
    if (sendto(socket_fd, bonjour, strlen(bonjour), 0,
               (struct sockaddr*)&adresse_serveur, taille_adresse) < 0) {
        perror("sendto Bonjour"); exit(EXIT_FAILURE);
    }
    printf("Envoye au serveur : %s", bonjour);

    printf("\n=== Reception des messages ===\n");
    for (int i = 0; i < NB_MESSAGES; i++) {
        n = recvfrom(socket_fd, buffer, sizeof(buffer) - 1, 0,
                     NULL, NULL);
        if (n < 0) { perror("recvfrom"); break; }
        buffer[n] = '\0';
        printf("%s", buffer);
        nb_recus++;
    }

    const char *au_revoir = "Au revoir\n";
    if (sendto(socket_fd, au_revoir, strlen(au_revoir), 0,
               (struct sockaddr*)&adresse_serveur, taille_adresse) < 0) {
        perror("sendto Au revoir");
    } else {
        printf("Envoye au serveur : %s", au_revoir);
    }

    printf("\n=== Bilan client : %d messages d'heure recus ===\n", nb_recus);
    close(socket_fd);
    return EXIT_SUCCESS;
}
