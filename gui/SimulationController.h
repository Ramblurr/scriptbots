#ifndef SIMULATIONCONTROLLER_H
#define SIMULATIONCONTROLLER_H

#include "World.h"

#include <QtCore/QObject>


class SimulationController : public QObject
{
Q_OBJECT
public:
    explicit SimulationController(QObject* parent = 0);
    virtual ~SimulationController();

public slots:
    void startSimulation();
    void pauseSimulation();
    void resetSimulation();
private:
    
    World *mWorld;
};

#endif // SIMULATIONCONTROLLER_H
