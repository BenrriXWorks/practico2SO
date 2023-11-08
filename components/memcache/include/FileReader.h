#include "FileHandler.h"
#include <string>
#include <fstream>
#include <vector>

#include <iostream>
#include <locale>
#include <codecvt>

#pragma once

class FileReader : public FileHandler 
{
public:
    std::string readLine();
    std::vector<std::string> readLines();
    std::string read();
    bool open(std::string path);
    bool eof();
    bool close();
    FileReader& operator>>(std::string& line);
    
};