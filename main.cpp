#include "GLView.h"
#include "core/World.h"

#include <sys/time.h>

#include "config.h"

#include "World.h"

#ifdef LOCAL_GLUT32
    #include "glut.h"
#else
    #include <GL/glut.h>
#endif

#include <stdio.h>
#include <ctime>

extern void doQt(int argc, char **argv);

#ifdef HAVE_QT
    //#include "MainWindow.h"
#else
    GLView* GLVIEW = new GLView(0);
#endif

int main(int argc, char **argv) {
    srand(time(0));
    if (conf::WIDTH%conf::CZ!=0 || conf::HEIGHT%conf::CZ!=0) printf("CAREFUL! The cell size variable conf::CZ should divide evenly into  both conf::WIDTH and conf::HEIGHT! It doesn't right now!");
   
    if( argc == 1) {
        
#ifdef HAVE_QT
    doQt(argc, argv);
#else
    printf("p= pause, d= toggle drawing (for faster computation), f= draw food too, += faster, -= slower");
    doGlut(argc,argv);
#endif

    } else if( argc == 2 ) {
        if( strcmp(argv[1], "-nogui") ) {
            printf("usage: %s [-nogui]\n", argv[0]);
            printf("without options starts in GUI mode.\n");
            printf("with -nogui flag starts without GUI\n");
            return 1;
        }
        World* w = new World();
        int fps = 0, total_seconds = 0;
        struct timeval start, end;
        gettimeofday(&start, NULL);
        while(1) {
            ++fps;
                w->update();
                gettimeofday(&end, NULL);
                if( (end.tv_sec  - start.tv_sec) >= 1 ) {
                    total_seconds += 1;
                    printf("fps: %d, %d\n", fps, total_seconds);
                    fps = 0;
                    gettimeofday(&start, NULL);
                }
                if( total_seconds >= 30 )
                    break;
        }
    }
    return 0;
}


