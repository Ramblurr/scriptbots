#include "GLView.h"
#include "World.h"

#include <ctime>

#include "config.h"


#ifdef LOCAL_GLUT32
    #include "glut.h"
#else
    #include <GL/glut.h>
#endif

#include <stdio.h>

#include "gui/MainWindow.h"
#include <QApplication>

GLView* GLVIEW = new GLView(0);

void doGlut(int argc, char **argv) {
    World* world = new World();
    GLVIEW->setWorld(world);

    //GLUT SETUP
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(30,30);
    glutInitWindowSize(conf::WWIDTH,conf::WHEIGHT);
    glutCreateWindow("TEST");
    glClearColor(0.9f, 0.9f, 1.0f, 0.0f);
    glutDisplayFunc(gl_renderScene);
    glutIdleFunc(gl_handleIdle);
    glutReshapeFunc(gl_changeSize);

    glutKeyboardFunc(gl_processNormalKeys);
    glutMouseFunc(gl_processMouse);
    glutMotionFunc(gl_processMouseActiveMotion);

    glutMainLoop();
}

void doQt(int argc, char **argv) {
    
    QApplication app( argc, argv );
    World* world = new World();
    MainWindow foo(world);
    foo.show();
    app.exec();
}

int main(int argc, char **argv) {
    srand(time(0));
    if (conf::WIDTH%conf::CZ!=0 || conf::HEIGHT%conf::CZ!=0) printf("CAREFUL! The cell size variable conf::CZ should divide evenly into  both conf::WIDTH and conf::HEIGHT! It doesn't right now!");
    printf("p= pause, d= toggle drawing (for faster computation), f= draw food too, += faster, -= slower");
    
    if(0) doGlut(argc,argv);
    else doQt(argc, argv);
    
    
    return 0;
}
