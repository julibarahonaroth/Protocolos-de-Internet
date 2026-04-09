#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MAX 1024
#define PORT 8080

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[MAX];
    char name[50];
    socklen_t addr_size;
    int bytes;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        perror("Socket fallo");
        exit(1);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
        perror("Direccion invalida");
        close(sockfd);
        exit(1);
    }

    addr_size = sizeof(server_addr);

    /* Enviar nombre */
    printf("Ingresar nombre: ");
    if (fgets(name, sizeof(name), stdin) == NULL) {
        printf("Error al leer nombre.\n");
        close(sockfd);
        exit(1);
    }
    name[strcspn(name, "\n")] = '\0';

    if (sendto(sockfd, name, strlen(name), 0,
               (struct sockaddr*)&server_addr, addr_size) < 0) {
        perror("Error al enviar nombre");
        close(sockfd);
        exit(1);
    }

    while (1) {
        printf("%s: ", name);
        memset(buffer, 0, MAX);

        if (fgets(buffer, MAX, stdin) == NULL) {
            printf("Error al leer mensaje.\n");
            break;
        }

        if (sendto(sockfd, buffer, strlen(buffer), 0,
                   (struct sockaddr*)&server_addr, addr_size) < 0) {
            perror("Error al enviar mensaje");
            break;
        }

        if (strncmp(buffer, "FIN", 3) == 0) {
            printf("Terminando la comunicacion...\n");
            break;
        }

        memset(buffer, 0, MAX);
        bytes = recvfrom(sockfd, buffer, MAX - 1, 0, NULL, NULL);

        if (bytes < 0) {
            perror("Error al recibir respuesta");
            break;
        }

        buffer[bytes] = '\0';

        if (strncmp(buffer, "FIN", 3) == 0) {
            printf("Servidor finalizo la comunicacion.\n");
            break;
        }

        printf("Servidor: %s", buffer);
    }

    close(sockfd);
    return 0;
}