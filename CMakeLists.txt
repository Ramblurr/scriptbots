# Main CMake file
# Locates all the project dependencies
# And builds either a Qt or GLUT based executable

project(scriptbots)

set( CMAKE_BUILD_TYPE Debug) 
set( DEBUG_BUILD_TYPE ON )

cmake_minimum_required(VERSION 2.8)
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package( Qt4 4.6.0 COMPONENTS QtGui QtCore QtGl OPTIONAL )
find_package(OpenMP)

if (WIN32 AND NOT GLUT_FOUND)
    message(STATUS "Using in source glut32 library")
    find_library(GLUT32_LIBRARY NAMES glut32 PATHS ${CMAKE_CURRENT_SOURCE_DIR})
    SET (LOCAL_GLUT32 1)
endif()

if (OPENMP_FOUND)
    SET (HAVE_OPENMP 1)
    message(STATUS "Using OpenMP")
    SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    SET (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
endif()

include_directories( 
    core/
    gui
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${GLUT_INCLUDE_DIRS} )

# core simulation
add_subdirectory(core)
# Qt Gui stuff
add_subdirectory(gui)

include_directories(${CMAKE_CURRENT_BINARY_DIR} ${GLUT_INCLUDE_DIRS} )

# set(QT4_FOUND 0 )
if(QT4_FOUND)
    set(HAVE_QT 1)
    set(sb_LIBS 
        sbcore
        sbgui
        ${OPENGL_LIBRARIES} )
    set(sb_SRCS main.cpp)
    message(STATUS "Building Qt GUI version ***")
else()
    message(STATUS "Building GLUT GUI version ***")
    set(sb_LIBS 
        sbcore 
        ${OPENGL_LIBRARIES} 
        ${GLUT_LIBRARY} )
    set(sb_SRCS main.cpp GLView.cpp)
endif()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)
add_executable(scriptbots ${sb_SRCS})

if (WIN32 AND NOT GLUT_FOUND)
    target_link_libraries(scriptbots ${sb_LIBS}  ${GLUT32_LIBRARY} )
else()
    target_link_libraries(scriptbots  ${sb_LIBS})
endif()

# copy glut32.dll to the output dirs
# not sure how msvc could be true and win32 not, but just in case..
if (MSVC AND WIN32 AND NOT GLUT_FOUND) 
    message(STATUS "Copying glut32.dll to the Debug and Release directories")
    file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/Debug)
    file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/Release)
    file(COPY glut32.dll DESTINATION ${PROJECT_BINARY_DIR}/Debug)
    file(COPY glut32.dll DESTINATION ${PROJECT_BINARY_DIR}/Release)
endif()

