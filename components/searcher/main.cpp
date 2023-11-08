#include "include/FastSocket.h"
#include <array>

#define portFront getenv("PORT_C1")
#define addr getenv("ADDRESS")
#define buffsize getenv("BUFFER_SIZE")

int main(){


    std::array<const char*, 5> genericQueries = {
        "hola perro casa\nfrontend\ncache",
        "test busqueda\nfrontend\ncache",
        "hola gato\nfrontend\ncache",
        "busqueda generica\nfrontend\ncache",
        "si perro\nfrontend\ncache"
    };

    while (true) {
        int connectionFd = -1;
        while ((connectionFd = FastSocket::ClientSocket(atoi(portFront),addr)) == -1){
            printf("No se pudo conectar con el cache... reintentando en 5 segundos\n");
            close(connectionFd);
            sleep(5);
        }
        printf("Conexion establecida con el servidor de cache\n");
        while (true){
            std::string msg = genericQueries[rand()%5];
            int sent = FastSocket::sendmsg(connectionFd,msg, atoi(buffsize));
            if (sent == -1 || sent == 0) {
                printf("Se rechazo el mensaje %s\n",msg.c_str());
                close(connectionFd);
                break;
            }

            std::string response;
            if (FastSocket::recvmsg(connectionFd, response, atoi(buffsize)) == -1 || recvmsg == 0){
                printf("Error al recibir la respuesta\n");
                close(connectionFd);
                break;
            }
            
            printf("Response: %s\n",response.c_str());
            sleep(2);
        }
        close(connectionFd);
    }

    return 0;
}