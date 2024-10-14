#pragma once
#include <ostream>

class Node {
    private:
        int row, col;
        char type;
        short dirt;
        int priority;

        Node* next;

    public:
        Node() {
            row = 0;
            col = 0;
            type = 'D';
            dirt = 0;
            priority = 0;
            next = nullptr;
        } 

        Node(int r, int c, char type, short dirt, int priority) {
            this->row = r;
            this->col = c;
            this->type = type;
            this->dirt = dirt;
            this->priority = priority;
            this->next = nullptr;
        }

        void setCoords(int r, int c) {
            this->row = r;
            this->col = c;
        }

        int getRow() {
            return row;
        }

        int getCol() {
            return col;
        }

        char getType() {
            return type;
        }

        void setType(char type) {
            this->type = type;
        }

        short getDirt() {
            return dirt;
        }

        void setDirt(short dirt) {
            this->dirt = dirt;
        }

        int getPriority() {
            return priority;
        }

        void setPriority(int priority) {
            this->priority = priority;
        }

        Node* getNext() {
            return next;
        }

        void setNext(Node* next) {
            this->next = next;
        }

        friend std::ostream& operator<<(std::ostream& os, const Node& node) {
            os << node.type;
            if (node.type == 'D') {
                os << node.dirt;
            }

            os << " : (" << node.col << ", " << node.row << ")\t";

            return os;
        }
};
