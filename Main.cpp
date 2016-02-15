#include <iostream>

using namespace std;

#define WIDTH 16
#define HEIGHT 16

#define NORTH 1
#define SOUTH 2
#define EAST 4
#define WEST 8

#define VISITED 16
#define UNVISITED 239

class Space {
private:
    uint8_t c_data;
    uint8_t c_visitedFrom;

public:
    uint8_t c_fullPathPos;
    uint8_t c_partPathPos;

    Space(){
        c_data = 0;
        c_visitedFrom = 0;
        c_fullPathPos = 0;
        c_partPathPos = 0;
    }

    bool northWall() {
        return c_data & NORTH;
    }

    bool southWall() {
        return c_data & SOUTH;
    }

    bool eastWall() {
        return c_data & EAST;
    }

    bool westWall() {
        return c_data & WEST;
    }

    void hasNorthWall() {
        c_data |= NORTH;
    }

    void hasSouthWall() {
        c_data |= SOUTH;
    }

    void hasEastWall() {
        c_data |= EAST;
    }

    void hasWestWall() {
        c_data |= WEST;
    }

    bool visited() {
        return c_data & VISITED;
    }

    void visit() {
        c_data |= VISITED;
    }

    void unVisit(){
        c_data &= UNVISITED;
    }

    uint8_t visitedFromX() {
        return c_visitedFrom & 15;
    };

    uint8_t visitedFromY() {
        return (c_visitedFrom >> 4) & 15;
    };

    void setVisitedFromX(uint8_t x) {
        c_visitedFrom = (c_visitedFrom & 240) | x;
    };

    void setVisitedFromY(uint8_t y) {
        c_visitedFrom = (c_visitedFrom & 15) | (y << 4);
    };
};

struct QueueNode {
    uint8_t x;
    uint8_t y;
    QueueNode *prev;
};

Space g_board[WIDTH][HEIGHT];
QueueNode *g_queueHead;
QueueNode *g_queueTail;

void resetFullPath() {
    for(int i = 0; i < WIDTH; i++){
        for(int j = 0; j < HEIGHT; j++){
            g_board[i][j].c_fullPathPos = 0;
            g_board[i][j].unVisit();
        }
    }
}

void resetPartPath() {
    for(int i = 0; i < WIDTH; i++){
        for(int j = 0; j < HEIGHT; j++){
            g_board[i][j].c_partPathPos = 0;
            g_board[i][j].unVisit();
        }
    }
}

void print(uint8_t size){
    for(int i = 0; i < size; i++){
        for(int j = 0; j < size; j++){
            cout << i << "\t" << j << endl;
            cout << "Visited: " << g_board[i][j].visited() << endl;
            cout << "North: " << g_board[i][j].northWall() << endl;
            cout << "South: " << g_board[i][j].southWall() << endl;
            cout << "East: " << g_board[i][j].eastWall() << endl;
            cout << "West: " << g_board[i][j].westWall() << endl;
            cout << "Visited from x: " << (int)g_board[i][j].visitedFromX() << endl;
            cout << "Visited from y: " << (int)g_board[i][j].visitedFromY() << endl;
            cout << endl;
        }
    }
}

QueueNode* dequeue(){
    QueueNode* res = g_queueHead;
    if(res != NULL) g_queueHead = g_queueHead->prev;
    return res;
}

void enqueue(QueueNode* n){
    if(g_queueTail != NULL){
        g_queueTail->prev = n;
    }
    g_queueTail = n;
}

void floodFillMain(int startX, int startY, int goalX, int goalY){
    // Still working on this.
}

int main() {
    resetFullPath();
    resetPartPath();

    g_board[0][0].visit();
    g_board[0][0].setVisitedFromX(4);
    g_board[0][0].setVisitedFromY(15);
    g_board[0][0].hasEastWall();

    print(1);
}