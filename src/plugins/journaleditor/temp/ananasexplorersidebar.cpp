#include "ananasexplorersidebar.h"
#include <QTextStream>
#include <QtXml/QtXml>
#include <Qt>
#include <coreplugin/editormanager/editormanager.h>
#include <coreplugin/messagemanager.h>
#include <coreplugin/messageoutputwindow.h>
#include <coreplugin/icore.h>
#include <coreplugin/filemanager.h>
#include <texteditor/fontsettings.h>
#include <texteditor/texteditorsettings.h>
#include <qtscripteditor/qtscripteditor.h>
#include <projectexplorer/project.h>
#include "libananas/configinfo.h"

using namespace AnanasProjectManager;
using namespace AnanasProjectManager::Internal;

namespace {
bool debug = true;
}


ananasListViewModel::ananasListViewModel(DomCfgItem *document, QObject *parent)
    : QAbstractItemModel(parent)
{
    rootItem = document;
}

ananasListViewModel::~ananasListViewModel()
{
    delete rootItem;
}

int ananasListViewModel::columnCount(const QModelIndex &/*parent*/) const
{
    return 1;
}
QString ananasListViewModel::info() const
{
  QDomElement rootnode = rootItem->node().toDocument().documentElement();
  return rootnode.namedItem(md_info ).toElement().namedItem("name").toElement().text();
}

QVariant ananasListViewModel::data(const QModelIndex &index, int role) const
{
if ( !index.isValid() )
        return QVariant();
if ( role == Qt::DecorationRole )
{
        DomCfgItem *item = static_cast<DomCfgItem*> ( index.internalPointer() );

        QDomNode node = item->node();
        return item->iconNode();

}
if ( role == Qt::DisplayRole )
{
        DomCfgItem *item = static_cast<DomCfgItem*> ( index.internalPointer() );
        QDomNode node = item->node();
        QString nodeName = node.nodeName();
        if ( nodeName=="xml" )
        {
                return info();
        }
        QString name = QObject::tr("%1");
        return name.arg(item->cfgName());
}
return QVariant();
}

Qt::ItemFlags ananasListViewModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant ananasListViewModel::headerData(int section, Qt::Orientation orientation,
                              int role) const
{
    Q_UNUSED(section);
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
         return info();
    }

    return QVariant();
}

QModelIndex ananasListViewModel::createIndexByTags(const QString & md_const,int row, int column, DomCfgItem *parent) const
{
        QDomNodeList listNodes = rootItem->node().toDocument().elementsByTagName(md_const);
        QDomNode node = listNodes.item(0);
        DomCfgItem *nodeMd = new DomCfgItem(node,row,parent);
        return createIndex(row, column, nodeMd);
}

QModelIndex ananasListViewModel::index(int row, int column, const QModelIndex &parent)
            const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    DomCfgItem *parentItem;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<DomCfgItem*>(parent.internalPointer());

    DomCfgItem *childItem = parentItem->child(row);

    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex ananasListViewModel::parent(const QModelIndex &child) const
{
    if (!child.isValid())
        return QModelIndex();

    DomCfgItem *childItem = static_cast<DomCfgItem*>(child.internalPointer());
    DomCfgItem *parentItem = childItem->parent();

    if (!parentItem || parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int ananasListViewModel::rowCount(const QModelIndex &parent) const
{
    if (parent.column() > 0)
        return 0;

    DomCfgItem *parentItem;

    if (!parent.isValid())
        {
        parentItem = rootItem;
        return md_row_count;
        }
    else
        parentItem = static_cast<DomCfgItem*>(parent.internalPointer());
    return parentItem->childCount();
}

bool ananasListViewModel::hasChildren ( const QModelIndex & parent ) const
{
   DomCfgItem *item;
if (!parent.isValid()) {
    return true;
} else
    item = static_cast<DomCfgItem*>(parent.internalPointer());
QDomNode node = item->node();
if (node.nodeName()==md_field)
    return false;
if (item->hasChildren())
 return true;
return false;
}

AnanasExplorerSideBar::AnanasExplorerSideBar(/*const QString &fname,*/ QWidget *parent):QTreeView(parent)
{
    ProjectExplorer::ProjectExplorerPlugin *pe = ProjectExplorer::ProjectExplorerPlugin::instance();
    connect(pe, SIGNAL(currentProjectChanged(ProjectExplorer::Project*)),
                this, SLOT(setCurrentFile(ProjectExplorer::Project*)));
    if (pe->currentProject())
        setCurrentFile(pe->currentProject());
         connect(this,
            SIGNAL( customContextMenuRequested(const QPoint &) ),
            this,SLOT(showmenu(const QPoint &)));
connect(this,SIGNAL(doubleClicked ( const QModelIndex & )),this,SLOT(doubleClicked ( const QModelIndex & )));

    //}

}

void AnanasExplorerSideBar::setCurrentFile(ProjectExplorer::Project* project)
{
    if (!project)
        return ;
    QString fileName=project->file()->fileName();
    if (debug)
        qDebug() << "AnanasExplorerSideBar::setCurrentFile(" << fileName << ")";

    QString configfile;

    cfgFile=fileName;
    valueRc = read(fileName);
    if ( valueRc.count()!=0 ) {
    configfile = valueRc["configfile"];
    if ( !configfile.isEmpty()) {
        if (read_xml()!=RC_ERROR) {
           QAbstractItemModel *data = new ananasListViewModel(cfg,this->parent());
           QItemSelectionModel *selections = new QItemSelectionModel(data);
           setModel(data);
           setHeaderHidden(true);
           setSelectionModel(selections);
           setUniformRowHeights(true);
           header()->setStretchLastSection(false);
           viewport()->setAttribute(Qt::WA_StaticContents);
           header()->setResizeMode(QHeaderView::Stretch);
           setAttribute(Qt::WA_MacShowFocusRect, false);
           setContextMenuPolicy(Qt::CustomContextMenu);
           setWindowFlags(Qt::Widget);
       }
       else
        Core::MessageManager::instance()->printToOutputPane(tr("Not found file %1").arg(configfile));
       }
 }
}

QHash<QString, QString> AnanasExplorerSideBar::read(const QString &fname){
  QFile file( fname );
  QHash<QString, QString> values;
 if ( file.open( QIODevice::ReadOnly ) )
 {
        QTextStream stream( &file );
        QString line, k, v;

        stream.setCodec("UTF-8");
        while ( !stream.atEnd() ) {
                line = stream.readLine(); // line of text excluding '\n'
                k = line.section("=",0,0);
                v = line.section("=",1,100); if ( v.isNull() ) v = "";
                values.insert(k,v);
        }
        file.close();
}
return values;
}

AnanasExplorerSideBar::~AnanasExplorerSideBar()
{
}

int AnanasExplorerSideBar::read_xml()
{
    QFile file(valueRc["configfile"]);
    QByteArray buf;
    QDomDocument xml;
    QString err;
    int errLine = 0, errColumn = 0;
    if ( !file.open( QIODevice::ReadOnly ) ) return RC_ERROR;
    buf = file.readAll();
    file.close();
    xml.setContent( QString("") );
    if ( !xml.setContent( buf, false, &err, &errLine, &errColumn ) ) {
        Core::ICore::instance()->messageManager()->printToOutputPane(QObject::tr(
                     "Error read configuration line:%1 col:%2 %3"
                     ).arg( errLine ).arg( errColumn ).arg( err ),true);
        return RC_ERROR;
    }
    cfg = new DomCfgItem(xml,0,0);
    return RC_OK;
}

void AnanasExplorerSideBar::showmenu (const QPoint &pos)
{
        DomCfgItem *item = static_cast<DomCfgItem*> ( currentIndex().internalPointer() );

        QMenu *contextMenu = item->menu();
        if (contextMenu) {
        connect(contextMenu,
            SIGNAL(triggered(QAction *)),
            this,SLOT(actionTree( QAction *)));
        contextMenu->exec(viewport()->mapToGlobal(pos));
        }
}
void AnanasExplorerSideBar::actionTree(QAction *a)
{
DomCfgItem *item = static_cast<DomCfgItem*> ( currentIndex().internalPointer() );
QDomNode node = item->node();
    if (a->text()==tr("Property")) {

        if (node.nodeName()=="xml") {
            configInfo *info = new configInfo(item,this,Qt::Dialog);
            info->show();
        }
    }
    if (a->text()==tr("Edit")) {
        if (node.nodeName()==md_catalogue) {
           QString titlePattern = tr("Catalogue $");
           QDomNode stringView =  item->node();
           Core::EditorManager* manager = Core::EditorManager::instance();

           QString cfgName = item->cfgName();
           Core::IEditor* editor = manager->openEditorWithContents("Directory Editor", &cfgName,"");
           if (editor) {
            manager->activateEditor(editor);

           if (!QMetaObject::invokeMethod(editor->widget(), "setData",Qt::DirectConnection,
            Q_ARG(DomCfgItem*, item)))
               qCritical() << "Can't invoke method!";
            connect(manager,SIGNAL(editorsClosed(QList<Core::IEditor*>)),editor->widget(),SLOT(updateMD(QList<Core::IEditor*>)));
          }
        }
        //JOURNAL EDITOR
        if(node.nodeName()==md_journal){
            QString titlePattern = tr("Journal $");
            QDomNode stringView =  item->node();
            Core::EditorManager* manager = Core::EditorManager::instance();

            QString cfgName = item->cfgName();
            Core::IEditor* editor = manager->openEditorWithContents("Journal Editor", &cfgName,"");
            if (editor) {
             manager->activateEditor(editor);

            if (!QMetaObject::invokeMethod(editor->widget(), "setData",Qt::DirectConnection,
              Q_ARG(DomCfgItem*, item)))
                 qCritical() << "Can't invoke method!";
              connect(manager,SIGNAL(editorsClosed(QList<Core::IEditor*>)),editor->widget(),SLOT(updateMD(QList<Core::IEditor*>)));
            }
        }
    }

    if (a->text()==tr("New")) {
        if (node.nodeName()==md_catalogue) {
           QString titlePattern = tr("Catalogue $");

           DomCfgItem *catalogues=item->parent();
           //DomCfgItem *lastcatalogue = catalogues->child(catalogues->childCount()-1);
           DomCfgItem* catalogue = catalogues->newCatalogue();

           Core::EditorManager* manager = Core::EditorManager::instance();

           QString cfgName = catalogue->cfgName();
           Core::IEditor* editor = manager->openEditorWithContents("Directory Editor", &cfgName,"");
           if (editor) {
            manager->activateEditor(editor);

           if (!QMetaObject::invokeMethod(editor->widget(), "setData",Qt::DirectConnection,
            Q_ARG(DomCfgItem*, catalogue)))
               qCritical() << "Can't invoke method!";
            connect(manager,SIGNAL(editorsClosed(QList<Core::IEditor*>)),editor->widget(),SLOT(updateMD(QList<Core::IEditor*>)));
          }
        }
        if(node.nodeName()==md_journal){
            QString titlePattern = tr("Journal $");

            DomCfgItem *journals=item->parent();
            DomCfgItem* journal = journals->newJournal();
            Core::EditorManager* manager = Core::EditorManager::instance();

            QString cfgName = journal->cfgName();
            Core::IEditor* editor = manager->openEditorWithContents("Journal Editor", &cfgName,"");
            if (editor) {
             manager->activateEditor(editor);

            if (!QMetaObject::invokeMethod(editor->widget(), "setData",Qt::DirectConnection,
             Q_ARG(DomCfgItem*, journal)))
                qCritical() << "Can't invoke method!";
             connect(manager,SIGNAL(editorsClosed(QList<Core::IEditor*>)),editor->widget(),SLOT(updateMD(QList<Core::IEditor*>)));
        }
        }
    }

    if (a->text()==tr("Open global module")) {

        QDomNode global =  item->root()->node().namedItem(md_root).namedItem(md_metadata).namedItem(md_globals).namedItem(md_sourcecode);

        QString titlePattern = tr("Global module $");

        Core::EditorManager* manager = Core::EditorManager::instance();
        Core::IEditor* editor = manager->openEditorWithContents("Qt Script Editor", &titlePattern,global.toElement().text());

        if (editor)
            manager->activateEditor(editor);
        }
   if (a->text()==tr("Open element view")) {
        openSprModule();
        }
}

void AnanasExplorerSideBar::updateActions()
{
//    foreach (Core::IEditor *editor, editors) {
//        delete editor;
//    }
    qDebug() << "Close";
}
void AnanasExplorerSideBar::openSprModule()
{
        DomCfgItem *item = static_cast<DomCfgItem*> ( currentIndex().internalPointer() );
        QString titlePattern = tr("Directory $");
        QDomNode stringView = item->node();
        Core::EditorManager* manager = Core::EditorManager::instance();

        Core::IEditor* editor = manager->openEditorWithContents("Qt Script Editor", &titlePattern,stringView.toElement().text());

        if (editor) {
            manager->activateEditor(editor);
        }
}
void AnanasExplorerSideBar::closeEvent( QCloseEvent * event )
{
    Q_UNUSED(event);
    //int response = 0;

}

void AnanasExplorerSideBar::doubleClicked ( const QModelIndex & index )
{
if ( index.isValid() ) {
        DomCfgItem *item = static_cast<DomCfgItem*> ( index.internalPointer() );
        QString nodeName = item->node().nodeName();
        if ( nodeName==md_field )
        {
            Core::EditorManager* manager = Core::EditorManager::instance();

            QString cfgName = item->cfgName();
            Core::IEditor* editor = manager->openEditorWithContents("Field Editor", &cfgName,"");
            if (editor) {
               connect(editor, SIGNAL(changed()), this, SLOT(updateActions()));
             manager->activateEditor(editor);
             QMetaObject::invokeMethod(editor->widget(), "setData",
             Q_ARG(DomCfgItem*, item));
             connect(manager,SIGNAL(editorsClosed(QList<Core::IEditor*>)),editor->widget(),SLOT(updateMD(QList<Core::IEditor*>)));
          }
        }
//        if ( nodeName==md_form )
//        {
//                dEditDialog *dialog = new dEditDialog(this,"",Qt::Dialog|Qt::WDestructiveClose);
//                dialog->setData(item);
//                dialog->show();
//        }
        if ( nodeName==md_svfunction)
        {
                openSprModule();
        }


 }
}
