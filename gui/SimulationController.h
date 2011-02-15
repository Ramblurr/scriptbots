#ifndef SIMULATIONCONTROLLER_H
#define SIMULATIONCONTROLLER_H

#include "World.h"
#include "View.h"
#include "registertypes.h"

#include <QtCore/QObject>


class SimulationController : public  QObject, public View
{
Q_OBJECT
public:
    SimulationController();
    virtual ~SimulationController();
    
    virtual void drawAgent(const Agent& a);
    virtual void drawFood(int x, int y, float quantity);

signals:
    void doDrawAgent(const Agent & a);
    void gameState(const std::vector<Agent> &agents);
    void doDrawFood(int x, int y, float quantity);

public slots:
    void startSimulation();
    void pauseSimulation();
    void resetSimulation();

private slots:
    void doDraw();
    void doTick();
    
private:
    
    World *mWorld;
    int mModcounter;
    int mSkipdraw;
};

#endif // SIMULATIONCONTROLLER_H
