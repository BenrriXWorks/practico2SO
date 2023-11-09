#include "include/FastSocket.h"
#include "include/SearchQuery.h"

#define portFront getenv("PORT_C1")
#define portBack getenv("PORT_C2")
#define addr getenv("ADDRESS")
#define buffsize getenv("BUFFER_SIZE")

void event(int frontendFd, int connectionFd) {
    while (true) {
        std::string query;
        int recvResult = FastSocket::recvmsg(frontendFd, query, atoi(buffsize));
        if (recvResult == -1) {
            printf("Error al recibir el mensaje\n");
            break;
        } else if (!recvResult) {
            printf("Cliente desconectado\n");
            break;
        }

        if (query == "?") {
            if (FastSocket::sendmsg(frontendFd, "?", atoi(buffsize)) <= 0) {
                printf("Error al responder con '1'\n");
                break;
            }
            printf("ping recibido de fd %d\n", frontendFd);
        }
        else {
            // Procesa la consulta y responde con "Mensaje recibido!"
            SearchQuery sq = SearchQuery::fromString(query);
            printf("---------\n%s\n", sq.toString().c_str());
            if (FastSocket::sendmsg(frontendFd, "Mensaje recibido!\n", atoi(buffsize)) <= 0) {
                printf("Error al responder el mensaje\n");
                break;
            }
            std::string msg =sq.toString();
            int sent = FastSocket::sendmsg(connectionFd, msg, atoi(buffsize));
            if (sent <= 0) {
                printf("Se rechazó el mensaje %s\n", msg.c_str());
                break;
            }
            printf("Mensaje enviado: %s\n", msg.c_str());

        }
    }
    close(frontendFd);
}

int main() {
    if (nullptr == getenv("BUFFER_SIZE")) {
        printf("Falta la variable de entorno BUFFER_SIZE\n");
        return EXIT_FAILURE;
    }
    if (portFront == nullptr || portBack == nullptr || addr == nullptr) {
        printf("Falta el puerto (C1 O C2) o la dirección en env\n");
        printf("portFront: %s | portBack: %s | addr: %s\n", portFront, portBack, addr);
        return EXIT_FAILURE;
    }
    
    int databaseFd;
    while (true) {
        printf("Conectando al backend\n");
        while ((databaseFd = FastSocket::ClientSocket(atoi(portBack), addr)) == -1) {
            printf("Conexion fallida, reintentando en 5 segundos\n");
            sleep(5);
        }
        break;
        //if (FastSocket::ping(databaseFd, "1") == -1) break;
        //else close(databaseFd), printf("Error al realizar el ping! Reintentando en 2 segundos\n"),sleep(2);
    }

    printf("Conexion exitosa!\n");

    while (true) {
        printf("Esperando cliente\n");
        int frontendFd = FastSocket::ServerSocket(atoi(portFront), 1);
        if (frontendFd == -1) {
            printf("No se pudo crear el socket, intentándolo nuevamente en 5 segundos\n");
            sleep(5);
            continue;
        }

        printf("Se ha conectado un cliente\n");

        // Lógica del cliente (manejo de mensajes) en la función 'event'
        event(frontendFd,databaseFd);
    }

    
    return EXIT_SUCCESS;
}
