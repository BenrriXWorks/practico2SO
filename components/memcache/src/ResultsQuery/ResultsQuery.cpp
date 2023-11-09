//v1.0
#include "../../include/ResultsQuery.h"

std ::string ResultsQuery::toString(){
    std::stringstream ss;

    

    ss << query << "\n" << origin << "\n" << destination << "\n"
       << tiempo << "\n" << ori << "\n" << std::to_string(isFound) << "\n";
    for (const auto &result : results) {
        ss << result.second << "," << result.first << " ";
    }
    // Convertir query (multimap) a cadena
    for (const auto &result : results) {
        ss << result.second << "," << result.first << " ";
    }
    return ss.str();
    
}

ResultsQuery ResultsQuery::fromString(std::string query){
    const auto splittedQuery = split(query,'\n');
    if(splittedQuery.size()!= 7){
        printf("Query invalida: %s\n",query.c_str());
        return ResultsQuery("","","","","",0,std::multimap<unsigned int,std::string,std::greater<unsigned int>>());
    }
    bool isFound = (splittedQuery[5] == "1");

    std::string resultadosCadena = splittedQuery[6];
    std::istringstream resultadosStream(resultadosCadena);
    std::string elemento;
    std::multimap<unsigned int, std::string, std::greater<unsigned int>> resultsMap; 
    while (std::getline(resultadosStream, elemento, ' ')) {
        auto pos = elemento.find(',');
        if (pos != std::string::npos) {
            std::string archivo = elemento.substr(0, pos);
            std::string puntajeStr = elemento.substr(pos + 1);
            unsigned int puntaje = std::stoi(puntajeStr);
            resultsMap.insert(std::make_pair(puntaje, archivo));
        }
    }

    return ResultsQuery(splittedQuery[0],splittedQuery[1],splittedQuery[2],splittedQuery[3],splittedQuery[4],isFound,std::multimap<unsigned int,std::string,std::greater<unsigned int>>());
}