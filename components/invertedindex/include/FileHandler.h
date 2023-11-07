#include <string>
#include <fstream>
#include <filesystem>
#include <iostream>

#pragma once

class FileHandler 
{
protected:
    std::wfstream fs;
    std::string fullRoute;
public:
    bool checkFile(std::string path);
    virtual bool open(std::string path) = 0;
    inline void close() {fs.close();};
    bool checkDirectory(std::string path);
    inline std::string getFilename() const {return fullRoute.find("\\/") ? fullRoute.substr(fullRoute.find_last_of("\\/") + 1) : fullRoute;};
    inline std::string getFullRoute() const {return fullRoute;};
    inline bool isOpen() {return fs.is_open();}
};
