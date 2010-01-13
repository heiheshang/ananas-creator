TEMPLATE = lib
TARGET = AnanasProjectManager
CONFIG += uitools
INCLUDEPATH += ./libananas/

include(../../qtcreatorplugin.pri)
include(ananasprojectmanager_dependencies.pri)
HEADERS = ananasproject.h \
    ananasprojectplugin.h \
    ananasprojectmanager.h \
    ananasprojectconstants.h \
    ananasprojectnodes.h \
    ananasprojectwizard.h \
    ananasnewprojectwizard.h \
    ananasprojectfileseditor.h \
    ananasmakestep.h \
    ananasviewnavigationwidgetfactory.h \
    ananasexplorersidebar.h \
    libananas/acfg.h \
    libananas/configinfo.h
SOURCES = ananasproject.cpp \
    ananasprojectplugin.cpp \
    ananasprojectmanager.cpp \
    ananasprojectnodes.cpp \
    ananasprojectwizard.cpp \
    ananasnewprojectwizard.cpp \
    ananasprojectfileseditor.cpp \
    ananasmakestep.cpp \
    ananasviewnavigationwidgetfactory.cpp \
    ananasexplorersidebar.cpp \
    libananas/acfg.cpp \
    libananas/configinfo.cpp
FORMS = libananas/configinfo.ui
RESOURCES += ananasproject.qrc \
    libananas/designer.qrc
OTHER_FILES += AnanasProjectManager.pluginspec
