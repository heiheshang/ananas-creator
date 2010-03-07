#include "journaleditor.h"
#include <coreplugin/editormanager/editormanager.h>
#include <QTableWidgetItem>
#include <QStandardItemModel>
#include <coreplugin/messagemanager.h>
#include <coreplugin/messageoutputwindow.h>
#include <QDebug>

using namespace JOURNALEditor;

JournalEditor::JournalEditor(QWidget* parent, const char* name, Qt::WindowFlags fl)
    : QMainWindow(parent, fl)
{
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



    connect(cbType,SIGNAL(clicked()),this,SLOT(typeChange()));
    connect(bAddDoc,SIGNAL(clicked()),this,SLOT(addDoc()));
    connect(bRemoveDoc, SIGNAL(clicked()),this,SLOT(removeDoc()));
    connect(bMoveUp,SIGNAL(clicked()),this,SLOT(moveUp()));
    connect(bMoveDown,SIGNAL(clicked()),this,SLOT(moveDown()));

    getUsedDoc();
    getAllDocsList();
    getJournalDocs();
    getJournalColumns();
}

//void JournalEditor::doubleClickedElement ( int row, int ) {
//    Core::EditorManager* manager = Core::EditorManager::instance();
//
//    DomCfgItem *fields = item->find(md_element);
//    QString cfgName = fields->child(row)->cfgName();
//    Core::IEditor* editor = manager->openEditorWithContents("Field Editor", &cfgName,"");
//    if (editor) {
//        manager->activateEditor(editor);
//        QMetaObject::invokeMethod(editor->widget(), "setData",
//        Q_ARG(DomCfgItem*, fields->child(row)));
//        connect(manager,SIGNAL(editorsClosed(QList<Core::IEditor*>)),editor->widget(),SLOT(updateMD(QList<Core::IEditor*>)));
//    }
//}


void JournalEditor::typeChange()
{
    if ( cbType->currentIndex() != 1 )
        tabWidget18->removeTab(2);

    else
        tabWidget18->insertTab(2,tabWidget18,QObject::tr("Documents"));
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
    if (columnsDoc->selectedItems().count()==0)
        return;
    DomCfgItem* eitem = item->find(md_columns)->child(columnsDoc->indexOfTopLevelItem(columnsDoc->selectedItems().last()));
    eitem->moveUp();
    getJournalColumns();
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
    if (columnsDoc->selectedItems().count()==0)
        return;
    DomCfgItem* eitem = item->find(md_columns)->child(columnsDoc->indexOfTopLevelItem(columnsDoc->selectedItems().last()));
    eitem->moveDown();
    getJournalColumns();
//    ananasListViewItem *aitem, *after;
//    aitem = (ananasListViewItem *)journalDocs->selectedItem();
//    if ( aitem )
//    {
//        after = (ananasListViewItem *)aitem->itemBelow();
//        if ( after )
//            aitem->moveItem( after );
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
//    if ( index == 1 ) eStrViewF->setEnabled( TRUE );
//    else eStrViewF->setEnabled( FALSE );
}

void JournalEditor::eSvG_activated( int index )
{
//    if ( index == 1 ) eStrViewFG->setEnabled( TRUE );
//    else eStrViewFG->setEnabled( FALSE );
}

void JournalEditor::setModified(bool modified)
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
}

void JournalEditor::getAllDocsList()
{
    int i,childCount;
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
    QList<QTreeWidgetItem *> items;
    QHashIterator<QString, DomCfgItem*> i(used_doc);
    columnsDoc->clear();
    treeDocs->clear();
    DomCfgItem* columns = item->find(md_columns);
    columnsDoc->setColumnCount(1);
    for (int i=0;i<=columns->childCount()-1;i++) {
           QTreeWidgetItem* column = new QTreeWidgetItem((QTreeWidget*)0, QStringList(QString("%1").arg(columns->child(i)->attr(mda_name))));
           items.append(column);
           QDomNode field = columns->child(i)->node().firstChild();
           while(!field.isNull()) {
               DomCfgItem* f = item->root()->findObjectById(field.childNodes().item(0).nodeValue());
               QString nameObject = f->parent()->parent()->cfgName()+"."+f->attr(mda_name);
               QTreeWidgetItem *fi = new QTreeWidgetItem(column, QStringList(QString("%1").arg(nameObject)));
               field = field.nextSibling();
           }
        }
    columnsDoc->insertTopLevelItems(0, items);
    columnsDoc->setColumnWidth ( 0, columnsDoc->width());
    columnsDoc->setSelectionBehavior(QAbstractItemView::SelectRows);

    treeDocs->setColumnCount(1);
    items.clear();

    i.toFront();
    while (i.hasNext()) {
         i.next();
         QTreeWidgetItem *docItem = new QTreeWidgetItem((QTreeWidget*)0, QStringList(QString("%1").arg(i.value()->attr(mda_name))));
         items.append(docItem);
         DomCfgItem* header = i.value()->find(md_header);
         for (int i=0;i<=header->childCount()-1;i++) {
             QTreeWidgetItem *field = new QTreeWidgetItem(docItem, QStringList(QString("%1").arg(header->child(i)->attr(mda_name))));
         }
     }

    treeDocs->insertTopLevelItems(0, items);
    treeDocs->setColumnWidth ( 0, treeDocs->width());
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
