#include "include/FastSocket.h"
#include <array>

#define portFront getenv("PORT_C1")
#define addr getenv("ADDRESS")
#define buffsize getenv("BUFFER_SIZE")

int main(){

    int connectionFd = FastSocket::ClientSocket(atoi(portFront),addr);

    std::array<const char*, 5> genericQueries = {
        "hola+perro+casa+techo",
        "test+busqueda",
        "hola+gato",
        "busqueda+generica",
        "si+perro"
    };
    while (true){
        std::string msg = genericQueries[rand()%5];
        int sent = FastSocket::sendmsg(connectionFd,msg, atoi(buffsize));
        if (sent == -1) {
            printf("Se rechazo el mensaje %s\n",msg.c_str());
            continue;
        }

        std::string response;
        FastSocket::recvmsg(connectionFd, response, atoi(buffsize));
        printf("Response: %s\n",response.c_str());
        sleep(2);
    }
    return 0;
}