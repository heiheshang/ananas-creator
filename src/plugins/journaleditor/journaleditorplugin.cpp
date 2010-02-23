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

#include "journaleditorplugin.h"
#include "journaleditor.h"

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

using namespace JOURNALEditor;
using namespace JOURNALEditor::Internal;


//class DirectoryEditorFind : public Find::IFindSupport
//{
//    Q_OBJECT
//public:
//    DirectoryEditorFind(DirectoryEditor *editor) { m_editor = editor; m_incrementalStartPos = -1; }
//    ~DirectoryEditorFind() {}
//
//    bool supportsReplace() const { return false; }
//    IFindSupport::FindFlags supportedFindFlags() const
//    {
//        return IFindSupport::FindBackward | IFindSupport::FindCaseSensitively;
//    }
//
//    void resetIncrementalSearch() { m_incrementalStartPos = -1; }
//    void clearResults() { m_editor->highlightSearchResults(QByteArray()); }
//    QString currentFindString() const { return QString(); }
//    QString completedFindString() const { return QString(); }
//
//
//    int find(const QByteArray &pattern, int pos, Find::IFindSupport::FindFlags findFlags) {
//        if (pattern.isEmpty()) {
//            m_editor->sorPosition(pos);
//            return pos;
//        }
//
//        int found = m_editor->find(pattern, pos, Find::IFindSupport::textDocumentFlagsForFindFlags(findFlags));
//        if (found < 0)
//            found = m_editor->find(pattern,
//                                   (findFlags & Find::IFindSupport::FindBackward)?m_editor->dataSize()-1:0,
//                                   Find::IFindSupport::textDocumentFlagsForFindFlags(findFlags));
//        return found;
//    }
//
//    bool findIncremental(const QString &txt, Find::IFindSupport::FindFlags findFlags) {
//        QByteArray pattern = txt.toLatin1();
//        if (m_incrementalStartPos < 0)
//            m_incrementalStartPos = m_editor->selectionStart();
//        int pos = m_incrementalStartPos;
//        findFlags &= ~Find::IFindSupport::FindBackward;
//        int found =  find(pattern, pos, findFlags);
//        if (found >= 0)
//            m_editor->highlightSearchResults(pattern, Find::IFindSupport::textDocumentFlagsForFindFlags(findFlags));
//        else
//            m_editor->highlightSearchResults(QByteArray(), 0);
//        return found >= 0;
//    }
//
//    bool findStep(const QString &txt, Find::IFindSupport::FindFlags findFlags) {
//        QByteArray pattern = txt.toLatin1();
//        bool wasReset = (m_incrementalStartPos < 0);
//        int pos = m_editor->cursorPosition();
//        if (findFlags & Find::IFindSupport::FindBackward)
//            pos = m_editor->selectionStart()-1;
//        int found = find(pattern, pos, findFlags);
//        if (found)
//            m_incrementalStartPos = found;
//        if (wasReset && found >= 0)
//            m_editor->highlightSearchResults(pattern, Find::IFindSupport::textDocumentFlagsForFindFlags(findFlags));
//        return found >= 0;
//    }
//    bool replaceStep(const QString &, const QString &,
//                     Find::IFindSupport::FindFlags) { return false;}
//    int replaceAll(const QString &, const QString &,
//                   Find::IFindSupport::FindFlags) { return 0; }
//
//private:
//    DirectoryEditor *m_editor;
//    int m_incrementalStartPos;
//};

 JournalEditorFile::JournalEditorFile(JournalEditor *parent) :
        Core::IFile(parent),
        m_mimeType(QLatin1String(JOURNALEditor::Constants::C_JOURNALEDITOR_MIMETYPE))
    {
        m_editor = parent;
        //connect(m_editor, SIGNAL(lazyDataRequested(int, bool)), this, SLOT(provideData(int)));
    }
    JournalEditorFile::~JournalEditorFile() {}

    QString JournalEditorFile::mimeType() const { return m_mimeType; }

    bool JournalEditorFile::save(const QString &fileName) {
        Q_UNUSED(fileName);
        //Core::MessageManager::instance()->printToOutputPane(tr("DirectoryEditor"));
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
        return false;
    }

    bool JournalEditorFile::open(const QString &fileName) {
        Q_UNUSED(fileName);
//        QFile file(fileName);
//        if (file.open(QIODevice::ReadOnly)) {
//            m_fileName = fileName;
//            if (file.isSequential()) {
//                m_editor->setData(file.readAll());
//            } else {
//                m_editor->setLazyData(0, file.size());
//                m_editor->editorInterface()->setDisplayName(QFileInfo(fileName).fileName());
//            }
//            file.close();
//            return true;
//        }
        return false;
    }

    void JournalEditorFile::provideData(int block) {
//        QFile file(m_fileName);
//        if (file.open(QIODevice::ReadOnly)) {
//            int blockSize = m_editor->lazyDataBlockSize();
//            file.seek(block * blockSize);
//            QByteArray data = file.read(blockSize);
//            if (data.size() != blockSize)
//                data.resize(blockSize);
//            m_editor->addLazyData(block, data);
//            file.close();
//        }
    }
    void JournalEditorFile::setFilename(const QString &filename) {
        m_fileName = filename;
    }

    QString JournalEditorFile::fileName() const {
        return m_fileName;
    }

    QString JournalEditorFile::defaultPath() const { return QString(); }
    QString JournalEditorFile::suggestedFileName() const { return QString(); }
    QString JournalEditorFile::fileFilter() const { return QString(); }
    QString JournalEditorFile::fileExtension() const { return QString(); }

    bool JournalEditorFile::isModified() const {
        return false;/*m_editor->isModified()*/;
    }
    bool JournalEditorFile::isReadOnly() const {
        //const QFileInfo fi(m_fileName);
        return false;//!fi.isWritable();
    }

    bool JournalEditorFile::isSaveAsAllowed() const { return true; }

    void JournalEditorFile::modified(ReloadBehavior *behavior) {
//        const QString fileName = m_fileName;
//
//        switch (*behavior) {
//        case  Core::IFile::ReloadNone:
//            return;
//        case Core::IFile::ReloadUnmodified:
//            if (!isModified()) {
//                open(fileName);
//                return;
//            }
//            break;
//        case Core::IFile::ReloadAll:
//            open(fileName);
//            return;
//        case Core::IFile::ReloadPermissions:
//            emit changed();
//            return;
//        case Core::IFile::AskForReload:
//            break;
//        }
//
//        switch (Core::Utils::reloadPrompt(fileName, isModified(), Core::ICore::instance()->mainWindow())) {
//        case Core::Utils::ReloadCurrent:
//            open(fileName);
//            break;
//        case Core::Utils::ReloadAll:
//            open(fileName);
//            *behavior = Core::IFile::ReloadAll;
//            break;
//        case Core::Utils::ReloadSkipCurrent:
//            break;
//        case Core::Utils::ReloadNone:
//            *behavior = Core::IFile::ReloadNone;
//            break;
//        }
    }



 JournalEditorInterface::JournalEditorInterface(JournalEditor *parent)
        : Core::IEditor(parent)
    {
        Core::UniqueIDManager *uidm = Core::UniqueIDManager::instance();
        m_editor = parent;
        m_file = new JournalEditorFile(parent);
//        m_context << uidm->uniqueIdentifier(Core::Constants::K_DEFAULT_BINARY_EDITOR);
        m_context << uidm->uniqueIdentifier(Constants::C_JOURNALEDITOR);
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
    JournalEditorInterface::~JournalEditorInterface() {}

    QWidget *JournalEditorInterface::widget() { return m_editor; }

    QList<int> JournalEditorInterface::context() const { return m_context; }

    bool JournalEditorInterface::createNew(const QString & /* contents */ ) {
        m_file->setFilename(QString());
        return true;
    }
    bool JournalEditorInterface::open(const QString &fileName ) {
        return true;/*m_file->open(fileName)*/;
    }
    Core::IFile *JournalEditorInterface::file() { return m_file; }
    const char* JournalEditorInterface::kind() const
    { return JOURNALEditor::Constants::C_JOURNALEDITOR; }
    QString JournalEditorInterface::displayName() const { return m_displayName; }
    void JournalEditorInterface::setDisplayName(const QString &title) { m_displayName = title; emit changed(); }

    bool JournalEditorInterface::duplicateSupported() const { return false; }
    Core::IEditor *JournalEditorInterface::duplicate(QWidget* )
    {   return 0; }

    QByteArray JournalEditorInterface::saveState() const { return QByteArray(); } // TODO
    bool JournalEditorInterface::restoreState(const QByteArray & /* state */) { return false; } // TODO

    QWidget *JournalEditorInterface::toolBar() { return 0 /*m_toolBar*/; }

    bool JournalEditorInterface::isTemporary() const { return true; }

///////////////////////////////// JournalEditorFactory //////////////////////////////////

JournalEditorFactory::JournalEditorFactory(JournalEditorPlugin *owner) :
    m_kind(QLatin1String(JOURNALEditor::Constants::C_JOURNALEDITOR)),
    m_mimeTypes(QLatin1String(JOURNALEditor::Constants::C_JOURNALEDITOR_MIMETYPE)),
    m_owner(owner)
{
}

QString JournalEditorFactory::kind() const
{
    return m_kind;
}

Core::IFile *JournalEditorFactory::open(const QString &fileName)
{

    Core::EditorManager *em = Core::EditorManager::instance();
    Core::IEditor *iface = em->openEditor(fileName, kind());
    return iface ? iface->file() : 0;
}

Core::IEditor *JournalEditorFactory::createEditor(QWidget *parent)
{
    JournalEditor *editor = new JournalEditor(parent);
    m_owner->initializeEditor(editor);
    return editor->editorInterface();
}

QStringList JournalEditorFactory::mimeTypes() const
{
    return m_mimeTypes;
}

///////////////////////////////// BinEditorPlugin //////////////////////////////////

JournalEditorPlugin::JournalEditorPlugin()
{
    m_undoAction = m_redoAction = m_copyAction = m_selectAllAction = 0;
}

JournalEditorPlugin::~JournalEditorPlugin()
{
}

QAction *JournalEditorPlugin::registerNewAction(const QString &id, const QString &title)
{
    QAction *result = new QAction(title, this);
    Core::ICore::instance()->actionManager()->registerAction(result, id, m_context);
    return result;
}

QAction *JournalEditorPlugin::registerNewAction(const QString &id,
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

void JournalEditorPlugin::initializeEditor(JournalEditor *editor)
{
    JournalEditorInterface *editorInterface = new JournalEditorInterface(editor);
    //QObject::connect(editor, SIGNAL(modificationChanged(bool)), editorInterface, SIGNAL(changed()));
    editor->setEditorInterface(editorInterface);

    Core::UniqueIDManager *uidm = Core::UniqueIDManager::instance();
    m_context << uidm->uniqueIdentifier(Constants::C_JOURNALEDITOR);
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
    //DirectoryEditorFind *DirectoryEditorFind = new DirectoryEditorFind(editor);
    //aggregate->add(binEditorFind);
    aggregate->add(editor);
}

bool JournalEditorPlugin::initialize(const QStringList &arguments, QString *errorMessage)
{
    Q_UNUSED(arguments)
    Q_UNUSED(errorMessage)

    Core::ICore *core = Core::ICore::instance();
    if (!core->mimeDatabase()->addMimeTypes(QLatin1String(":/directoryeditor/DirectoryEditor.mimetypes.xml"), errorMessage))
        return false;

    connect(core, SIGNAL(contextAboutToChange(Core::IContext *)),
        this, SLOT(updateCurrentEditor(Core::IContext *)));

    addAutoReleasedObject(new JournalEditorFactory(this));

    return true;
}

void JournalEditorPlugin::extensionsInitialized()
{
}

void JournalEditorPlugin::updateCurrentEditor(Core::IContext *object)
{
    do {
        if (!object) {
            if (!m_currentEditor)
                return;

            m_currentEditor = 0;
            break;
        }
        JournalEditor *editor = qobject_cast<JournalEditor *>(object->widget());
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
    updateActions();
}

void JournalEditorPlugin::updateActions()
{
    bool hasEditor = (m_currentEditor != 0);
//    if (m_selectAllAction)
//        m_selectAllAction->setEnabled(hasEditor);
//    if (m_undoAction)
//        m_undoAction->setEnabled(m_currentEditor && m_currentEditor->isUndoAvailable());
//    if (m_redoAction)
//        m_redoAction->setEnabled(m_currentEditor && m_currentEditor->isRedoAvailable());
//    if (m_copyAction)
//        m_copyAction->setEnabled(m_currentEditor && m_currentEditor->hasSelection());
}

void JournalEditorPlugin::undoAction()
{
//    if (m_currentEditor)
//        m_currentEditor->undo();
}

void JournalEditorPlugin::redoAction()
{
//    if (m_currentEditor)
//        m_currentEditor->redo();
}

void JournalEditorPlugin::copyAction()
{
//    if (m_currentEditor)
//        m_currentEditor->copy();
}

void JournalEditorPlugin::selectAllAction()
{
//    if (m_currentEditor)
//        m_currentEditor->selectAll();
}


Q_EXPORT_PLUGIN(JournalEditorPlugin)

//#include "journaleditorplugin.moc"
