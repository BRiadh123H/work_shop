#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define TAILLE_BUFFER 4096

int main(int argc, char *argv[]) {
    int    socket_fd;
    int    port;
    char  *nom_serveur;
    struct sockaddr_in adresse_serveur;
    struct hostent    *info_serveur;
    char   ligne[TAILLE_BUFFER];
    char   buffer[TAILLE_BUFFER];
    int    n;

    if (argc != 3) {
        fprintf(stderr, "Usage : %s <serveur> <port>\n", argv[0]);
        fprintf(stderr, "Exemple : %s example.com 80\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    nom_serveur = argv[1];
    port        = atoi(argv[2]);

    info_serveur = gethostbyname(nom_serveur);
    if (info_serveur == NULL) {
        fprintf(stderr, "Erreur : impossible de résoudre '%s'\n", nom_serveur);
        exit(EXIT_FAILURE);
    }

    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&adresse_serveur, 0, sizeof(adresse_serveur));
    adresse_serveur.sin_family = AF_INET;
    adresse_serveur.sin_port   = htons(port);
    memcpy(&adresse_serveur.sin_addr,
           info_serveur->h_addr_list[0],
           info_serveur->h_length);

    if (connect(socket_fd,
                (struct sockaddr*)&adresse_serveur,
                sizeof(adresse_serveur)) < 0) {
        perror("connect");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }
    printf("Connecté à %s:%d\n", nom_serveur, port);
    printf("Tapez votre requête HTTP (ligne vide pour terminer) :\n");

    while (fgets(ligne, sizeof(ligne), stdin) != NULL) {
        size_t len = strlen(ligne);

        if (len > 0 && ligne[len-1] == '\n') {
            ligne[len-1] = '\r';
            ligne[len]   = '\n';
            ligne[len+1] = '\0';
            len = len + 1;
        }

        if (write(socket_fd, ligne, len) < 0) {
            perror("write");
            close(socket_fd);
            exit(EXIT_FAILURE);
        }

        if (strcmp(ligne, "\r\n") == 0) {
            break;
        }
    }

    printf("\n=== Réponse du serveur ===\n");
    while ((n = read(socket_fd, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[n] = '\0';
        printf("%s", buffer);
    }
    if (n < 0) {
        perror("read");
    }
    printf("\n=== Fin de la réponse ===\n");

    close(socket_fd);
    return EXIT_SUCCESS;
}
