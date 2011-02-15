#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QtCore/QThread>
#include "World.h"

extern void doQt(int argc, char **argv);

class GLDrawer;
class SimulationController;

class QToolBar;

class MainWindow : public QMainWindow
{
Q_OBJECT
public:
    explicit MainWindow( QWidget* parent = 0 );
    ~MainWindow();

//     virtual bool eventFilter( QObject* , QEvent* );
signals:
    void startSimulation();
    void pauseSimulation();
    void resetSimulation();

private slots:
    void timeout();
    void slotStart();
    void slotPause();
    void slotReset();
    
private:
    void setupToolbar();
    
    QToolBar *mToolBar;
    GLDrawer *mGLWidget;
    SimulationController *mController;
    QThread mThread;
};

#endif // MAINWINDOW_H
