#include <string>
#include <thread>
#include "include/FastSocket.h"
#include <iostream>

#define PORT 8080
#define SERVER_ADDRESS "127.0.0.1"

using namespace std;

int main(){

    // Crear el socket
    int client_socket = FastSocket::ClientSocket(PORT,SERVER_ADDRESS);
    if (client_socket == -1) return (printf("No se pudo iniciar el socket\n"), EXIT_FAILURE);

    // Enviar el mensaje
    printf("Conexion establecida!\n");
    printf("Que mensaje desea enviar: ");
    string input; getline(cin, input);
    FastSocket::sendmsg(client_socket, input, 1024);
    string response = ""; FastSocket::recvmsg(client_socket, response, 1024);

    printf("Respuesta: %s\n", response.c_str());

    close(client_socket);

    return EXIT_SUCCESS;
}
