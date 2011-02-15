#include "SimulationController.h"

#include "vmath.h"

#include <QtCore/QDebug>
#include <QtCore/QTimer>

SimulationController::SimulationController() : QObject( 0 ), mInitialized(false), mWorld(0), mModcounter(0), mFrames(0), mSkipdraw(2), mDraw(true)
{
    mTime.start();
}

SimulationController::~SimulationController()
{

}

void SimulationController::doTick()
{
//     while(1) {
    if( !mWorld->isPaused()  ) {
        mWorld->update();
        ++mModcounter;
        ++mFrames;
        if( mDraw /*&& mModcounter % mSkipdraw  == 0*/) {
            SimState *s  = 0;
            if( mStack.isEmpty() ) {
//                 qDebug() << "SimStack stack is empty..";
                if( mStack.size() < 10000 )
                    s = new SimState;
            } else {
                s = mStack.pop();
            }
            if(s) {
                mWorld->getAgents(s->agents );
        //             mWorld->getFood(s->food);
                mStateBatch.enqueue(s);
            }
        }
        int msec = mTime.elapsed();
        if( msec >= 1000 ) {
//             emit fps(mFrames);
            qDebug() << "fps:" << mFrames;
            mFrames = 0;
            mTime.restart();
        }
        QMetaObject::invokeMethod(this, "doTick", Qt::QueuedConnection);
    } 
}

void SimulationController::reapState(SimState* state)
{
    mStack.push(state);
}

void SimulationController::sendUpdate()
{
    emit simStateBatch(mStateBatch);
    mStateBatch.clear();
    QTimer::singleShot(100, this, SLOT(sendUpdate()) );
}


void SimulationController::startSimulation()
{
    if(!mInitialized) {
        mStack.clear();
        qDebug() << "Initializing state pool";
        int max = 500;
        for(int i = 0; i < max; ++i) {
            if( i% 10 == 0 )
                qDebug() << "  making " << i << "/" <<max;
            SimState *s = new SimState;
            s->agents.resize(60);
            mStack.push(s);
        }
        qDebug() << "done";
        if( !mWorld ) {
            qDebug() << "created new world";
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
    qDebug() << "pause";
    mWorld->setPaused(true);
}

void SimulationController::resetSimulation()
{
    mWorld->reset();
    mWorld->setPaused(true);
    mModcounter = 0;
    qDebug() << "world reset and paused";
    
}

void SimulationController::toggleDrawing()
{
    mDraw = !mDraw;
}

