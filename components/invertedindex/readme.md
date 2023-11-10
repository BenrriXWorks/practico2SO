# Buscador de indice invertido

Este programa se encarga de leer y mantener un índice invertido, funcionando como un buscador. El índice invertido tiene el siguiente formato:

palabra:(archivo,cantidad);(archivo,cantidad)...

**Ejemplo:**

observarlos:(file022.txt,3);(file009.txt,2)

El índice invertido opera como un servidor de socket y espera recibir un mensaje con el siguiente formato:

query           (la palabra o frase a buscar)
origin          (desde donde viene el mensaje)
destination     (el destino del mensaje, en este caso invertedIndex)

**Ejemplo:**

hola
cache
invertedIndex

El programa responderá con un mensaje en el siguiente formato:

query           (palabra o frase a buscar)
origin          (desde donde ira en mensaje, en este caso invertedIndex)
destination     (a donde llega)
tiempo          (tiempo que se demora en nanosegundos)
origin          (el programa que lo encontro, en este caso BACKEND)
isFound         (booleano de si fue encontrado el archivo)
archivo1,puntaje1 archivo2,puntaje2.. (los archivos donde se encuentra y los puntajes)

**Ejemplo:**

hola
invertedIndex
cache
57960
BACKEND
1
file021.txt,23 file005.txt,9 file022.txt,7 file001.txt,6 

### Variables de Entorno Necesarias:

- `PORT_C2`: Puerto de conexión para recibir peticiones.
- `BACKEND_ADDRESS`: Dirección del backend .
- `INDEX_ROUTE`: Ruta del archivo de índice invertido.
- `TOP_K`: Cantidad máxima de textos que se deben encontrar.
- `BUFFER_SIZE`: Tamaño del buffer de los mensajes.


# Compilacion
para compilar ejecute el archivo make 
bash: $ make

# Uso
Se llama a la funcion main teniendo las variables de entorno cargadas
bash $ ./main
bash 2$ PORT_C2=1033 BACKEND_ADDRESS=127.0.0.1 INDEX_ROUTE=index.idx TOP_K=4 
        BUFFER_SIZE=512 ./main