#include "journaleditor.h"
#include <coreplugin/editormanager/editormanager.h>
#include <QTableWidgetItem>
#include <QStandardItemModel>
#include <coreplugin/messagemanager.h>
#include <coreplugin/messageoutputwindow.h>
#include <QDebug>
#include <QInputDialog>

using namespace JOURNALEditor;

tableViewModel::tableViewModel(DomCfgItem *document, QObject *parent)
    : QAbstractItemModel(parent)
{
    rootItem = document;
}

tableViewModel::~tableViewModel()
{
    //delete rootItem;
}

int tableViewModel::columnCount(const QModelIndex &/*parent*/) const
{
    return 1;
}

QVariant tableViewModel::data(const QModelIndex &index, int role) const
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
        if (item->nodeName()==md_fieldid)
        {
            DomCfgItem* f = item->root()->findObjectById(item->node().childNodes().item(0).nodeValue());
            QString nameObject = f->parent()->parent()->cfgName()+"."+f->attr(mda_name);
            return nameObject;


        }
        QString name = QObject::tr("%1");
        return name.arg(item->cfgName());
}
return QVariant();
}

Qt::ItemFlags tableViewModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant tableViewModel::headerData(int section, Qt::Orientation orientation,
                              int role) const
{
    Q_UNUSED(section);
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
         return tr("Colums");//QString();
    }

    return QVariant();
}

QModelIndex tableViewModel::createIndexByTags(const QString & md_const,int row, int column, DomCfgItem *parent) const
{
        QDomNodeList listNodes = rootItem->node().toDocument().elementsByTagName(md_const);
        QDomNode node = listNodes.item(0);
        DomCfgItem *nodeMd = new DomCfgItem(node,row,parent);
        return createIndex(row, column, nodeMd);
}

QModelIndex tableViewModel::index(int row, int column, const QModelIndex &parent)
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

QModelIndex tableViewModel::parent(const QModelIndex &child) const
{
    if (!child.isValid())
        return QModelIndex();

    DomCfgItem *childItem = static_cast<DomCfgItem*>(child.internalPointer());
    DomCfgItem *parentItem = childItem->parent();

    if (!parentItem || parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int tableViewModel::rowCount(const QModelIndex &parent) const
{
    if (parent.column() > 0)
        return 0;

    DomCfgItem *parentItem;

    if (!parent.isValid())
        {
        parentItem = rootItem;
        return parentItem->childCount();
        }
    else
        parentItem = static_cast<DomCfgItem*>(parent.internalPointer());
    return parentItem->childCount();
}

bool tableViewModel::hasChildren ( const QModelIndex & parent ) const
{
   DomCfgItem *item;
if (!parent.isValid()) {
    return true;
} else
    item = static_cast<DomCfgItem*>(parent.internalPointer());
    QDomNode node = item->node();
    if (item->nodeName()==md_fieldid)
        return false;
    if (item->nodeName()==md_column)
        return node.hasChildNodes();
    if (item->hasChildren())
     return true;
    return false;
}

void tableViewModel::insertCol(QDomElement &col)
{
    beginInsertRows(QModelIndex(),rowCount(QModelIndex()),rowCount(QModelIndex())+1);
    rootItem->find(md_columns)->node().appendChild(col);
    createIndex(rowCount(QModelIndex()), 0, rootItem->child(rowCount(QModelIndex())));
    endInsertRows();
}

bool tableViewModel::removeRows ( int row, int count, const QModelIndex & parent )
{
    DomCfgItem* column;

    if (!parent.isValid()) {
        column = rootItem;
    }
    else
    {
        column = static_cast<DomCfgItem*> ( parent.internalPointer() );
    }
//    if (column->nodeName()==md_column)
//    {
//        beginRemoveRows(parent,row,row+count-1);
//        DomCfgItem* p = column->parent();
//        p->remove(row);
//        endRemoveRows();
//        return;
//    }
    //if (column->nodeName()==md_fieldid)
    //{
    beginRemoveRows(parent,row,row+count-1);
        for (int i=0;i<=column->childCount()-1;i++)
            qDebug() << column->child(i)->attr(mda_name);
        column->remove(row);
        for (int i=0;i<=column->childCount()-1;i++)
            qDebug() << column->child(i)->attr(mda_name);
        endRemoveRows();
        return true;
    //}

}

docViewModel::docViewModel(DomCfgItem *document, QObject *parent)
    : QAbstractItemModel(parent)
{
    rootItem = document;
}

docViewModel::~docViewModel()
{
    //delete rootItem;
}

int docViewModel::columnCount(const QModelIndex &/*parent*/) const
{
    return 1;
}

QVariant docViewModel::data(const QModelIndex &index, int role) const
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

        QString name = QObject::tr("%1");
        return name.arg(item->cfgName());
}
return QVariant();
}

Qt::ItemFlags docViewModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant docViewModel::headerData(int section, Qt::Orientation orientation,
                              int role) const
{
    Q_UNUSED(section);
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
         return tr("Colums");//QString();
    }

    return QVariant();
}

QModelIndex docViewModel::createIndexByTags(const QString & md_const,int row, int column, DomCfgItem *parent) const
{
        QDomNodeList listNodes = rootItem->node().toDocument().elementsByTagName(md_const);
        QDomNode node = listNodes.item(0);
        DomCfgItem *nodeMd = new DomCfgItem(node,row,parent);
        return createIndex(row, column, nodeMd);
}

QModelIndex docViewModel::index(int row, int column, const QModelIndex &parent)
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

QModelIndex docViewModel::parent(const QModelIndex &child) const
{
    if (!child.isValid())
        return QModelIndex();

    DomCfgItem *childItem = static_cast<DomCfgItem*>(child.internalPointer());
    DomCfgItem *parentItem = childItem->parent();

    if (!parentItem || parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int docViewModel::rowCount(const QModelIndex &parent) const
{
    if (parent.column() > 0)
        return 0;

    DomCfgItem *parentItem;

    if (!parent.isValid())
        {
        parentItem = rootItem;
        return parentItem->childCount();
        }
    else
        parentItem = static_cast<DomCfgItem*>(parent.internalPointer());
        if (parentItem->nodeName()==md_document)
            return 1;
    return parentItem->childCount();
}

bool docViewModel::hasChildren ( const QModelIndex & parent ) const
{
   DomCfgItem *item;
if (!parent.isValid()) {
    return true;
} else
    item = static_cast<DomCfgItem*>(parent.internalPointer());
    QDomNode node = item->node();
    if (node.nodeName()==md_field || node.nodeName()==md_tables || node.nodeName()==md_forms)
        return false;

    if (item->hasChildren())
     return true;
    return false;
}

JournalEditor::JournalEditor(QWidget* parent, const char* name, Qt::WindowFlags fl)
    : QMainWindow(parent, fl)
{
    Q_UNUSED(name);
    setupUi(this);
    //(void)statusBar();
    //init();
}

/*
 *  Destroys the object and frees any allocated resources
 */
JournalEditor::~JournalEditor()
{
    destroy();
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void JournalEditor::languageChange()
{
    retranslateUi(this);
}

void JournalEditor::setData( DomCfgItem *o )
{
    item = o;

    setWindowTitle( tr("Journal:") + item->attr( mda_name ) );
    eName->setText( item->attr( mda_name ) );
    eDescription->setText( item->attr( md_description ) );

    cbType->setCurrentIndex(item->attr(mda_type).toInt());

    connect(cbType,SIGNAL(currentIndexChanged(int)),this,SLOT(typeChange(int)));
    connect(bAddDoc,SIGNAL(clicked()),this,SLOT(addDoc()));
    connect(bRemoveDoc, SIGNAL(clicked()),this,SLOT(removeDoc()));
    connect(bMoveUp,SIGNAL(clicked()),this,SLOT(moveUp()));
    connect(bMoveDown,SIGNAL(clicked()),this,SLOT(moveDown()));
    connect(bRemoveDocCol,SIGNAL(clicked()),this,SLOT(removeCol()));
    connect(bAddCol,SIGNAL(clicked()),this,SLOT(addCol()));
    connect(bNewCol,SIGNAL(clicked()),this,SLOT(newCol()));
    getUsedDoc();
    getAllDocsList();
    getJournalDocs();
    getJournalColumns();
}



void JournalEditor::typeChange(int )
{
//    if ( cbType->currentIndex() != 1 )
//        tabWidget18->removeTab(2);
//    else
//    tabWidget18->insertTab(2,tabWidget18,QObject::tr("Documents"));

    item->setAttr(mda_type,QString(cbType->currentIndex()));

}

void JournalEditor::newCol()
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("Column name"),
             tr("column name:"), QLineEdit::Normal,
             "", &ok);
    if (ok && !text.isEmpty()) {
        QDomElement i;
        QDomDocument xml;
        i = xml.createElement(md_column);
        i.setAttribute(mda_name,text);
        i.setAttribute(mda_id,(int)item->nextID());
        ((tableViewModel*) columnsDoc->model())->insertCol(i);
    }

}

void JournalEditor::addDoc()
{
    int i,childCount;
    DomCfgItem* docs = item->root()->find(md_documents);
    childCount = journalDocs->rowCount();
    QList<QTableWidgetItem*>  itemList;
    itemList= allDocs->selectedItems();
    QTableWidgetItem * newItem;
    for(i=0; i<itemList.count();i++){
        DomCfgItem* doc = docs->child(allDocs->currentRow());
        if (used_doc.contains(doc->attr(mda_id)))
            continue;
        QDomElement i;
        QDomDocument xml;
        i = xml.createElement(md_used_doc);
        QDomNode node = item->find(md_columns)->node().appendChild(i);
        QDomText t = xml.createTextNode(doc->attr(mda_id));
        node.appendChild(t);
        used_doc.insert(doc->attr(mda_id),doc);
        newItem = new QTableWidgetItem(docs->child(allDocs->currentRow())->attr(mda_name));
        journalDocs->setRowCount(childCount+1);
        journalDocs->setItem(childCount,0,newItem);
        journalDocs->setRowHeight(childCount,20);
        childCount++;
    }
    journalDocs->setColumnWidth ( 0, journalDocs->width());
    getJournalColumns();
//journalDocs->setSelectionBehavior(QAbstractItemView::SelectRows);

//    ananasListViewItem *cur = (ananasListViewItem *)allDocs->selectedItem();
//    if (!cur)
//        return;
//    if ( md->objClass( cur->obj ) != md_document )
//        return;
//    new ananasListViewItem(journalDocs, journalDocs->lastItem(), md, cur->obj );
}


void JournalEditor::removeDoc()
{
    int n = journalDocs->selectedItems().count();
    if (n){
        for(int i=0; i<n;i++){
            QString name = journalDocs->selectedItems().last()->text();
            journalDocs->removeRow(journalDocs->selectedItems().last()->row());
            deleteDoc(name);
            getJournalColumns();
        }
    }
}

void JournalEditor::deleteDoc(QString name)
{
    //Надо доделать удаление полей удаленого документа
    QHashIterator<QString, DomCfgItem*> i(used_doc);
    while (i.hasNext()) {
         i.next();
         if (i.value()->attr(mda_name) == name) {
             QDomNode journalDoc = item->find(md_columns)->node().firstChild();
             while(!journalDoc.isNull()) {
                 QString id = "";
                 if (journalDoc.nodeName()==md_used_doc) {
                    id = journalDoc.childNodes().item(0).nodeValue();
                    if (i.key() == id) {
                        journalDoc.removeChild(journalDoc.childNodes().item(0));
                        break;
                    }
                 }
                 journalDoc = journalDoc.nextSibling();
             }
             used_doc.remove(i.key());
             break;
         }
     }
}
void JournalEditor::moveUp()
{
//    if (columnsDoc->selectedItems().count()==0)
//        return;
//    DomCfgItem* eitem = item->find(md_columns)->child(columnsDoc->indexOfTopLevelItem(columnsDoc->selectedItems().last()));
//    eitem->moveUp();
//    getJournalColumns();
//    ananasListViewItem *aitem, *after;
//
//    aitem = (ananasListViewItem *) journalDocs->selectedItem();
//    if ( aitem )
//    {
//        after = (ananasListViewItem *)aitem->itemAbove();
//        if ( after )
//            after->moveItem( aitem );
//    }
}


void JournalEditor::moveDown()
{
//    if (columnsDoc->selectedItems().count()==0)
//        return;
//    DomCfgItem* eitem = item->find(md_columns)->child(columnsDoc->indexOfTopLevelItem(columnsDoc->selectedItems().last()));
//    eitem->moveDown();
//    getJournalColumns();
//    ananasListViewItem *aitem, *after;
//    aitem = (ananasListViewItem *)journalDocs->selectedItem();
//    if ( aitem )
//    {
//        after = (ananasListViewItem *)aitem->itemBelow();
//        if ( after )
//            aitem->moveItem( after );
//    }
}

void JournalEditor::addCol()
{
    QModelIndex indexCol = columnsDoc->currentIndex();
    if (!indexCol.isValid())
        return;
    QModelIndex indexDoc = treeDocs->currentIndex();
    if (!indexDoc.isValid())
        return;

    DomCfgItem *selectField = static_cast<DomCfgItem*> ( indexDoc.internalPointer() );

    if (selectField->nodeName()!=md_field)
        return;

    DomCfgItem *column = static_cast<DomCfgItem*> ( indexCol.internalPointer() );

    if (column->nodeName()==md_fieldid) {
        column = column->parent();
        indexCol = indexCol.parent();
    }

    if (column->nodeName()==md_column) {
                QDomElement j;
                QDomDocument xml;
                j = xml.createElement(md_fieldid);
                QDomNode node = column->node().appendChild(j);
                QDomText t = xml.createTextNode(selectField->attr(mda_id));
                node.appendChild(t);
                columnsDoc->reset();
                columnsDoc->expand(indexCol);
    }

}

void JournalEditor::removeCol()
{
    QModelIndex index = columnsDoc->selectionModel()->currentIndex();
    QAbstractItemModel* model = columnsDoc->model();
    model->removeRow(index.row(),index.parent());
    //if (!indexCol.isValid())
    //    return;
    //((tableViewModel*)(columnsDoc->model()))->removeCol(indexCol);


//    DomCfgItem *column = static_cast<DomCfgItem*> ( indexCol.internalPointer() );
//    if (column->nodeName()==md_fieldid) {
//        column->parent()->remove(indexCol.row());
//        columnsDoc->reset();
//        columnsDoc->expand(indexCol.parent());
//    }

}

//void JournalEditor::doubleClickedGroup ( int row, int ) {
//    Core::EditorManager* manager = Core::EditorManager::instance();
//
//    DomCfgItem *fields = item->find(md_group);
//
//    QString cfgName = fields->child(row)->cfgName();
//    Core::IEditor* editor = manager->openEditorWithContents("Field Editor", &cfgName,"");
//    if (editor) {
//        manager->activateEditor(editor);
//        QMetaObject::invokeMethod(editor->widget(), "setData",
//        Q_ARG(DomCfgItem*, fields->child(row)));
//        connect(manager,SIGNAL(editorsClosed(QList<Core::IEditor*>)),editor->widget(),SLOT(updateMD(QList<Core::IEditor*>)));
//    }
//}

//void JournalEditor::doubleClickedForm ( int row, int ) {
////    Core::EditorManager* manager = Core::EditorManager::instance();
////
////    DomCfgItem *fields = item->find(md_group);
////
////    QString cfgName = fields->child(row)->cfgName();
////    Core::IEditor* editor = manager->openEditorWithContents("Field Editor", &cfgName,"");
////    if (editor) {
////        manager->activateEditor(editor);
////        QMetaObject::invokeMethod(editor->widget(), "setData",
////        Q_ARG(DomCfgItem*, fields->child(row)));
////        connect(manager,SIGNAL(editorsClosed(QList<Core::IEditor*>)),editor->widget(),SLOT(updateMD(QList<Core::IEditor*>)));
////    }
//}


void JournalEditor::init()
{
        //delete statusBar();


}


void JournalEditor::updateMD(QList<Core::IEditor*> editor)
{
  bool findWidget = false;
  foreach (Core::IEditor* ed, editor)
  {
      if (ed->widget()==this) {
          findWidget=true;
          break;
      }
  }
  if (findWidget==false)
      return;

        item->setAttr( mda_name, eName->text().trimmed() );
        item->setAttr(md_description, eDescription->toPlainText().trimmed());


}

void JournalEditor::destroy()
{
//    updateMD();
//     ( (MainForm*)this->topLevelWidget() )->wl->remove( this );
//     ( (MainForm*)this->topLevelWidget() )->removeTab(name());
}




void JournalEditor::eSv_activated( int index )
{
    Q_UNUSED(index);
//    if ( index == 1 ) eStrViewF->setEnabled( TRUE );
//    else eStrViewF->setEnabled( FALSE );
}

void JournalEditor::eSvG_activated( int index )
{
    Q_UNUSED(index);
//    if ( index == 1 ) eStrViewFG->setEnabled( TRUE );
//    else eStrViewFG->setEnabled( FALSE );
}

void JournalEditor::setModified(bool /*modified*/)
{
//    int unmodifiedState = modified ? -1 : m_undoStack.size();
//    if (unmodifiedState == m_unmodifiedState)
//        return;
//    m_unmodifiedState = unmodifiedState;
//    emit modificationChanged(m_undoStack.size() != m_unmodifiedState);
}

bool JournalEditor::isModified() const
{
    //return (m_undoStack.size() != m_unmodifiedState);
    return true;
}

void JournalEditor::getAllDocsList()
{

    int childCount;
    DomCfgItem* docs = item->root()->find(md_documents);
    QTableWidgetItem *newItem;
    childCount =0;
    for(int i=0;i<docs->childCount();i++) {
        newItem = new QTableWidgetItem(docs->child(i)->attr(mda_name));
        allDocs->setRowCount(childCount+1);
        allDocs->setItem(childCount,0,newItem);
        allDocs->setRowHeight(childCount,20);
        childCount++;
    }
    allDocs->setColumnWidth ( 0, allDocs->width());
    allDocs->setSelectionBehavior(QAbstractItemView::SelectRows);
}


void JournalEditor::getJournalDocs()
{
    int childCount=0;
    QHashIterator<QString, DomCfgItem*> i(used_doc);
    while (i.hasNext()) {
         i.next();
         QTableWidgetItem *newItem;
         newItem = new QTableWidgetItem(i.value()->attr(mda_name));
         journalDocs->setRowCount(childCount+1);
         journalDocs->setColumnCount(1);
         journalDocs->setItem(childCount,0,newItem);
         journalDocs->setRowHeight(childCount,20);
         childCount++;
     }

    journalDocs->setColumnWidth ( 0, journalDocs->width());
    journalDocs->setSelectionBehavior(QAbstractItemView::SelectRows);
}

void JournalEditor::getJournalColumns()
{
    tableViewModel* tableModel = new tableViewModel(item->find(md_columns),this);
    columnsDoc->setModel(tableModel);
    docViewModel* docModel = new docViewModel(item->root()->find(md_documents),this);
    treeDocs->setModel(docModel);
//    QList<QTreeWidgetItem *> items;
//    QHashIterator<QString, DomCfgItem*> i(used_doc);
//    columnsDoc->clear();
//    treeDocs->clear();
//    DomCfgItem* columns = item->find(md_columns);
//    columnsDoc->setColumnCount(1);
//    for (int i=0;i<=columns->childCount()-1;i++) {
//           QTreeWidgetItem* column = new QTreeWidgetItem((QTreeWidget*)0, QStringList(QString("%1").arg(columns->child(i)->attr(mda_name))));
//           items.append(column);
//           QDomNode field = columns->child(i)->node().firstChild();
//           while(!field.isNull()) {
//               DomCfgItem* f = item->root()->findObjectById(field.childNodes().item(0).nodeValue());
//               QString nameObject = f->parent()->parent()->cfgName()+"."+f->attr(mda_name);
//               new QTreeWidgetItem(column, QStringList(QString("%1").arg(nameObject)));
//               field = field.nextSibling();
//           }
//        }
//    columnsDoc->insertTopLevelItems(0, items);
//    columnsDoc->setColumnWidth ( 0, columnsDoc->width());
//    columnsDoc->setSelectionBehavior(QAbstractItemView::SelectRows);
//
//    treeDocs->setColumnCount(1);
//    items.clear();
//
//    i.toFront();
//    while (i.hasNext()) {
//         i.next();
//         QTreeWidgetItem *docItem = new QTreeWidgetItem((QTreeWidget*)0, QStringList(QString("%1").arg(i.value()->attr(mda_name))));
//         items.append(docItem);
//         DomCfgItem* header = i.value()->find(md_header);
//         for (int i=0;i<=header->childCount()-1;i++) {
//             new QTreeWidgetItem(docItem, QStringList(QString("%1").arg(header->child(i)->attr(mda_name))));
//         }
//     }
//
//    treeDocs->insertTopLevelItems(0, items);
//    treeDocs->setColumnWidth ( 0, treeDocs->width());
}
void JournalEditor::getUsedDoc()
{
    QString id = "";
    QDomNode journalDoc = item->find(md_columns)->node().firstChild();
    while(!journalDoc.isNull()) {
        if (journalDoc.nodeName()==md_used_doc) {
           id = journalDoc.childNodes().item(0).nodeValue();
           DomCfgItem* docs = item->root()->find(md_documents)->findObjectById(id);
           used_doc.insert(id,docs);
        }
        journalDoc = journalDoc.nextSibling();
    }
}
