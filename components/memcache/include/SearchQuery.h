#include <string>
#include "Functions.h"
#include <set>
#include <algorithm>


/*mensaje={
origen:”./searcher”,
destino:”./memcache”,
contexto:{ txtToSerarch:”hola mundo”}
}*/
class SearchQuery
{
public:
    std::set<std::string> query;
    std::string origin, destination;
    SearchQuery(std::set<std::string> query, std::string origin, std::string destination) : 
        query(query), origin(origin), destination(destination){};
    
    std::string toString();

    static SearchQuery fromString(std::string query);
};