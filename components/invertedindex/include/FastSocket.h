// Ver 1.3
#include <string>
#include <sys/socket.h>
#include <stdexcept>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

class FastSocket
{

public:
    // Devuelve el file descriptor de la conexion entre un socket cliente y un servidor.
    static int ClientSocket(int port, const char* serverAddress) noexcept;

    // Inicia un socket en modo servidor y lo devuelve
    static int ServerSocket(int port, int maxConnections) noexcept;

    // Espera un mensaje y lo guarda en response, si hay un error cierra el socket
    static int recvmsg(int socketFd, std::string& response, int bufferSize) noexcept;

    // Envia un string por un socket, si hay un error cierra el socket
    static int sendmsg(int socketFd, std::string message, int bufferSize) noexcept;
    static int sendmsg(int socketFd, const char* message, int bufferSize) noexcept;

    // Envia un mensaje y espera una respuesta
    static bool ping(int socketFd, const char* message, const char* expectedResponse, int buffsize) noexcept;

};