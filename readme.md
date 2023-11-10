# Buscador de indice invertido en capas por socket

## Descripcion:
Los siguientes programas conforman una red de busqueda de la siguiente manera:
invertedindex: Permite cargar un archivo de indice invertido y hacer
busquedas sobre el, se comunica por sockets como un servidor.
memcache: Permite almacenar resultados para poder desplegarlos sin tener
que realizar la busqueda en el backend, ahorrando recursos.
searcher: Cliente que permite enviar peticiones de busqueda y mostrar
sus resultados.
En esta carpeta se ofrecen lanzadores que cargan las variables de entorno
necesarias automaticamente y lanzan los procesos por separado, siendo un
nexo para facilitar el uso de la aplicacion pero sin ser necesarias.

## Variables de entorno (en .env):
    INDEX_ROUTE: Ruta del archivo index
    PORT_C1: Puerto para alojar el cache
    PORT_C2: Puerto para alojar el backend
    BUFFER_SIZE: Tamano maximo del buffer de comunicacion
    FRONTEND_ADDRESS: Direccion IP (cache) para conectar cache-frontend
    BACKEND_ADDRESS: Direccion IP (backend) para conectar cache-backend
    TOP_K: Cantidad de archivos a desplegar de una busqueda
    CACHE_SIZE: Tamano maximo del cache
+ Las variables de entorno se almacenan en el .env y se cargan automaticamente
al usar cualquier archivo ./launch{}.sh

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
Los programas dentro de Components deben compilarse por separado, cada
uno cuenta con un make. Las instrucciones de cada uno estan en su propio readme.md

## Uso:
Se pueden cargar los programas por separado indicandole previamente sus
variables de entorno, o usando un ejecutable ./launch{}.sh.
+ Para ejecutar todas las capas:
Bash: $ ./launchFull.sh
+ Para ejecutar solo backend y cache:
Bash: $ ./launchServer.sh
+ Para ejecutar solo cliente (frontend/searcher):
Bash: $ ./launchClient.sh