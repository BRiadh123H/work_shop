#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define TAILLE_BUFFER 256
#define NB_MESSAGES   60

void recuperer_enfants(int sig) {
    (void)sig;
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

void service_heure(int sock) {
    for (int i = 0; i < NB_MESSAGES; i++) {
        time_t maintenant = time(NULL);
        struct tm *t = localtime(&maintenant);
        char message[TAILLE_BUFFER];
        snprintf(message, sizeof(message),
                 "Il est %02d:%02d:%02d !\n",
                 t->tm_hour, t->tm_min, t->tm_sec);
        if (write(sock, message, strlen(message)) < 0) break;
        sleep(1);
    }
}

void service_proc(int sock) {
    FILE *fp = popen("ps -e | wc -l", "r");
    char message[TAILLE_BUFFER];
    if (fp == NULL) {
        snprintf(message, sizeof(message), "Erreur : popen\n");
    } else {
        int nb;
        fscanf(fp, "%d", &nb);
        pclose(fp);
        snprintf(message, sizeof(message),
                 "Nombre de processus : %d\n", nb - 1);
    }
    write(sock, message, strlen(message));
}

void service_echo(int sock) {
    char buffer[TAILLE_BUFFER];
    write(sock, "Tapez une ligne :\n", 18);
    int n = read(sock, buffer, sizeof(buffer) - 1);
    if (n <= 0) return;
    buffer[n] = '\0';
    for (int i = 0; i < n; i++) buffer[i] = toupper((unsigned char)buffer[i]);
    write(sock, buffer, strlen(buffer));
}

void traiter_client(int sock) {
    char buffer[TAILLE_BUFFER];

    const char *menu =
        "Services disponibles : HEURE, PROC, ECHO\n"
        "Tapez le nom du service :\n";
    write(sock, menu, strlen(menu));

    int n = read(sock, buffer, sizeof(buffer) - 1);
    if (n <= 0) return;
    buffer[n] = '\0';

    while (n > 0 && (buffer[n-1] == '\n' || buffer[n-1] == '\r')) {
        buffer[--n] = '\0';
    }

    printf("[Enfant %d] Service demande : '%s'\n", getpid(), buffer);

    if (strcmp(buffer, "HEURE") == 0) {
        service_heure(sock);
    } else if (strcmp(buffer, "PROC") == 0) {
        service_proc(sock);
    } else if (strcmp(buffer, "ECHO") == 0) {
        service_echo(sock);
    } else {
        const char *err = "Service inconnu.\n";
        write(sock, err, strlen(err));
    }
}

int main(int argc, char *argv[]) {
    int    socket_ecoute, socket_client;
    int    port;
    struct sockaddr_in adresse_serveur, adresse_client;
    socklen_t taille_adresse = sizeof(adresse_client);

    if (argc != 2) {
        fprintf(stderr, "Usage : %s <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    port = atoi(argv[1]);

    signal(SIGCHLD, recuperer_enfants);

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
        perror("bind"); exit(EXIT_FAILURE);
    }
    if (listen(socket_ecoute, 5) < 0) {
        perror("listen"); exit(EXIT_FAILURE);
    }
    printf("Serveur concurrent en ecoute sur le port %d (PID %d)...\n",
           port, getpid());

    while (1) {
        socket_client = accept(socket_ecoute,
                               (struct sockaddr*)&adresse_client,
                               &taille_adresse);
        if (socket_client < 0) {
            if (errno == EINTR) continue;
            perror("accept"); continue;
        }

        printf("[Parent %d] Nouveau client %s:%d\n",
               getpid(),
               inet_ntoa(adresse_client.sin_addr),
               ntohs(adresse_client.sin_port));

        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            close(socket_client);
            continue;
        }

        if (pid == 0) {
            close(socket_ecoute);
            traiter_client(socket_client);
            close(socket_client);
            printf("[Enfant %d] Termine.\n", getpid());
            exit(EXIT_SUCCESS);
        } else {
            close(socket_client);
        }
    }

    close(socket_ecoute);
    return EXIT_SUCCESS;
}
