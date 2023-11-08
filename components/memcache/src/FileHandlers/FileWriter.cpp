#include "../../include/FileWriter.h"


bool FileWriter::writeLine(const std::string line) {
    if (!fs.is_open()) {
        std::cerr << "FileWriter: File is not open." << std::endl;
        return false;
    }
    // Convierte la línea de UTF-8 a ancho completo (UTF-16)
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::wstring utf16Line = converter.from_bytes(line);

    // Escribir la línea seguida de un salto de línea
    fs << utf16Line << L'\n';
    return true;
}

bool FileWriter::writeLines(const std::vector<std::string> lines) {
    if (!fs.is_open()) {
        std::cerr << "FileWriter: File is not open." << std::endl;
        return false;
    }

    // Escribir cada línea seguida de un salto de línea
    for (const std::string& line : lines) {
        fs << std::wstring(line.begin(),line.end()) << L'\n';
    }
    return true;
}

bool FileWriter::write(const std::string text) {
    if (!fs.is_open()) {
        std::cerr << "FileWriter: File is not open." << std::endl;
        return false;
    }

    // Convierte el texto de UTF-8 a ancho completo (UTF-16) y escríbelo
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::wstring utf16Text = converter.from_bytes(text);
    fs << utf16Text;
    return true;
}

bool FileWriter::open(const std::string path) {
    size_t pos;
    // Encuentra la posición de la última barra diagonal o barra inclinada en la ruta
    if ((pos = path.find_last_of("\\/")) != std::string::npos) {
        // Extrae el directorio de la ruta del archivo
        std::string directory = path.substr(0, pos + 1);
        // Verifica si el directorio existe
        if (!checkDirectory(directory)) {
            // Si no existe, crea el directorio
            try {
                std::filesystem::create_directory(directory);
                std::cerr << "Creating directory: " << directory << std::endl;
            }catch(...){
                std::cerr << "FileWriter: No se pudo crear el directorio" << std::endl;
                return false;
            }
        }
        fullRoute = path;
    }

    // Luego, abre el archivo
    fs.open(path.c_str(), std::ios::out | std::ios::binary);
    if (!fs.is_open()) {
        std::cerr << "FileWriter: Failed to open the file." << std::endl;
        return false;
    }
    fs.imbue(std::locale(fs.getloc(), new std::codecvt_utf8<wchar_t>));
    fullRoute = path;
    return true;

}

bool FileWriter::openAppend(const std::string path) {
    size_t pos;
    // Encuentra la posición de la última barra diagonal o barra inclinada en la ruta
    if ((pos = path.find_first_of("\\/")) != std::string::npos) {
        // Extrae el directorio de la ruta del archivo
        std::string directory = path.substr(0, pos + 1);
        // Verifica si el directorio existe
        if (!checkDirectory(directory)) {
            try {
                std::filesystem::create_directory(directory);
                std::cerr << "Creating directory: " << directory << std::endl;
            }catch(...){
                std::cerr << "FileWriter: No se pudo crear el directorio" << std::endl;
                return false;
            }
        }
    }

    // Abrir el archivo
    fs.open(path.c_str(), std::ios::out | std::ios::binary | std::ios::app);
    if (!fs.is_open()) {
        std::cerr << "FileWriter: Failed to open the file." << std::endl;
        return false;
    }
    fullRoute = path;
    return true;
}

bool FileWriter::close() {
    if (fs.is_open()) {
        fs.close();
        return true;
    }
    return false;
}

FileWriter& FileWriter::operator<<(const std::string& str) {
    write(str);
    // Devolvemos la referencia al objeto actual
    return *this;
}
