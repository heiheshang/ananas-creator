TEMPLATE = lib
TARGET = DirectoryEditor
include(directoryeditor_dependencies.pri)
HEADERS += directoryeditorplugin.h \
    directoryeditor.h \
    directoryeditorconstants.h \
    ananasprojectmanager/libananas/acfg.h
SOURCES += directoryeditorplugin.cpp \
    directoryeditor.cpp \
    ananasprojectmanager/libananas/acfg.cpp
RESOURCES += directoryeditor.qrc \
    ../ananasprojectmanager/libananas/designer.qrc
FORMS = directoryeditor.ui
OTHER_FILES += DirectoryEditor.pluginspec

