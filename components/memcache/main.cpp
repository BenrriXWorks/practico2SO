#include "include/FastSocket.h"

int main(){
    if (nullptr == getenv("BUFFER_SIZE")){
        printf("Falta la variable de entorno BUFFER_SIZE");
        return EXIT_FAILURE;
    }
    printf("BufferSize = %s\n",getenv("BUFFER_SIZE"));
    
    return EXIT_SUCCESS;
}