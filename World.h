#ifndef WORLD_H
#define WORLD_H

#include "Agent.h"
#include "settings.h"
#include <vector>

class World
{
public:
    World();
    ~World();
private:
    void update();
    void setInputs();
    void processOutputs();
    void brainsTick();  //takes in[] to out[] for every agent
    
    void writeReport();
    
    void reproduce(int ai, float MR, float MR2);
    void addNewByCrossover();
    void addRandomBots(int num);
    
    
    int modcounter;
    int epoch;
    int idcounter;
    
    std::vector<Agent> agents;
    
    // food
    int FW;
    int FH;
    int fx;
    int fy;
    float food[conf::WIDTH/conf::CZ][conf::HEIGHT/conf::CZ];
    int CLOSED; //if environment is closed, then no random bots are added per time interval
};

#endif // WORLD_H
