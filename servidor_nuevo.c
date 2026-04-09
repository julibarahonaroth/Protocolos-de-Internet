#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 9090
#define MAX 1024

//SERVIDOR CALCULADORA


int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;
    char buffer[MAX];
    char respuesta[MAX];
    char operacion[20];
    double a,b,resultado;

    //creo el socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd ==-1){
        perror("Error al crear el socket");
        exit(1);
    }

    memset(&server_addr,0,sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);


    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error en bind");
        close(server_fd);
        exit(1);
    }

    if (listen(server_fd, 1) < 0) {
        perror("Error en listen");
        close(server_fd);
        exit(1);
    }

    printf("Servidor calculadora listo en puerto %d...\n", PORT);

    addr_size = sizeof(client_addr);
    client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &addr_size);
    if (client_fd < 0) {
        perror("Error en accept");
        close(server_fd);
        exit(1);
    }

    printf("Cliente conectado.\n");


    while (1) {
        memset(buffer, 0, MAX);

        int bytes = recv(client_fd, buffer, MAX, 0);

        if (bytes <= 0) {
            printf("Cliente desconectado\n");
            break;
        }

        if (strncmp(buffer, "FIN", 3) == 0) {
            printf("Fin de conexión\n");
            break;
        }

        printf("Operacion recibida: %s", buffer);

        memset(operacion, 0, sizeof(operacion));
        memset(respuesta, 0, MAX);

        if (sscanf(buffer, "%19s %lf %lf", operacion, &a, &b) == 3) {
            if (strcmp(operacion, "SUMA") == 0) {
                resultado = a + b;
                snprintf(respuesta, MAX, "Resultado: %.2f\n", resultado);
            } else if (strcmp(operacion, "RESTA") == 0) {
                resultado = a - b;
                snprintf(respuesta, MAX, "Resultado: %.2f\n", resultado);
            } else if (strcmp(operacion, "MULT") == 0) {
                resultado = a * b;
                snprintf(respuesta, MAX, "Resultado: %.2f\n", resultado);
            } else if (strcmp(operacion, "DIV") == 0) {
                if (b == 0) {
                    snprintf(respuesta, MAX, "Error: division por cero.\n");
                } else {
                    resultado = a / b;
                    snprintf(respuesta, MAX, "Resultado: %.2f\n", resultado);
                }
            } else {
                snprintf(respuesta, MAX, "Error: operacion no valida.\n");
            }
        } else {
            snprintf(respuesta, MAX, "Error: formato invalido. Use: SUMA 4 5\n");
        }

        send(client_fd, respuesta, strlen(respuesta), 0);
    }

    close(client_fd);
    close(server_fd);
    return 0;
}