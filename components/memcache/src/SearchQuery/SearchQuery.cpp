#include "../../include/SearchQuery.h"

using namespace std;

std::string SearchQuery::toString(){
    return 
        accumulate(
            query.begin(), query.end(),
            std::string(),
            [](const std::string& a, const std::string& b) {
                return a + (a.empty() ? "" : " ") + b;
            }
        ) + "\n" + origin + "\n" + destination;

}

SearchQuery SearchQuery::fromString(std::string query){
    const auto splittedQuery = split(query,'\n');
    if (splittedQuery.size() != 3) {
        printf("Query invalida: %s\n",query.c_str());
        return SearchQuery(set<string>(),"","");
    }
    set<string> elements;
    for (const string& s : split(splittedQuery.front(),' ')) elements.emplace(s);
    
    return SearchQuery(elements,splittedQuery[1],splittedQuery[2]);
}

std::string SearchQuery::queryToString() {
    return std::accumulate(query.begin(), query.end(), std::string(),
        [](const std::string& a, const std::string& b) { return a + (a.empty() ? "" : " ") + b; });
}