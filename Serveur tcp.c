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
    int    socket_ecoute, socket_client;
    int    port;
    struct sockaddr_in adresse_serveur, adresse_client;
    socklen_t taille_adresse = sizeof(adresse_client);
    char   buffer[TAILLE_BUFFER];
    int    n;
    int    nb_envoyes = 0;

    if (argc != 2) {
        fprintf(stderr, "Usage : %s <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    port = atoi(argv[1]);

    socket_ecoute = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_ecoute < 0) { perror("socket"); exit(EXIT_FAILURE); }

    int opt = 1;
    setsockopt(socket_ecoute, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    memset(&adresse_serveur, 0, sizeof(adresse_serveur));
    adresse_serveur.sin_family      = AF_INET;
    adresse_serveur.sin_addr.s_addr = htonl(INADDR_ANY);
    adresse_serveur.sin_port        = htons(port);

    if (bind(socket_ecoute, (struct sockaddr*)&adresse_serveur,
             sizeof(adresse_serveur)) < 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    if (listen(socket_ecoute, 5) < 0) { perror("listen"); exit(EXIT_FAILURE); }
    printf("Serveur en ecoute sur le port %d...\n", port);

    socket_client = accept(socket_ecoute,
                           (struct sockaddr*)&adresse_client,
                           &taille_adresse);
    if (socket_client < 0) { perror("accept"); exit(EXIT_FAILURE); }
    printf("Client connecte depuis %s:%d\n",
           inet_ntoa(adresse_client.sin_addr),
           ntohs(adresse_client.sin_port));

    n = read(socket_client, buffer, sizeof(buffer) - 1);
    if (n <= 0) { perror("read Bonjour"); exit(EXIT_FAILURE); }
    buffer[n] = '\0';
    printf("Recu du client : %s\n", buffer);

    for (int i = 0; i < NB_MESSAGES; i++) {
        time_t maintenant = time(NULL);
        struct tm *t = localtime(&maintenant);
        char message[TAILLE_BUFFER];

        snprintf(message, sizeof(message),
                 "Il est %02d:%02d:%02d !\n",
                 t->tm_hour, t->tm_min, t->tm_sec);

        if (write(socket_client, message, strlen(message)) < 0) {
            perror("write");
            break;
        }
        nb_envoyes++;
        printf("Envoye [%d/%d] : %s", nb_envoyes, NB_MESSAGES, message);

        sleep(1);
    }

    n = read(socket_client, buffer, sizeof(buffer) - 1);
    if (n > 0) {
        buffer[n] = '\0';
        printf("Recu du client : %s\n", buffer);
    }

    printf("\n=== Bilan serveur : %d messages envoyes ===\n", nb_envoyes);
    close(socket_client);
    close(socket_ecoute);
    return EXIT_SUCCESS;
}
