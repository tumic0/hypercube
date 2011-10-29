CONFIG -= qt
CONFIG += console
TARGET = hypercube-cli
HEADERS += CORE/misc.h \
    CORE/margin.h \
    CORE/config.h \
    CORE/color.h \
    CORE/coordinates.h \
    CORE/edge.h \
    CORE/graph.h \
    CORE/sa.h \
    CORE/sa_log.h \
    CORE/vertex.h \
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
    IO/encodings/koi8rcvt.h \
    IO/encodings/koi8r.h \
    IO/encoding.h \
    IO/io.h
SOURCES += CORE/edge.cpp \
    CORE/vertex.cpp \
    CORE/color.cpp \
    CORE/graph.cpp \
    CORE/sa.cpp \
    IO/encodings/latin1cvt.cpp \
    IO/encodings/latin2cvt.cpp \
    IO/encodings/cp1250cvt.cpp \
    IO/encodings/utf8cvt.cpp \
    IO/encodings/koi8rcvt.cpp \
    IO/modules.cpp \
    IO/providers/list.cpp \
    IO/providers/matrix.cpp \
    IO/providers/ps.cpp \
    IO/providers/svg.cpp \
    CLI/cli.cpp \
    CLI/main.cpp
INCLUDEPATH += ./
