#include "SimulationController.h"

#include "vmath.h"

#include <QtCore/QDebug>
#include <QtCore/QTimer>

SimulationController::SimulationController() : QObject( 0 ), mWorld(0), mModcounter(0), mFrames(0), mSkipdraw(2), mDraw(true)
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
        if( mDraw && mModcounter % mSkipdraw  == 0 ) {
            emit gameState( mWorld->getAgents() );
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

void SimulationController::startSimulation()
{
    if( !mWorld ) {
        qDebug() << "created new world";
        mWorld = new World();
    }
    mTime.restart();
    qDebug() << "unpaused";
    mWorld->setPaused(false);
//     QTimer::singleShot(50, this, SLOT(doDraw()) );
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

