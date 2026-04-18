/*programa que --> 
1.escucha paquetes que pasan por la red
2.los guarda en memoria 
3.revisa que tipo de paquete es
4.muestra cierta info 
mi sniffer: captura todo el trafico Ethernet, solo IPv4, muestra la IP origen y destino 
y despues mira que protocolo es (TCP/UDP/ICMP)*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netpacket/packet.h>
#include <net/ethernet.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>

int main() {
    //descriptor del socket 
    int sockfd;
    char buffer[65536];     //bloque grande de memoria

    // Crear raw socket (captura todo los paquetes ethernet)
    sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (sockfd < 0) {
        perror("Socket error");
        return 1;
    }

    printf("Sniffer iniciado...\n");

    //quedate capturando paquetes siempre --> recvfrom() : recibe un paquete del socket y lo guarda en buffer
    while (1) {
        int packet_size = recvfrom(sockfd, buffer, sizeof(buffer), 0, NULL, NULL);
        if (packet_size < 0) {
            perror("recvfrom error");
            continue;
        }

        //interpretar principio del paquete como cabecera Ethernet
        struct ethhdr *eth = (struct ethhdr *)buffer;

        // Solo IPv4
        if (ntohs(eth->h_proto) != 0x0800)
            continue;

        //apuntar al encabezado IP
        struct iphdr *ip = (struct iphdr *)(buffer + sizeof(struct ethhdr));

        //se toma el encabezado IP y se guarda
        struct sockaddr_in src, dst;
        src.sin_addr.s_addr = ip->saddr;    //ip del origen
        dst.sin_addr.s_addr = ip->daddr;    //ip del destino

        //muestra IP origen y IP destino
        printf("\n[IP] %s → %s\n",
               inet_ntoa(src.sin_addr),
               inet_ntoa(dst.sin_addr));


        //ahora miramos los protocolos
        // TCP
        if (ip->protocol == 6) {
            struct tcphdr *tcp = (struct tcphdr *)(buffer + sizeof(struct ethhdr) + ip->ihl * 4);

            printf("[TCP] detectado\n");
        }

        // UDP
        else if (ip->protocol == 17) {
            struct udphdr *udp = (struct udphdr *)(buffer + sizeof(struct ethhdr) + ip->ihl * 4);

            printf("[UDP] Puerto origen: %d | Puerto destino: %d\n",
                   ntohs(udp->source),
                   ntohs(udp->dest));
        }

        // ICMP (opcional mostrar también)
        else if (ip->protocol == 1) {
            printf("[ICMP] paquete detectado\n");
        }
    }

    close(sockfd);
    return 0;
}