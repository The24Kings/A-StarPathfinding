#pragma once 

// Doubly linked list node for priority queue
// FIFO queue, but with a priority value
class Queue {
    private:
        int count;
        Node* head;

    public:
        Queue() {
            this->count = 0;
            this->head = nullptr;
        }

        Queue(Node* head) {
            this->count = 1;
            this->head = head;
        }

        ~Queue() {
            //deleteQueue();
        }

        Node* getHead() {
            return head;
        }

        void setHead(Node* head) {
            this->head = head;
        }

        Node* findHighPriority();
        Node* dequeue(Node* node);
        void enqueue(Node* node);
        bool isEmpty();
        void deleteQueue();
        int size();

};

Node* Queue::findHighPriority() {
    Node* max = this->head;
    Node* current = max->getNext();

    while (current != nullptr) {
        if (current->getPriority() <= max->getPriority()) { // Lower priority is better
            max = current;
        }

        current = current->getNext();
    }

    return max;
}

// Dequeue a specific node from the queue
Node* Queue::dequeue(Node* node) {
    Node* current = this->head;
    Node* prev = nullptr;

    while (current != nullptr) {
        if (current == node) {
            if (prev != nullptr) {
                prev->setNext(node->getNext());
            } else {
                head = node->getNext();
            }

            node->setNext(nullptr);

            return current;
        }

        prev = current;
        current = current->getNext();
    }

    count--;
    return node;
}

// Enqueue a node to the queue
void Queue::enqueue(Node* node) {
    Node* current = this->head;

    while (current->getNext() != nullptr) {
        current = current->getNext();
    }

    current->setNext(node);
    count++;
}

bool Queue::isEmpty() {
    return this->head == nullptr;
}

void Queue::deleteQueue() {
    Node* current = this->head;
    Node* next;

    while (current != nullptr) {
        next = current->getNext();
        delete current;
        current = next;
    }
}

int Queue::size() {
    return count;
}
