project(scriptbots)
cmake_minimum_required(VERSION 2.8)
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

if (WIN32 AND NOT GLUT_FOUND)
    find_library(GLUT32_LIBRARY NAMES glut32 PATHS ${CMAKE_CURRENT_SOURCE_DIR})
    SET (LOCAL_GLUT32 1)
endif()

find_package(OpenMP)

if (OPENMP_FOUND)
    SET (HAVE_OPENMP 1)
    SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    SET (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
endif()
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)

include_directories(${CMAKE_CURRENT_BINARY_DIR} ${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS})

set( SB_SRCS
    View.cpp
    GLView.cpp
    main.cpp
    DWRAONBrain.cpp
    Agent.cpp
    World.cpp
    vmath.cpp )

add_executable(scriptbots  ${SB_SRCS})

if (WIN32 AND NOT GLUT_FOUND)
    target_link_libraries(scriptbots ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} ${GLUT32_LIBRARY})
else()
    target_link_libraries(scriptbots ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})
endif()

# copy glut32.dll to the output dirs
# not sure how msvc could be true and win32 not, but just in case..
if (MSVC AND WIN32 AND NOT GLUT_FOUND) 
    file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/Debug)
    file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/Release)
    file(COPY glut32.dll DESTINATION ${PROJECT_BINARY_DIR}/Debug)
    file(COPY glut32.dll DESTINATION ${PROJECT_BINARY_DIR}/Release)
endif()

