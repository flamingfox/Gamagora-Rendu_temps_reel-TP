TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt


LIBS += -L$$PWD/extLib/glew/lib/Release/Win32 -lglew32
LIBS += -L$$PWD/extLib/glfw/lib-mingw-w64 -lglfw3
LIBS += -lopengl32 -lgdi32

INCLUDEPATH += ./extLib
INCLUDEPATH += ./extLib\glew\include
INCLUDEPATH += ./extLib\glfw\include

SOURCES += \
    test.cpp \
    lib/mesh.cpp \
    lib/objmanager.cpp \
    lib/vector3d.cpp


DISTFILES += \
    basic.fsl \
    basic.vsl

HEADERS += \
    lib/mesh.h \
    lib/objmanager.h \
    lib/vector3d.h


