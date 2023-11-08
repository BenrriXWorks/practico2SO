#include "include/FastSocket.h"
#include "include/SearchQuery.h"
#include <thread>
#include <vector>

#define portFront getenv("PORT_C1")
#define portBack getenv("PORT_C2")
#define addr getenv("ADDRESS")
#define buffsize getenv("BUFFER_SIZE")

void clientHandler(int frontendFd) {
    while (true) {
        std::string query;
        int recvResult = FastSocket::recvmsg(frontendFd, query, atoi(buffsize));
        if (recvResult == -1) {
            printf("Error al recibir el mensaje\n");
            break;
        } else if (recvResult == 0) {
            printf("Cliente desconectado\n");
            break;
        }
        SearchQuery sq = SearchQuery::fromString(query);
        printf("---------\n%s\n", sq.toString().c_str());
        if (FastSocket::sendmsg(frontendFd, "Mensaje recibido!\n", atoi(buffsize)) <= 0) {
            printf("Error al responder el mensaje\n");
            break;
        }
    }
    close(frontendFd);
}

int main() {
    if (nullptr == getenv("BUFFER_SIZE")) {
        printf("Falta la variable de entorno BUFFER_SIZE");
        return EXIT_FAILURE;
    }
    if (portFront == nullptr || portBack == nullptr || addr == nullptr) {
        printf("Falta el puerto (C1 O C2) o la dirección en env\n");
        printf("portFront: %s | portBack: %s | addr: %s\n", portFront, portBack, addr);
        return EXIT_FAILURE;
    }
    printf("Conectando al backend\n");
    int databaseFd;
    while ((databaseFd = FastSocket::ClientSocket(atoi(portBack), addr)) == -1) {
        printf("Conexion fallida, reintentando en 5 segundos\n");
        sleep(5);
    }
    printf("Conexion exitosa!\n");

    std::vector<std::thread> clientThreads;

    while (true) {
        printf("Esperando cliente\n");
        int frontendFd = FastSocket::ServerSocket(atoi(portFront), -1);
        if (frontendFd == -1) {
            printf("No se pudo crear el socket al frontend, saliendo!\n");
            printf("Intentandolo nuevamente en 5 segundos\n");
            sleep(5);
            continue;
        }

        printf("Se ha conectado con un cliente\n");
        std::thread handlerThread(clientHandler, frontendFd);
        clientThreads.push_back(std::move(handlerThread));
    }

    for (auto& thread : clientThreads) {
        thread.join();
    }

    close(databaseFd);

    return EXIT_SUCCESS;
}
