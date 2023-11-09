#include "include/FastSocket.h"
#include <array>
#include <unistd.h>

#define portFront getenv("PORT_C1")
#define addr getenv("ADDRESS")
#define buffsize getenv("BUFFER_SIZE")

int main() {
    std::array<const char*, 5> genericQueries = {
        "hola perro casa\nfrontend\ncache",
        "test busqueda\nfrontend\ncache",
        "hola gato\nfrontend\ncache",
        "busqueda generica\nfrontend\ncache",
        "si perro\nfrontend\ncache"
    };

    while (true) {
        printf("Intentando establecer conexión con el servidor de cache\n");
        int connectionFd = FastSocket::ClientSocket(atoi(portFront), addr);
        if (connectionFd <= 0) {
            printf("No se pudo conectar con el cache... reintentando en 2 segundos\n");
            sleep(2);
            continue;
        }
        printf("conexion aceptada\n");
        // Hacer un 'ping' al servidor
        if (FastSocket::ping(connectionFd, "?", "1", atoi(buffsize))) continue;

        printf("Conexión establecida con el servidor de cache\n");

        while (true) {
            std::string msg = genericQueries[rand() % 5];
            int sent = FastSocket::sendmsg(connectionFd, msg, atoi(buffsize));
            if (sent <= 0) {
                printf("Se rechazó el mensaje %s\n", msg.c_str());
                break;
            }
            printf("Mensaje enviado: %s\n", msg.c_str());

            std::string response;
            printf("Esperando respuesta...\n");
            if (FastSocket::recvmsg(connectionFd, response, atoi(buffsize)) <= 0) {
                printf("Error al recibir la respuesta\n");
                break;
            }
            printf("Response: %s\n", response.c_str());

            sleep(2);
        }
        close(connectionFd);
    }

    return 0;
}