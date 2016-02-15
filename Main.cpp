#include <iostream>
#include <unistd.h>

using namespace std;

#define WIDTH 16
#define HEIGHT 16

#define CENTER_X 7
#define CENTER_Y 7

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

    QueueNode(uint8_t nx, uint8_t ny){
        x = nx;
        y = ny;
    }
};

Space g_board[WIDTH][HEIGHT];
QueueNode *g_queueHead;
QueueNode *g_queueTail;
uint8_t currentX;
uint8_t currentY;

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

void initializeWalls(){
    for(int i = 0; i < WIDTH; i++){
        g_board[i][0].hasNorthWall();
        g_board[i][HEIGHT - 1].hasSouthWall();
    }
    for(int i = 0; i < HEIGHT; i++){
        g_board[0][i].hasWestWall();
        g_board[WIDTH - 1][i].hasEastWall();
    }
}

void print(){
    char blue[] = { 0x1b, '[', '3', '4', 'm', 0 };
    char red[] = { 0x1b, '[', '3', '1', 'm', 0 };
    char yellow[] = { 0x1b, '[', '3', '3', 'm', 0 };
    char underline[] = {0x1b, '[', '4', 'm', 0};
    char noColor[] = { 0x1b, '[', '3', '9', 'm', 0};
    char noFormat[] = {0x1b, '[', '0', 'm', 0};

    cout << noColor;

    cout << underline;
    cout << " ";
    for(int i = 0; i < WIDTH; i++) cout << "  ";
    cout << noFormat << endl;

    for(int y = 0; y < HEIGHT; y++){
        if(y == HEIGHT - 1){
            cout << underline;
        }
        cout << '|';
        cout << noFormat;
        for(int x = 0; x < WIDTH; x++){
            if(x == CENTER_X && y == CENTER_Y) cout << red;
            else {
                if(g_board[x][y].c_fullPathPos != 0) cout << yellow;
                else {
                    if (g_board[x][y].visited()) cout << blue;
                }
            }
            if(g_board[x][y].southWall()) cout << underline;
            cout << (x == currentX && y == currentY ? 'X' : 'O');
            cout << noColor;
            if(g_board[x][y].eastWall()) cout << '|';
            else cout << ' ';
            cout << noFormat;
        }
        cout << endl;
    }
    cout << endl;
}

QueueNode* dequeue(){
    QueueNode* res = g_queueHead;
    if(res != NULL) g_queueHead = g_queueHead->prev;
    if(g_queueHead == NULL) g_queueTail == NULL;
    return res;
}

void enqueue(QueueNode* n){
    if(g_queueTail != NULL){
        g_queueTail->prev = n;
    }
    else{
        g_queueHead = n;
    }
    g_queueTail = n;
}

void clearQueue(){
    while(g_queueTail != NULL) delete dequeue();
}

void floodFillFrom(uint8_t startX, uint8_t startY){
    if(!g_board[startX][startY].northWall() && !g_board[startX][startY - 1].visited()){
        g_board[startX][startY - 1].visit();
        g_board[startX][startY - 1].setVisitedFromX(startX);
        g_board[startX][startY - 1].setVisitedFromY(startY);
        enqueue(new QueueNode(startX, startY - 1));
    }

    if(!g_board[startX][startY].southWall() && !g_board[startX][startY + 1].visited()){
        g_board[startX][startY + 1].visit();
        g_board[startX][startY + 1].setVisitedFromX(startX);
        g_board[startX][startY + 1].setVisitedFromY(startY);
        enqueue(new QueueNode(startX, startY + 1));
    }

    if(!g_board[startX][startY].eastWall() && !g_board[startX + 1][startY].visited()){
        g_board[startX + 1][startY].visit();
        g_board[startX + 1][startY].setVisitedFromX(startX);
        g_board[startX + 1][startY].setVisitedFromY(startY);
        enqueue(new QueueNode(startX + 1, startY));
    }

    if(!g_board[startX][startY].westWall() && !g_board[startX - 1][startY].visited()){
        g_board[startX - 1][startY].visit();
        g_board[startX - 1][startY].setVisitedFromX(startX);
        g_board[startX - 1][startY].setVisitedFromY(startY);
        enqueue(new QueueNode(startX - 1, startY));
    }
}

void printDebug(uint8_t size){
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

uint8_t floodFillMain(uint8_t startX, uint8_t startY, uint8_t goalX, uint8_t goalY){
    uint8_t originalStartX = startX;
    uint8_t  originalStartY = startY;

    resetFullPath();
    clearQueue();

    g_board[startX][startY].visit();

    while(!g_board[goalX][goalY].visited()){
        floodFillFrom(startX, startY);

        if(g_queueHead == NULL) return 1;

        QueueNode* next = dequeue();
        startX = next->x;
        startY = next->y;
        delete next;
        print();
        usleep(250000);
    }

    uint8_t counter = 0;
    startX = goalX;
    startY = goalY;
    while(startX != originalStartX || startY != originalStartY){
        g_board[startX][startY].c_fullPathPos = counter;
        counter++;
        uint8_t tempX = startX;
        startX = g_board[startX][startY].visitedFromX();
        startY = g_board[tempX][startY].visitedFromY();
    }

    return 0;
}

int main() {
    currentX = 0;
    currentY = 0;
    resetFullPath();
    resetPartPath();
    initializeWalls();
    (int)floodFillMain(currentX, currentY, CENTER_X, CENTER_Y);
    print();
}