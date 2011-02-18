#include "SimulationController.h"

#include "vmath.h"

#include <QDebug>
#include <QTimer>

SimulationController::SimulationController() : QObject( 0 ), mInitialized(false), mWorld(0), mModcounter(0), mFrames(0), mSkipdraw(2), mDraw(true)
{
    mTime.start();
}

SimulationController::~SimulationController()
{
    //TODO memory cleanup?
}

void SimulationController::doTick()
{
    if( mWorld->isPaused() )
        return;

    if( !mDraw ) {
        mWorld->update();
        ++mModcounter;
        ++mFrames;
        int msec = mTime.elapsed();
        if( msec >= 1000 ) {
            emit ticksPerSecond(mFrames);
            mFrames = 0;
            mTime.restart();
        }
    } else if(!mStack.isEmpty() ) {
        mWorld->update();
        ++mModcounter;
        ++mFrames;
//         if( mDraw /*&& mModcounter % mSkipdraw  == 0*/) {
//             if( !mStack.isEmpty() ) {
        SimState *s = mStack.pop();
        mWorld->getAgents(s->agents );
//      mWorld->getFood(s->food);
        mStateBatch.enqueue(s);
        int msec = mTime.elapsed();
        if( msec >= 1000 ) {
            emit ticksPerSecond(mFrames);
            mFrames = 0;
            mTime.restart();
        }
    }
    QMetaObject::invokeMethod(this, "doTick", Qt::QueuedConnection);
}

void SimulationController::reapState(SimState* state)
{
    mStack.push(state);
}

void SimulationController::sendUpdate()
{
    if( mDraw ) {
        emit simStateBatch(mStateBatch);
        mStateBatch.clear();
        QTimer::singleShot(100, this, SLOT(sendUpdate()) );
    }
}


void SimulationController::startSimulation()
{
    if(!mInitialized) {
        mStack.clear();
//         qDebug() << "Initializing state pool";
        int max = 500;
        for(int i = 0; i < max; ++i) {
            SimState *s = new SimState;
            
            s->agents.resize(60);
            mStack.push(s);
            emit preparingStateBuffer(i, max);
        }
//         qDebug() << "done";
        if( !mWorld ) {
//             qDebug() << "created new world";
            mWorld = new World();
        }
        mInitialized = true;
    }
    mTime.restart();
    qDebug() << "unpaused";
    mWorld->setPaused(false);
    QTimer::singleShot(100, this, SLOT(sendUpdate()) );
    QMetaObject::invokeMethod(this, "doTick", Qt::QueuedConnection);
}

void SimulationController::pauseSimulation()
{
    if(!mInitialized)
        return;
    qDebug() << "pause";
    mWorld->setPaused(true);
}

void SimulationController::resetSimulation()
{
    if(!mInitialized)
        return;
    mWorld->reset();
    mWorld->setPaused(true);
    mModcounter = 0;
    qDebug() << "world reset and paused";
    
}

void SimulationController::toggleDrawing()
{
    if( !mDraw ) {
        QTimer::singleShot(100, this, SLOT( sendUpdate()) );
        mDraw = true;
    } else {
        mDraw = false;
    }
}

void SimulationController::decrementSkip()
{
    mSkipdraw += 1;
}

void SimulationController::incrementSkip()
{
    mSkipdraw -= 1;
}

