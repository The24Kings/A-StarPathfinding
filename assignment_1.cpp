#include <stdio.h>
#include <iostream>
#include <ostream>
#include <fstream>
#include <vector>

#include "node.h"
#include "queue.h"
#include "vacuum.h"

typedef std::vector<std::vector<Node>> Grid;

#define GET_NODE_PTR(grid, row, col) &grid.at(row).at(col)
#define GET_NODE(grid, row, col) grid.at(row).at(col)

Grid create_grid(std::string, std::vector<Node*>&);
int distance(int, int, int, int);
bool check_goal_state(Grid, Vacuum*);
int calculate_hueristic(Vacuum*, Node*, std::vector<Node*>);
bool isValid(int, int);
void sort_goals(std::vector<Node*>&, Vacuum*);

int ROW = 0;
int COL = 0;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <filename>" << std::endl;
        return 1;
    }

    std::vector<Node*> goals;
    Grid grid = create_grid(argv[1], goals);

    if (grid.empty()) {
        std::cout << "Failed to create grid" << std::endl;
        return 1;
    }

    /* Print the grid
    for (int i = 0; i < grid.size(); i++) {
        for (int j = 0; j < grid.at(i).size(); j++) {
            std::cout << grid.at(i).at(j);
        }
        std::cout << std::endl;
    }*/

    ROW = grid.size();
    COL = grid.at(0).size();

    // Find the Vacuum Node
    Vacuum* vacuum = new Vacuum();

    for (int row = 0; row < ROW; row++) {
        for (int col = 0; col < COL; col++) {
            if (GET_NODE(grid, row, col).getType() == 'V') {
                vacuum->setCoords(row, col);
                GET_NODE(grid, row, col).setType('D');
                GET_NODE(grid, row, col).setPriority(10);

                break;
            }
        }
    }

    std::cout << std::endl << *vacuum << std::endl;

    // Create a priority queue with the vacuum node as head
    Queue queue = Queue(GET_NODE_PTR(grid, vacuum->getRow(), vacuum->getCol()));

    // A* Algorithm
    Node* dest = nullptr;
    double hNew = 0;

    do {
        int vRow = vacuum->getRow();
        int vCol = vacuum->getCol();

        //std::cout << "Vacuum: (" << vCol << ", " << vRow << ")" << std::endl;

        // Sort the goals based on distance from the vacuum
        sort_goals(goals, vacuum);

        // Add each direction as children of parent node to the queue
        if (isValid(vRow - 1, vCol)) { // North
            dest = &grid.at(vRow - 1).at(vCol);

            hNew = calculate_hueristic(vacuum, dest, goals);
            dest->setPriority(hNew);

            //std::cout << "North: " << *dest << std::endl;

            queue.enqueue(dest); 
        }
        if (isValid(vRow, vCol + 1)) { // East
            dest = &grid.at(vRow).at(vCol + 1);

            hNew = calculate_hueristic(vacuum, dest, goals);
            dest->setPriority(hNew);

            //std::cout << "East: " << *dest << std::endl;

            queue.enqueue(dest);
        }  
        if (isValid(vRow + 1, vCol)) { // South
            dest = &grid.at(vRow + 1).at(vCol);

            hNew = calculate_hueristic(vacuum, dest, goals);
            dest->setPriority(hNew);

            //std::cout << "South: " << *dest << std::endl;

            queue.enqueue(dest);
            
        }  
        if (isValid(vRow, vCol - 1)) { // West
            dest = &grid.at(vRow).at(vCol - 1);

            hNew = calculate_hueristic(vacuum, dest, goals);
            dest->setPriority(hNew);

            //std::cout << "West: " << *dest << std::endl;

            queue.enqueue(dest); //FIXME: When enqueueing the same node multiple times, the queue will get stuck in an infinite loop
        }

        // Find the highest priority Cell
        Node* next = queue.findHighPriority();
        //std::cout << "Next: " << *next << std::endl;

        queue.dequeue(next);

        // Move the vacuum to that cell
        if (vacuum->getRow() + 1 == next->getRow()) { std::cout << "Moving Down" << std::endl; }
        if (vacuum->getCol() + 1 == next->getCol()) { std::cout << "Moving Right" << std::endl; }
        if (vacuum->getRow() - 1 == next->getRow()) { std::cout << "Moving Up" << std::endl; }
        if (vacuum->getCol() - 1 == next->getCol()) { std::cout << "Moving Left" << std::endl; }
        
        vacuum->setCoords(next->getRow(), next->getCol());

        // Check if the vacuum needs to do an action
        if (next->getType() == 'D' && vacuum->container() + next->getDirt() < vacuum->containerMax() && next->getDirt() > 0) {
            std::cout << "Cleaning" << std::endl;
            vacuum->suck(next);
        }

        if (next->getType() == 'G' && vacuum->container() > 0) {
            std::cout << "Dumping" << std::endl;
            vacuum->dump();
        }

    } while (check_goal_state(grid, vacuum));

    return 0;
}

Grid create_grid(std::string filename, std::vector<Node*>& goals) {
    std::ifstream file;
    std::string line;
    Grid grid;

    file.open(filename);

    if (file.eof()) {
        std::cout << "File is empty" << std::endl;
        return grid;
    }

    // Load input file and create a grid
    int row = 0;
    int col = 0;
    int num;

    while (getline(file, line)) {
        grid.push_back(std::vector<Node>());
        
        // Parse the line
        for (int i = 1; i < line.length(); i++) {
            switch (line[i]) {
                case 'D': // Dirt
                    num = line[i+1] - '0';
                    grid.at(row).push_back(Node(row, col, 'D', num, 0));

                    col++;
                    i++;
                    break;
                case 'O': // Wall
                    grid.at(row).push_back(Node(row, col, 'O', 0, 0));

                    col++;
                    break;
                case 'C': // Cat
                    grid.at(row).push_back(Node(row, col, 'C', 0, 0));

                    col++;
                    break;
                case 'G': // Garbage
                    grid.at(row).push_back(Node(row, col, 'G', 0, 0));
                    
                    col++;
                    break;
                case 'V': // Vacuum
                    grid.at(row).push_back(Node(row, col, 'V', 0, 0));
                    col++;
                    break;
            }
        }

        col = 0;
        row++;
    }

    file.close();

    // Find the goal nodes
    for (int i = 0; i < grid.size(); i++) {
        for (int j = 0; j < grid.at(i).size(); j++) {
            Node cell = grid.at(i).at(j);

            // Dirt Piles
            if (cell.getType() == 'D' && cell.getDirt() > 0) {
                goals.push_back(&grid.at(i).at(j));
            
            // Obstacles
            } else if (cell.getType() == 'O') {
                goals.push_back(&grid.at(i).at(j));
            
            // Cats
            } else if (cell.getType() == 'C') {
                goals.push_back(&grid.at(i).at(j));
            
            // Garbage
            } else if (cell.getType() == 'G') {
                goals.push_back(&grid.at(i).at(j));

            } else {
                continue;
            }
        }
    }

    return grid;
}


/**
 * @brief Manhattan Distance
 * 
 * @param r1 Row 1
 * @param c1 Column 1
 * @param r2 Row 2
 * @param c2 Column 2
 * @return int 
 */
int distance(int r1, int c1, int r2, int c2) {
    return abs(c1 - c2) + abs(r1 - r2);
}

bool check_goal_state(Grid grid, Vacuum* vacuum) {
    for (int i = 0; i < grid.size(); i++) {
        for (int j = 0; j < grid.at(i).size(); j++) {
            Node cell = grid.at(i).at(j);

            if (cell.getType() == 'D' && cell.getDirt() > 0) {
                return true;
            }
        }
    }

    if (vacuum->container() != 0) {
        return true;
    }

    return false;
}

/*
    Calculate the hueristic for the A* algorithm
    Smaller hueristic is better

    Manhattan Distance:
    h(n) = |x1 - x2| + |y1 - y2|

    Calculate the distance between the start and goal cells

    Goal Cell:
        Dirt -> Closest dirt
        Obsticle -> Avoid at all costs
        Cat -> -10 points for cat in path
        Garbage -> High value when vacuum is almost full and the next dirt pile has more dirt than the vacuum can hold
    
    Start Cell:

    Return the hueristic

    Desentivize the cat and obsticle cells
    Encourage the vacuum to pick up dirt piles until it is almost full
    Then encourage the vacuum to dump the dirt into the garbage and discourage the vacuum from picking up more dirt 
*/
int calculate_hueristic(Vacuum* vacuum, Node* dest, std::vector<Node*> goals) {
    int h = 0;

    Node* closest_dirt = nullptr;
    Node* closest_garbage = nullptr;
    Node* closest_cat = nullptr;
    Node* closest_obsticle = nullptr;
    int vacuum_capacity = vacuum->container();

    // Find the closest goals
    for (int g = 0; g < goals.size(); g++) {
        if (goals.at(g)->getType() == 'D') {
            if (closest_dirt == nullptr) {
                closest_dirt = goals.at(g);
            }
        } else if (goals.at(g)->getType() == 'G') {
            if (closest_garbage == nullptr) {
                closest_garbage = goals.at(g);
            }
        } else if (goals.at(g)->getType() == 'C') {
            if (closest_cat == nullptr) {
                closest_cat = goals.at(g);
            }
        } else if (goals.at(g)->getType() == 'O') {
            if (closest_obsticle == nullptr) {
                closest_obsticle = goals.at(g);
            }
        } else {
            continue;
        }
    }

    // Calculate the distance to the closest dirt pile unless the vacuum is always full
    if (closest_dirt != nullptr && vacuum_capacity < vacuum_capacity + closest_dirt->getDirt()) {
        h += distance(vacuum->getRow(), vacuum->getCol(), closest_dirt->getRow(), closest_dirt->getCol());
    } else {
        h += 10;
    }

    // Calculate the distance to the closest garbage pile
    if (closest_garbage != nullptr) {
        h += distance(vacuum->getRow(), vacuum->getCol(), closest_garbage->getRow(), closest_garbage->getCol());
    }

    // Calculate the distance to the closest obsticle
    if (closest_obsticle != nullptr) {
        h += 10 - distance(vacuum->getRow(), vacuum->getCol(), closest_obsticle->getRow(), closest_obsticle->getCol()); // Incentivize the vacuum to avoid obsticles (Arbitrary value)
    }   

    if (dest->getType() == 'C') {
        h += 10;
    }

    if (dest->getType() == 'O') {
        h += 100;
    }

    return h;

}

bool isValid(int row, int col) {
    return (row >= 0) && (row < ROW) && (col >= 0) && (col < COL);
}

void sort_goals(std::vector<Node*>& goals, Vacuum* vacuum) {
    // Sort the goals in the vecotr based on distance from the vacuum
    //std::cout << "Sorting Goals" << std::endl;

    // Bubble Sort
    for (int i = 0; i < goals.size(); i++) {
        for (int j = 0; j < goals.size(); j++) {
            int d1 = distance(vacuum->getRow(), vacuum->getCol(), goals.at(i)->getRow(), goals.at(i)->getCol());
            int d2 = distance(vacuum->getRow(), vacuum->getCol(), goals.at(j)->getRow(), goals.at(j)->getCol());

            if (d1 < d2) {
                Node* temp = goals.at(i);
                goals.at(i) = goals.at(j);
                goals.at(j) = temp;
            }
        }
    }
}
