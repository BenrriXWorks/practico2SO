#pragma once

#include <string>
#include "Functions.h"
#include <set>
#include <numeric>

class SearchQuery
{
public:
    std::multiset<std::string> query;
    std::string strQuery, origin, destination;
    SearchQuery(std::multiset<std::string> query, std::string origin, std::string destination) : 
        query(query), origin(origin), destination(destination){};
    
    std::string toString();
    std::string queryToString();

    static SearchQuery fromString(std::string query);
};