# CMake  file for the Qt GUI component
# Responsible for including the Qt headers
# and performing all the operations needed to compile
# the Qt sources
#
# It creates a static library called 'sbgui'

if(QT4_FOUND)
    SET( QT_USE_QTOPENGL TRUE )

    INCLUDE( ${QT_USE_FILE} )

    set( sbGUI_SRCS 
        SimulationController.cpp 
        GLDrawer.cpp 
        MainWindow.cpp
    )

    set( sbGUI_Headers
        MainWindow.h
        SimulationController.h
    )

    qt4_wrap_cpp( sbMoc ${sbGUI_Headers} )

    add_library( sbgui ${sbMoc} ${sbGUI_Headers} ${sbGUI_SRCS})
    target_link_libraries(sbgui sbcore ${QT_LIBRARIES} )
endif()