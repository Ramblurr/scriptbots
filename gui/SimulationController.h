#ifndef SIMULATIONCONTROLLER_H
#define SIMULATIONCONTROLLER_H

#include "World.h"
#include "View.h"
#include "registertypes.h"

#include <QtCore/QObject>
#include <QtCore/QTime>
#include <QtCore/QQueue>
#include <QtCore/QStack>

/**
 * SimulationController controls the core simulation routine.
 * It is designed to run in its own thread:
 *      SimulationController * controller = new SimulationController;
 *      controller->moveToThread(&mThread);
 *      mThread.start(QThread::HighestPriority);
 * 
 * After it is running it its own thread the simulation can be controlled
 * via signals and slots: start, pause, reset, toggleDrawing, etc.
 * 
 * It emits several signals that report on the state of the simulation.
 * 
 * This class implements a Producer/Consumer queue via signals/slots and
 * passes SimState objects between the GUI and simulation threads. 
 * SimState objects are preallocated and their vector<Agent> members pre-sized
 * then stored in a pool to speed up the storing of states. 
 * This dramatically increases ticks/sec while rendering/caching states.
 * 
 */
class SimulationController : public  QObject
{
Q_OBJECT
public:
    SimulationController();
    virtual ~SimulationController();
    
signals:
    /**
     * A queue of SimState objects.
     * They are in order of computation. Use them to render the world, cache it
     * or whatever.
     * 
     * When you are done with a SimState* return ownership of it to the 
     * controller with the reapState(SimState *state) slot.
     */
    void simStateBatch(QQueue<SimState*> states);
    
    /**
     * The number of simulation ticks per second
     */
    void ticksPerSecond(int tps);

public slots:
    /**
     * Starts the simulation
     * Either unpauses the existing sim or starts a new one.
     * */
    void startSimulation();
    
    /**
     * Pauses the existing simulation
     * This doesn't _just_ disable drawing, it actually pauses
     * the computation of the simulation.
     */
    void pauseSimulation();
    /**
     * Resets the simulation
     * Starts the simulation over
     */
    void resetSimulation();
    /**
     * Disables drawing
     * Turns off drawing while continuing to compute the simulation.
     * This results in massive increase in ticks/second
     */
    void toggleDrawing();
    
    void incrementSkip();
    void decrementSkip();
    
    /**
     * Re-takes ownership of a state object
     * Notifies the Controller that a state object is finished being
     * rendered or cached (or whatever) and it can be reused.
     * 
     * Internally it is added to a stack of SimStates. See the main class
     * blurb to read about how this is used to speed up ticks per second
     * 
     * DO NOT pass a deleted state object here or you will cause a segfault.
     */
    void reapState(SimState *state);

private slots:
    /**
     * Performs one tick of simulation computation
     */
    void doTick();
    /**
     * Sends a batch of SimStates
     */
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
