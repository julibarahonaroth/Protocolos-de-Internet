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
    int sockfd;                                     //descriptor del socket UDP
    struct sockaddr_in server_addr, client_addr;    
    char buffer[MAX];
    char client_name[50];
    socklen_t addr_size;
    int bytes;

    //crea el socket UDP
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        perror("Socket no pudo abrirse");
        exit(1);
    }

    //configuro la direccion del servidor
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    //asigno al IP + direccion
    if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) != 0) {
        perror("Bind fallo");
        close(sockfd);
        exit(1);
    }

    printf("Servidor UDP esperando mensajes en el puerto %d...\n", PORT);


    addr_size = sizeof(client_addr);        //recvfrom() necesita saber el tamaño

    //recibe el mensaje y necesita saber quien lo mando
    //controla errores
    memset(client_name, 0, sizeof(client_name));
    bytes = recvfrom(sockfd, client_name, sizeof(client_name) - 1, 0,       
                     (struct sockaddr*)&client_addr, &addr_size);
    if (bytes < 0) {
        perror("Error al recibir nombre");
        close(sockfd);
        exit(1);
    }
    client_name[bytes] = '\0';

    printf("Cliente conectado: %s\n", client_name);

    while (1) {
        memset(buffer, 0, MAX);

        bytes = recvfrom(sockfd, buffer, MAX - 1, 0,
                         (struct sockaddr*)&client_addr, &addr_size);           //recibe el mensaje

        if (bytes < 0) {
            perror("Error al recibir mensaje");
            break;
        }

        buffer[bytes] = '\0';

        if (strncmp(buffer, "FIN", 3) == 0) {
            printf("%s ha finalizado la comunicacion.\n", client_name);
            break;
        }

        printf("%s: %s", client_name, buffer);

        printf("Servidor: ");
        memset(buffer, 0, MAX);

        if (fgets(buffer, MAX, stdin) == NULL) {
            printf("Error al leer desde teclado.\n");
            break;
        }

        //envia respuesta al cliente que la mando (no hay conexion previa por eso especifica)
        if (sendto(sockfd, buffer, strlen(buffer), 0,
                   (struct sockaddr*)&client_addr, addr_size) < 0) {
            perror("Error al enviar mensaje");
            break;
        }

        if (strncmp(buffer, "FIN", 3) == 0) {
            printf("Servidor finaliza la comunicacion.\n");
            break;
        }
    }

    close(sockfd);
    return 0;
}