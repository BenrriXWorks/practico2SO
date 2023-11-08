// Ver 1.1
#include <string>
#include <sys/socket.h>
#include <stdexcept>
#include <arpa/inet.h>
#include <queue>
#include <unistd.h>
#include <cstring>

class FastSocket
{
private:
    static std::queue<int> openedSockets;

public:
    // Devuelve el file descriptor de la conexion entre un socket cliente y un servidor.
    static int ClientSocket(int port, const char* serverAddress) noexcept;

    // Inicia un socket en modo servidor y lo devuelve
    static int ServerSocket(int port, int maxConnections) noexcept;

    // Espera un mensaje y lo guarda en response
    static int recvmsg(int socketFd, std::string& response, int bufferSize) noexcept;

    // Envia un string por un socket
    static int sendmsg(int socketFd, std::string message, int bufferSize) noexcept;
    static int sendmsg(int socketFd, const char* message, int bufferSize) noexcept;

    ~FastSocket();

};