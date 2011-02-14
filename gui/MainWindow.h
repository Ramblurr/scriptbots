#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>

#include "../World.h"

class GLDrawer;
class MainWindow : public QMainWindow
{
Q_OBJECT
public:
    explicit MainWindow( World* w, QWidget *parent = 0 );
    ~MainWindow();

//     virtual bool eventFilter( QObject* , QEvent* );
private slots:
    void timeout();
    
private:
    GLDrawer *mGLWidget;
    World *mWorld;
};

#endif // MAINWINDOW_H
