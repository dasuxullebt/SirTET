######################################################################
# Automatically generated by qmake (3.0) So. Dez. 7 00:17:45 2014
######################################################################

CONFIG=debug qt
TEMPLATE = app
TARGET = sirtet
INCLUDEPATH += .
QT += core
# Input
SOURCES += sirtet.cpp BoolField.cpp GameState.cpp CursesDisplay.cpp BrailleDisplay.cpp
LIBS += -lncurses -lbrlapi
HEADERS += BoolField.hpp GameState.hpp CursesDisplay.hpp BrailleDisplay.hpp Display.hpp
QMAKE_CXXFLAGS += -std=c++11
QMAKE_CXXFLAGS_DEBUG += -fsanitize=address -fno-omit-frame-pointer
QMAKE_CXX = clang++
QMAKE_LFLAGS_DEBUG += -fsanitize=address
QMAKE_LINK = clang++
