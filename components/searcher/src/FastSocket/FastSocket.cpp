// Ver 1.0
#include "../../include/FastSocket.h"

std::queue<int> FastSocket::openedSockets = std::queue<int>();

// Devuelve el file descriptor de la conexion entre un socket cliente y un servidor.
int FastSocket::ClientSocket(int port, const char* serverAddress) noexcept {
    // Crear el socket (fileDescription)
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) return (printf("No se pudo iniciar el socket\n"), -1);

    // Establecer la direccion de conexion
    struct sockaddr_in connectionAddress;
    connectionAddress.sin_family = AF_INET;
    connectionAddress.sin_port = htons(port);
    if (serverAddress == nullptr) return (printf("No se estableció una dirección para la conexión\n"),-1);
    connectionAddress.sin_addr.s_addr = inet_addr(serverAddress);

    if (connect(clientSocket, (sockaddr*)&connectionAddress, sizeof(connectionAddress)) == -1)
        return (printf("No se pudo conectar el fd con la direccion\n"), -1);
    
    openedSockets.push(clientSocket);
    return clientSocket;
}

// Espera un mensaje y lo guarda en response
int FastSocket::recvmsg(int socketFd, std::string& response, int bufferSize) noexcept {
    const char* buffer = new char [bufferSize];
    int recvCode = recv(socketFd, (void*)buffer, bufferSize, 0);
    response = buffer;
    return recvCode;
}

// Envia un string por un socket
int FastSocket::sendmsg(int socketFd, std::string message, int bufferSize) noexcept {
    char* buffer = new char[bufferSize];
    strcpy(buffer, message.substr(0,bufferSize-1).c_str());
    return send(socketFd, buffer, bufferSize, 0);
}

// Envia un string por un socket
int FastSocket::sendmsg(int socketFd, const char* message, int bufferSize) noexcept {
    return send(socketFd, message, bufferSize, 0);
}


// Inicia un socket en modo servidor y lo devuelve
int FastSocket::ServerSocket(int port, int maxConnections) noexcept {
    
    // Iniciar socket del dominio (TCP, IPv4)
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) return (printf("No se pudo crear el socket servidor\n"),-1);

    // Definir direccion del socket 
    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port); // PUERTO
    serverAddress.sin_addr.s_addr = INADDR_ANY; // Acepta cualquier tipo de mensaje local

    // Vincular el socket a la direccion y puerto especificado
    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1)
        return (printf("No se pudo vincular el fd al puerto destino\n"),-1);
    
    openedSockets.push(serverSocket); // Guardar socket para cerrarlo luego
    // Esperar conexiones
    printf("Esperando conexiones\n");
    if (listen(serverSocket, maxConnections) == -1) return(printf("No se pudo escuchar al socket\n"),-1);
    int connectionSocket = accept(serverSocket, NULL, NULL);

    openedSockets.push(connectionSocket);
    return connectionSocket;
}


FastSocket::~FastSocket(){
    while (!openedSockets.empty()){
        close(openedSockets.front());
        openedSockets.pop();
    }
}