#include "FileHandler.h"
#include <string>
#include <fstream>
#include <vector>
#pragma once

class FileWriter : public FileHandler 
{
public:
    bool writeLine(const std::string line);
    bool writeLines(const std::vector<std::string> lines);
    bool write(const std::string text);
    bool open(const std::string path);
    bool openAppend(const std::string path);
    bool close();
    FileWriter& operator<<(const std::string& s);
    inline FileWriter& operator<<(const char& c) {return *this << std::string(1,c);}
    inline FileWriter& operator<<(const char* word) {return *this << std::string(word);}
    inline FileWriter& operator<<(int num) {return *this << std::to_string(num);}
};