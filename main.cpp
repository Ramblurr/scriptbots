#include "GLView.h"
#include "core/World.h"

#include <ctime>

#include "config.h"


#ifdef LOCAL_GLUT32
    #include "glut.h"
#else
    #include <GL/glut.h>
#endif

#include <stdio.h>

#ifdef HAVE_QT
    #include "MainWindow.h"
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
   
    return 0;
}


