#include "FileReader.h"
#include "vector"

#pragma once

class FileStack
{
private:
    std::vector<FileReader*> cola;
    size_t size=0;
public:
    FileReader* next(); // Se pasa el siguiente reader abierto.
    // Constructor a partir de un vector de rutas de archivos y filtrado por una extension (* = wildcard)
    FileStack(std::vector<std::string> listOfPaths, std::string extension);
    // Constructor estatico a partir de una ruta donde hay archivos y filtrado por una extension (* = wildcard)
    static FileStack fromDirectory(std::string directory, std::string extension);
    // True si no quedan elementos encolados.
    inline bool empty() {return size==0;};
    // Devuelve si ya se cerro el ultimo elemento de la cola
    inline bool lastFileClosed() {return !cola.front()->isOpen();};
    // Devuelve un vector con los archivos en un directorio
    static std::vector<std::string> filesInDirectory(const std::filesystem::path& directorio);
    // Cuantos archivos quedan encolados:
    inline size_t getSize(){return size;}
};