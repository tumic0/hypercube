TARGET = hypercube
QT += core \
    gui
HEADERS += IO/list.h \
    IO/matrix.h \
    GUI/vertexitem.h \
    GUI/edgeitem.h \
    IO/io.h \
    GUI/graphview.h \
    IO/ps.h \
    IO/svg.h \
    GUI/graphtab.h \
    GUI/colorcombobox.h \
    GUI/numericedit.h \
    GUI/gui.h
SOURCES += IO/list.cpp \
    CORE/color.cpp \
    CORE/edge.cpp \
    CORE/graph.cpp \
    CORE/sa.cpp \
    CORE/vertex.cpp \
    IO/matrix.cpp \
    IO/io.cpp \
    GUI/main.cpp \
    GUI/vertexitem.cpp \
    GUI/edgeitem.cpp \
    GUI/graphview.cpp \
    IO/ps.cpp \
    IO/svg.cpp \
    GUI/graphtab.cpp \
    GUI/colorcombobox.cpp \
    GUI/numericedit.cpp \
    GUI/gui.cpp
DEFINES += SA_LOG_SUPPORT
RESOURCES += hypercube.qrc
INCLUDEPATH += ./

win32 {
	RC_FILE = hypercube.rc
}
macx {
	ICON = icons/hypercube.icns
}
