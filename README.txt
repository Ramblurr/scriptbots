SCRIPTBOTS
Author: Andrej Karpathy
License: Do whatever you want with this code

Project website: 
https://sites.google.com/site/scriptbotsevo/home

Older project website (just in case)
http://code.google.com/p/scriptbots/

------------------------------------------------------
BUILDING:

To compile scriptbots you will need:
* CMake >= 2.8 (http://www.cmake.org/cmake/resources/software.html)
* OpenGL and GLUT (http://www.opengl.org/resources/libraries/glut/)
* * Linux: freeglut (http://freeglut.sourceforge.net/) 

Can use OpenMP to speed up everything, in case you have multicore cpu

If you are on Ubuntu (or debian) you can install all the dependencies with:
$ apt-get install cmake build-essential libopenmpi-dev libglut32-dev libxi-dev libxmu-dev

To build ScriptBots on Linux:
$ cd path/to/source
$ mkdir build
$ cd build
$ cmake ../ # this is the equiv of ./configure
$ make

To execute ScriptBots simply type the following in the build directory:
$ ./scriptbots

For Windows: 
Follow basically the same steps, but after running cmake open up the VS solution (.sln) file it generates and compile the project from VS.


---------------------------------------------------------
USAGE:

Follow the above instructions to compile then run the program. Hit 'd' to make it go MUCH FASTER (and not draw). Speed can also be controlled with + and -.


---------------------------------------------------------
QUESTIONS COMMENTS are best posted at the google group, available on project site
or contact me at andrej.karpathy@gmail.com

Contributors:
Casey Link <unnamedrambler@gmail.com>
