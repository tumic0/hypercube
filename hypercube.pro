TARGET = hypercube
QT += core \
    gui
HEADERS += common/misc.h \
    common/margin.h \
    IO/matrix.h \
    common/config.h \
    common/color.h \
    common/coordinates.h \
    common/edge.h \
    common/graph.h \
    common/sa.h \
    common/sa_log.h \
    common/vertex.h \
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
SOURCES += IO/matrix.cpp \
    common/edge.cpp \
    common/vertex.cpp \
    IO/io.cpp \
    common/color.cpp \
    common/graph.cpp \
    common/sa.cpp \
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
RC_FILE = hypercube.rc
INCLUDEPATH += ./
