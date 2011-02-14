# CMake file that compiles the core simulation
# This module is free from graphcs and drawing components
#
# It creates a library called 'sbcore'
include_directories(
    ${CMAKE_CURRENT_BINARY_DIR} 
    ${OPENGL_INCLUDE_DIRS} )
    
set( sb_SRCS
    View.cpp
    DWRAONBrain.cpp
    Agent.cpp
    World.cpp
    vmath.cpp )
    
add_library( sbcore ${sb_SRCS} )
