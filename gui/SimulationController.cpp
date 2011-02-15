#include "SimulationController.h"

#include "vmath.h"

#include <QtCore/QDebug>
#include <QtCore/QTimer>

SimulationController::SimulationController() : QObject( 0 ), mWorld(0), mModcounter(0), mSkipdraw(2)
{
    
}

SimulationController::~SimulationController()
{

}

void SimulationController::doTick()
{
    if( !mWorld->isPaused()  ) {
        mWorld->update();
        ++mModcounter;
        if( mModcounter % mSkipdraw  == 0 ) {
            emit gameState( mWorld->getAgents() );
        }
        QMetaObject::invokeMethod(this, "doTick", Qt::QueuedConnection);
    }
}

void SimulationController::doDraw()
{
    if( !mWorld->isPaused()  ) {
//         if( mModcounter % mSkipdraw  == 0 ) {
            emit gameState( mWorld->getAgents() );
//         }
    }
    QTimer::singleShot(500, this, SLOT(doDraw()) );
}


void SimulationController::startSimulation()
{
    if( !mWorld ) {
        qDebug() << "created new world";
        mWorld = new World();
    }
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

void SimulationController::drawAgent(const Agent& a)
{
//     qDebug() << "draw agent at" << a.pos.x << a.pos.y;
//     Agent a1(a);
    doDrawAgent(a);
}

void SimulationController::drawFood(int x, int y, float quantity)
{
    emit doDrawFood(x,y,quantity);
}
