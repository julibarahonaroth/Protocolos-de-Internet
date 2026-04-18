#Protocolos de Internet - TP1B
El objetivo del trabajo es comprender el funcionamiento de los sockets RAW_SOCKETS y 
a analizar el trafico de red en todos los niveles tanto ICMP, TCP y UDP.

raw_socket_sniffer.c --> codigo principal
raw_socket_sniffer_explicado.c --> ejercicio 1: se modifico el codigo para mostrar en pantalla cada paso. (creacion del socket, recepcion de paquetes, analisis de cabeceras)
raw_socket_sniffer_todoTrafico.c --> ejercicio 2: se adapto el sniffer para capturar todo el trafico de red usando RAW socket
raw_socket_3.c --> ejercicio 3: se envio trafico desde el cliente desarrollado previanmente y se verifica la captura del sniffer.
raw_socket_4.c --> ejercicio 4: se genera el trafico ICPMP (ping) + LOG
(ejercicio 5 teorico)
raw_socket_6.c --> ejercicio 6: se extiende el sniffer para identificar distintos protocolos segun la cabecera IP.

TP1B-BARAHONA_BERTOLA.pdf es un informe de resolucion de todo el trabajo
