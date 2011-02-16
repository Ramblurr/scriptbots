#include "GLDrawer.h"

#include <math.h>

#include "settings.h"

#include <QtCore/QDebug>
#include <QtCore/QTime>
#include <QtCore/QTimer>
#include <QtCore/QCoreApplication>

void drawCircle2(float x, float y, float r) {
    float n;
    for (int k=0;k<17;k++) {
        n = k*(3.14/8);
        glVertex3f(x+r*sin(n),y+r*cos(n),0);
    }
}

GLDrawer::GLDrawer(QWidget* parent): QGLWidget(parent), drawfood(true), modcounter(0), skipdraw(1), frames(0), draw(true)
{
//     mUpdateTimer.setInterval(33);
//     connect(&mUpdateTimer, SIGNAL(timeout()), SLOT(updateGL()));
    QTimer::singleShot(20, this, SLOT(updateGL()));
    mTime.start();
}

QSize GLDrawer::minimumSizeHint() const
{
    return QSize(500,500);
}

QSize GLDrawer::sizeHint() const
{
    return QSize(conf::WWIDTH,conf::WHEIGHT);
}


void GLDrawer::initializeGL()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glClearColor(0.9f, 0.9f, 1.0f, 0.0f);
    glOrtho(0,conf::WWIDTH,conf::WHEIGHT,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
}

void GLDrawer::resizeGL(int w, int h)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glClearColor(0.9f, 0.9f, 1.0f, 0.0f);
    glOrtho(0,conf::WWIDTH,conf::WHEIGHT,0,0,1);
    glViewport(0,0,(GLsizei) w, (GLsizei) h);
}

void GLDrawer::paintGL()
{
    if( !draw )
        return;
    QTime now;
    now.start();
    ++modcounter;
    ++frames;
    if(!mStateQueue.isEmpty()) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glPushMatrix();
        SimState* s = mStateQueue.dequeue();

        int len = s->food.size();
        for(int i=0; i < len ; ++i) {
            drawFood(s->food[i].x, s->food[i].y, s->food[i].quantity);
        }

        len = s->agents.size();
        for(int i=0; i< len; ++i) {
            drawAgent( s->agents[i] );
        }
        emit finished(s);
        glPopMatrix();
    }
    int msec = mTime.elapsed();
    if( msec >= 1000 ) {
        qDebug() << "FPS: " << frames;
        mTime.restart();
        frames = 0;
    }
    QTimer::singleShot(qMax(0,100-now.elapsed()), this, SLOT(updateGL())); //should get around 50 FPS
}

void GLDrawer::drawAgent(const Agent& agent)
{
    float n;
    float r= conf::BOTRADIUS;
    float rp= conf::BOTRADIUS+2;
    //handle selected agent
    if (agent.selectflag>0) {

        //draw selection
        glBegin(GL_POLYGON);
        glColor3f(1,1,0);
        drawCircle2(agent.pos.x, agent.pos.y, conf::BOTRADIUS+5);
        glEnd();

        glPushMatrix();
        glTranslatef(agent.pos.x-80,agent.pos.y+20,0);
        //draw inputs, outputs
        float col;
        float yy=15;
        float xx=15;
        float ss=16;
        glBegin(GL_QUADS);
        for (int j=0;j<INPUTSIZE;j++) {
            col= agent.in[j];
            glColor3f(col,col,col);
            glVertex3f(0+ss*j, 0, 0.0f);
            glVertex3f(xx+ss*j, 0, 0.0f);
            glVertex3f(xx+ss*j, yy, 0.0f);
            glVertex3f(0+ss*j, yy, 0.0f);
        }
        yy+=5;
        for (int j=0;j<OUTPUTSIZE;j++) {
            col= agent.out[j];
            glColor3f(col,col,col);
            glVertex3f(0+ss*j, yy, 0.0f);
            glVertex3f(xx+ss*j, yy, 0.0f);
            glVertex3f(xx+ss*j, yy+ss, 0.0f);
            glVertex3f(0+ss*j, yy+ss, 0.0f);
        }
        yy+=ss*2;

        //draw brain. Eventually move this to brain class?
        float offx=0;
        ss=8;
        for (int j=0;j<BRAINSIZE;j++) {
            col= agent.brain.boxes[j].out;
            glColor3f(col,col,col);
            glVertex3f(offx+0+ss*j, yy, 0.0f);
            glVertex3f(offx+xx+ss*j, yy, 0.0f);
            glVertex3f(offx+xx+ss*j, yy+ss, 0.0f);
            glVertex3f(offx+ss*j, yy+ss, 0.0f);
            if ((j+1)%30==0) {
                yy+=ss;
                offx-=ss*30;
            }
        }

        glEnd();
        glPopMatrix();
    }

    //draw giving/receiving
    if(agent.dfood!=0){
        glBegin(GL_POLYGON);
        float mag=cap(abs(agent.dfood)/conf::FOODTRANSFER/3);
        if(agent.dfood>0) glColor3f(0,mag,0); //draw boost as green outline
        else glColor3f(mag,0,0);
        for (int k=0;k<17;k++){
            n = k*(M_PI/8);
            glVertex3f(agent.pos.x+rp*sin(n),agent.pos.y+rp*cos(n),0);
            n = (k+1)*(M_PI/8);
            glVertex3f(agent.pos.x+rp*sin(n),agent.pos.y+rp*cos(n),0);
        }
        glEnd();
    }

    //TODO Fix event drawing
    //draw indicator of this agent... used for various events
//     if (agent.indicator>0) {
//         glBegin(GL_POLYGON);
//         glColor3f(agent.ir,agent.ig,agent.ib);
//         drawCircle2(agent.pos.x, agent.pos.y, conf::BOTRADIUS+((int)agent.indicator));
//         glEnd();
//         agent.indicator-=1;
//     }

    //viewcone of this agent
    glBegin(GL_LINES);
    //and view cones
    glColor3f(0.5,0.5,0.5);
    for (int j=-2;j<3;j++) {
        if (j==0)continue;
        glVertex3f(agent.pos.x,agent.pos.y,0);
        glVertex3f(agent.pos.x+(conf::BOTRADIUS*4)*cos(agent.angle+j*M_PI/8),agent.pos.y+(conf::BOTRADIUS*4)*sin(agent.angle+j*M_PI/8),0);
    }
    //and eye to the back
    glVertex3f(agent.pos.x,agent.pos.y,0);
    glVertex3f(agent.pos.x+(conf::BOTRADIUS*1.5)*cos(agent.angle+M_PI+3*M_PI/16),agent.pos.y+(conf::BOTRADIUS*1.5)*sin(agent.angle+M_PI+3*M_PI/16),0);
    glVertex3f(agent.pos.x,agent.pos.y,0);
    glVertex3f(agent.pos.x+(conf::BOTRADIUS*1.5)*cos(agent.angle+M_PI-3*M_PI/16),agent.pos.y+(conf::BOTRADIUS*1.5)*sin(agent.angle+M_PI-3*M_PI/16),0);
    glEnd();

    glBegin(GL_POLYGON); //body
    glColor3f(agent.red,agent.gre,agent.blu);
    drawCircle2(agent.pos.x, agent.pos.y, conf::BOTRADIUS);
    glEnd();

    glBegin(GL_LINES);
    //outline
    if (agent.boost) glColor3f(0.8,0,0); //draw boost as green outline
    else glColor3f(0,0,0);

    for (int k=0;k<17;k++)
    {
        n = k*(M_PI/8);
        glVertex3f(agent.pos.x+r*sin(n),agent.pos.y+r*cos(n),0);
        n = (k+1)*(M_PI/8);
        glVertex3f(agent.pos.x+r*sin(n),agent.pos.y+r*cos(n),0);
    }
    //and spike
    glColor3f(0.5,0,0);
    glVertex3f(agent.pos.x,agent.pos.y,0);
    glVertex3f(agent.pos.x+(3*r*agent.spikeLength)*cos(agent.angle),agent.pos.y+(3*r*agent.spikeLength)*sin(agent.angle),0);
    glEnd();

    //and health
    int xo=18;
    int yo=-15;
    glBegin(GL_QUADS);
    //black background
    glColor3f(0,0,0);
    glVertex3f(agent.pos.x+xo,agent.pos.y+yo,0);
    glVertex3f(agent.pos.x+xo+5,agent.pos.y+yo,0);
    glVertex3f(agent.pos.x+xo+5,agent.pos.y+yo+40,0);
    glVertex3f(agent.pos.x+xo,agent.pos.y+yo+40,0);

    //health
    glColor3f(0,0.8,0);
    glVertex3f(agent.pos.x+xo,agent.pos.y+yo+20*(2-agent.health),0);
    glVertex3f(agent.pos.x+xo+5,agent.pos.y+yo+20*(2-agent.health),0);
    glVertex3f(agent.pos.x+xo+5,agent.pos.y+yo+40,0);
    glVertex3f(agent.pos.x+xo,agent.pos.y+yo+40,0);

    //if this is a hybrid, we want to put a marker down
    if (agent.hybrid) {
        glColor3f(0,0,0.8);
        glVertex3f(agent.pos.x+xo+6,agent.pos.y+yo,0);
        glVertex3f(agent.pos.x+xo+12,agent.pos.y+yo,0);
        glVertex3f(agent.pos.x+xo+12,agent.pos.y+yo+10,0);
        glVertex3f(agent.pos.x+xo+6,agent.pos.y+yo+10,0);
    }

    glColor3f(1-agent.herbivore,agent.herbivore,0);
    glVertex3f(agent.pos.x+xo+6,agent.pos.y+yo+12,0);
    glVertex3f(agent.pos.x+xo+12,agent.pos.y+yo+12,0);
    glVertex3f(agent.pos.x+xo+12,agent.pos.y+yo+22,0);
    glVertex3f(agent.pos.x+xo+6,agent.pos.y+yo+22,0);

    //how much sound is this bot making?
    glColor3f(agent.soundmul,agent.soundmul,agent.soundmul);
    glVertex3f(agent.pos.x+xo+6,agent.pos.y+yo+24,0);
    glVertex3f(agent.pos.x+xo+12,agent.pos.y+yo+24,0);
    glVertex3f(agent.pos.x+xo+12,agent.pos.y+yo+34,0);
    glVertex3f(agent.pos.x+xo+6,agent.pos.y+yo+34,0);

    //draw giving/receiving
    if (agent.dfood!=0) {

        float mag=cap(abs(agent.dfood)/conf::FOODTRANSFER/3);
        if (agent.dfood>0) glColor3f(0,mag,0); //draw boost as green outline
        else glColor3f(mag,0,0);
        glVertex3f(agent.pos.x+xo+6,agent.pos.y+yo+36,0);
        glVertex3f(agent.pos.x+xo+12,agent.pos.y+yo+36,0);
        glVertex3f(agent.pos.x+xo+12,agent.pos.y+yo+46,0);
        glVertex3f(agent.pos.x+xo+6,agent.pos.y+yo+46,0);
    }


    glEnd();
}

void GLDrawer::drawFood(int x, int y, float quantity)
{
    //draw food
    glBegin(GL_QUADS);
    glColor3f(0.9-quantity,0.9-quantity,1.0-quantity);
    glVertex3f(x*conf::CZ,y*conf::CZ,0);
    glVertex3f(x*conf::CZ+conf::CZ,y*conf::CZ,0);
    glVertex3f(x*conf::CZ+conf::CZ,y*conf::CZ+conf::CZ,0);
    glVertex3f(x*conf::CZ,y*conf::CZ+conf::CZ,0);
    glEnd();
}

void GLDrawer::storeStates(QQueue< SimState* > states)
{
    mStateQueue.append(states);
    qDebug() << "state buffer: " << mStateQueue.size();
}


void GLDrawer::decrementSkip()
{
    skipdraw -= 1;
    qDebug() << "skipdraw:" << skipdraw;
}

void GLDrawer::incrementSkip()
{
    skipdraw += 1;
    qDebug() << "skipdraw:" << skipdraw;
}


void GLDrawer::toggleDrawing()
{
    if(draw) {
        draw = false;
    } else {
        draw = true;
        QTimer::singleShot(0, this, SLOT(updateGL()));
    }
}
