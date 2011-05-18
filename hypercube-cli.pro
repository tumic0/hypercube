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
    IO/utf8cvt.h \
    IO/io.h \
    IO/ps.h \
    IO/svg.h \
    IO/matrix.h \
    IO/list.h \
    CLI/cli.h
SOURCES += CORE/edge.cpp \
    CORE/vertex.cpp \
    CORE/color.cpp \
    CORE/graph.cpp \
    CORE/sa.cpp \
    IO/utf8cvt.cpp \
    IO/io.cpp \
    IO/ps.cpp \
    IO/svg.cpp \
    IO/matrix.cpp \
    IO/list.cpp \
    CLI/cli.cpp \
    CLI/main.cpp
INCLUDEPATH += ./
