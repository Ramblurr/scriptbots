#ifndef GLVIEW_H
#define GLVIEW_H


#include "View.h"
#include "World.h"

class GLView;

extern GLView* GLVIEW;

void gl_processNormalKeys(unsigned char key, int x, int y);
void gl_processMouse(int button, int state, int x, int y);
void gl_processMouseActiveMotion(int x, int y);
void gl_changeSize(int w, int h);
void gl_handleIdle();
void gl_renderScene();

class GLView : public View
{

public:
    GLView(World* w);
    virtual ~GLView();
    
    virtual void drawAgent(const Agent &a);
    
    void setWorld(World* w);
    
    //GLUT functions
    void processNormalKeys(unsigned char key, int x, int y);
    void processMouse(int button, int state, int x, int y);
    void processMouseActiveMotion(int x, int y);
    void changeSize(int w, int h);
    void handleIdle();
    void renderScene();
    
private:
    
    World *world;
    bool paused;
    bool draw;
    int skipdraw;
    bool drawFood;
    char buf2[10];
    int modcounter;
    
};

#endif // GLVIEW_H
