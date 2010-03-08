#ifndef JOURNALEDITOR_H
#define JOURNALEDITOR_H

#include <QMainWindow>
#include "ananasprojectmanager/libananas/acfg.h"
#include <extensionsystem/iplugin.h>
#include <coreplugin/editormanager/ieditorfactory.h>
#include <coreplugin/ifile.h>
#include <texteditor/basetexteditor.h>
#include <ui_journaleditor.h>

namespace Core {
class IEditor;
}

namespace TextEditor {
class FontSettings;
}

namespace JOURNALEditor {


    class JournalEditor : public QMainWindow , public Ui::JournalEdit
{
    Q_OBJECT

public:
    JournalEditor(QWidget* parent = 0, const char* name = 0, Qt::WindowFlags fl = Qt::Window );
    ~JournalEditor();

    //aRoleEditor *re;
    QHash<int,DomCfgItem*> fields, fieldsg;
    //aAliasEditor *al;
    DomCfgItem *item;
    //DomCfgItem * docs;
    //DomCfgItem * doc;
    void setModified(bool);
    bool isModified() const;
    Core::IEditor *editorInterface() const { return m_ieditor; }
    void setEditorInterface(Core::IEditor *ieditor) { m_ieditor = ieditor; }
    Q_INVOKABLE void setData( DomCfgItem * o );
public slots:
    //virtual void setData( DomCfgItem * o );
    virtual void updateMD(QList<Core::IEditor*> editor);
    virtual void eSv_activated( int index );
    virtual void eSvG_activated( int index );

protected:
    int newVariable;

protected slots:
    virtual void languageChange();

private:
    QHash<QString,DomCfgItem*> used_doc;
    void init();
    void destroy();
    Core::IEditor *m_ieditor;
    void getAllDocsList();
    void getJournalDocs();
    void getJournalColumns();
    void getUsedDoc();
    void deleteDoc(QString name);
private slots:
    void typeChange();
    void addDoc();
    void removeDoc();
    void moveUp();
    void moveDown();
    void removeCol();
};
}

#endif // JOURNALEDITOR_H
