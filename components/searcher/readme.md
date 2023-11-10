# Buscador
Este programa tiene la función de interactuar con el usuario, recibiendo sus consultas de palabras para buscar. Su principal responsabilidad consiste en delegar la tarea de búsqueda de consultas a un caché, actuando como el componente frontend en este proceso.

El programa enviara a caché un mensaje con el siguiente formato:

query           (la palabra o frase a buscar)
origin          (desde donde viene el mensaje, en este caso frontend)
destination     (el destino del mensaje, en este caso caché)

**Ejemplo:**

hola
frontend
cache

### Variables de Entorno Necesarias:

- `PORT_C1`: Puerto de conexión con el caché para engiar solicitudes.
- `FRONTEND_ADDRESS`: Dirección del frontend.
- `BUFFER_SIZE`: Tamaño del buffer de los mensajes.

# Compilacion
para compilar ejecute el archivo make 
bash: $ make

# Uso
Se llama a la funcion main teniendo las variables de entorno cargadas
bash $ ./main
bash 2$ PORT_C1=1032 FRONTEND_ADDRESS=127.0.0.1 BUFFER_SIZE=512 ./main


