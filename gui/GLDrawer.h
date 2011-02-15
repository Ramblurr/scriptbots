#ifndef GLDRAWER_H
#define GLDRAWER_H

#include "World.h"
#include "registertypes.h"

#include <QtOpenGL/QGLWidget>
#include <QtCore/QQueue>
#include <QtCore/QTimer>

struct Food {
    Food(){}
    Food(int ax, int ay, int q) 
    { 
        x = ax;
        y = ay;
        quantity = q;
    }
    Food( const Food & o) {
        x = o.x;
        y = o.y;
        quantity = o.quantity;
    }
    int x;
    int y;
    float quantity;
};

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
    
public slots:
    void storeState(const std::vector<Agent> &agents);
    void incrementSkip();
    void decrementSkip();
    
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
    
    QQueue<std::vector< Agent > > mStateQueue;
    QTimer mUpdateTimer;
};

#endif // GLDRAWER_H
