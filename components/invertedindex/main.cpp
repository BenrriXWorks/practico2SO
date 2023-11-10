#include <cstdio>
#include "include/FileReader.h"
#include "include/FileWriter.h"
#include "include/Functions.h"
#include "include/robin_hood.h"
#include <map>
#include <chrono>
#include <unistd.h>
#include "include/FastSocket.h"
#include "include/ResultsQuery.h"
#include "include/SearchQuery.h"
#include <string>

using namespace std;

#define each const auto&
#define indexType robin_hood::unordered_map<std::string, robin_hood::unordered_map<std::string, unsigned int>>
 
#define portCache getenv("PORT_C2")
#define addr getenv("BACKEND_ADDRESS")
#define filePath getenv("INDEX_ROUTE")
#define topk_str getenv("TOP_K")
#define buffsize getenv("BUFFER_SIZE")

void event(int frontendFd);
string searchMsg(SearchQuery query);
multimap<unsigned int, string, greater<unsigned int>> searchWords(const std::vector<std::string>& words, indexType& index); 
robin_hood::unordered_map<std::string, unsigned int> scoreIntersect(vector<robin_hood::unordered_map<std::string, unsigned int>*>& v);


int main(int argc, char** argv){
    
    if(portCache == nullptr || addr == nullptr || filePath == nullptr || topk_str == nullptr || buffsize == nullptr){
        printf("Falta alguna de las variables de entorno en env\n");
        printf("PORT_C2: %s | ADDRESS: %s | INDEX_ROUTE: %s | TOP_K: %s | BUFFER_SIZE: %s",portCache,addr,filePath,topk_str,buffsize);
        return EXIT_FAILURE;
    }
    while (true) {
        printf("Esperando cliente\n");
        int frontendFd = FastSocket::ServerSocket(atoi(portCache), 1);
        if (frontendFd == -1) {
            printf("No se pudo crear el socket, intentándolo nuevamente en 5 segundos\n");
            sleep(5);
            continue;
        }
        printf("Se ha conectado con un cliente\n");

        event(frontendFd);

    }
    return EXIT_SUCCESS;
}

void event(int frontendFd){
    while(true){
        std:: string query;
        int recvResult = FastSocket::recvmsg(frontendFd, query, atoi(buffsize));
        if (recvResult == -1) {
            printf("Error al recibir el mensaje\n");
            break;
        } else if (recvResult == 0) {
            printf("Cliente desconectado\n");
            break;
        }
        if (query == string("?")) {
            if (FastSocket::sendmsg(frontendFd, "?", atoi(buffsize)) <= 0) {
                printf("Error al responder con '1'\n");
                break;
            }
            printf("ping recibido de fd %d\n", frontendFd);
            continue;
        }
        cout <<query << endl;
        SearchQuery sq = SearchQuery::fromString(query);
        
        string result = searchMsg(sq);

        if (FastSocket::sendmsg(frontendFd, result, atoi(buffsize)) <= 0) {
            printf("Error al responder el mensaje\n");
            break;
        }
        printf("Mensaje Enviado!:\n");
        cout << result << endl;
    }
}

string searchMsg(SearchQuery sq){
    string query = sq.getQuery();
    string filepath = filePath;
    // Cargar el archivo de indice
    FileReader fr;
    if (!fr.open(filepath)){
        printf("Main: No se pudo cargar el archivo de indice en %s\n",filepath.c_str());
        return "";
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

    auto clockInit = chrono::high_resolution_clock::now();
    multimap<unsigned int, string, greater<unsigned int>> invertedOrderedMap = searchWords(split(strip(query),' '), index);
    auto clockEnd = chrono::high_resolution_clock::now();

    multimap<unsigned int, string, greater<unsigned int>> finalMap;
    auto it = invertedOrderedMap.begin();
    for (int i = 0; i < atoi(topk_str) && it != invertedOrderedMap.end(); ++i, ++it) {
    finalMap.insert(*it);
    }
    auto Time = chrono::duration_cast<chrono::nanoseconds>(clockEnd - clockInit).count();
    bool isFound = !finalMap.empty();

    string tiempoStr = std::to_string(Time);
    
    // Crear ResultsQuery con los resultados
    ResultsQuery rq = ResultsQuery(query,"invertedIndex",sq.destination,tiempoStr, isFound ? "BACKEND":"", isFound, finalMap);
    cout << rq.toString() << endl;
    return rq.toString();
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
