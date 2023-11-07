#include "../../include/FileStack.h"

FileReader* FileStack::next(){

    if (cola.empty())
        throw std::runtime_error("La cola esta vacia");
    FileReader *file = cola.at(--size);
    return file;
};


FileStack::FileStack(std::vector<std::string> listOfPaths, std::string extension){
    try{
        size = listOfPaths.size();
        for (std::string s : listOfPaths){
            // Se separa el archivo encontrado de la extension
            size_t dotPos = s.find_last_of('.');
            if (dotPos == std::string::npos) // Si no tiene extension
                printf("El archivo {%s} no tiene una extension valida, saltando\n",s.c_str()),
                --size;
            else if (extension != "*" && s.substr(dotPos+1) != extension) // Si no es de la extension
                --size;
            else {
                FileReader *fr = new FileReader();
                if (!fr->open(s)) // Si el archivo es inaccessible
                    printf("El archivo en {%s} no ha podido ser leido. Saltando.\n",s.c_str()),
                    --size;
                else
                    cola.push_back(fr);   
            }           
        }
    }
    catch(...){
        printf("FileStack: Error de inicializacion\n");
        exit(EXIT_FAILURE);
    }
    
};

FileStack FileStack::fromDirectory(std::string directory, std::string extension){
    return FileStack(filesInDirectory(directory),extension);
}

std::vector<std::string> FileStack::filesInDirectory(const std::filesystem::path& directorio) {
    std::vector<std::string> archivos;
    // Si el path es válido, hacer push back de los archivos
    if (std::filesystem::is_directory(directorio)) {
        for (const auto& archivo : std::filesystem::directory_iterator(directorio)) {
            if (archivo.is_regular_file()) {
                archivos.push_back(archivo.path().string());
            }
        }
    }
    return archivos;
}