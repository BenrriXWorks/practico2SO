#include <string>
#include "SearchQuery.h"
#include "ResultsQuery.h"
#include "Node.h"


class Cache
{
private:
    uint8_t size = 0;
    uint8_t maxSize = 0;
    Node<ResultsQuery> *first = nullptr, *last = nullptr;
public:
    ResultsQuery* find(std::string element);
    void insert(ResultsQuery element);

    Cache(uint8_t maxElements) : maxSize(maxElements) {};

};