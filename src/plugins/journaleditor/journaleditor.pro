TEMPLATE = lib
TARGET = JournalEditor
include(journaleditor_dependencies.pri)
HEADERS += journaleditorplugin.h \
    journaleditor.h \
    journaleditorconstants.h \
    ananasprojectmanager/libananas/acfg.h
SOURCES += journaleditorplugin.cpp \
    journaleditor.cpp \
    ananasprojectmanager/libananas/acfg.cpp
RESOURCES += journaleditor.qrc \
    ../ananasprojectmanager/libananas/designer.qrc
FORMS = journaleditor.ui
OTHER_FILES += JournalEditor.pluginspec
