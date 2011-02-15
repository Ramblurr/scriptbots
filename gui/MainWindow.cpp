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
#include <QtCore/QQueue>

void doQt(int argc, char **argv) 
{
    qRegisterMetaType<Agent>();
    qRegisterMetaType<Food>();
    qRegisterMetaType<std::vector<Agent> >();
    qRegisterMetaType<QQueue<SimState*> >("QQueue<SimState*>");
//     qRegisterMetaType<QQueue<Agent> >();
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

    connect( this,        SIGNAL( toggleDrawing() ), 
              mController, SLOT(   toggleDrawing() ), Qt::QueuedConnection );
    
    connect( mController, SIGNAL( simStateBatch( QQueue<SimState*> ) ),
              mGLWidget,   SLOT(   storeStates( QQueue<SimState*> ) ), Qt::QueuedConnection );
    
    connect( mController, SIGNAL( fps(int) ), 
              this,   SLOT( slotFpsUpdate(int) ), Qt::QueuedConnection );
    
    connect( mGLWidget, SIGNAL(   finished( SimState* ) ),
              mController,   SLOT( reapState(SimState* ) ), Qt::QueuedConnection );
    
    mController->moveToThread(&mThread);
    mThread.start(QThread::HighestPriority);
    
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
    QAction* toggleDrawAct = mToolBar->addAction( 
                                QIcon::fromTheme( "media-seek-forward" ), 
                                tr( "&Toggle Drawing" ), 
                                this, 
                                SLOT( slotToggleDrawing() ) );
    
    QAction* incrSkipAct = mToolBar->addAction( 
                                QIcon::fromTheme( "list-add" ), 
                                tr( "&Increment Skip" ), 
                                mGLWidget, 
                                SLOT( incrementSkip() ) );
    
    QAction* decrSkipAct = mToolBar->addAction( 
                                QIcon::fromTheme( "list-remove" ), 
                                tr( "&Decrement Skip" ), 
                                mGLWidget, 
                                SLOT( decrementSkip() ) );
    
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

void MainWindow::slotToggleDrawing()
{
    emit toggleDrawing();
}

void MainWindow::slotFpsUpdate(int fps)
{
//     qDebug() << "fps:" << fps;
    QString title = "FPS: " + QString::number(fps);
    setWindowTitle(title);
}


// #include "MainWindow.moc"