#ifndef GLDRAWER_H
#define GLDRAWER_H

#include "World.h"
#include "registertypes.h"

#include <QtOpenGL/QGLWidget>
#include <QtCore/QQueue>
#include <QtCore/QTime>


Q_DECLARE_METATYPE(Food);

class GLDrawer : public QGLWidget
{
Q_OBJECT
public:
    GLDrawer(QWidget *parent);

    virtual QSize minimumSizeHint() const;
    virtual QSize sizeHint() const;
    void drawAgent(const Agent &a);
    void drawFood(int x, int y, float quantity);

signals:
    void finished(SimState *state);

public slots:
    void storeStates(QQueue<SimState*> states);
    void incrementSkip();
    void decrementSkip();
    void toggleDrawing();
    
protected:
    virtual void initializeGL();
    virtual void resizeGL(int w, int h);
    virtual void paintGL();

private:

    bool paused;
    bool draw;
    int skipdraw;
    bool drawfood;
    char buf[100];
    char buf2[10];
    int modcounter;
    int lastUpdate;
    int frames;
    
    QQueue<SimState*> mStateQueue;
    QTime mTime;
};

#endif // GLDRAWER_H
