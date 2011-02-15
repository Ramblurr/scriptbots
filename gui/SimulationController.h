#ifndef SIMULATIONCONTROLLER_H
#define SIMULATIONCONTROLLER_H

#include "World.h"
#include "View.h"
#include "registertypes.h"

#include <QtCore/QObject>
#include <QtCore/QTime>

class SimulationController : public  QObject
{
Q_OBJECT
public:
    SimulationController();
    virtual ~SimulationController();
    
signals:
    void simState(SimState *state);
    void fps(int fps);

public slots:
    void startSimulation();
    void pauseSimulation();
    void resetSimulation();
    void toggleDrawing();

private slots:
    void doTick();
    
private:
    
    bool mDraw;
    World *mWorld;
    int mModcounter;
    int mFrames;
    int mSkipdraw;
    QTime mTime;
};

#endif // SIMULATIONCONTROLLER_H
