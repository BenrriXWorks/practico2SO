#include "include/FastSocket.h"
#include "include/SearchQuery.h"
#include "include/ResultsQuery.h"
#include "include/Cache.h"
#include <iostream>
#include <chrono>
using namespace std;

#define CACHE_PORT getenv("PORT_C1")
#define BACKEND_PORT getenv("PORT_C2")
#define BACKEND_ADDRESS getenv("BACKEND_ADDRESS")
#define BUFFER_SIZE getenv("BUFFER_SIZE")
#define CACHE_SIZE getenv("CACHE_SIZE")

void event(int& frontendFd, int& connectionFd, Cache& cache) {
    while (true) {
        std::string query;
        int recvResult = FastSocket::recvmsg(frontendFd, query, atoi(BUFFER_SIZE));
        if (recvResult == -1) {
            printf("Error al recibir el mensaje\n");
            close(frontendFd);
            frontendFd = -1;
            break;
        } else if (!recvResult) {
            printf("Cliente desconectado\n");
            close(frontendFd);
            frontendFd = -1;
            break;
        }

        if (query == "?") {
            if (FastSocket::sendmsg(frontendFd, "?", atoi(BUFFER_SIZE)) <= 0) {
                printf("Error al responder con '1'\n");
                frontendFd = -1;
                break;
            }
            printf("ping recibido de fd %d\n", frontendFd);
            continue;
        }


        // Descompone la consulta en l
        SearchQuery sq = SearchQuery::fromString(query);
    auto startTime = chrono::high_resolution_clock::now();
        // Si la consulta esta en el cache
        ResultsQuery* finded;
        if ((finded = cache.find(sq.queryToString())) != nullptr){
            auto endTime = chrono::high_resolution_clock::now();
            finded->origin="cache";
            finded->destination="frontend";
            finded->ori="CACHE";
            finded->tiempo=to_string(chrono::duration_cast<chrono::nanoseconds>(endTime-startTime).count());
            
            if (FastSocket::sendmsg(frontendFd, finded->toString(), atoi(BUFFER_SIZE)) <= 0) {
                printf("Error al responder el mensaje\n");
                close(frontendFd);
                frontendFd = -1;
                break;
            }
            printf("Encontrado en cache: \n%s\n",finded->toString().c_str());
        }
        else {

            if (connectionFd == -1 || FastSocket::sendmsg(connectionFd, sq.toString(), atoi(BUFFER_SIZE)) <=  0) {
                printf("Se rechazó el mensaje %s\n", sq.toString().c_str());
                close(connectionFd);
                connectionFd = -1;
                break;
            }

            std::string response;
            printf("Esperando respuesta...\n");
            if (FastSocket::recvmsg(connectionFd, response, atoi(BUFFER_SIZE)) <= 0) {
                printf("Error al recibir la respuesta\n");
                close(connectionFd);
                connectionFd = -1;
                break;
            }
            ResultsQuery rq = ResultsQuery::fromString(response);
            cache.insert(rq);
            rq.origin="cache";
            rq.destination="frontend";
            FastSocket::sendmsg(frontendFd, rq.toString(), atoi(BUFFER_SIZE));
            printf("Mensaje enviado: %s\n", rq.toString().c_str());

        }
    }
    
}

void reconnect(int& fd, const char* target) {
    printf("Conectando al %s\n", target);
    while (true) {
        if (fd != -1) {
            if (strcmp(target, "backend") == 0) {
                if (FastSocket::ping(fd, "?", "?", atoi(BUFFER_SIZE))) {
                    printf("Conexión establecida con el %s\n", target);
                    return;  // Conexión exitosa, salir de la función
                }
            } else {
                // Si el objetivo no es "backend", no se requiere ping, por lo que se considera conexión exitosa
                printf("Conexión establecida con el %s\n", target);
                return;
            }
        }
        close(fd);
        printf("Error con el %s, reconectando en 3 segundos...\n", target);
        sleep(3);
        fd = (strcmp(target, "backend") == 0) ?
            FastSocket::ClientSocket(atoi(BACKEND_PORT), BACKEND_ADDRESS) :
            FastSocket::ServerSocket(atoi(CACHE_PORT), 1);
    }
}

int main() {

    if (BUFFER_SIZE == nullptr || CACHE_PORT == nullptr || BACKEND_PORT == nullptr ||
    CACHE_SIZE == nullptr || BACKEND_ADDRESS == nullptr) {
        printf("Falta el puerto (C1 O C2) o la dirección en env\n");
        printf("CACHE_SIZE: %s | BUFFER_SIZE: %s | CACHE_PORT: %s | BACKEND_PORT: %s | BACKEND_ADDRESS: %s\n",
            CACHE_SIZE, BUFFER_SIZE, CACHE_PORT, BACKEND_PORT, BACKEND_ADDRESS);
        return EXIT_FAILURE;
    }

    Cache cache(atoi(CACHE_SIZE));

    int backendFd = FastSocket::ClientSocket(atoi(BACKEND_PORT), BACKEND_ADDRESS);
    int frontendFd = FastSocket::ServerSocket(atoi(CACHE_PORT), 1);
    while (true) {

        reconnect(backendFd, "backend");  
        reconnect(frontendFd, "frontend"); 
             
        printf("Se ha conectado un cliente!\n");

        // Lógica del cliente (manejo de mensajes) en la función 'event'
        event(frontendFd,backendFd, cache);
    }

    
    return EXIT_SUCCESS;
}
