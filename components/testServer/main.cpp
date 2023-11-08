#include <cstdio>
#include <string>
#include <thread>
#include "include/FastSocket.h"

using namespace std;

#define PORT 8080
#define BUFFSIZE 1024

int main(){

    int fd = FastSocket::ServerSocket(PORT,-1);

    // Recibir mensaje
    string rec = "";
    FastSocket::recvmsg(fd, rec, BUFFSIZE);
    printf("Mensaje recibido: %s\n",rec.c_str());

    // Eviar mensaje
    FastSocket::sendmsg(fd, string("Hallo!"), BUFFSIZE);

    // Terminar la conexion (igual se hara automaticamente al finalizar el programa)
    close(fd);

    return 0;
}