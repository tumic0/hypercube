TARGET = hypercube
QT += core \
    gui
HEADERS += IO/list.h \
    IO/matrix.h \
    IO/utf8cvt.h \
    IO/io.h \
    IO/ps.h \
    IO/svg.h \
    GUI/graphview.h \
    GUI/vertexitem.h \
    GUI/edgeitem.h \
    GUI/icons.h \
    GUI/graphtab.h \
    GUI/colorcombobox.h \
    GUI/numericedit.h \
    GUI/gui.h
SOURCES += CORE/color.cpp \
    CORE/edge.cpp \
    CORE/graph.cpp \
    CORE/sa.cpp \
    CORE/vertex.cpp \
    IO/utf8cvt.cpp \
    IO/list.cpp \
    IO/matrix.cpp \
    IO/io.cpp \
    IO/ps.cpp \
    IO/svg.cpp \
    GUI/main.cpp \
    GUI/vertexitem.cpp \
    GUI/edgeitem.cpp \
    GUI/graphview.cpp \
    GUI/graphtab.cpp \
    GUI/colorcombobox.cpp \
    GUI/numericedit.cpp \
    GUI/gui.cpp
DEFINES += SA_LOG_SUPPORT
RESOURCES += hypercube.qrc
TRANSLATIONS = lang/hypercube_cs.ts
INCLUDEPATH += ./
win32:RC_FILE = hypercube.rc
macx:ICON = icons/hypercube.icns
