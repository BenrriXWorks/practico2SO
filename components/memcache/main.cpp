#include "include/FastSocket.h"

#define portFront getenv("PORT_C1")
#define portBack getenv("PORT_C2")
#define addr getenv("ADDRESS")
#define buffsize getenv("BUFFER_SIZE")

int main(){
    if (nullptr == getenv("BUFFER_SIZE")){
        printf("Falta la variable de entorno BUFFER_SIZE");
        return EXIT_FAILURE;
    }
    if (portFront == nullptr || portBack == nullptr || addr == nullptr){
        printf("Falta el puerto (C1 O C2) o la direccion en env\n");
        printf("portFront: %d | portBack: %d | addr: %d\n",portFront==nullptr,portBack==nullptr,addr==nullptr);
        return EXIT_FAILURE;
    }
    printf("Conectando al backend\n");
    int databaseFd;
    while ((databaseFd = FastSocket::ClientSocket(atoi(portBack), addr)) == -1){
        printf("Conexion fallida, reintentando en 5 segundos\n");
        sleep(5);
    }
    printf("Conexion exitosa!");
    printf("Esperando cliente\n");
    int frontendFd = FastSocket::ServerSocket(atoi(portFront), -1);
    printf("Se ha conectado con un cliente\n");
    std::string query;
    while (true){
        if (FastSocket::recvmsg(frontendFd,query,atoi(buffsize)) == -1){
            printf("Error al recibir el mensaje\n");
            continue;
        }
        printf("Solicitud: %s\n",query.c_str());
        if (FastSocket::sendmsg(frontendFd,"Mensaje recibido!\n", atoi(buffsize))==-1)
            printf("Error al responder el mensaje\n");
    }

    close(frontendFd);
    close(databaseFd);

    return EXIT_SUCCESS;
}