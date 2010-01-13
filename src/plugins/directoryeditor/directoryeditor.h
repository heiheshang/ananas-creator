#ifndef DIRECTORYEDITOR_H
#define DIRECTORYEDITOR_H

#include <QMainWindow>
#include "ananasprojectmanager/libananas/acfg.h"
#include <ui_directoryeditor.h>

namespace Core {
class IEditor;
}

namespace TextEditor {
class FontSettings;
}

namespace DIRECTORYEditor {


class DirectoryEditor : public QMainWindow , public Ui::DirectoryEdit
{
    Q_OBJECT

public:
    DirectoryEditor(QWidget* parent = 0, const char* name = 0, Qt::WindowFlags fl = Qt::Window );
    ~DirectoryEditor();

    //aRoleEditor *re;
    QHash<int,DomCfgItem*> fields, fieldsg;
    //aAliasEditor *al;
    DomCfgItem *item;

    void setModified(bool);
    bool isModified() const;

    Core::IEditor *editorInterface() const { return m_ieditor; }
    void setEditorInterface(Core::IEditor *ieditor) { m_ieditor = ieditor; }
    Q_INVOKABLE void setData( DomCfgItem * o );
public slots:
    //virtual void setData( DomCfgItem * o );
    virtual void updateMD();
    virtual void eSv_activated( int index );
    virtual void eSvG_activated( int index );

protected:
    int newVariable;
    void hideEvent (QHideEvent * event);
protected slots:
    virtual void languageChange();

private:
    void init();
    void destroy();
    Core::IEditor *m_ieditor;
    void GetElementAttributesList();
    void GetGroupAttributesList();
    void GetFormsList();
    void CatList();
private slots:
    void doubleClicked ( int row, int column );
    void edit_field ();
    void createNewElementAttribute_clicked();
};
}

#endif // DIRECTORYEDITOR_H
