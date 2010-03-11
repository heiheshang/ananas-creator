TEMPLATE = lib
TARGET = DocumentEditor
include(documenteditor_dependencies.pri)
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

