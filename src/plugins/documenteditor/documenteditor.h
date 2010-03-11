
#ifndef DOCUMENTEDITOR_H
#define DOCUMENTEDITOR_H

#include <QMainWindow>
#include "ananasprojectmanager/libananas/acfg.h"
#include <extensionsystem/iplugin.h>
#include <coreplugin/editormanager/ieditorfactory.h>
#include <coreplugin/ifile.h>
#include <texteditor/basetexteditor.h>
#include <ui_documenteditor.h>

namespace Core {
class IEditor;
}

namespace TextEditor {
class FontSettings;
}

namespace DocumentEditor {

    class headerViewModel : public QAbstractListModel
    {
            Q_OBJECT
    public:
         headerViewModel(DomCfgItem *document, QObject *parent = 0);
        ~headerViewModel();

        int rowCount(const QModelIndex &parent) const;
        //int columnCount(const QModelIndex &parent) const;

        QVariant data(const QModelIndex &index, int role) const;

        Qt::ItemFlags flags(const QModelIndex &index) const;

    private:

        DomCfgItem *rootItem;

    };

    class tableViewModel : public QAbstractItemModel
    {
            Q_OBJECT
    public:
         tableViewModel(DomCfgItem *document, QObject *parent = 0);
        ~tableViewModel();

        QModelIndex index(int row, int column, const QModelIndex &parent) const;
        QModelIndex parent(const QModelIndex &child) const;

        int rowCount(const QModelIndex &parent) const;
        int columnCount(const QModelIndex &parent) const;

        QVariant data(const QModelIndex &index, int role) const;
        QVariant headerData(int section, Qt::Orientation orientation, int role) const;

        Qt::ItemFlags flags(const QModelIndex &index) const;
        QString info() const;
        bool hasChildren ( const QModelIndex & parent = QModelIndex() ) const;
    private:

        DomCfgItem *rootItem;
        QModelIndex createIndexByTags(const QString & md_const,int row, int column,DomCfgItem *parent) const;

    };


class DocumentEdit : public QMainWindow , public Ui::DocumentEdit
{
    Q_OBJECT

public:
    DocumentEdit(QWidget* parent = 0, const char* name = 0, Qt::WindowFlags fl = Qt::Window );
    ~DocumentEdit();

    //aRoleEditor *re;
    //QHash<int,DomCfgItem*> fields, fieldsg;
    //aAliasEditor *al;
    DomCfgItem *item;

    void setModified(bool);
    bool isModified() const;

    Core::IEditor *editorInterface() const { return m_ieditor; }
    void setEditorInterface(Core::IEditor *ieditor) { m_ieditor = ieditor; }
    Q_INVOKABLE void setData( DomCfgItem * o );
public slots:
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
    void getHeaderAttributesList();
    void getTablesAttributesList();

    QHash<QString,QString> openForm;
private slots:
//    void doubleClickedElement ( int row, int column );
//    void doubleClickedGroup ( int row, int column );
//    void doubleClickedForm ( int row, int column );
//    void editElementAttribute_clicked();
//    void editGroupAttribute_clicked();
//    void createNewElementAttribute_clicked();
//    void createNewGroupAttribute_clicked();
//    void elementAttributesList_selectionChanged();
//    void deleteElementAttribute_clicked();
//    void activateElement(QTableWidgetItem* item);
//    void activateGroup(QTableWidgetItem* item);
//    void activateForm(QTableWidgetItem* item);
//    void moveUpElementAttribute_clicked();
//    void moveDownElementAttribute_clicked();
//    void deleteGroupAttribute_clicked();
//    void moveUpGroupAttribute_clicked();
//    void moveDownGroupAttribute_clicked();
//    void editForm_clicked();
//    void deleteForm_clicked();
//    void updateForm(QList<Core::IEditor*> editor);
};
}

#endif // DOCUMENTEDITOR_H
