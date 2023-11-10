template <typename T>
class Node {
public:
    Node *next = nullptr;
    T element;

    Node(T element) : element(element) {};
};