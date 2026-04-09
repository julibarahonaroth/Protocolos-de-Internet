#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 9090
#define MAX 1024

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[MAX];

    //creo el socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Error al crear socket");
        exit(1);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
        perror("IP invalida");
        close(sockfd);
        exit(1);
    }

    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error al conectar");
        close(sockfd);
        exit(1);
    }

    printf("Conectado al servidor calculadora.\n");
    printf("Ingrese operaciones como: SUMA 4 5\n");
    printf("Operaciones validas: SUMA, RESTA, MULT, DIV\n");
    printf("Escriba FIN para terminar.\n");

    while (1) {
        printf("Operacion: ");
        if (fgets(buffer, MAX, stdin) == NULL) {
            break;
        }

        send(sockfd, buffer, strlen(buffer), 0);

        if (strncmp(buffer, "FIN", 3) == 0) {
            break;
        }

        memset(buffer, 0, MAX);
        int bytes = recv(sockfd, buffer, MAX - 1, 0);

        if (bytes <= 0) {
            printf("Servidor desconectado.\n");
            break;
        }

        buffer[bytes] = '\0';
        printf("%s", buffer);
    }

    close(sockfd);
    return 0;
}