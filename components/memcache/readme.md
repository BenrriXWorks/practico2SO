# Cache de buscador

## Descripcion:
Esta aplicacion funciona como una memoria para que al recibir peticiones se puedan responder
directamente en lugar de hacer toda la peticion hacia el backend. Almacena peticiones de search
y devuelve el resultado si esta guardado, sino busca en el backend, lo devuelve y lo almacena, reemplazando
con un algoritmo LRU.
El orden esperado de carga seria iniciar backend->cache->searcher.

## Variables de entorno:
    * PORT_C1: (CACHE_PORT) Puerto de conexion del cache para recibir peticiones
    * PORT_C2: (BACKEND_PORT) Puerto de conexion con el backend para enviar peticiones
    * BACKEND_ADDRESS: Direccion del backend para enviar peticiones
    * BUFFER_SIZE: Tamano del buffer de los mensajes
    * CACHE_SIZE: Tamano maximo de la lista cache 
+ Las variables de entorno deben estar cargadas. El programa no se encarga de cargar un archivo .env

## Estructuras de mensajes de los sockets:
--------------------------------------
+ SearchQuery (Peticiones de busqueda, ignorar tabulacion):
    palabra1 palabra2 ... palabraN
    origen
    destino
- Ej:
    hola
    frontend
    cache

+ ResultsQuery (Respuestas de peticiones, ignorar tabulacion):
    palabras
    origen
    destino
    tiempoBusquedaNanosegundos
    lugarDeBusquedaRealizada
    (1/0 segun si se encontro resultados o no, respectivamente)
    archivo1,puntaje1 archivo2,puntaje2 archivo3,puntaje3
- Ej:
    hola
    cache
    frontend
    558936
    BACKEND
    1
    file021.txt,23 file005.txt,9 file022.txt,7 file001.txt,6


## Compilacion:
Basta con ejecutar el archivo make, ej bash: $ make

## Uso:
Solo se llama al main teniendo las variables cargadas:
- Ej bash 1: $ ./main
- Ej bash 2: $ BUFFER_SIZE=512 FRONTEND_ADDRESS=127.0.0.1 BACKEND_ADDRESS=127.0.0.1 PORT_C1=1032 PORT_C2=1033 CACHE_SIZE=4 ./main
