#include "MainWindow.h"

#include "GLDrawer.h"
#include "SimulationController.h"

#include <QtCore/QTimer>
#include <QtCore/QDebug>
#include <QtGui/QMenuBar>
#include <QtGui/QToolBar>
#include <QtGui/QAction>
#include <QtGui/QApplication>

void doQt(int argc, char **argv) {
    
    QApplication app( argc, argv );
    MainWindow foo;
    foo.show();
    app.exec();
}

MainWindow::MainWindow( QWidget *parent )  : QMainWindow(parent)
{
//     mGLWidget = new GLDrawer(this);

    mToolBar = new QToolBar(this);
    setupToolbar();
    
    mController = new SimulationController;
    
    connect( this,        SIGNAL( startSimulation() ), 
              mController, SLOT(   startSimulation() ) );
    
    connect( this,        SIGNAL( pauseSimulation()), 
              mController, SLOT(   pauseSimulation() ) );
    
    connect( this,        SIGNAL( resetSimulation() ), 
              mController, SLOT(   resetSimulation() ) );
    
    mController->moveToThread(&mThread);
    mThread.start();
    
//     setCentralWidget(mGLWidget);
    
//     QTimer* t = new QTimer( this );
//     t->start( 50 );
//     connect( t, SIGNAL( timeout(  ) ),
//                    this, SLOT( timeout(  ) ) );
}

MainWindow::~MainWindow() 
{
}

void MainWindow::setupToolbar()
{
    QAction* runAct = mToolBar->addAction( 
                                QIcon::fromTheme( "media-playback-start" ), 
                                tr( "&Start Simulation" ), 
                                this, 
                                SLOT( slotStart() ) );
    QAction* pauseAct = mToolBar->addAction( 
                                QIcon::fromTheme( "media-playback-pause" ), 
                                tr( "&Pause Simulation" ), 
                                this, 
                                SLOT( slotPause() ) );
    QAction* resetAct = mToolBar->addAction( 
                                QIcon::fromTheme( "view-refresh" ), 
                                tr( "&Reset Simulation" ), 
                                this, 
                                SLOT( slotReset() ) );
    mToolBar->addAction(runAct);
    mToolBar->addAction(pauseAct);
    mToolBar->addAction(resetAct);
//     menuBar()->addMenu("&Hi");
    
}


void MainWindow::timeout()
{
//     mGLWidget->updateGL();
}

void MainWindow::slotStart()
{
    emit startSimulation();
}

void MainWindow::slotPause()
{
    emit pauseSimulation();
}

void MainWindow::slotReset()
{
    emit resetSimulation();
}

// #include "MainWindow.moc"