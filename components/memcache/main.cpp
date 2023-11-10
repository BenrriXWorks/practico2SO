#include "include/FastSocket.h"
#include "include/SearchQuery.h"
#include "include/ResultsQuery.h"
#include "include/Cache.h"
#include <iostream>
#include <chrono>
#include "include/Functions.h"
using namespace std;

#define CACHE_PORT getenv("PORT_C1")
#define BACKEND_PORT getenv("PORT_C2")
#define BACKEND_ADDRESS getenv("BACKEND_ADDRESS")
#define BUFFER_SIZE getenv("BUFFER_SIZE")
#define CACHE_SIZE getenv("CACHE_SIZE")

void event(int& frontendFd, int& connectionFd, Cache& cache);
void reconnect(int& fd, const char* target);

int main() {

    // Revisar las variables de entorno
    if (BUFFER_SIZE == nullptr || CACHE_PORT == nullptr || BACKEND_PORT == nullptr ||
    CACHE_SIZE == nullptr || BACKEND_ADDRESS == nullptr) {
        printf("Falta el puerto (C1 O C2) o la dirección en env\n");
        printf("CACHE_SIZE: %s | BUFFER_SIZE: %s | CACHE_PORT: %s | BACKEND_PORT: %s | BACKEND_ADDRESS: %s\n",
            CACHE_SIZE, BUFFER_SIZE, CACHE_PORT, BACKEND_PORT, BACKEND_ADDRESS);
        return EXIT_FAILURE;
    }

    Cache cache(atoi(CACHE_SIZE)); // Crear la cache

    // Crear los File descriptors de los sockets (encapsulado en FastSocket)
    int backendFd = FastSocket::ClientSocket(atoi(BACKEND_PORT), BACKEND_ADDRESS);
    int frontendFd = FastSocket::ServerSocket(atoi(CACHE_PORT), 1);
    while (true) {
        reconnect(backendFd, "backend");  
        reconnect(frontendFd, "frontend"); 
        printf("Se ha conectado un cliente!\n");
        event(frontendFd, backendFd, cache);
    }
    // Considerar que el ciclo de arriba nunca terminara
    return EXIT_SUCCESS;
}

// Maneja los eventos segun las entradas que reciba
void event(int& frontendFd, int& connectionFd, Cache& cache) {
    while (true) {
        std::string query;
        int recvResult = FastSocket::recvmsg(frontendFd, query, atoi(BUFFER_SIZE));

        // Si el cliente se desconecto o no se recibio el mensaje correctamente
        if (recvResult == -1 || !recvResult) {
            frontendFd = -1;
            break;
        }
        // Si es un ping retornar '?'
        if (query == string("?")) {
            if (FastSocket::sendmsg(frontendFd, "?", atoi(BUFFER_SIZE)) <= 0) {
                frontendFd = -1;
                break;
            }
            printf("ping recibido de fd %d\n", frontendFd);
            continue;
        }
        // Ahora que el mensaje esta recibido
        SearchQuery sq = SearchQuery::fromString(query);
        auto startTime = chrono::high_resolution_clock::now();

        // Buscar en la cache
        ResultsQuery* finded;
        if ((finded = cache.find(sq.queryToString())) != nullptr) {
            auto endTime = chrono::high_resolution_clock::now();
            finded->origin = "cache";
            finded->destination = "frontend";
            finded->ori = "CACHE";
            finded->tiempo = to_string(chrono::duration_cast<chrono::nanoseconds>(endTime - startTime).count());
            
            if (FastSocket::sendmsg(frontendFd, finded->toString(), atoi(BUFFER_SIZE)) <= 0) {
                frontendFd = -1;
                break;
            }
            printf("Encontrado en cache:\n%s\n\n", finded->toString().c_str());
            continue; // Volver a esperar una solicitud
        }

        // Preguntar al backend si no esta en la cache
        if (connectionFd == -1 || FastSocket::sendmsg(connectionFd, sq.toString(), atoi(BUFFER_SIZE)) <=  0) {
            printf("Se rechazó el mensaje:\n%s\n\n", sq.toString().c_str());
            close(connectionFd);
            connectionFd = -1;
            break;
        }
        // Esperar la respuesta del backend
        std::string response;
        printf("Esperando respuesta...\n");
        if (FastSocket::recvmsg(connectionFd, response, atoi(BUFFER_SIZE)) <= 0) {
            printf("Error al recibir la respuesta\n\n");
            close(connectionFd);
            connectionFd = -1;
            break;
        }
        // Enviar la respuesta al frontend y guardarla en la cache.
        ResultsQuery rq = ResultsQuery::fromString(response);
        rq.origin="cache";
        rq.destination="frontend";
        FastSocket::sendmsg(frontendFd, rq.toString(), atoi(BUFFER_SIZE));
        cache.insert(rq);
        printf("Busqueda backend: \n%s\n\n", rq.toString().c_str());
    
    }
    
}

// Hace un ping para revisar la conexion, sino vuelve a abrir el socket iterativamente.
void reconnect(int& fd, const char* target) {
    printf("Conectando al %s\n", target);
    while (true) {
        if (fd != -1) {
            if (strcmp(target, "backend") == 0) {
                // Si el objetivo es backend hacer ping para comprobar el estado
                string res;
                if (FastSocket::ping(fd,"?","?",atoi(BUFFER_SIZE))){
                        printf("Conexión establecida con el %s\n", target);
                        return;
                }
            } else {
                // Si el objetivo no es "backend" no hacer ping
                printf("Conexión establecida con el %s\n", target);
                return;
            }
        }
        close(fd);
        printf("Error con el %s, reconectando en 3 segundos...\n", target);
        sleep(3);
        // Volver a crear el file descriptor
        fd = (strcmp(target, "backend") == 0) ?
            FastSocket::ClientSocket(atoi(BACKEND_PORT), BACKEND_ADDRESS) :
            FastSocket::ServerSocket(atoi(CACHE_PORT), 1);
    }
}