#include <iostream>
#include <unistd.h>

using namespace std;

#define WIDTH 16
#define HEIGHT 16

#define CENTER_X 13
#define CENTER_Y 4

#define START_X 0
#define START_Y 0

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

    void setVisitedFromX(const uint8_t& x) {
        c_visitedFrom = (c_visitedFrom & 240) | x;
    };

    void setVisitedFromY(const uint8_t& y) {
        c_visitedFrom = (c_visitedFrom & 15) | (y << 4);
    };
};

class QueueNode {
private:
    uint8_t pos;

public:
    QueueNode *prev;

    QueueNode(uint8_t x, uint8_t y){
        pos = (pos & 240) | x;
        pos = (pos & 15) | (y << 4);
        prev = NULL;
    }

    uint8_t x(){
        return pos & 15;
    }

    uint8_t y(){
        return (pos >> 4) & 15;
    }
};

Space g_board[WIDTH][HEIGHT];
QueueNode *g_queueHead;
QueueNode *g_queueTail;
uint8_t g_currentX;
uint8_t g_currentY;

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

void unvisitAll(){
    for(int i = 0; i < WIDTH; i++){
        for(int j = 0; j < HEIGHT; j++){
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
    char blue[] = { 0x1b, '[', '4', '4', 'm', 0 };
    char red[] = { 0x1b, '[', '4', '1', 'm', 0 };
    char yellow[] = { 0x1b, '[', '4', '3', 'm', 0 };
    char cyan[] = { 0x1b, '[', '4', '6', 'm', 0 };
    char magenta[] = { 0x1b, '[', '4', '5', 'm', 0 };
    char green[] = { 0x1b, '[', '4', '2', 'm', 0 };
    char underline[] = {0x1b, '[', '4', 'm', 0};
    char noColor[] = { 0x1b, '[', '4', '9', 'm', 0};
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
            if(x == g_currentX && y == g_currentY) cout << green;
            else{
                if (x == START_X && y == START_Y) cout << yellow;
                else {
                    if (x == CENTER_X && y == CENTER_Y) cout << red;
                    else {
                        if (g_board[x][y].c_fullPathPos != 0) cout << yellow;
                        else {
                            if (g_board[x][y].c_partPathPos != 0) cout << cyan;
                            else {
                                if (g_board[x][y].visited()) cout << blue;
                            }
                        }
                    }
                }
            }
            if(g_board[x][y].southWall()) cout << underline;
            cout << (x == g_currentX && y == g_currentY ? 'X' : 'O');
            cout << noColor;
            if(g_board[x][y].eastWall()) cout << '|';
            else cout << ' ';
            cout << noFormat;
        }
        cout << endl;
    }
    cout << endl;
    usleep(50000);
}

QueueNode* dequeue(){
    QueueNode* res = g_queueHead;
    if(res != NULL) g_queueHead = g_queueHead->prev;
    if(g_queueHead == NULL) g_queueTail = NULL;
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

uint8_t moveTo(const uint8_t& x, const uint8_t& y){
    g_currentX = x;
    g_currentY = y;
    print();
    return 0;
}

QueueNode nextInMainPath(const uint8_t& x, const uint8_t& y){
    uint8_t nextInd = g_board[x][y].c_fullPathPos - 1;
    if(nextInd == 0) return QueueNode(CENTER_X, CENTER_Y);
    if(g_board[x - 1][y].c_fullPathPos == nextInd) return QueueNode(x - 1, y);
    if(g_board[x + 1][y].c_fullPathPos == nextInd) return QueueNode(x + 1, y);
    if(g_board[x][y - 1].c_fullPathPos == nextInd) return QueueNode(x, y - 1);
    if(g_board[x][y + 1].c_fullPathPos == nextInd) return QueueNode(x, y + 1);
}

QueueNode prevInMainPath(const uint8_t& x, const uint8_t& y){
    uint8_t nextInd = g_board[x][y].c_fullPathPos + 1;
    if(nextInd == 0) return QueueNode(CENTER_X, CENTER_Y);
    if(g_board[x - 1][y].c_fullPathPos == nextInd) return QueueNode(x - 1, y);
    if(g_board[x + 1][y].c_fullPathPos == nextInd) return QueueNode(x + 1, y);
    if(g_board[x][y - 1].c_fullPathPos == nextInd) return QueueNode(x, y - 1);
    if(g_board[x][y + 1].c_fullPathPos == nextInd) return QueueNode(x, y + 1);
}

bool pointInMainPath(const uint8_t& x, const uint8_t& y){
    uint8_t currentX = START_X;
    uint8_t currentY = START_Y;
    if(currentX == x && currentY == y) return true;

    while(currentX != CENTER_X || currentY != CENTER_Y){
        QueueNode nextPoint = nextInMainPath(currentX, currentY);
        currentX = nextPoint.x();
        currentY = nextPoint.y();
        if(currentX == x && currentY == y) return true;
    }
    return false;
}

QueueNode nextInPartPath(const uint8_t& x, const uint8_t& y){
    uint8_t nextInd = g_board[x][y].c_partPathPos - 1;
    if(nextInd == 0){
        if(pointInMainPath(x - 1, y) && !g_board[x][y].westWall()) return QueueNode(x - 1, y);
        if(pointInMainPath(x + 1, y) && !g_board[x][y].eastWall()) return QueueNode(x + 1, y);
        if(pointInMainPath(x, y - 1) && !g_board[x][y].northWall()) return QueueNode(x, y - 1);
        if(pointInMainPath(x, y + 1) && !g_board[x][y].southWall()) return QueueNode(x, y + 1);
    }
    if(g_board[x - 1][y].c_partPathPos == nextInd) return QueueNode(x - 1, y);
    if(g_board[x + 1][y].c_partPathPos == nextInd) return QueueNode(x + 1, y);
    if(g_board[x][y - 1].c_partPathPos == nextInd) return QueueNode(x, y - 1);
    if(g_board[x][y + 1].c_partPathPos == nextInd) return QueueNode(x, y + 1);
}

uint8_t followMain(){
    while(g_currentX != CENTER_X || g_currentY != CENTER_Y) {
        QueueNode next = nextInMainPath(g_currentX, g_currentY);
        if (moveTo(next.x(), next.y()) != 0) return 1;
    }
    return 0;
}

uint8_t followMainBack(){
    while(g_currentX != START_X || g_currentY != START_Y) {
        QueueNode next = prevInMainPath(g_currentX, g_currentY);
        if (moveTo(next.x(), next.y()) != 0) return 1;
    }
    return 0;
}

uint8_t followPart(){
    while(!pointInMainPath(g_currentX, g_currentY)) {
        QueueNode next = nextInPartPath(g_currentX, g_currentY);
        if (moveTo(next.x(), next.y()) != 0) return 1;
    }
    return 0;
}

void floodFillFrom(const uint8_t& startX, const uint8_t& startY){
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

uint8_t floodFillMain(){
    uint8_t currentX = START_X;
    uint8_t currentY = START_Y;

    resetFullPath();
    clearQueue();

    g_board[START_X][START_Y].visit();

    while(!g_board[CENTER_X][CENTER_Y].visited()){
        floodFillFrom(currentX, currentY);

        if(g_queueHead == NULL) return 1;

        QueueNode* next = dequeue();
        currentX = next->x();
        currentY = next->y();
        delete next;
        print();
    }

    uint8_t counter = 0;
    currentX = CENTER_X;
    currentY = CENTER_Y;
    while(currentX != START_X || currentY != START_Y){
        g_board[currentX][currentY].c_fullPathPos = counter;
        counter++;
        uint8_t tempX = currentX;
        currentX = g_board[currentX][currentY].visitedFromX();
        currentY = g_board[tempX][currentY].visitedFromY();
    }
    g_board[currentX][currentY].c_fullPathPos = counter;

    return 0;
}

uint8_t floodFillPartial(){
    uint8_t currentX = g_currentX;
    uint8_t currentY = g_currentY;

    resetPartPath();
    clearQueue();

    g_board[currentX][currentY].visit();

    while(!pointInMainPath(currentX, currentY)){
        floodFillFrom(currentX, currentY);

        if(g_queueHead == NULL) return 1;

        QueueNode* next = dequeue();
        currentX = next->x();
        currentY = next->y();
        delete next;
        print();
    }

    uint8_t counter = 0;
    while(currentX != g_currentX || currentY != g_currentY){
        g_board[currentX][currentY].c_partPathPos = counter;
        counter++;
        uint8_t tempX = currentX;
        currentX = g_board[currentX][currentY].visitedFromX();
        currentY = g_board[tempX][currentY].visitedFromY();
    }
    g_board[currentX][currentY].c_partPathPos = counter;

    return 0;
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

int main() {
    g_currentX = 5;
    g_currentY = 10;
    initializeWalls();

    g_board[3][4].hasEastWall();
    g_board[4][4].hasWestWall();
    g_board[5][5].hasSouthWall();
    g_board[5][6].hasNorthWall();

    while(true) {
        resetFullPath();
        resetPartPath();
        if(floodFillMain() != 0){
            cout << "Could not flood fill main" << endl;
            return 1;
        }
        print();
        if(floodFillPartial() != 0){
            cout << "Could not flood fill part" << endl;
            return 1;
        }
        print();
        unvisitAll();
        if(followPart() != 0){
            cout << "Could not follow part" << endl;
            return 1;
        }
        if(followMain() != 0) {
            cout << "Could not follow main" << endl;
            return 1;
        }
        if(followMainBack() == 0) break;
        else {
            cout << "Could not follow main back" << endl;
            return 1;
        }
    }
    print();
}