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
 
#define PORT_CACHE getenv("PORT_C2")
#define addr getenv("BACKEND_ADDRESS")
#define INDEX_ROUTE getenv("INDEX_ROUTE")
#define TOPK_STR getenv("TOP_K")
#define BUFFER_SIZE getenv("BUFFER_SIZE")

void event(int frontendFd, indexType& index);
string searchMsg(SearchQuery query, indexType& index);
multimap<unsigned int, string, greater<unsigned int>> searchWords(const std::vector<std::string>& words, indexType& index); 
robin_hood::unordered_map<std::string, unsigned int> scoreIntersect(vector<robin_hood::unordered_map<std::string, unsigned int>*>& v);
indexType createIndex();

int main(int argc, char** argv){
    
    // Revisar si falta una variable de entorno
    if(PORT_CACHE == nullptr || addr == nullptr || INDEX_ROUTE == nullptr || TOPK_STR == nullptr || BUFFER_SIZE == nullptr){
        printf("Falta alguna de las variables de entorno en env\n");
        printf("PORT_C2: %s | ADDRESS: %s | INDEX_ROUTE: %s | TOP_K: %s | BUFFER_SIZE: %s",PORT_CACHE,addr,INDEX_ROUTE,TOPK_STR,BUFFER_SIZE);
        return EXIT_FAILURE;
    }

    // Cargar el indice invertido
    indexType index = createIndex();

    // Esperar a que se conecte el memcache
    while (true) {
        printf("Esperando cliente\n");
        int frontendFd = FastSocket::ServerSocket(atoi(PORT_CACHE), 1);
        if (frontendFd == -1) {
            printf("No se pudo crear el socket, intentándolo nuevamente en 5 segundos\n");
            sleep(5);
            continue;
        }
        printf("Se ha conectado con un cliente\n");
        event(frontendFd, index);
    }
    return EXIT_SUCCESS;
}

// Manejar los eventos de los mensajes
void event(int frontendFd, indexType& index){

    while(true){
        std:: string query;
        int recvResult = FastSocket::recvmsg(frontendFd, query, atoi(BUFFER_SIZE));

        // Si el mensaje no se recibio o el cliente se desconecto
        if (recvResult <= 0) {
            printf("Conexion perdida con el cliente\n");
            break;
        }

        // Si el mensaje es un ping
        if (query == string("?")) {
            if (FastSocket::sendmsg(frontendFd, "?", atoi(BUFFER_SIZE)) <= 0) {
                printf("Error al responder con '1'\n");
                break;
            }
            printf("ping recibido de fd %d\n", frontendFd);
            continue;
        }
        printf("Mensaje recibido:\n%s\n",query.c_str());
        SearchQuery sq = SearchQuery::fromString(query);
        
        // Hacer la busqueda
        string result = searchMsg(sq, index);

        // Enviar mensaje con respuesta
        if (FastSocket::sendmsg(frontendFd, result, atoi(BUFFER_SIZE)) <= 0) {
            printf("Error al responder el mensaje\n");
            break;
        }

        printf("Mensaje Enviado:\n%s\n",result.c_str());
    }
}

indexType createIndex(){
    // Cargar el archivo de indice
    FileReader fr;
    if (!fr.open(string(INDEX_ROUTE))){
        printf("Main: No se pudo cargar el archivo de indice en %s\n",INDEX_ROUTE);
        return indexType();
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
    return index;
}

string searchMsg(SearchQuery sq, indexType& index){
    string query = split1(sq.toString(),'\n').first;

    // Buscar en el indice
    auto clockInit = chrono::high_resolution_clock::now();
    multimap<unsigned int, string, greater<unsigned int>> invertedOrderedMap = searchWords(split(strip(query),' '), index);
    auto clockEnd = chrono::high_resolution_clock::now();

    // Guardar solo los top k
    multimap<unsigned int, string, greater<unsigned int>> finalMap;
    int i = 0;
    for (each elem : invertedOrderedMap) if (atoi(TOPK_STR) < ++i) break; else finalMap.emplace(elem);

    auto timeFindAndSort = chrono::duration_cast<chrono::nanoseconds>(clockEnd - clockInit).count();
    bool isFound = !finalMap.empty();
    string tiempoStr = std::to_string(timeFindAndSort);
    
    return ResultsQuery(query,"invertedIndex",sq.destination,tiempoStr, isFound ? "BACKEND":"", isFound, finalMap).toString();
}


multimap<unsigned int, string, greater<unsigned int>> searchWords(const std::vector<std::string>& words, indexType& index){

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
