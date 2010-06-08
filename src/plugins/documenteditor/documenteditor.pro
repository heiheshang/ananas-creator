TEMPLATE = lib
TARGET = DocumentEditor
QT += script
include(../../libs/utils/utils.pri)
include(documenteditor_dependencies.pri)
include(../../shared/indenter/indenter.pri)

HEADERS += documenteditorplugin.h \
    documenteditor.h \
    documenteditorconstants.h \
    ananasprojectmanager/libananas/acfg.h
SOURCES += documenteditorplugin.cpp \
    documenteditor.cpp \
    ananasprojectmanager/libananas/acfg.cpp
RESOURCES += documenteditor.qrc \
    ../ananasprojectmanager/libananas/designer.qrc
FORMS = documenteditor.ui
OTHER_FILES += documenteditor.pluginspec
