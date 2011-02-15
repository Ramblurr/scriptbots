#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "World.h"

#include <QtGui/QMainWindow>
#include <QtCore/QThread>

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
    
public slots:
    void slotFpsUpdate(int);
signals:
    void startSimulation();
    void pauseSimulation();
    void resetSimulation();
    void toggleDrawing();

private slots:
    void timeout();
    void slotStart();
    void slotPause();
    void slotReset();
    void slotToggleDrawing();
    
private:
    void setupToolbar();
    
    QToolBar *mToolBar;
    GLDrawer *mGLWidget;
    SimulationController *mController;
    QThread mThread;
};

#endif // MAINWINDOW_H
