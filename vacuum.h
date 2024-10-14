#pragma once

class Vacuum {
    private:
        int row, col;
        int dirt;

    public:
        Vacuum() {
            row = 0;
            col = 0;
            dirt = 0;
        }

        Vacuum(int row, int col, int dirt) {
            this->row = row;
            this->col = col;
            this->dirt = dirt;
        }

        void setCoords(int row, int col) {
            this->row = row;
            this->col = col;
        }

        int getRow() {
            return row;
        }

        int getCol() {
            return col;
        }

        int getDirt() {
            return dirt;
        }

        void setDirt(int dirt) {
            this->dirt = dirt;
        }

        void dump() {
            dirt = 0;
        }

        int container() {
            return dirt;
        }

        void suck(Node* node) {
            dirt = node->getDirt();
            node->setDirt(0);
        }

        int containerMax() {
            return 25;
        }

        friend std::ostream& operator<<(std::ostream& os, const Vacuum& vacuum) {
            os << "Vacuum: (" << vacuum.col << ", " << vacuum.row << ") Dirt: " << vacuum.dirt;
            return os;
        }
};
