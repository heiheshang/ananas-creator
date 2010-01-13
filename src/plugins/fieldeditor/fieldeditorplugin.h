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

#ifndef FIELDEDITORPLUGIN_H
#define FIELDEDITORPLUGIN_H

#include <extensionsystem/iplugin.h>
#include <coreplugin/editormanager/ieditorfactory.h>

#include <QtCore/QtPlugin>
#include <QtCore/QPointer>
#include <QtCore/QStringList>
#include <QtGui/QAction>
#include <qtoolbar.h>
#include <utils/linecolumnlabel.h>
#include <fieldeditor/fieldeditorconstants.h>
#include <ananasprojectmanager/libananas/acfg.h>

namespace Core {
class IWizard;
}

namespace FIELDEditor {
class FieldEditor;
namespace Internal {
class FieldEditorFactory;


class FieldEditorPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT

public:
    FieldEditorPlugin();
    ~FieldEditorPlugin();

    bool initialize(const QStringList &arguments, QString *error_message = 0);
    void extensionsInitialized();

    // Connect editor to settings changed signals.
    void initializeEditor(FieldEditor *editor);
private slots:
    void updateCurrentEditor(Core::IContext *object);

private:
    QList<int> m_context;
    QAction *registerNewAction(const QString &id, const QString &title = QString());
    QAction *registerNewAction(const QString &id, QObject *receiver, const char *slot,
                               const QString &title = QString());

    friend class FieldEditorFactory;
    Core::IEditor *createEditor(QWidget *parent);

    typedef QList<Core::IWizard *> WizardList;
    WizardList m_wizards;
    FieldEditorFactory *m_factory;
    QPointer<FieldEditor> m_currentEditor;
};

class  FieldEditorFactory : public Core::IEditorFactory
{
    Q_OBJECT

public:
    explicit FieldEditorFactory(FieldEditorPlugin *owner);

    virtual QStringList mimeTypes() const;

    Core::IEditor *createEditor(QWidget *parent);
    QString kind() const;
    Core::IFile *open(const QString &fileName);

private:
    const QString m_kind;
    const QStringList m_mimeTypes;
    FieldEditorPlugin *m_owner;
};

class FieldEditorFile : public Core::IFile
{
    Q_OBJECT
public:
    FieldEditorFile(FieldEditor *parent) ;
    ~FieldEditorFile();

    virtual QString mimeType() const;

    bool save(const QString &fileName = QString());

    bool open(const QString &fileName);

private slots:
    void provideData(int block);
public:

    void setFilename(const QString &filename);

    QString fileName() const;

    QString defaultPath() const;
    QString suggestedFileName() const;
    QString fileFilter() const;
    QString fileExtension() const;

    bool isModified() const;
    bool isReadOnly() const;

    bool isSaveAsAllowed() const;

    void modified(ReloadBehavior *behavior);

private:
    const QString m_mimeType;
    FieldEditor *m_editor;
    QString m_fileName;
};


class FieldEditorInterface : public Core::IEditor
{
    Q_OBJECT
public:
    FieldEditorInterface(FieldEditor *parent);
    ~FieldEditorInterface();

    QWidget *widget();

    QList<int> context() const;

    bool createNew(const QString & /* contents */ = QString());
    bool open(const QString &fileName = QString());
    Core::IFile *file();
    const char *kind() const;
    QString displayName() const;
    void setDisplayName(const QString &title);

    bool duplicateSupported() const;
    IEditor *duplicate(QWidget * /* parent */);

    QByteArray saveState() const;
    bool restoreState(const QByteArray & /* state */);

    QWidget *toolBar();

    bool isTemporary() const;
    void setData(DomCfgItem* item);
signals:
    void changed();

public slots:
    //void updateCursorPosition(int position) {
    //    m_cursorPositionLabel->setText(m_editor->addressString((uint)position),
    //                                   m_editor->addressString((uint)m_editor->data().size()));
    //}

private:
    FieldEditor *m_editor;
    QString m_displayName;
    FieldEditorFile *m_file;
    QList<int> m_context;
    QToolBar *m_toolBar;
    Core::Utils::LineColumnLabel *m_cursorPositionLabel;
};


} // namespace Internal
} // namespace FieldEditor

#endif // FIELDEDITORPLUGIN_H
