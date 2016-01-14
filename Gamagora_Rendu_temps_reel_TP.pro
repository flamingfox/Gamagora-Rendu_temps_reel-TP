TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt


LIBS += -LC:/lib/glew/lib/Release/win32 -lglew32
LIBS += -LC:/lib/glfw/lib-mingw-w64 -lglfw3
LIBS  += -lopengl32 -lgdi32

INCLUDEPATH += C:\lib
INCLUDEPATH += C:\lib\glew\include
INCLUDEPATH += C:\lib\glfw\include

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


