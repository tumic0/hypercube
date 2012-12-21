TARGET = hypercube
QT += core \
    gui
HEADERS += CORE/sa_log.h \
    CORE/graph.h \
    CORE/misc.h \
    CORE/vertex.h \
    CORE/sa.h \
    CORE/margin.h \
    CORE/coordinates.h \
    CORE/config.h \
    CORE/edge.h \
    CORE/color.h \
    CORE/container.h \
    CORE/colormap.h \
    CORE/adjlist.h \
    CORE/vector.h \
    CORE/line.h \
    IO/io.h \
    IO/modules.h \
    IO/encoding.h \
    IO/providers/ps.h \
    IO/providers/ps/encodings/latin2.ps.h \
    IO/providers/ps/encodings/latin1.h \
    IO/providers/ps/encodings/koi8r.ps.h \
    IO/providers/ps/encodings/latin2.h \
    IO/providers/ps/encodings/koi8r.h \
    IO/providers/ps/default.h \
    IO/providers/ps/cyrillic.h \
    IO/providers/ps/fonts/nimbus.pfa.h \
    IO/providers/ps/fonts/nimbus.h \
    IO/providers/ps/fonts/helvetica_ce.h \
    IO/providers/ps/fonts/helvetica.h \
    IO/providers/ps/ce.h \
    IO/providers/ps/snippet.h \
    IO/providers/svg.h \
    IO/providers/matrix.h \
    IO/providers/list.h \
    IO/providers/dot.h \
    IO/providers/gml.h \
    IO/providers/graphml.h \
    IO/encodings/latin1cvt.h \
    IO/encodings/cp1250cvt.h \
    IO/encodings/latin1.h \
    IO/encodings/utf8.h \
    IO/encodings/latin2cvt.h \
    IO/encodings/koi8rcvt.h \
    IO/encodings/latin2.h \
    IO/encodings/cp1250.h \
    IO/encodings/koi8r.h \
    IO/encodings/utf8cvt.h \
    GUI/vertexitem.h \
    GUI/icons.h \
    GUI/graphview.h \
    GUI/numericedit.h \
    GUI/colorcombobox.h \
    GUI/gui.h \
    GUI/graphtab.h \
    GUI/edgeitem.h \
    GUI/settings.h
SOURCES += \
    CORE/edge.cpp \
    CORE/graph.cpp \
    CORE/sa.cpp \
    CORE/vertex.cpp \
    CORE/colormap.cpp \
    CORE/line.cpp \
    IO/modules.cpp \
    IO/encodings/koi8rcvt.cpp \
    IO/encodings/latin1cvt.cpp \
    IO/encodings/latin2cvt.cpp \
    IO/encodings/cp1250cvt.cpp \
    IO/providers/ps/snippet.cpp \
    IO/providers/list.cpp \
    IO/providers/matrix.cpp \
    IO/providers/dot.cpp \
    IO/providers/gml.cpp \
    IO/providers/graphml.cpp \
    IO/providers/ps.cpp \
    IO/providers/svg.cpp \
    IO/encodings/utf8cvt.cpp \
    IO/io.cpp \
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
win32 {
    RC_FILE = hypercube.rc
	DEFINES += _CRT_SECURE_NO_WARNINGS
}
macx:ICON = icons/hypercube.icns
