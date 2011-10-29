TARGET = hypercube
QT += core \
    gui
HEADERS += IO/encodings/koi8rcvt.h \
    IO/encodings/koi8r.h \
    IO/encodings/latin1cvt.h \
    IO/encodings/latin1.h \
    IO/encodings/cp1250.h \
    IO/encodings/cp1250cvt.h \
    IO/modules.h \
    IO/encodings/latin2.h \
    IO/encodings/utf8.h \
    IO/providers/list.h \
    IO/providers/matrix.h \
    IO/providers/ps.h \
    IO/providers/svg.h \
    IO/encodings/latin2cvt.h \
    IO/encodings/utf8cvt.h \
    IO/encoding.h \
    GUI/icons.h \
    GUI/vertexitem.h \
    GUI/edgeitem.h \
    IO/io.h \
    GUI/graphview.h \
    GUI/graphtab.h \
    GUI/colorcombobox.h \
    GUI/numericedit.h \
    GUI/gui.h
SOURCES += IO/encodings/koi8rcvt.cpp \
    IO/encodings/latin1cvt.cpp \
    IO/encodings/latin2cvt.cpp \
    IO/encodings/cp1250cvt.cpp \
    IO/modules.cpp \
    IO/providers/list.cpp \
    IO/providers/matrix.cpp \
    IO/providers/ps.cpp \
    IO/providers/svg.cpp \
    IO/encodings/utf8cvt.cpp \
    CORE/color.cpp \
    CORE/edge.cpp \
    CORE/graph.cpp \
    CORE/sa.cpp \
    CORE/vertex.cpp \
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
