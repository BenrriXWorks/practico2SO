// FileReader.cpp
#include "../../include/FileReader.h"


std::string FileReader::readLine() {
    std::wstring utf16Line;
    std::getline(fs, utf16Line);

    // Convierte la línea de ancho completo a UTF-8
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::string utf8Line = converter.to_bytes(utf16Line);

    return utf8Line;
}

std::vector<std::string> FileReader::readLines() {
    std::vector<std::string> vec;
    while (!fs.eof()) 
        vec.push_back(readLine());
    close();
    return vec;
}

std::string FileReader::read() {
    std::wstring utf16Data;
    wchar_t wch;
    
    while (fs.get(wch)) {
        utf16Data.push_back(wch);
    }

    // Elimina espacios en blanco adicionales al final de utf16Data
    while (!utf16Data.empty() && std::iswspace(utf16Data.back())) {
        utf16Data.pop_back();
    }

    // Convierte el búfer de ancho completo a una cadena UTF-8
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::string result = converter.to_bytes(utf16Data);
    close();

    return result;
}



bool FileReader::open(std::string path) {
    if (!checkFile(path))
        return false;

    // Establece la localización del archivo para manejar UTF-8
    fs.open(path, std::ios::in);
    if (fs.is_open()) {
        fs.imbue(std::locale(std::locale(), new std::codecvt_utf8<wchar_t>));
        fullRoute = path;
        return true;
    } else {
        return false;
    }
}

bool FileReader::eof() {
    return fs.eof();
}

bool FileReader::close() {
    if (fs.is_open()) {
        fs.close();
        return true;
    }
    return false;
}

FileReader& FileReader::operator>>(std::string& line) {
    line = readLine();
    return *this;
}