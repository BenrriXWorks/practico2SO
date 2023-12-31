#include "include/FastSocket.h"
#include "include/Functions.h"
#include "include/SearchQuery.h"
#include "include/ResultsQuery.h"
#include <array>
#include <unistd.h>
#include <chrono>
#include <numeric>
#include <iostream>
#include <execution>
#include <set>

#define TOTAL_ENV 3
#define PORT_FRONTEND_CACHE getenv("PORT_C1")
#define SERVER_ADDRESS getenv("FRONTEND_ADDRESS")
#define BUFFER_SIZE getenv("BUFFER_SIZE")

using namespace std;


bool checkenv();
inline void countdown(int n);
void menu(const int& fileDescriptor, const int& buffsize);
void menuContinuar(int fd);

int main() {

    if (!checkenv()) return(printf("Error al cargar entorno\n"),EXIT_FAILURE);

    int port = atoi(PORT_FRONTEND_CACHE);
    int buffsize = atoi(BUFFER_SIZE);
    // Ciclo de intentar conectarse
    while (true){
        clearWindow();
        printf("Intentando conectar con el servidor...\n");
        int fd = FastSocket::ClientSocket(port, SERVER_ADDRESS); // Establecer conexion
        if (fd != -1 && FastSocket::ping(fd, "?", "?", buffsize) == true) {
            printf("Conexion exitosa!\n");
            menu(fd,buffsize);
            printf("Volviendo a intentar la conexion\n");
        };
        close(fd); // Cerrar el fd
        printf("Reintentando en: \n"), fflush(stdout), countdown(5);
    }
    return 0;
}

bool checkenv(){
    array<char*,TOTAL_ENV> env = {
        PORT_FRONTEND_CACHE,
        SERVER_ADDRESS,
        BUFFER_SIZE
    };

    if (any_of(env.begin(), env.end(), [](char* a) { return (a == nullptr); })) {
        printf("Falta alguna variable de entorno, las necesarias son:\nPORT_C1, ADDRESS, BUFFER_SIZE\n");
        return false;
    }
    try{
        return atoi(BUFFER_SIZE) + atoi(PORT_FRONTEND_CACHE);
    }
    catch(...) {
        return false;
    }
}

inline void countdown(int n){
    for (int i = 0; i <= n; i++) printf("\r%s>%s", string(i,'=').c_str(),string(n-i,'-').c_str()), fflush(stdout), sleep(1);
}

void menu(const int& fileDescriptor, const int& buffsize){
    
    clearWindow();
    {
        printf("BUSCADOR BASADO EN INDICE INVERTIDO\n");
        string msg;
        printf("Bienvenido, que desea buscar?: "), getline(cin, msg);
        multiset<string> queryWords;
        for (const string& word : split(msg,' ')) queryWords.emplace(word);
        SearchQuery query(queryWords,"frontend","cache");
        printf("Query:\n%s\n", query.toString().c_str());

        // Enviar y recibir los query
        auto clockInit = chrono::high_resolution_clock::now();
        if (FastSocket::sendmsg(fileDescriptor, query.toString(), buffsize) == -1)
            return (void)printf("No se pudo enviar el mensaje\n");
        if (FastSocket::recvmsg(fileDescriptor, msg, buffsize) <= 0)
            return (void)printf("No se pudo recibir el mensaje\n");
        auto clockEnd = chrono::high_resolution_clock::now();

        // Decodificar el query
        ResultsQuery rq = ResultsQuery::fromString(msg);

        auto timeFindAndSort = chrono::duration_cast<chrono::nanoseconds>(clockEnd - clockInit).count();
        string tiempoStr = std::to_string(timeFindAndSort);

        // Imprimir los resultados
        printf("\nRespuesta:(tiempo = %sns, origen = %s, tiempo total = %sns):\n",rq.tiempo.c_str(),rq.ori.c_str(),tiempoStr.c_str());
        int count = 0;
        if (rq.isFound == 1)
            for (const auto &pair:rq.results)
                printf("%i) %s,%i\n",++count,pair.second.c_str(),pair.first);
        else printf("No se encontraron resultados :(\n");

        menuContinuar(fileDescriptor);
    }
    menu(fileDescriptor, buffsize);
}

void menuContinuar(int fd){
    string msg;
    printf("\nQuieres buscar otra cosa? (s/n): "), getline(cin, msg);
    if (msg == "n") close(fd),clearWindow(),printf("Adios!\n"),exit(EXIT_SUCCESS);
    if (msg != "s") menuContinuar(fd); 
}
