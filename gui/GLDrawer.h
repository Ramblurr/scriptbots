#ifndef GLDRAWER_H
#define GLDRAWER_H

#include <QtOpenGL/qgl.h>

#include "World.h"
#include "View.h"


class GLDrawer : public QGLWidget, public View
{
public:
    GLDrawer(QWidget *parent);

    virtual QSize minimumSizeHint() const;
    virtual QSize sizeHint() const;
    
    virtual void drawAgent(const Agent &a);
    virtual void drawFood(int x, int y, float quantity);
    
    void setWorld(World* w);
    
protected:
    virtual void initializeGL();
    virtual void resizeGL(int w, int h);
    virtual void paintGL();
private:

    World *world;
    bool paused;
    bool draw;
    int skipdraw;
    bool drawfood;
    char buf[100];
    char buf2[10];
    int modcounter;
    int lastUpdate;
    int frames;
};

#endif // GLDRAWER_H
