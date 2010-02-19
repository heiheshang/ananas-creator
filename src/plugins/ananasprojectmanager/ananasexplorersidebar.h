//
#ifndef ANANASEXPLORERSIDEBAR_H
#define ANANASEXPLORERSIDEBAR_H

#include <QObject>
#include <QAbstractItemModel>
#include <QTreeWidget>
#include <projectexplorer/projectexplorer.h>
#include "libananas/acfg.h"

namespace AnanasProjectManager {
namespace Internal {

class ananasListViewModel : public QAbstractItemModel
{
        Q_OBJECT
public:
     ananasListViewModel(DomCfgItem *document, QObject *parent = 0);
    ~ananasListViewModel();

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

class AnanasExplorerSideBar : public QTreeView
{
    Q_OBJECT
public:
    AnanasExplorerSideBar(/*const QString &fname,*/ QWidget *parent);
    ~AnanasExplorerSideBar();
private:
    QHash<QString, QString> read(const QString &fname);
    int read_xml();
    DomCfgItem *cfg;
    QString  cfgFile;
    QHash<QString, QString> valueRc;
private:
        void openSprModule();
protected:
        void closeEvent( QCloseEvent * event );
private slots:
        void showmenu (const QPoint &p);
        void actionTree (QAction *a);
        void doubleClicked ( const QModelIndex & index );
        void setCurrentFile(ProjectExplorer::Project* project);
        void updateActions();
};
}
}
#endif // ANANASEXPLORERSIDEBAR_H
