project(scriptbots)
cmake_minimum_required(VERSION 2.8)
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

include_directories(${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS})

set( SB_SRCS
    main.cpp
    vmath.cpp )

add_executable(scriptbots ${SB_SRCS})

target_link_libraries(scriptbots ${OPENGL_LIBRARIES} ${GLUT_LIBRARY})