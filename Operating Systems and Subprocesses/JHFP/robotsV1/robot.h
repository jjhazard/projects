#ifndef ROBOT_H
#define ROBOT_H

#include <vector>
#include <pthread.h>

struct door{
    int doorID; //related to color
    int doorX;
    int doorY;
};

struct robotAndBox{
    //coordinates of robot and his box
    int robX;
    int robY;
    int boxX;
    int boxY;
    door* assignedDoor;
};

struct gridWorld{
    std::vector<robotAndBox*> robotList;
    std::vector<door*> doorList;
    pthread_mutex_t lock;
    int** grid;
    int numCols; //height
    int numRows; //width
    FILE *fp;
};

struct robotThread{
    pthread_t id; //id of robot
    int threadIndex;
    gridWorld* grid; //pointer to the world
    robotAndBox* robox;
    bool isActive;
};

bool push(struct robotThread* data, int dest[2]);
bool move(struct robotThread* data, int dest[2], bool flag);
void write(robotThread* t, char* func, char dir);
void* robotFunc(void* data);

#endif
