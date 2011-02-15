#include "MainWindow.h"

#include "GLDrawer.h"
#include "SimulationController.h"
#include "Agent.h"
#include "registertypes.h"

#include <QtCore/QTimer>
#include <QtCore/QDebug>
#include <QtGui/QMenuBar>
#include <QtGui/QToolBar>
#include <QtGui/QAction>
#include <QtGui/QApplication>

void doQt(int argc, char **argv) 
{
    qRegisterMetaType<Agent>();
    qRegisterMetaType<Food>();
    qRegisterMetaType<std::vector<Agent> >();
    QApplication app( argc, argv );
    MainWindow foo;
    foo.show();
    app.exec();
}

MainWindow::MainWindow( QWidget *parent )  : QMainWindow(parent)
{
    mGLWidget = new GLDrawer(this);
    setCentralWidget(mGLWidget);

    mToolBar = new QToolBar(this);
    addToolBar(mToolBar);
    setupToolbar();
    
    mController = new SimulationController;
    
    connect( this,        SIGNAL( startSimulation() ), 
              mController, SLOT(   startSimulation() ), Qt::QueuedConnection );
    
    connect( this,        SIGNAL( pauseSimulation()), 
              mController, SLOT(   pauseSimulation() ), Qt::QueuedConnection );
    
    connect( this,        SIGNAL( resetSimulation() ), 
              mController, SLOT(   resetSimulation() ), Qt::QueuedConnection );

    connect( mController, SIGNAL( gameState(std::vector<Agent>) ), 
              mGLWidget,   SLOT(   storeState(std::vector<Agent> ) ), Qt::QueuedConnection );
    
    /*connect( mController, SIGNAL( doDrawAgent( Agent ) ),
              mGLWidget,   SLOT(   storeAgent ( Agent ) ), Qt::QueuedConnection );

    connect( mController, SIGNAL( doDrawFood(int,int,float) ),
              mGLWidget,   SLOT(   storeFood(int,int,float) ) );*/
    
    mController->moveToThread(&mThread);
    mThread.start();
    
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
    qDebug() << "draw agent at";
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