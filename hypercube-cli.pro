CONFIG -= qt
CONFIG += console
TARGET = hypercube-cli
HEADERS += common/misc.h \
    common/margin.h \
    common/config.h \
    common/color.h \
    common/coordinates.h \
    common/edge.h \
    common/graph.h \
    common/sa.h \
    common/sa_log.h \
    common/vertex.h \
    IO/io.h \
    IO/ps.h \
    IO/svg.h \
    IO/matrix.h \
    CLI/cli.h
SOURCES += IO/matrix.cpp \
    common/edge.cpp \
    common/vertex.cpp \
    common/color.cpp \
    common/graph.cpp \
    common/sa.cpp \
    IO/io.cpp \
    IO/ps.cpp \
    IO/svg.cpp \
    CLI/cli.cpp \
    CLI/main.cpp