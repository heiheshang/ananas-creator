TEMPLATE = lib
TARGET = FieldEditor

include(fieldeditor_dependencies.pri)

HEADERS += fieldeditorplugin.h \
        fieldeditor.h \
        fieldeditorconstants.h \
        ananasprojectmanager/libananas/acfg.h
SOURCES += fieldeditorplugin.cpp \
        fieldeditor.cpp \
        ananasprojectmanager/libananas/acfg.cpp
RESOURCES += fieldeditor.qrc \
             ../ananasprojectmanager/libananas/designer.qrc
FORMS = fieldeditor.ui
OTHER_FILES += FieldEditor.pluginspec
