#ifndef DIRECTORYEDITOR_H
#define DIRECTORYEDITOR_H

#include <QMainWindow>
#include "ananasprojectmanager/libananas/acfg.h"
#include <extensionsystem/iplugin.h>
#include <coreplugin/editormanager/ieditorfactory.h>
#include <coreplugin/ifile.h>
#include <texteditor/basetexteditor.h>
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
    virtual void updateMD(QList<Core::IEditor*> editor);
    virtual void eSv_activated( int index );
    virtual void eSvG_activated( int index );

protected:
    int newVariable;

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
    void doubleClickedElement ( int row, int column );
    void doubleClickedGroup ( int row, int column );
    void doubleClickedForm ( int row, int column );
    void editElementAttribute_clicked();
    void editGroupAttribute_clicked();
    void createNewElementAttribute_clicked();
    void createNewGroupAttribute_clicked();
    void elementAttributesList_selectionChanged();
    void deleteElementAttribute_clicked();
    void activateElement(QTableWidgetItem* item);
    void activateGroup(QTableWidgetItem* item);
    void activateForm(QTableWidgetItem* item);
    void moveUpElementAttribute_clicked();
    void moveDownElementAttribute_clicked();
    void deleteGroupAttribute_clicked();
    void moveUpGroupAttribute_clicked();
    void moveDownGroupAttribute_clicked();
    void deleteForm_clicked();
};
}

#endif // DIRECTORYEDITOR_H
