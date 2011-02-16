#include "GLView.h"
#include "core/World.h"

#include <ctime>

#include "config.h"

// #include <QtCore/QTime>
// #include "World.h"

#ifdef LOCAL_GLUT32
    #include "glut.h"
#else
    #include <GL/glut.h>
#endif

#include <stdio.h>

extern void doQt(int argc, char **argv);

#ifdef HAVE_QT
    //#include "MainWindow.h"
#else
    GLView* GLVIEW = new GLView(0);
#endif

int main(int argc, char **argv) {
    srand(time(0));
    if (conf::WIDTH%conf::CZ!=0 || conf::HEIGHT%conf::CZ!=0) printf("CAREFUL! The cell size variable conf::CZ should divide evenly into  both conf::WIDTH and conf::HEIGHT! It doesn't right now!");
    printf("p= pause, d= toggle drawing (for faster computation), f= draw food too, += faster, -= slower");
    
#ifdef HAVE_QT
    doQt(argc, argv);
#else
    doGlut(argc,argv);
#endif
//    World* w = new World();
//    QTime t;
//    t.start();
//    int fps = 0;
//    bool go = true;
//    int foo = 0;
//    while(1) {
//        ++fps;
//         w->update();
//         int msec = t.elapsed();
//         if( msec >= 1000 ) {
//             foo += 1000;
//             printf("fps: %d, %d\n", fps, foo);
//             fps= 0;
//             t.restart();
//         }
//         if( foo >= 30000 )
//             return 1;
//    }
    return 0;
}


