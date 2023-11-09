// Ver 1.3
#include "../../include/FastSocket.h"

int FastSocket::ClientSocket(int port, const char* serverAddress) noexcept {
    // Crear el socket (fileDescription)
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        perror("Error al iniciar el socket");
        return -1;
    }

    // Establecer el tiempo de espera para el 'connect', recv y send
    struct timeval timeout;
    timeout.tv_sec = 3;
    timeout.tv_usec = 0;

    if (setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) == -1 ||
        setsockopt(clientSocket, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) == -1) {
        perror("Error al establecer el tiempo de espera para connect, recv y send");
        close(clientSocket);
        return -1;
    }


    // Establecer la direccion de conexion
    struct sockaddr_in connectionAddress;
    connectionAddress.sin_family = AF_INET;
    connectionAddress.sin_port = htons(port);
    if (serverAddress == nullptr) {
        printf("No se estableció una dirección para la conexión\n");
        close(clientSocket); // Cerrar el socket si no se especifica una dirección
        return -1;
    }
    connectionAddress.sin_addr.s_addr = inet_addr(serverAddress);

    // Configurar el socket para reutilizar la dirección IP y el puerto
    int opt = 1;
    if (setsockopt(clientSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) == -1 ||
        setsockopt(clientSocket, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(int)) == -1) {
        perror("Error al configurar SO_REUSEADDR o SO_REUSEPORT");
        close(clientSocket);
        return -1;
    }

    printf("Conectando el fd con addr\n");
    int connectionResponse = connect(clientSocket, (sockaddr*)&connectionAddress, sizeof(connectionAddress));
    if (connectionResponse == -1) {
        perror("No se pudo conectar el fd con la dirección");
        close(clientSocket);
        return -1;
    }

    // Configurar el tiempo de espera para 'send'
    struct timeval sendTimeout;
    sendTimeout.tv_sec = 3;
    sendTimeout.tv_usec = 0;

    if (setsockopt(clientSocket, SOL_SOCKET, SO_SNDTIMEO, &sendTimeout, sizeof(sendTimeout)) == -1) {
        perror("Error al establecer el tiempo de espera para send");
        close(clientSocket);
        return -1;
    }

    return clientSocket;
}

// Espera un mensaje y lo guarda en response, cierra el socket si hubo un error
int FastSocket::recvmsg(int socketFd, std::string& response, int bufferSize) noexcept {
    const char* buffer = new char [bufferSize];
    int recvCode = recv(socketFd, (void*)buffer, bufferSize, 0);
    response = buffer;
    if (recvCode <= -1) close(socketFd);
    return recvCode;
}

// Envia un string por un socket, cierra el socket si hubo un error
int FastSocket::sendmsg(int socketFd, std::string message, int bufferSize) noexcept {
    char* buffer = new char[bufferSize];
    strcpy(buffer, message.substr(0,bufferSize-1).c_str());
    int status = send(socketFd, buffer, bufferSize, 0);
    if (status == -1) close(socketFd);
    return status;
}

// Envia un string por un socket, cierra el socket si hubo un error
int FastSocket::sendmsg(int socketFd, const char* message, int bufferSize) noexcept {
    int status = send(socketFd, message, bufferSize, 0);
    if (status == -1) close(socketFd);
    return status;
}


// Inicia un socket en modo servidor y lo devuelve
int FastSocket::ServerSocket(int port, int maxConnections) noexcept {
    
    // Iniciar socket del dominio (TCP, IPv4)
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) return (printf("No se pudo crear el socket servidor\n"),-1);

    // Preparar por si se quiere reusar el socket
    int opt = 1, opt2 = 2;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) + 
    setsockopt(serverSocket,SOL_SOCKET, SO_REUSEPORT, &opt2, sizeof(opt)) < 0){
        printf("Error con setsockopt al preparar el reuso del puerto\n");
        return -1;
    }

    // Definir direccion del socket 
    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port); // PUERTO
    serverAddress.sin_addr.s_addr = INADDR_ANY; // Acepta cualquier tipo de mensaje local

    // Vincular el socket a la direccion y puerto especificado
    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1)
        return (printf("No se pudo vincular el fd al puerto destino\n"),-1);
    
    // Esperar conexiones
    printf("Esperando conexiones\n");
    if (listen(serverSocket, maxConnections) == -1) return(close(serverSocket),printf("No se pudo escuchar al socket\n"),-1);
    int connectionSocket = accept(serverSocket, NULL, NULL);

    return connectionSocket;
}

bool FastSocket::ping(int socketFd, const char* message, const char* expectedResponse, int buffsize) noexcept {

    // Enviar el ping message
    if (sendmsg(socketFd, message, 1) <= 0) return false;
    // Esperar la respuesta
    std::string response;
    if (recvmsg(socketFd, response, buffsize) <= 0 || response != expectedResponse) return false;
    return true; // Éxito
}