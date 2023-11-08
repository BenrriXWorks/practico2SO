#!/bin/bash

# Obtiene el directorio actual
current_dir="$PWD"

# Carga las variables de entorno desde el archivo .env en el directorio actual
if [ -f "$current_dir/.env" ]; then
  export $(cat "$current_dir/.env" | xargs)
fi
# Inicia cada componente en una ventana nueva
gnome-terminal -- bash -c "cd $current_dir/components/searcher; ./main; exec bash"
