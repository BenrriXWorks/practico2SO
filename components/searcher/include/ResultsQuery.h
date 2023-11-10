//v 1.0

#include <string>
#include "Functions.h"
#include <set>
#include <algorithm>
#include <map>
#include <unistd.h>
#include <sstream>


class ResultsQuery
{
public:
    std::string query,origin, destination,tiempo, ori;
    int isFound;
    std::multimap<unsigned int, std::string, std::greater<unsigned int>> results;
    ResultsQuery (std::string query, std::string origin, std::string destination,std::string tiempo, std::string ori ,bool isFound, std::multimap<unsigned int, std::string, std::greater<unsigned int>> results):
        query(query),origin(origin),destination(destination),tiempo(tiempo),ori(ori),isFound(isFound), results(results){};

    std::string toString();
    static ResultsQuery fromString(std::string query);

};