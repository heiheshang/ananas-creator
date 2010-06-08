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
        void insertCol(QDomElement &col);
        bool removeRows ( int row, int count, const QModelIndex & parent = QModelIndex() ) ;
    private:

        DomCfgItem *rootItem;
        QModelIndex createIndexByTags(const QString & md_const,int row, int column,DomCfgItem *parent) const;

    };

    class docViewModel : public QAbstractItemModel
    {
            Q_OBJECT
    public:
         docViewModel(DomCfgItem *document, QObject *parent = 0);
        ~docViewModel();

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
    void typeChange(int index);
    void addDoc();
    void removeDoc();
    void moveUp();
    void moveDown();
    void removeCol();
    void addCol();
    void newCol();
};
}

#endif // JOURNALEDITOR_H
