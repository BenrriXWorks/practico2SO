#include <cstdio>
#include "include/FileReader.h"
#include "include/FileWriter.h"
#include "include/Functions.h"
#include "include/robin_hood.h"
#include <map>
#include <chrono>
#include <unistd.h>

using namespace std;

#define each const auto&
#define indexType robin_hood::unordered_map<std::string, robin_hood::unordered_map<std::string, unsigned int>>

multimap<unsigned int, string, greater<unsigned int>> searchWords(const std::vector<std::string>& words, indexType& index); 
robin_hood::unordered_map<std::string, unsigned int> scoreIntersect(vector<robin_hood::unordered_map<std::string, unsigned int>*>& v);
void showMenu(indexType index, int topk);
void clearScreen();

int main(int argc, char** argv){

    if (argc != 3) {
        fputs_unlocked("Main: La llamada a la funcion debe ser:\n $ ./main INVERTED_INDEX_FILE TOPK\n",stdout);
        return EXIT_FAILURE;
    }

    string filepath = string(argv[1]);
    string topk_str = string(argv[2]);
    // Se revisa que TOPK solo sean numeros
    if (topk_str.find_first_not_of("1234567890") != string::npos){
        printf("Main: La variable topk {%s} tiene caracteres invalidos, debe ser un numero\n",topk_str.c_str());
        return EXIT_FAILURE;
    }
    int topk_int = stoi(topk_str);
    if (topk_int < 5){ // Comprobar que topk sea mayor a 4
        printf("Main: Debes establecer TOPK > 4. Saliendo\n");
        return EXIT_FAILURE;
    }
    // Cargar el archivo de indice
    FileReader fr;
    if (!fr.open(filepath)){
        printf("Main: No se pudo cargar el archivo de indice en %s\n",filepath.c_str());
        return EXIT_FAILURE;
    }

    // Leer el indice como un unordered map de string a unordered map de string a unsigned int
    indexType index;
    for (each line : fr.readLines())
        if (!line.empty()){
            auto wordAndFiles = split1(line,':');
            for (each file : split(wordAndFiles.second,';')){
                auto filenameAndCount = split1(strip(strip(file,'('),')'),',');
                index[wordAndFiles.first][filenameAndCount.first] = stoi(filenameAndCount.second);
            }
        }
    // Desplegar el menu
    showMenu(index,topk_int);
    fputs_unlocked("Gracias por usar el buscador!\n",stdout);

    return EXIT_SUCCESS;
}

void clearScreen(){
    #ifndef _WIN32
        system("clear");
    #else
        system("cls");
    #endif
}

multimap<unsigned int, string, greater<unsigned int>> searchWords(const std::vector<std::string>& words, indexType& index){

    // Establecer threads para el trabajo

    // Vector con los maps asociados a las palabras a buscar
    vector<robin_hood::unordered_map<std::string, unsigned int>*> miniMaps;
    for (each word : words) {
        if (!index.count(word)) return *new multimap<unsigned int, string, greater<unsigned int>> ();
        miniMaps.push_back((&index[word]));
    }

    // Hacer las intersecciones sumando puntos y ordenar
    multimap<unsigned int, string, greater<unsigned int>> invertedSortedResults;
    for (each wordAndCount : scoreIntersect(miniMaps))
        invertedSortedResults.emplace(wordAndCount.second,wordAndCount.first);
    
    return invertedSortedResults;
}

robin_hood::unordered_map<std::string, unsigned int> scoreIntersect(vector<robin_hood::unordered_map<std::string, unsigned int>*>& v){

    // Intersectar los maps desde el primero hacia adelante.
    robin_hood::unordered_map<std::string, unsigned int> intersection = *v.at(0);
    for (size_t i = 1; i < v.size(); i++){
        const auto intersectionCopy = intersection;
        for (const robin_hood::pair<string,uint>& elem : intersectionCopy)
            if (v[i]->count(elem.first))
                intersection[elem.first] += v[i]->at(elem.first);
            else
                intersection.erase(elem.first);
    }
    return intersection;
}

void showMenu(indexType index, int topk){
    std::string input;
    do {

        clearScreen();
        printf("\"BUSCADOR BASADO EN INDICE INVERTIDO\" (%d)\n\nLos top K documentos seran = %d\n\n",getpid(), topk);
    
        fputs_unlocked("Escriba texto a buscar: ",stdout);
        getline(std::cin, input);

        auto clockInit = chrono::high_resolution_clock::now();
        multimap<unsigned int, string, greater<unsigned int>> invertedOrderedMap = searchWords(split(strip(input),' '), index);
        auto clockEnd = chrono::high_resolution_clock::now();

        // Escribir en pantalla los 
        printf("Respuesta (tiempo = %lums/%luns | Resultados = %lu ):\n\n",chrono::duration_cast<chrono::milliseconds>(clockEnd-clockInit).count(),
            chrono::duration_cast<chrono::nanoseconds>(clockEnd-clockInit).count(),invertedOrderedMap.size());
        int i = 0;
        for (each element : invertedOrderedMap) {
            if (++i > topk) break;
            printf("\t%d) %s %d\n", i, element.second.c_str(), element.first);
        }
        
        do {
            fputs_unlocked("\nDesea continuar (S/N): ",stdout);
            getline(std::cin, input);
            input = input == "N" ? "n" : input == "S" ? "s" : input;
        } while (input != "n" && input != "s");


    } while (input != "n");
}