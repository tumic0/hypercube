CONFIG -= qt
CONFIG += console
TARGET = hypercube-cli
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
    IO/encoding.h \
    IO/providers/ps.h \
    IO/providers/ps/encodings/iso2.ps.h \
    IO/providers/ps/encodings/iso7.ps.h \
    IO/providers/ps/encodings/koi8r.ps.h \
    IO/providers/ps/encodings/iso1.h \
    IO/providers/ps/encodings/iso2.h \
    IO/providers/ps/encodings/iso7.h \
    IO/providers/ps/encodings/koi8r.h \
    IO/providers/ps/western.h \
    IO/providers/ps/cyrillic.h \
    IO/providers/ps/greek.h \
    IO/providers/ps/ce.h \
    IO/providers/ps/fonts/moderna.pfa.h \
    IO/providers/ps/fonts/nimbus.pfa.h \
    IO/providers/ps/fonts/moderna.h \
    IO/providers/ps/fonts/nimbus.h \
    IO/providers/ps/fonts/helvetica.h \
    IO/providers/ps/snippet.h \
    IO/providers/xml/xml.h \
    IO/providers/gxl.h \
    IO/providers/svg.h \
    IO/providers/matrix.h \
    IO/providers/list.h \
    IO/providers/dot.h \
    IO/providers/gml.h \
    IO/providers/graphml.h \
    IO/encodings/8bit.h \
    IO/encodings/iso1cvt.h \
    IO/encodings/iso2cvt.h \
    IO/encodings/iso5cvt.h \
    IO/encodings/iso7cvt.h \
    IO/encodings/koi8rcvt.h \
    IO/encodings/koi8ucvt.h \
    IO/encodings/cp1250cvt.h \
    IO/encodings/cp1251cvt.h \
    IO/encodings/cp1252cvt.h \
    IO/encodings/cp1253cvt.h \
    IO/encodings/utf8cvt.h \
    IO/encodings/iso1.h \
    IO/encodings/iso2.h \
    IO/encodings/iso5.h \
    IO/encodings/iso7.h \
    IO/encodings/cp1250.h \
    IO/encodings/cp1251.h \
    IO/encodings/cp1252.h \
    IO/encodings/cp1253.h \
    IO/encodings/koi8r.h \
    IO/encodings/koi8u.h \
    IO/encodings/utf8.h \
    IO/io.h \
    IO/modules.h \
    CLI/cli.h
SOURCES += CORE/edge.cpp \
    CORE/vertex.cpp \
    CORE/graph.cpp \
    CORE/sa.cpp \
    CORE/colormap.cpp \
    CORE/line.cpp \
    IO/encodings/8bit.cpp \
    IO/encodings/iso1cvt.cpp \
    IO/encodings/iso2cvt.cpp \
    IO/encodings/iso5cvt.cpp \
    IO/encodings/iso7cvt.cpp \
    IO/encodings/cp1250cvt.cpp \
    IO/encodings/cp1251cvt.cpp \
    IO/encodings/cp1252cvt.cpp \
    IO/encodings/cp1253cvt.cpp \
    IO/encodings/koi8rcvt.cpp \
    IO/encodings/koi8ucvt.cpp \
    IO/encodings/utf8cvt.cpp \
    IO/modules.cpp \
    IO/providers/xml/xml.cpp \
    IO/providers/gxl.cpp \
    IO/providers/list.cpp \
    IO/providers/matrix.cpp \
    IO/providers/dot.cpp \
    IO/providers/gml.cpp \
    IO/providers/graphml.cpp \
    IO/providers/ps.cpp \
    IO/providers/ps/snippet.cpp \
    IO/providers/svg.cpp \
    IO/io.cpp \
    CLI/cli.cpp \
    CLI/main.cpp
INCLUDEPATH += ./
win32 {
    RC_FILE = hypercube-cli.rc
    DEFINES += _CRT_SECURE_NO_WARNINGS
}
