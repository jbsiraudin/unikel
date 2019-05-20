TARGET = QGLviewerExamples
CONFIG *= qt opengl release
QT *= opengl xml

HEADERS = ./src/viewer.h \
    ./src/gl/basicColors.h \
    ./src/gl/GLUtilityMethods.h \
    ./src/gl/openglincludeQtComp.h \
    ./src/point3.h \
    ./src/mesh.h \
    ./src/basicIO.h \
    ./qt/QBrushDisk.h \
    ./qt/QSmartAction.h \
    ./qt/QSmartColorPicker.h \
    ./qt/QTextInfoModalWindow.h \
    ./qt/TextureHandler.h \
    src/fieldadvector.h \
    src/kelvinlet.h \
    src/lightkelvinlet.h \
    src/unikelvinlet.h
SOURCES = ./src/viewer.cpp ./src/main.cpp \
    ./src/gl/basicColors.cpp \
    ./src/gl/GLUtilityMethods.cpp \
    src/unikelvinlet.cpp

# Linux
 INCLUDEPATH *= /usr/local/include
 LIBS += -L/usr/local/lib -lQGLViewer-qt5 -lgsl -lgslcblas -lglut -lGLU -lgomp
