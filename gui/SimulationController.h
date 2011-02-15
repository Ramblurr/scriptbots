#ifndef SIMULATIONCONTROLLER_H
#define SIMULATIONCONTROLLER_H

#include "World.h"
#include "View.h"
#include "registertypes.h"

#include <QtCore/QObject>
#include <QtCore/QTime>
#include <QtCore/QQueue>
#include <QtCore/QStack>

class SimulationController : public  QObject
{
Q_OBJECT
public:
    SimulationController();
    virtual ~SimulationController();
    
signals:
    void simState(SimState *state);
    void simStateBatch(QQueue<SimState*> states);
    void fps(int fps);

public slots:
    void startSimulation();
    void pauseSimulation();
    void resetSimulation();
    void toggleDrawing();
    void reapState(SimState *state);

private slots:
    void doTick();
    void sendUpdate();
    
private:
    
    bool mInitialized;
    bool mDraw;
    World *mWorld;
    int mModcounter;
    int mFrames;
    int mSkipdraw;
    QTime mTime;
    QQueue<SimState*> mStateBatch;
    QStack<SimState*> mStack;
};

#endif // SIMULATIONCONTROLLER_H
