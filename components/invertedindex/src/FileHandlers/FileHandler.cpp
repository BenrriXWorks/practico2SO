#include "../../include/FileHandler.h"

bool FileHandler::checkDirectory(std::string path){
    
    if (path.empty()) return true;

    // Verifica si el directorio existe
    if (std::filesystem::exists(path) && std::filesystem::is_directory(path)) {
        // La ruta especificada es un directorio existente
        return true;
    } else {
        // La ruta no es un directorio existente
        std::cerr << "FileHandler: Directory " << path.c_str() << " does not exist." << std::endl;
        return false; 
    }

}

bool FileHandler::checkFile(std::string pathAndFile) {
    // Revisa si el archivo es valido
    size_t pos;
    if ((pos = pathAndFile.find_last_of("\\/")) != std::string::npos)
        return checkDirectory(pathAndFile.substr(0,pos)) && std::ifstream(pathAndFile).good();
    else
        return std::ifstream(pathAndFile).good();
}

