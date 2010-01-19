/**************************************************************************
**
** This file is part of Qt Creator
**
** Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** Commercial Usage
**
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
**
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at http://qt.nokia.com/contact.
**
**************************************************************************/

#include "fieldeditorplugin.h"
#include "fieldeditor.h"

#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QDebug>
#include <QtGui/QMenu>
#include <QtGui/QAction>
#include <QtGui/QMainWindow>
#include <QtGui/QHBoxLayout>


#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/coreconstants.h>
#include <coreplugin/editormanager/editormanager.h>
#include <coreplugin/icore.h>
#include <coreplugin/mimedatabase.h>
#include <coreplugin/uniqueidmanager.h>
#include <coreplugin/messagemanager.h>
#include <coreplugin/messageoutputwindow.h>
#include <extensionsystem/pluginmanager.h>
#include <find/ifindsupport.h>
#include <texteditor/fontsettings.h>
#include <texteditor/texteditorsettings.h>
#include <utils/linecolumnlabel.h>
#include <utils/reloadpromptutils.h>

using namespace FIELDEditor;
using namespace FIELDEditor::Internal;



 FieldEditorFile::FieldEditorFile(FieldEditor *parent) :
        Core::IFile(parent),
        m_mimeType(QLatin1String(FIELDEditor::Constants::C_FIELDEDITOR_MIMETYPE))
    {
        m_editor = parent;
        //connect(m_editor, SIGNAL(lazyDataRequested(int, bool)), this, SLOT(provideData(int)));
    }
    FieldEditorFile::~FieldEditorFile() {}

    QString FieldEditorFile::mimeType() const { return m_mimeType; }

    bool FieldEditorFile::save(const QString &fileName) {
        Core::MessageManager::instance()->printToOutputPane(tr("Save %1").arg(fileName));
//        QFile file(fileName);
//
//        QByteArray data;
//        if (m_editor->inLazyMode()) {
//            QFile read(m_fileName);
//            if (!read.open(QIODevice::ReadOnly))
//                return false;
//            data = read.readAll();
//            read.close();
//            if (!m_editor->applyModifications(data))
//                return false;
//        } else {
//            data = m_editor->data();
//        }
//        if (file.open(QIODevice::WriteOnly)) {
//            file.write(data);
//            file.close();
//            m_editor->setModified(false);
//            m_editor->editorInterface()->setDisplayName(QFileInfo(fileName).fileName());
//            m_fileName = fileName;
//            emit changed();
//            return true;
//        }
        return true;
    }

    bool FieldEditorFile::open(const QString &fileName) {
//        QFile file(fileName);
//        if (file.open(QIODevice::ReadOnly)) {
//            m_fileName = fileName;
//            if (file.isSequential()) {
//                m_editor->setData(file.readAll());
//            } else {
//                m_editor->setLazyData(0, file.size());
                m_editor->editorInterface()->setDisplayName(fileName);
//            }
//            file.close();
//            return true;
//        }
        return true;
    }


    void FieldEditorFile::setFilename(const QString &filename) {
        m_fileName = filename;
    }

    QString FieldEditorFile::fileName() const {
        return m_fileName;
    }

    QString FieldEditorFile::defaultPath() const { return QString(); }
    QString FieldEditorFile::suggestedFileName() const { return QString(); }
    QString FieldEditorFile::fileFilter() const { return QString(); }
    QString FieldEditorFile::fileExtension() const { return QString(); }

    bool FieldEditorFile::isModified() const {
        return false;
    }
    bool FieldEditorFile::isReadOnly() const {
        return false;
    }

    bool FieldEditorFile::isSaveAsAllowed() const { return false; }

    void FieldEditorFile::modified(ReloadBehavior *behavior) {
    }

FieldEditorInterface::FieldEditorInterface(FieldEditor *parent)
        : Core::IEditor(parent)
    {
        Core::UniqueIDManager *uidm = Core::UniqueIDManager::instance();
        m_editor = parent;
        m_file = new FieldEditorFile(parent);
//        m_context << uidm->uniqueIdentifier(Core::Constants::K_DEFAULT_BINARY_EDITOR);
        m_context << uidm->uniqueIdentifier(Constants::C_FIELDEDITOR);
//        m_cursorPositionLabel = new Core::Utils::LineColumnLabel;
//
//        QHBoxLayout *l = new QHBoxLayout;
//        QWidget *w = new QWidget;
//        l->setMargin(0);
//        l->setContentsMargins(0, 0, 5, 0);
//        l->addStretch(1);
//        l->addWidget(m_cursorPositionLabel);
//        w->setLayout(l);
//
//        m_toolBar = new QToolBar;
//        m_toolBar->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
//        m_toolBar->addWidget(w);
//
//        connect(m_editor, SIGNAL(cursorPositionChanged(int)), this, SLOT(updateCursorPosition(int)));
    }
    FieldEditorInterface::~FieldEditorInterface() {}

    QWidget *FieldEditorInterface::widget() { return m_editor; }

    QList<int> FieldEditorInterface::context() const { return m_context; }

    bool FieldEditorInterface::createNew(const QString & /* contents */ ) {
        m_file->setFilename(QString());
        return true;
    }
    bool FieldEditorInterface::open(const QString &fileName ) {
        return true;
    }
    Core::IFile *FieldEditorInterface::file() { return m_file; }
    const char* FieldEditorInterface::kind() const
    { return FIELDEditor::Constants::C_FIELDEDITOR; }
    QString FieldEditorInterface::displayName() const { return m_displayName; }
    void FieldEditorInterface::setDisplayName(const QString &title) { m_displayName = title; emit changed(); }

    bool FieldEditorInterface::duplicateSupported() const { return false; }
    Core::IEditor *FieldEditorInterface::duplicate(QWidget* )
    {   return 0; }

    QByteArray FieldEditorInterface::saveState() const { return QByteArray(); } // TODO
    bool FieldEditorInterface::restoreState(const QByteArray & /* state */) { return false; } // TODO

    QWidget *FieldEditorInterface::toolBar() { return 0 ; }

    bool FieldEditorInterface::isTemporary() const { return true; }

///////////////////////////////// FieldEditorFactory //////////////////////////////////

FieldEditorFactory::FieldEditorFactory(FieldEditorPlugin *owner) :
    m_kind(QLatin1String(FIELDEditor::Constants::C_FIELDEDITOR)),
    m_mimeTypes(QLatin1String(FIELDEditor::Constants::C_FIELDEDITOR_MIMETYPE)),
    m_owner(owner)
{
}

QString FieldEditorFactory::kind() const
{
    return m_kind;
}

Core::IFile *FieldEditorFactory::open(const QString &fileName)
{

    Core::EditorManager *em = Core::EditorManager::instance();
    Core::IEditor *iface = em->openEditor(fileName, kind());
    return iface ? iface->file() : 0;
}

Core::IEditor *FieldEditorFactory::createEditor(QWidget *parent)
{
    FieldEditor *editor = new FieldEditor(parent);
    m_owner->initializeEditor(editor);
    return editor->editorInterface();
}

QStringList FieldEditorFactory::mimeTypes() const
{
    return m_mimeTypes;
}

///////////////////////////////// BinEditorPlugin //////////////////////////////////

FieldEditorPlugin::FieldEditorPlugin()
{
    //m_undoAction = m_redoAction = m_copyAction = m_selectAllAction = 0;
}

FieldEditorPlugin::~FieldEditorPlugin()
{
}

QAction *FieldEditorPlugin::registerNewAction(const QString &id, const QString &title)
{
    QAction *result = new QAction(title, this);
    Core::ICore::instance()->actionManager()->registerAction(result, id, m_context);
    return result;
}

QAction *FieldEditorPlugin::registerNewAction(const QString &id,
                                            QObject *receiver,
                                            const char *slot,
                                            const QString &title)
{
    QAction *rc = registerNewAction(id, title);
    if (!rc)
        return 0;

    connect(rc, SIGNAL(triggered()), receiver, slot);
    return rc;
}

void FieldEditorPlugin::initializeEditor(FieldEditor *editor)
{
    FieldEditorInterface *editorInterface = new FieldEditorInterface(editor);
    //QObject::connect(editor, SIGNAL(modificationChanged(bool)), editorInterface, SIGNAL(changed()));
    editor->setEditorInterface(editorInterface);

    Core::UniqueIDManager *uidm = Core::UniqueIDManager::instance();
    m_context << uidm->uniqueIdentifier(Constants::C_FIELDEDITOR);
//    if (!m_undoAction) {
//        m_undoAction      = registerNewAction(QLatin1String(Core::Constants::UNDO),
//                                              this, SLOT(undoAction()),
//                                              tr("&Undo"));
//        m_redoAction      = registerNewAction(QLatin1String(Core::Constants::REDO),
//                                              this, SLOT(redoAction()),
//                                              tr("&Redo"));
//        m_copyAction      = registerNewAction(QLatin1String(Core::Constants::COPY),
//                                              this, SLOT(copyAction()));
//        m_selectAllAction = registerNewAction(QLatin1String(Core::Constants::SELECTALL),
//                                              this, SLOT(selectAllAction()));
//    }

    // Font settings
    //TextEditor::TextEditorSettings *settings = TextEditor::TextEditorSettings::instance();
    //editor->setFontSettings(settings->fontSettings());
    //connect(settings, SIGNAL(fontSettingsChanged(TextEditor::FontSettings)),
    //        editor, SLOT(setFontSettings(TextEditor::FontSettings)));

    //QObject::connect(editor, SIGNAL(undoAvailable(bool)), this, SLOT(updateActions()));
    //QObject::connect(editor, SIGNAL(redoAvailable(bool)), this, SLOT(updateActions()));
    //QObject::connect(editor, SIGNAL(copyAvailable(bool)), this, SLOT(updateActions()));

    Aggregation::Aggregate *aggregate = new Aggregation::Aggregate;
    aggregate->add(editor);
}

bool FieldEditorPlugin::initialize(const QStringList &arguments, QString *errorMessage)
{
    Q_UNUSED(arguments)
    Q_UNUSED(errorMessage)

    Core::ICore *core = Core::ICore::instance();
    if (!core->mimeDatabase()->addMimeTypes(QLatin1String(":/directoryeditor/DirectoryEditor.mimetypes.xml"), errorMessage))
        return false;

    connect(core, SIGNAL(contextAboutToChange(Core::IContext *)),
        this, SLOT(updateCurrentEditor(Core::IContext *)));

    addAutoReleasedObject(new FieldEditorFactory(this));

    return true;
}

void FieldEditorPlugin::extensionsInitialized()
{
}

void FieldEditorPlugin::shutdown()
{
}

void FieldEditorPlugin::updateCurrentEditor(Core::IContext *object)
{
    do {
        if (!object) {
            if (!m_currentEditor)
                return;

            m_currentEditor = 0;
            break;
        }
        FieldEditor *editor = qobject_cast<FieldEditor *>(object->widget());
        if (!editor) {
            if (!m_currentEditor)
                return;

            m_currentEditor = 0;
            break;
        }

        if (editor == m_currentEditor)
            return;

        m_currentEditor = editor;

    } while (false);
    //updateActions();
}


Q_EXPORT_PLUGIN(FieldEditorPlugin)

//#include "fieldeditorplugin.moc"
