#include "SimulationController.h"

#include <QtCore/QDebug>
SimulationController::SimulationController(QObject* parent) : mWorld(0)
{
    
}

SimulationController::~SimulationController()
{

}

void SimulationController::startSimulation()
{
//     if( !mWorld )
//         mWorld = new World();
    qDebug() << "starT";
}

void SimulationController::pauseSimulation()
{
qDebug() << "pause";
}

void SimulationController::resetSimulation()
{
qDebug() << "reset";
}
