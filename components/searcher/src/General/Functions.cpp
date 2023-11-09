#include "../../include/Functions.h"
using namespace std;

vector<string> split(const string word, char delim){
    size_t r=0, l = 0;
    string token;
    vector<string> vec;
    while ((r=word.find(delim,l)) != string::npos) {
        vec.push_back(word.substr(l,r-l));
        l = r+1;
    }
    vec.push_back(word.substr(l));
    return vec;
}

pair<vector<int>, bool> splitInt(const string word, char delim) {
    vector<string> partes = split(word, delim);
    vector<int> vec;
    
    for (const string& parte : partes) {
        try {
            int num = stoi(parte);
            vec.push_back(num);
        } catch (...) {
            printf("Functions-splitInt: Error, caracter desconocido encontrado en %s\n", word.c_str());
            return make_pair(vec, false);
        }
    }
    
    return make_pair(vec, true);
}

string strip(const string line, char c) {
    size_t l = line.find_first_not_of(c);
    if (l == string::npos) return "";
    size_t r = line.find_last_not_of(c);
    if (r < l) return "";
    return line.substr(l, r - l + 1);
}

string stripAllOf(string line, string chars){
    for (char c : chars)
        line = strip(line,c);
    return line;
}

string strip(const string line){
    return strip(line,' ');
}

string soloMin(string line){
    string resultado;
    for (char c : line){
        if((c>'a' && c < 'z' )|| c == ' ')
            resultado += c;
    }
    return resultado;
}

std::pair<std::string,std::string> split1(const std::string& word, const char delim){
    size_t pos = word.find(delim);
    return pos == string::npos ? make_pair(word,"") : make_pair(word.substr(0,pos),word.substr(pos+1));
}

void clearWindow(){
    #ifdef _WIN32
        if (system("cls") != 0) printf("No se pudo limpiar la consola\n");
    #else
        if (system("clear") != 0) printf("No se pudo limpiar la consola\n");
    #endif
}
