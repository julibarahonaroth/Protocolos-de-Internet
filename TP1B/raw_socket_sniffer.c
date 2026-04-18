#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>


int main() {
    //descriptor del socket: es como el numero que usa el socket para trabajar despues  
    int sockfd;
    char buffer[65536]; //donde se guarda el paquete recibido --> 65536 porque alcanza para un paquete IP grande
    struct sockaddr saddr;
    socklen_t saddr_len = sizeof(saddr);

    // Crear raw socket 
    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);   //--> crea un socket para capturar (IPv4, raw, protocolo ICMP)
    if (sockfd < 0) {
        perror("Socket error");
        return 1;       //Si falla el socket devuelvo un numero negativo
    }

    printf("Sniffer ICMP iniciado... (Ctrl+C para detener)\n");

    //bucle principal: El programa va a quedarse escuchando paquetes hasta que: capture uno ICMP y llegue al break o lo detengas con Ctrl + C 
    while (1) {
        ssize_t packet_size = recvfrom(sockfd, buffer, sizeof(buffer), 0, &saddr, &saddr_len);  //lee los datos y lo guarda en un buffer
        if (packet_size < 0) {
            perror("recvfrom error");
            continue;
        }

        struct ip *ip = (struct ip *)buffer;
        if (ip->ip_p == IPPROTO_ICMP) {
            struct icmp *icmp = (struct icmp *)(buffer + ip->ip_hl * 4);
            printf("ICMP capturado: Tipo=%d Código=%d ID=%d Secuencia=%d\n",
                icmp->icmp_type,
                icmp->icmp_code,
                ntohs(icmp->icmp_id),
                ntohs(icmp->icmp_seq)
            );

            // Guardar en archivo para analizar con Python
            FILE *f = fopen("icmp_packet.bin", "wb");
            if (f) {
                fwrite(buffer, 1, packet_size, f);
                fclose(f);
                printf("Paquete guardado en icmp_packet.bin\n");  
                break; // solo capturamos uno para el ejemplo
            }

            
        }
    }

    close(sockfd);
    return 0;
}

