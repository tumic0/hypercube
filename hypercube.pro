TARGET = hypercube
QT += core \
    gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
HEADERS += src/CORE/sa_log.h \
    src/CORE/graph.h \
    src/CORE/misc.h \
    src/CORE/vertex.h \
    src/CORE/sa.h \
    src/CORE/margin.h \
    src/CORE/coordinates.h \
    src/CORE/config.h \
    src/CORE/edge.h \
    src/CORE/color.h \
    src/CORE/container.h \
    src/CORE/colormap.h \
    src/CORE/adjlist.h \
    src/CORE/vector.h \
    src/CORE/line.h \
    src/IO/io.h \
    src/IO/modules.h \
    src/IO/encoding.h \
    src/IO/providers/xml/xml.h \
    src/IO/providers/ps.h \
    src/IO/providers/ps/encodings/iso2.ps.h \
    src/IO/providers/ps/encodings/iso5.ps.h \
    src/IO/providers/ps/encodings/iso7.ps.h \
    src/IO/providers/ps/encodings/iso1.h \
    src/IO/providers/ps/encodings/iso2.h \
    src/IO/providers/ps/encodings/iso5.h \
    src/IO/providers/ps/encodings/iso7.h \
    src/IO/providers/ps/western.h \
    src/IO/providers/ps/cyrillic.h \
    src/IO/providers/ps/greek.h \
    src/IO/providers/ps/ce.h \
    src/IO/providers/ps/fonts/moderna.pfa.h \
    src/IO/providers/ps/fonts/nimbus.pfa.h \
    src/IO/providers/ps/fonts/moderna.h \
    src/IO/providers/ps/fonts/nimbus.h \
    src/IO/providers/ps/fonts/helvetica.h \
    src/IO/providers/ps/snippet.h \
    src/IO/providers/svg.h \
    src/IO/providers/matrix.h \
    src/IO/providers/list.h \
    src/IO/providers/dot.h \
    src/IO/providers/gml.h \
    src/IO/providers/graphml.h \
    src/IO/providers/gxl.h \
    src/IO/encodings/8bit.h \
    src/IO/encodings/iso1cvt.h \
    src/IO/encodings/iso2cvt.h \
    src/IO/encodings/iso5cvt.h \
    src/IO/encodings/iso7cvt.h \
    src/IO/encodings/koi8rcvt.h \
    src/IO/encodings/koi8ucvt.h \
    src/IO/encodings/cp1250cvt.h \
    src/IO/encodings/cp1251cvt.h \
    src/IO/encodings/cp1252cvt.h \
    src/IO/encodings/cp1253cvt.h \
    src/IO/encodings/utf8cvt.h \
    src/IO/encodings/iso1.h \
    src/IO/encodings/iso2.h \
    src/IO/encodings/iso5.h \
    src/IO/encodings/iso7.h \
    src/IO/encodings/cp1250.h \
    src/IO/encodings/cp1251.h \
    src/IO/encodings/cp1252.h \
    src/IO/encodings/cp1253.h \
    src/IO/encodings/koi8r.h \
    src/IO/encodings/koi8u.h \
    src/IO/encodings/utf8.h \
    src/GUI/icons.h \
    src/GUI/settings.h \
    src/GUI/graphview.h \
    src/GUI/numericedit.h \
    src/GUI/colorcombobox.h \
    src/GUI/gui.h \
    src/GUI/graphtab.h \
    src/GUI/vertexitem.h \
    src/GUI/edgeitem.h \
    src/GUI/legenditem.h
SOURCES += \
    src/CORE/edge.cpp \
    src/CORE/graph.cpp \
    src/CORE/sa.cpp \
    src/CORE/vertex.cpp \
    src/CORE/colormap.cpp \
    src/CORE/line.cpp \
    src/CORE/misc.cpp \
    src/IO/modules.cpp \
    src/IO/encodings/8bit.cpp \
    src/IO/encodings/koi8rcvt.cpp \
    src/IO/encodings/koi8ucvt.cpp \
    src/IO/encodings/iso1cvt.cpp \
    src/IO/encodings/iso2cvt.cpp \
    src/IO/encodings/iso5cvt.cpp \
    src/IO/encodings/iso7cvt.cpp \
    src/IO/encodings/cp1250cvt.cpp \
    src/IO/encodings/cp1251cvt.cpp \
    src/IO/encodings/cp1252cvt.cpp \
    src/IO/encodings/cp1253cvt.cpp \
    src/IO/encodings/utf8cvt.cpp \
    src/IO/providers/xml/xml.cpp \
    src/IO/providers/ps/snippet.cpp \
    src/IO/providers/list.cpp \
    src/IO/providers/matrix.cpp \
    src/IO/providers/dot.cpp \
    src/IO/providers/gml.cpp \
    src/IO/providers/graphml.cpp \
    src/IO/providers/gxl.cpp \
    src/IO/providers/ps.cpp \
    src/IO/providers/svg.cpp \
    src/IO/io.cpp \
    src/GUI/main.cpp \
    src/GUI/vertexitem.cpp \
    src/GUI/edgeitem.cpp \
    src/GUI/legenditem.cpp \
    src/GUI/graphview.cpp \
    src/GUI/graphtab.cpp \
    src/GUI/colorcombobox.cpp \
    src/GUI/numericedit.cpp \
    src/GUI/gui.cpp
DEFINES += SA_LOG_SUPPORT
RESOURCES += hypercube.qrc
TRANSLATIONS = lang/hypercube_cs.ts
INCLUDEPATH += ./src
win32 {
    RC_FILE = hypercube.rc
    DEFINES += _CRT_SECURE_NO_WARNINGS
}
macx:ICON = icons/hypercube.icns
