#include "../../include/Cache.h"

ResultsQuery* Cache::find(std::string query) {
    Node<ResultsQuery>* p = first;
    Node<ResultsQuery>* prev = nullptr;

    while (p != nullptr) {
        if (p->element.query == query) {
            // Se encontró la consulta, mover el nodo al final de la lista
            if (p != last) {
                // Si el nodo no es el último nodo
                if (prev) {
                    prev->next = p->next;
                } else {
                    first = p->next;
                }

                // Actualizar el puntero last solo si el nodo no es el último
                if (last) {
                    last->next = p;
                    last = p;
                    p->next = nullptr;
                }
            }
            return &(p->element); // Se devuelve el puntero a los resultados
        }

        prev = p;
        p = p->next;
    }

    return nullptr; // No se encontró la consulta, se devuelve nullptr
}

void Cache::insert(ResultsQuery element) {
    if (first == nullptr) {
        // La caché está vacía, crea el primer nodo
        first = last = new Node<ResultsQuery>(element);
    } else {
        // La caché no está vacía, agrega un nuevo nodo al final
        last->next = new Node<ResultsQuery>(element);
        last = last->next;

        if (size >= maxSize) {
            // La caché está llena, elimina el elemento más antiguo
            Node<ResultsQuery>* oldFirst = first;
            first = first->next;
            delete oldFirst;
        } else {
            size++;
        }
    }
}