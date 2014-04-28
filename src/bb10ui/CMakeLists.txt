# Builds the qtui module

set(_modules )

setup_qt_variables(Gui Network ${_modules})

set(SOURCES
    bb10ui.cpp
    bb10uisettings.cpp
    bb10uistyle.cpp
    channellistviewfilter.cpp
    chatline.cpp
    chatlinemodel.cpp
    chatlinemodelitem.cpp
    chatlineprovider.cpp
    chatview.cpp
    datamodeladapter.cpp
    nicklistitem.cpp
    nicklistitemprovider.cpp
    simplesetuppage.cpp
    uiapplication.cpp
    uimessageprocessor.cpp)
set(MOC_HDRS
    bb10ui.h
    bb10uistyle.h
    channellistviewfilter.h
    chatline.h
    chatlinemodel.h
    chatview.h
    datamodeladapter.h
    nicklistitem.h
    simplesetuppage.h
    uiapplication.h
    uimessageprocessor.h)
set(HEADERS
    bb10uisettings.h
    chatlinemodelitem.h
    chatlineprovider.h
    nicklistitemprovider.h)

include_directories(${CMAKE_SOURCE_DIR}/src/common
                    ${CMAKE_SOURCE_DIR}/src/client
                    ${CMAKE_SOURCE_DIR}/src/bb10ui
                    ${CMAKE_SOURCE_DIR}/src/uisupport
                    ${CMAKE_INCLUDE_PATH}
                    ${CMAKE_CURRENT_BINARY_DIR}
                    ${QUASSEL_QT_INCLUDES})

qt4_wrap_cpp(MOC ${MOC_HDRS} ${SPHDR})

add_library(mod_bb10ui STATIC ${SOURCES} ${SPSRC} ${MOC} ${DBUS} ${UI} ${HEADERS})
add_dependencies(mod_bb10ui mod_common mod_client mod_uisupport)
set_target_properties(mod_bb10ui PROPERTIES COMPILE_FLAGS "${QUASSEL_QT_COMPILEFLAGS}")
