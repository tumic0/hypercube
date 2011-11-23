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
    IO/encoding.h \
    IO/providers/ps.h \
    IO/providers/ps_snippets/encodings/latin2.ps.h \
    IO/providers/ps_snippets/encodings/latin1.h \
    IO/providers/ps_snippets/encodings/koi8r.ps.h \
    IO/providers/ps_snippets/encodings/latin2.h \
    IO/providers/ps_snippets/encodings/koi8r.h \
    IO/providers/ps_snippets/default.h \
    IO/providers/ps_snippets/cyrillic.h \
    IO/providers/ps_snippets/fonts/nimbus.pfa.h \
    IO/providers/ps_snippets/fonts/nimbus.h \
    IO/providers/ps_snippets/fonts/helvetica_ce.h \
    IO/providers/ps_snippets/fonts/helvetica.h \
    IO/providers/ps_snippets/ce.h \
    IO/providers/svg.h \
    IO/providers/matrix.h \
    IO/providers/ps_snippet.h \
    IO/providers/list.h \
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
    IO/io.h \
    IO/modules.h \
    CLI/cli.h
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
    IO/providers/ps_snippet.cpp \
    IO/providers/svg.cpp \
    CLI/cli.cpp \
    CLI/main.cpp
INCLUDEPATH += ./
