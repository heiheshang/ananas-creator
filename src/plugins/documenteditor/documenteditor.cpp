
#include "documenteditor.h"
#include "documenteditorconstants.h"
#include <coreplugin/editormanager/editormanager.h>
#include <QTableWidgetItem>
#include <QStandardItemModel>
#include <coreplugin/messagemanager.h>
#include <coreplugin/messageoutputwindow.h>
#include <QDebug>
#include <QFile>
#include <QListWidget>
#include <QListWidgetItem>
#include <QInputDialog>

#include <coreplugin/coreconstants.h>
#include <coreplugin/modemanager.h>
#include <coreplugin/uniqueidmanager.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/editormanager/ieditorfactory.h>
#include <coreplugin/editormanager/iexternaleditor.h>
#include <coreplugin/icore.h>
#include <coreplugin/baseview.h>
#include <coreplugin/imode.h>
#include <coreplugin/settingsdatabase.h>
#include <coreplugin/variablemanager.h>

#include <extensionsystem/pluginmanager.h>
#include <qtscripteditor/qtscripteditor.h>
#include <utils/consoleprocess.h>
#include <utils/qtcassert.h>

#include "qtscripteditor/qtscripteditorfactory.h"
#include "qtscripteditor/qtscripteditorconstants.h"
#include "qtscripteditor/qtscripteditorplugin.h"

using namespace Core;
using namespace Core::Internal;
using namespace Utils;
using namespace QtScriptEditor::Internal;
using namespace QtScriptEditor::Constants;

//enum {
//    UPDATE_DOCUMENT_DEFAULT_INTERVAL = 100
//};

using namespace DocumentEditor;


//===================================================================================================
//Модель для отображения элемента DomCfgItem
//===================================================================================================
domCfgViewModel::domCfgViewModel(DomCfgItem *document, QObject *parent)
    : QAbstractItemModel(parent)
{
    rootItem = document;
}

domCfgViewModel::~domCfgViewModel()
{
    //delete rootItem;
}

int domCfgViewModel::columnCount(const QModelIndex &/*parent*/) const
{
    return 1;
}

QVariant domCfgViewModel::data(const QModelIndex &index, int role) const
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

Qt::ItemFlags domCfgViewModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant domCfgViewModel::headerData(int section, Qt::Orientation orientation,
                              int role) const
{
    Q_UNUSED(section);
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
         return QString();
    }

    return QVariant();
}

QModelIndex domCfgViewModel::createIndexByTags(const QString & md_const,int row, int column, DomCfgItem *parent) const
{
        QDomNodeList listNodes = rootItem->node().toDocument().elementsByTagName(md_const);
        QDomNode node = listNodes.item(0);
        DomCfgItem *nodeMd = new DomCfgItem(node,row,parent);
        return createIndex(row, column, nodeMd);
}

QModelIndex domCfgViewModel::index(int row, int column, const QModelIndex &parent)
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

QModelIndex domCfgViewModel::parent(const QModelIndex &child) const
{
    if (!child.isValid())
        return QModelIndex();

    DomCfgItem *childItem = static_cast<DomCfgItem*>(child.internalPointer());
    DomCfgItem *parentItem = childItem->parent();

    if (!parentItem || parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int domCfgViewModel::rowCount(const QModelIndex &parent) const
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

bool domCfgViewModel::hasChildren ( const QModelIndex & parent ) const
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


DocumentEdit::DocumentEdit(QWidget* parent, const char* name, Qt::WindowFlags fl)
    : QMainWindow(parent, fl)
{
    setupUi(this);
    //(void)statusBar();
    //init();
}

/*
 *  Destroys the object and frees any allocated resources
 */
DocumentEdit::~DocumentEdit()
{
    destroy();
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void DocumentEdit::languageChange()
{
    retranslateUi(this);
}

void DocumentEdit::setData( DomCfgItem *o )
{
     item = o;
     setWindowTitle( tr("Document:") + item->attr( mda_name ) );
     eName->setText( item->attr( mda_name ) );
     eDescription->setText( item->attr( md_description ) );
     QDomNode source =  item->node().namedItem(md_sourcecode);

     QString titlePattern = tr("module document$");

     Core::ICore *core = Core::ICore::instance();
//     Core::BaseView *view = new Core::BaseView;
//     view->setUniqueViewName("Doc");
//     view->setWidget(TabPageModule);
//     view->setContext(QList<int>() << core->uniqueIDManager()
//         ->uniqueIdentifier(QLatin1String("DocWidget")));
//     view->setDefaultPosition(Core::IView::First);
//
     //Core::EditorManager* manager = new EditorManager(core,TabPageModule);
     Core::EditorManager* manager = Core::EditorManager::instance();
     Core::IEditor* editor = manager->openEditorWithContents("Qt Script Editor", &titlePattern,source.toElement().text());
//     ScriptEditor* editor = new ScriptEditor::ScriptEditor(
//             QList<int>() << core->uniqueIDManager()
//                      ->uniqueIdentifier(QLatin1String("DocWidget")),TabPageModule);
     //ExtensionSystem::PluginManager::instance()->getObject<QtScriptEditorPlugin>()->initializeEditor(editor);
//     QtScriptEditorPlugin::instance()->initializeEditor(editor);
     //foreach (QObject *object, ExtensionSystem::PluginManager::instance()->allObjects()) {
     //    qDebug() << object->objectName();
     //}

     if (editor) {
//         //formLayout->addWidget(editor->widget());
//         //tabWidget->addTab(editor->widget(),tr("Document module"));
         //editor->widget()->setParent(TabPageModule,Qt::SubWindow);
         manager->activateEditor(editor);
          editor->widget()->setParent(TabPageModule);
          formLayout->addWidget(editor->widget());
//         //connect(manager,SIGNAL(editorsClosed(QList<Core::IEditor*>)),editor->widget(),SLOT(updateMD(QList<Core::IEditor*>)));
     }
     //DomCfgItem *fields = o->find(md_element);//->find(md_field);
//     listField->setSortingEnabled(false);
//     QTableWidgetItem *newItem;
//     listField->setRowCount(fields->childCount());
//     listField->setColumnCount(2);
//     for(int i=0;i<fields->childCount();i++) {
//        if (fields->child(i)->nodeName()==md_field) {
//
//        newItem = new QTableWidgetItem(fields->child(i)->attr(mda_name));
//        newItem->setFlags(Qt::ItemIsSelectable);
//        listField->setItem(i,0,newItem);
//        QStringList type = fields->child(i)->attr(mda_type).split(QRegExp("\\b"));
//        newItem = new QTableWidgetItem(o->getNameByType(type));
//                newItem->setFlags(Qt::ItemIsSelectable);
//                listField->setItem(i,1,newItem);
//                listField->setRowHeight(i,20);
//        }
//        }
//        connect(elementAttributesList,SIGNAL(cellDoubleClicked ( int , int ) ),this,SLOT(doubleClickedElement(int,int)));
//        connect(groupAttributesList,SIGNAL(cellDoubleClicked ( int , int ) ),this,SLOT(doubleClickedGroup(int,int)));
//        connect(formsList,SIGNAL(cellDoubleClicked ( int , int ) ),this,SLOT(doubleClickedForm(int,int)));
//
        connect(editHeaderAttribute,SIGNAL(pressed() ),this,SLOT(editHeaderAttribute_clicked()));
        connect(createNewHeaderAttribute,SIGNAL(pressed()) ,this,SLOT(createNewHeaderAttribute_clicked()));

        connect(editTable,SIGNAL(pressed() ),this,SLOT(editTableAttribute_clicked()));
        connect(createNewTable,SIGNAL(pressed()) ,this,SLOT(createNewTable_clicked()));
//
//        connect(editForm,SIGNAL(pressed()),this,SLOT(editForm_clicked()));
//
        connect(deleteHeaderAttribute,SIGNAL(pressed()),this,SLOT(deleteHeaderAttribute_clicked()));
//        connect(deleteGroupAttribute,SIGNAL(pressed()),this,SLOT(deleteGroupAttribute_clicked()));
//        connect(deleteForm,SIGNAL(pressed()),this,SLOT(deleteForm_clicked()));
//
//        connect(elementAttributesList,SIGNAL(itemClicked(QTableWidgetItem*)),this,SLOT(activateElement(QTableWidgetItem*)));
//        connect(groupAttributesList,SIGNAL(itemClicked(QTableWidgetItem*)),this,SLOT(activateGroup(QTableWidgetItem*)));
//        connect(formsList,SIGNAL(itemClicked(QTableWidgetItem*)),this,SLOT(activateForm(QTableWidgetItem*)));
//
        connect(moveUpHeaderAttribute,SIGNAL(pressed()),this,SLOT(moveUpHeaderAttribute_clicked()));
        connect(moveDownHeaderAttribute,SIGNAL(pressed()),this,SLOT(moveDownHeaderAttribute_clicked()));
//
//        connect(moveUpGroupAttribute,SIGNAL(pressed()),this,SLOT(moveUpGroupAttribute_clicked()));
//        connect(moveDownGroupAttribute,SIGNAL(pressed()),this,SLOT(moveDownGroupAttribute_clicked()));
//

        getHeaderAttributesList();
        getTablesAttributesList();
        getFormsList();
        getJournalList();
}

//void DocumentEdit::doubleClickedElement ( int row, int ) {
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

//void DocumentEdit::doubleClickedGroup ( int row, int ) {
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

//void DocumentEdit::doubleClickedForm ( int row, int ) {
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


void DocumentEdit::init()
{



}


void DocumentEdit::updateMD(QList<Core::IEditor*> editor)
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

//     	aCfg *md = item->md;
// 	aCfgItem obj = item->obj, sv, g, e;
//
// 	al->updateMD();
// 	re->updateMD();
        item->setAttr( mda_name, eName->text().trimmed() );
// 	md->setAttr( obj, mda_name, eName->text().stripWhiteSpace() );
        item->setAttr(md_description, eDescription->toPlainText().trimmed());
// 	g = md->find( obj, md_group ); // Find group context
// 	e= md->find( obj, md_element ); // Find Element context
// 	//======== Element view save
// 	sv = md->find( e, md_string_view );
// 	if ( sv.isNull() ) sv = md->insert( e, md_string_view );
// 	md->setSText( sv, md_svfunction, eStrViewF->text() );
// 	if ( eSv->currentItem() == 0 ) md->setAttr( sv, mda_stdf, "1" );
// 	else {
// 	    md->setAttr( sv, mda_stdf, "0" );
// 	    if ( eSv->currentItem() == 1 ) md->setSText( sv, md_fieldid, "0" );
// 	    else md->setSText( sv, md_fieldid, QString("%1").arg( *(fields.find( eSv->currentItem() - 2 ) ) ) );
// 	}
// 	//======== Group view save
// 	sv = md->find( g, md_string_view );
// 	if ( sv.isNull() ) sv = md->insert( g, md_string_view );
// 	md->setSText( sv, md_svfunction, eStrViewFG->text() );
// 	if ( eSvG->currentItem() == 0 ) md->setAttr( sv, mda_stdf, "1" );
// 	else {
// 	    md->setAttr( sv, mda_stdf, "0" );
// 	    if ( eSvG->currentItem() == 1 ) md->setSText( sv, md_fieldid, "0" );
// 	    else md->setSText( sv, md_fieldid, QString("%1").arg( *(fieldsg.find( eSvG->currentItem() - 2 ) ) ) );
// 	}

}

void DocumentEdit::destroy()
{
//    updateMD();
//     ( (MainForm*)this->topLevelWidget() )->wl->remove( this );
//     ( (MainForm*)this->topLevelWidget() )->removeTab(name());
}




void DocumentEdit::eSv_activated( int index )
{
//    if ( index == 1 ) eStrViewF->setEnabled( TRUE );
//    else eStrViewF->setEnabled( FALSE );
}

void DocumentEdit::eSvG_activated( int index )
{
//    if ( index == 1 ) eStrViewFG->setEnabled( TRUE );
//    else eStrViewFG->setEnabled( FALSE );
}

void DocumentEdit::setModified(bool modified)
{
//    int unmodifiedState = modified ? -1 : m_undoStack.size();
//    if (unmodifiedState == m_unmodifiedState)
//        return;
//    m_unmodifiedState = unmodifiedState;
//    emit modificationChanged(m_undoStack.size() != m_unmodifiedState);
}

bool DocumentEdit::isModified() const
{
    //return (m_undoStack.size() != m_unmodifiedState);
}

/*!
 * \en
 * Reading of attributes from a configuration in the list
 * \_en
 * \ru
 * Считывание атрибутов из конфигурации в список
 * \_ru
*/
void DocumentEdit::getHeaderAttributesList()
{
    QAbstractItemModel* headers = new domCfgViewModel(item->find(md_header),this);

    headerAttributesList->setModel(headers);


        if (headerAttributesList->model()->rowCount()==0) {
             moveUpHeaderAttribute->setEnabled(false);
             moveDownHeaderAttribute->setEnabled(false);
             editHeaderAttribute->setEnabled(false);
             deleteHeaderAttribute->setEnabled(false);
        }
        else {
            moveUpHeaderAttribute->setEnabled(true);
            moveDownHeaderAttribute->setEnabled(true);
            editHeaderAttribute->setEnabled(true);
            deleteHeaderAttribute->setEnabled(true);
        }

//        aCfg *md = item->md;
//        aCfgItem obj = item->obj, cobj, e;
//        e = md->find( obj, md_element ); // Find Element context
//        for ( i = 0; i < md->count( e, md_field ); i++ ) {
//                cobj = md->find( e, md_field, i );
//                if ( !cobj.isNull() )
//                {
//                        new aListViewItem(elementAttributesList , md, cobj, QString::null );
//                        eFieldList->insertItem( QString("%1").arg(md->attr(cobj, mda_name)));
//                }
//        }
}

void DocumentEdit::getTablesAttributesList()
{
    QAbstractItemModel* tableModel = new domCfgViewModel(item->find(md_tables),this);
    tableAttributeTree->setModel(tableModel);

    if (tableAttributeTree->model()->rowCount()==0) {
         moveUpTableAttribute->setEnabled(false);
         moveDownTableAttribute->setEnabled(false);
         editTable->setEnabled(false);
         deleteTable->setEnabled(false);
    }
    else {
        moveUpTableAttribute->setEnabled(true);
        moveDownTableAttribute->setEnabled(true);
        editTable->setEnabled(true);
        deleteTable->setEnabled(true);
    }
}

void DocumentEdit::getFormsList()
{
    QAbstractItemModel* formModel = new domCfgViewModel(item->find(md_forms),this);
    formsList->setModel(formModel);

    if (formsList->model()->rowCount()==0) {
         editForm->setEnabled(false);
         deleteForm->setEnabled(false);
    }
    else {
        editForm->setEnabled(true);
        deleteForm->setEnabled(true);
    }
}

void DocumentEdit::getJournalList()
{
    QAbstractItemModel* journalModel = new domCfgViewModel(item->root()->find(md_journals),this);
    eDocJournal->setModel(journalModel);
}

///*!
// * \en
// * Init dEditCat form
// * Initialization of properties of objects of the form
// * \_en
// * ru
// * Инициализация формы редактора справочников
// * Инициализация свойств объектов формы
// * \_ru
//*/
//void dEditCat::init()
//{
//        delete statusBar();
//        eStrViewF->setInterpreter( new QSInterpreter() );
//        eStrViewF->setEnabled( FALSE );
//        formsList->header()->hide();
//        elementAttributesList->header()->hide();
//        groupAttributesList->header()->hide();
//        //Label_Cname->hide();
//        //eCname->hide();
//        //Label_Comment->hide();
//        //eComment->hide();
//}
//
//
///*!
// * \en
// * Reads values widgets and writes them in a configuration
// * \_en
// * \ru
//  * \brief ScriptAPI.
// * Читает значения виджетов и записывает их в конфигурацию
// * \_ru
//*/

///*!
// * \en
// * Destroy dEditCat form
// * \_en
// * \ru
// * Уничтожение формы редактора справочников
// * \_ru
//*/
//void dEditCat::destroy()
//{
//    updateMD();
//    ( (MainForm*)this->topLevelWidget() )->wl->remove( this );
//    ( (MainForm*)this->topLevelWidget() )->removeTab(name());
//}
//
//
///*!
// *  \en
// * Opportunity to write the function for element if it is chosen user function
// * \_en
// * \ru
// * Возможность написать свою функцию для елемента если выбрано user function
// * \_ru
// * \param index - \en Index of the chosen element in the list \_en \ru
// *	Индекс выбранного элемента в списке \_ru
//*/
//void dEditCat::eSv_activated( int index )
//{
//    if ( index == 1 ) eStrViewF->setEnabled( TRUE );
//    else eStrViewF->setEnabled( FALSE );
//}
//
///*!
// *  \en
// * Opportunity to write the function for group if it is chosen user function
// * \_en
// * \ru
// * Возможность написать свою функцию для группы если выбрано user function
// * \_ru
// * \param index - \en Index of the chosen element in the list \_en \ru
// *	Индекс выбранного элемента в списке \_ru
//*/
//void dEditCat::eSvG_activated( int index )
//{
//    if ( index == 1 ) eStrViewFG->setEnabled( TRUE );
//    else eStrViewFG->setEnabled( FALSE );
//}
//
//
///*!
// * \en
// * Processes the user pressing the button " New attribute of an element " and creates in metadata new attribute of an element.
// * Requests a name of attribute of the user.
// * \_en
// * \ru
// * Обрабатывает пользовательское нажатие кнопки "Новый атрибут элемента" и создает в метаданных
// * новый атрибут элемента. Запрашивает имя атрибута у пользователя.
// * \_ru
//*/
void DocumentEdit::createNewHeaderAttribute_clicked()
{
    DomCfgItem *element=item->find(md_header);
    DomCfgItem* field = element->newElement();

    Core::EditorManager* manager = Core::EditorManager::instance();

    QString cfgName = field->cfgName();
    Core::IEditor* editor = manager->openEditorWithContents("Field Editor", &cfgName,"");
    if (editor) {
        manager->activateEditor(editor);
        QMetaObject::invokeMethod(editor->widget(), "setData",
        Q_ARG(DomCfgItem*, field));
        connect(manager,SIGNAL(editorsClosed(QList<Core::IEditor*>)),editor->widget(),SLOT(updateMD(QList<Core::IEditor*>)));
    }
    headerAttributesList->reset();
    headerAttributesList->setCurrentIndex(headerAttributesList->model()->index(headerAttributesList->model()->rowCount(),0));
//        aListViewItem *newitem, *fielditem;
//        aCfgItem newobj;
//        aCfg *md = item->md;
//        aCfgItem obj = item->obj, e, field;
//        bool ok, yes=TRUE;
//        QString fieldName;
//        QStringList tN;
//        int i;
//        QString prompt = tr("Enter a new field name:");
//        e = md->find( obj, md_element ); // Find Element context
//        for ( i = 0; i < md->count( e, md_field); i++ )
//        {
//                field = md->find( e, md_field, i);
//                tN.append(md->attr(field, mda_name ));
//        }
//
//        while (yes)
//        {
//                fieldName = QInputDialog::getText(
//                        tr("Add new field"),
//                        prompt,
//                        QLineEdit::Normal,
//                        QString::null, &ok, this );
//                if ( ok && !fieldName.isEmpty() )
//                {
//                        if (tN.contains(fieldName) == 0 )
//                        {
//                                yes=FALSE;
//                        }else
//                        {
//                                prompt = QString(tr("Field <b>%1</b> alredy exist.\nEnter another field name:").arg(fieldName));
//                                yes=TRUE;
//                        }
//                }else{
//                        fieldName = "";
//                        yes=FALSE;
//                }
//        }
//
//        if (fieldName != "")
//        {
//                fielditem = item->findItemInMD(item, "catalogue", md->attr( obj, mda_name ), "element", "");
//
//                newobj = md->insert( md->find(obj, md_element), md_field, fieldName );
//                newitem = new aListViewItem(fielditem, fielditem->getLastChild(), md, newobj );
//                new aListViewItem( elementAttributesList, md, newobj );
//
//                newitem->setSelected( FALSE );
//                newitem->setOpen( FALSE );
//
//                QWorkspace *ws = mainform->ws;
//                aWindowsList *wl = mainform->wl;
//                QString oclass = md->objClass( obj );
//                int objid = md->id( newitem->obj );
//                if ( wl->find( objid ) )
//                {
//                        wl->get( objid )->setFocus();
//                        return;
//                }
//                QWidget *editor;
//                dEditField *e = new dEditField( ws, 0, WDestructiveClose );
//
//                editor = e;
//                e->setData(newitem);
//                QObject::connect(mainform, SIGNAL(tosave()), editor, SLOT( updateMD()));
//                QObject::connect(e, SIGNAL(destroyed()), this, SLOT(GetElementAttributesList()));
//                e->show();
//                mainform->addTab(++mainform->lastTabId,e->name());
//                e->parentWidget()->setGeometry(10,10,e->parentWidget()->frameSize().width(),
//                                e->parentWidget()->frameSize().height());
//                wl->insert( objid, e );
//                updateMD();
//        }

}

////*!
////*\ru
//// Активирует кнопки редактирование,удаления элемента списка атрибутов
////
//void DocumentEditor::activateElement(QTableWidgetItem* item)
//{
//    Q_UNUSED(item);
//    editElementAttribute->setEnabled(true);
//    deleteElementAttribute->setEnabled(true);
//}

//*!
//*\ru
// Активирует кнопки редактирование,удаления элемента списка групп
//
//void DocumentEditor::activateGroup(QTableWidgetItem* item)
//{
//    Q_UNUSED(item);
//    editGroupAttribute->setEnabled(true);
//    deleteGroupAttribute->setEnabled(true);
//}
//
////*!
////*\ru
//// Активирует кнопки редактирование,удаления элемента списка форм
////
//void DocumentEditor::activateForm(QTableWidgetItem* item)
//{
//    Q_UNUSED(item);
//    editForm->setEnabled(true);
//    deleteForm->setEnabled(true);
//}
//
/////*!
//// * \en
//// * Processes the user pressing button " Edit " and causes the form of editing of attribute of an element.
//// * \_en
//// * \ru
//// * Обрабатывает пользовательское нажатие кнопки "Редактировать"  и
////вызывает форму редактирования атрибута элемента.
//// * \_ru
////*/
void DocumentEdit::editHeaderAttribute_clicked()
{
    QModelIndex index = headerAttributesList->currentIndex();
    if (!index.isValid())
        return;
    DomCfgItem *element = static_cast<DomCfgItem*> ( index.internalPointer() );
    Core::EditorManager* manager = Core::EditorManager::instance();

    QString cfgName = element->cfgName();
    Core::IEditor* editor = manager->openEditorWithContents("Field Editor", &cfgName,"");
    if (editor) {
        manager->activateEditor(editor);
        QMetaObject::invokeMethod(editor->widget(), "setData",
        Q_ARG(DomCfgItem*, element));
        connect(manager,SIGNAL(editorsClosed(QList<Core::IEditor*>)),editor->widget(),SLOT(updateMD(QList<Core::IEditor*>)));
    }

    headerAttributesList->reset();
//    aCfg *md = item->md;
//    aCfgItem obj = item->obj;
//    QWorkspace *ws = mainform->ws;
//    aWindowsList *wl = mainform->wl;
//    aListViewItem *ai = (aListViewItem *) elementAttributesList->currentItem(), *elementitem;
//
//        elementitem = item->findItemInMD(item, "catalogue", md->attr( item->obj, mda_name ), "field", ai->text(0));
//
//    int objid = md->id( ai->obj );
//    if ( wl->find( objid ) )
//    {
//        wl->get( objid )->setFocus();
//        return;
//    }
//    QWidget *editor;
//    dEditField *e = new dEditField( ws, 0, WDestructiveClose );
//    editor = e;
//    e->setData(elementitem);
//    QObject::connect( mainform, SIGNAL( tosave() ), editor, SLOT( updateMD() ) );
//    QObject::connect(e, SIGNAL(destroyed()), this, SLOT(GetElementAttributesList()));
//    wl->insert( objid, editor );
//    e->show();
//    mainform->addTab(++mainform->lastTabId,e->name());
//    e->parentWidget()->setGeometry(10,10,e->parentWidget()->frameSize().width(),
//        e->parentWidget()->frameSize().height());
}

///*!
// * \en
// * Processes the user pressing button " Delete " and deletes from metadata attribute of an element in the catalogue.
// * \_en
// * \ru
// * Обрабатывает пользовательское нажатие кнопки "Удалить" и удаляет из метаданных атрибут элемента в справочнике.
// * \_ru
//*/
void DocumentEdit::deleteHeaderAttribute_clicked()
{

        if (headerAttributesList->model()->rowCount(QModelIndex()) == 0)
                return;
        QModelIndex currentIndex = headerAttributesList->currentIndex();
        int newRow = currentIndex.row();
        if (currentIndex.row()==headerAttributesList->model()->rowCount(QModelIndex())-1)
            newRow = currentIndex.row()-1;

        item->find(md_header)->remove(currentIndex.row());
        headerAttributesList->reset();
        headerAttributesList->setCurrentIndex(headerAttributesList->model()->index(newRow,0));
//        aCfg *md = item->md;
//        aListViewItem *i = (aListViewItem *) elementAttributesList->currentItem(), *elementitem;
//
//        if (elementAttributesList->currentItem()->isSelected())
//        {
//                elementitem = item->findItemInMD(item, "catalogue", md->attr( item->obj, mda_name ), "field", i->text(0));
//                if ( elementitem )
//                {
//                        if ( i->id ) {
//                                md->remove( i->obj );
//                                delete elementitem;
//                        }
//                }
                //if (elementAttributesList->rowCount() == 0)
                //{
                //        moveUpElementAttribute->setEnabled(FALSE);
                //        moveDownElementAttribute->setEnabled(FALSE);
                //        editElementAttribute->setEnabled(FALSE);
                //        deleteElementAttribute->setEnabled(FALSE);
                //}
//        }
}


/////*!
//// * \en
//// *  Processes the user pressing the button " Move up"
//// * \_en
//// * \ru
//// * Обрабатывает пользовательское нажатие кнопки "Переместить вверх"
//// * \_ru
////*/
void DocumentEdit::moveUpHeaderAttribute_clicked()
{
        //aListViewItem *ai = (aListViewItem *) elementAttributesList->currentItem(), *eitem;
        //aCfg *md = item->md;
        //QString itemName = item->attr(mda_name);
        //eitem = item->findItemInMD(item, "catalogue", md->attr(item->obj, mda_name ), "field", itemName);
        //eitem->moveUp();
        int currentRow = headerAttributesList->currentIndex().row();
        //DomCfgItem* eitem = item->find(md_header)->child(currentRow);
        QModelIndex index = headerAttributesList->currentIndex();
        DomCfgItem *eitem = static_cast<DomCfgItem*> ( index.internalPointer() );
        eitem->moveUp();
        //headerAttributesList->update(headerAttributesList->currentIndex());
        //headerAttributesList->update(headerAttributesList->model()->index(currentRow-1,0));
        headerAttributesList->reset();
        headerAttributesList->setCurrentIndex(headerAttributesList->model()->index(currentRow-1,0));
        //headerAttributesList->setUpdatesEnabled(true);
        //headerAttributesList->repaint();
        //elementAttributesList->setCurrentItem(elementAttributesList->findItem(itemName, 0));
        //elementAttributesList_selectionChanged();
}
//
///*!
// * \en
// *  Processes the user pressing the button " Move down"
// * \_en
// * \ru
// * Обрабатывает пользовательское нажатие кнопки "Переместить вниз"
// * \_ru
//*/
void DocumentEdit::moveDownHeaderAttribute_clicked()
{
        //aListViewItem *ai = (aListViewItem *) elementAttributesList->currentItem(), *eitem;
        //aCfg *md = item->md;
        //QString itemName = ai->text(0);
        //eitem = item->findItemInMD(item, "catalogue", md->attr(item->obj, mda_name ), "field", itemName);
        int currentRow = headerAttributesList->currentIndex().row();
        QModelIndex index = headerAttributesList->currentIndex();
        if (!index.isValid() )
                return;
        DomCfgItem *eitem = static_cast<DomCfgItem*> ( index.internalPointer() );
        qDebug() << eitem->cfgName();
        eitem->moveDown();
        //headerAttributesList->update(headerAttributesList->currentIndex());
        //headerAttributesList->update(headerAttributesList->model()->index(currentRow+1,0));
        headerAttributesList->reset();
        headerAttributesList->setCurrentIndex(headerAttributesList->model()->index(currentRow+1,0));
        //elementAttributesList->setCurrentItem(elementAttributesList->findItem(itemName, 0));
        //elementAttributesList_selectionChanged();
}


/////*!
//// * \en
//// *  Processes the user pressing the button " New attribute of group " and creates in metadata new attribute of an element.
//// * Requests a name of attribute of the user.
//// * \_en
//// * \ru
//// * Обрабатывает пользовательское нажатие кнопки "Новая табличная часть" и создает в метаданных
//// * новый атрибут группы. Запрашивает имя атрибута у пользователя.
//// * \_ru
////*/
void DocumentEdit::createNewTable_clicked()
{
    DomCfgItem *element=item->find(md_tables);
    QString otype=md_table;
    QString name=tr("New table");
    item->insert(element,otype,name,0);

    DomCfgItem *field=element->child(element->childCount()-1);

    bool ok;
    QString text = QInputDialog::getText(this, field->attr(mda_name),
             tr("table name:"), QLineEdit::Normal,
             field->attr(mda_name), &ok);
    if (ok && !text.isEmpty()) {
        field->setAttr(mda_name,text);
    }
    tableAttributeTree->reset();

//        aListViewItem *newitem, *fielditem;
//        aCfgItem newobj;
//        aCfg *md = item->md;
//        aCfgItem obj = item->obj, g, field;
//        bool ok, yes=TRUE;
//        QString fieldName;
//        QStringList tN;
//        int i;
//        QString prompt = tr("Enter a new field name:");
//        g = md->find( obj, md_group ); // Find Group context
//        for ( i = 0; i < md->count( g, md_field); i++ )
//        {
//                field = md->find( g, md_field, i);
//                tN.append(md->attr(field, mda_name ));
//        }
//
//        while (yes)
//        {
//                fieldName = QInputDialog::getText(
//                                tr("Add new field"),
//                                   prompt,
//       QLineEdit::Normal,
//       QString::null, &ok, this );
//                if ( ok && !fieldName.isEmpty() )
//                {
//                        if (tN.contains(fieldName) == 0 )
//                        {
//                                yes=FALSE;
//                        }else
//                        {
//                                prompt = QString(tr("Field <b>%1</b> alredy exist.\nEnter another field name:").arg(fieldName));
//                                yes=TRUE;
//                        }
//                }else{
//                        fieldName = "";
//                        yes=FALSE;
//                }
//        }
//
//        if (fieldName != "")
//        {
//                fielditem = item->findItemInMD(item, "catalogue", md->attr( obj, mda_name ), "group", "");
//
//                newobj = md->insert( md->find(obj, md_group), md_field, fieldName );
//                newitem = new aListViewItem(fielditem, fielditem->getLastChild(), md, newobj );
//                new aListViewItem( groupAttributesList, md, newobj );
//
//                newitem->setSelected( FALSE );
//                newitem->setOpen( FALSE );
//
//                QWorkspace *ws = mainform->ws;
//                aWindowsList *wl = mainform->wl;
//                QString oclass = md->objClass( obj );
//                int objid = md->id( newitem->obj );
//                if ( wl->find( objid ) )
//                {
//                        wl->get( objid )->setFocus();
//                        return;
//                }
//                QWidget *editor;
//                dEditField *e = new dEditField( ws, 0, WDestructiveClose );
//
//                editor = e;
//                e->setData(newitem);
//                QObject::connect(mainform, SIGNAL(tosave()), editor, SLOT( updateMD()));
//                QObject::connect(e, SIGNAL(destroyed()), this, SLOT(GetGroupAttributesList()));
//                e->show();
//                mainform->addTab(++mainform->lastTabId,e->name());
//                e->parentWidget()->setGeometry(10,10,e->parentWidget()->frameSize().width(),
//                                e->parentWidget()->frameSize().height());
//                wl->insert( objid, e );
//                updateMD();
//        }
//
}

/////*!
//// * \en
//// *  Processes the user pressing button " Edit " and causes the form of editing of attribute of group.
//// * \_en
//// * \ru
//// * Обрабатывает пользовательское нажатие кнопки "Редактировать"  и
////вызывает форму редактирования атрибута группы.
//// * \_ru
////*/
void DocumentEdit::editTableAttribute_clicked()
{
//    int currentRow;
//    currentRow = tableAttributeTree->currentIndex().row();
//    if (currentRow==-1) {
//        currentRow=1;
//    }
    QModelIndex index = tableAttributeTree->currentIndex();
    if (!index.isValid())
        return;
    DomCfgItem *element = static_cast<DomCfgItem*> ( index.internalPointer() );
    if (element->nodeName()==md_field) {
        Core::EditorManager* manager = Core::EditorManager::instance();

        QString cfgName = element->cfgName();
        Core::IEditor* editor = manager->openEditorWithContents("Field Editor", &cfgName,"");
        if (editor) {
            manager->activateEditor(editor);
            QMetaObject::invokeMethod(editor->widget(), "setData",
            Q_ARG(DomCfgItem*, element));
            connect(manager,SIGNAL(editorsClosed(QList<Core::IEditor*>)),editor->widget(),SLOT(updateMD(QList<Core::IEditor*>)));
        }
    }
    if (element->nodeName()==md_table) {
        bool ok;
        QString text = QInputDialog::getText(this, element->attr(mda_name),
                 tr("table name:"), QLineEdit::Normal,
                 element->attr(mda_name), &ok);
        if (ok && !text.isEmpty()) {
            element->setAttr(mda_name,text);
        }
    }
    tableAttributeTree->reset();
//        aCfg *md = item->md;
//        aCfgItem obj = item->obj;
//        QWorkspace *ws = mainform->ws;
//        aWindowsList *wl = mainform->wl;
//        aListViewItem *ai = (aListViewItem *) groupAttributesList->currentItem(), *groupitem;
//
//        groupitem = item->findItemInMD(item, "catalogue", md->attr( item->obj, mda_name ), "field", ai->text(0));
//
//        int objid = md->id( ai->obj );
//        if ( wl->find( objid ) )
//        {
//                wl->get( objid )->setFocus();
//                return;
//        }
//
//        QWidget *editor;
//        dEditField *e = new dEditField( ws, 0, WDestructiveClose );
//        editor = e;
//        e->setData(groupitem);
//        QObject::connect( mainform, SIGNAL( tosave() ), editor, SLOT( updateMD() ) );
//        QObject::connect(e, SIGNAL(destroyed()), this, SLOT(GetGroupAttributesList()));
//        wl->insert( objid, editor );
//        e->show();
//        mainform->addTab(++mainform->lastTabId,e->name());
//        e->parentWidget()->setGeometry(10,10,e->parentWidget()->frameSize().width(),
//        e->parentWidget()->frameSize().height());
}

/////*!
//// * \en
//// *  Processes the user pressing button " Delete " and deletes from metadata attribute of group in the catalogue.
//// * \_en
//// * \ru
//// * Обрабатывает пользовательское нажатие кнопки "Удалить" и удаляет из метаданных атрибут группы в справочнике.
//// * \_ru
////*/
//void DocumentEditor::deleteGroupAttribute_clicked()
//{
//        if (groupAttributesList->rowCount() == 0)
//                return;
//        item->find(md_group)->remove(groupAttributesList->currentRow());
//        GetGroupAttributesList();
////        aCfg *md = item->md;
////        aListViewItem *i = (aListViewItem *) groupAttributesList->currentItem(), *groupitem;
////
////        if (groupAttributesList->currentItem()->isSelected())
////        {
////                groupitem = item->findItemInMD(item, "catalogue", md->attr( item->obj, mda_name ), "field", i->text(0));
////                if ( i )
////                {
////                        if ( i->id ) {
////                                md->remove( i->obj );
////                                delete groupitem;
////                        }
////                }
//                GetGroupAttributesList();
//                if (groupAttributesList->rowCount() == 0)
//                {
//                        moveUpGroupAttribute->setEnabled(FALSE);
//                        moveDownGroupAttribute->setEnabled(FALSE);
//                        editGroupAttribute->setEnabled(FALSE);
//                        deleteGroupAttribute->setEnabled(FALSE);
//                }
////        }
//}
//
/////*!
//// * \en
//// *  Processes the user pressing the button " Move up "
//// * \_en
//// * \ru
//// * Обрабатывает пользовательское нажатие кнопки "Переместить вверх"
//// * \_ru
////*/
//void DocumentEditor::moveUpGroupAttribute_clicked()
//{
//    DomCfgItem* eitem = item->find(md_group)->child(groupAttributesList->currentRow());
//    eitem->moveUp();
//    GetGroupAttributesList();
////        aListViewItem *ai = (aListViewItem *) groupAttributesList->currentItem(), *gitem;
////        aCfg *md = item->md;
////        QString itemName = ai->text(0);
////        gitem = item->findItemInMD(item, "catalogue", md->attr( item->obj, mda_name ), "field", itemName);
////        gitem->moveUp();
////        GetGroupAttributesList();
////        groupAttributesList->setCurrentItem(groupAttributesList->findItem(itemName, 0));
////        groupAttributesList_selectionChanged();
//}
//
/////*!
//// * \en
//// *  Processes the user pressing the button " Move down "
//// * \_en
//// * \ru
//// * Обрабатывает пользовательское нажатие кнопки "Переместить вниз"
//// * \_ru
////*/
//void DocumentEditor::moveDownGroupAttribute_clicked()
//{
//    DomCfgItem* eitem = item->find(md_group)->child(elementAttributesList->currentRow());
//    eitem->moveDown();
//    GetGroupAttributesList();
////        aListViewItem *ai = (aListViewItem *) groupAttributesList->currentItem(), *gitem;
////        aCfg *md = item->md;
////        QString itemName = ai->text(0);
////        gitem = item->findItemInMD(item, "catalogue", md->attr( item->obj, mda_name ), "field", itemName);
////        gitem->moveDown();
////        GetGroupAttributesList();
////        groupAttributesList->setCurrentItem(groupAttributesList->findItem(itemName, 0));
////        groupAttributesList_selectionChanged();
//}
//
/////*!
//// * \en
//// *  Activation of buttons of moving under the list at a choice of an element of attribute
//// * \_en
//// * \ru
//// * Активация кнопок перемещения по списку при выборе атрибута элемента
//// * \_ru
////*/
//void DocumentEditor::elementAttributesList_selectionChanged()
//{
////   aListViewItem *ai = (aListViewItem *) elementAttributesList->currentItem();
////   if ( ai->nextSibling() )
////   {
////      if (!moveDownElementAttribute->isEnabled() ) moveDownElementAttribute->setEnabled(TRUE);
////   } else {
////      if (moveDownElementAttribute->isEnabled() ) moveDownElementAttribute->setEnabled(FALSE);
////   }
////
////   if ( elementAttributesList->firstChild() ==  elementAttributesList->currentItem())
////   {
////      if (moveUpElementAttribute->isEnabled() ) moveUpElementAttribute->setEnabled(FALSE);
////   } else {
////      if (!moveUpElementAttribute->isEnabled() ) moveUpElementAttribute->setEnabled(TRUE);
////   }
////        if (!editElementAttribute->isEnabled() ) editElementAttribute->setEnabled(TRUE);
////        if (!deleteElementAttribute->isEnabled() ) deleteElementAttribute->setEnabled(TRUE);
//    }
//
//
/////*!
//// * \en
//// *  Activation of the button of removal and editing at a choice of the form
//// * \_en
//// * \ru
//// * Активация кнопки удаления и редактирования при выборе формы
//// * \_ru
////*/
////void dEditCat::formsList_selectionChanged()
////{
////        if (!editForm->isEnabled() ) editForm->setEnabled(TRUE);
////        if (!deleteForm->isEnabled() ) deleteForm->setEnabled(TRUE);
////}
////
////
/////*!
//// * \en
//// * Edit of attribute of an element
//// * \_en
//// * \ru
//// * Редактирование атрибута элемента
//// * \_ru
////*/
////void dEditCat::elementAttributesList_doubleClicked( QListViewItem * )
////{
////   editElementAttribute_clicked();
////}
////
/////*!
//// * \en
//// *  Processes the user pressing the button " New form" and creates in metadata new form. Requests a name of form of the user.
//// * \_en
//// * \ru
//// * Обрабатывает пользовательское нажатие кнопки "Новая форма" и создает в метаданных новую форму.
//// * Запрашивает имя формы у пользователя.
//// * \_ru
////*/
////void dEditCat::createNewForm_clicked()
////{
////        long int i;
////        aListViewItem *newitem, *formitem;
////        aCfgItem newobj;
////        aCfg *md = item->md;
////        aCfgItem obj = item->obj, field;
////        aCfgItem cobj =md->find(obj, md_forms);// Find forms context
////        bool ok, yes=TRUE;
////        QString formName;
////        QStringList tN;
////        QString prompt = tr("Enter a new forms name:");
////
////        for ( i = 0; i < md->count( obj, md_form); i++ )
////        {
////                field = md->find( obj, md_form, i);
////                tN.append(md->attr(field, mda_name ));
////        }
////
////        while (yes)
////        {
////                formName = QInputDialog::getText(
////                        tr("Add new form"),
////                        prompt,
////                        QLineEdit::Normal,
////                        QString::null, &ok, this );
////                if ( ok && !formName.isEmpty() )
////                {
////                        if (tN.contains(formName) == 0 )
////                        {
////                                yes=FALSE;
////                        }
////                        else
////                        {
////                                prompt = QString(tr("Form <b>%1</b> alredy exist.\nEnter another form name:").arg(formName));
////                                yes=TRUE;
////                        }
////                }
////                else
////                {
////                        formName = "";
////                        yes=FALSE;
////                }
////        }
////
////        if (formName != "")
////        {
////                QString module, tpl_name = "sourcecode.tpl", tpldir;
////#ifdef Q_OS_WIN32
////                tpldir = qApp->applicationDirPath()+"/templates/";
////#else
////                BrInitError error;
////                if (br_init_lib(&error) == 0 && error != BR_INIT_ERROR_DISABLED)
////                {
////                        aLog::print(aLog::MT_INFO, tr("Warning: BinReloc failed to initialize (error code %1)\n").arg(error));
////                        aLog::print(aLog::MT_INFO, tr("Will fallback to hardcoded default path.\n"));
////                }
////                aLog::print(aLog::MT_DEBUG, tr("dEditDoc::EditForm BinReloc path to data dir is %1.\n").arg( br_find_data_dir("/usr/share") ));
////                tpldir = QString( br_find_data_dir("/usr/share") ) + "/ananas/templates/";
////#endif
////                formitem = item->findItemInMD(item, "catalogue", md->attr( obj, mda_name ), "forms", "");
////                newobj = md->insert( cobj, md_form, formName );
////                QFile fi( tpldir+tpl_name );
////                if ( fi.open( IO_ReadOnly ))
////                {
////                        aLog::print(aLog::MT_DEBUG,tr("dEditDoc open file %1 for read from templates directory %2").arg(tpl_name).arg(tpldir));
////                        module = QString::QString( fi.readAll() );
////                        fi.close();
////                        md->setSText( newobj, md_sourcecode, module );
////                }
////                else
////                {
////                        aLog::print(aLog::MT_ERROR,tr("dEditDoc file %1 not exists in templates directory %2").arg(tpl_name).arg(tpldir));
////                }
////                newitem = new aListViewItem(formitem, formitem->getLastChild(), md, newobj );
////                newitem->setSelected( FALSE );
////                newitem->setOpen( FALSE );
////                QWorkspace *ws = mainform->ws;
////                aWindowsList *wl = mainform->wl;
////                QString oclass = md->objClass( obj );
////                int objid = md->id( newitem->obj );
////                if ( wl->find( objid ) )
////                {
////                        wl->get( objid )->setFocus();
////                        return;
////                }
////                QWidget *editor;
////                dEditDialog *e = new dEditDialog( ws, 0, WDestructiveClose );
////                wl->insert( objid, e );
////                editor = e;
////                e->setData(newitem);
////                QObject::connect(mainform, SIGNAL(tosave()), editor, SLOT( updateMD()));
////                QObject::connect(e, SIGNAL(destroyed()), this, SLOT(GetFormsList()));
////                e->show();
////                mainform->addTab(++mainform->lastTabId,e->name());
////                e->parentWidget()->setGeometry(10,10,e->parentWidget()->frameSize().width(),
////                                e->parentWidget()->frameSize().height());
////                updateMD();
////                //newitem->edit();
////        }
////}
////

///*!
// * \en
// * \ru
// * Обрабатывает запись формы
// * \_ru
//*/
//void DocumentEdit::updateForm(QList<Core::IEditor*> editor)
//{
//     for (int i = 0; i < editor.size(); ++i) {
//        QHashIterator<QString, QString> of(openForm);
//        while (of.hasNext()) {
//            of.next();
//            if (of.value() == editor.at(i)->file()->fileName()) {
//                DomCfgItem* form = item->find(md_forms);
//                for (int j=0;j<=form->childCount()-1;j++) {
//                    QString id_form = form->child(j)->attr(mda_id);
//                    if (id_form == of.key()) {
//                        //Переписываем форму
//                        QFile file_ui(editor.at(i)->file()->fileName());
//                        QTextStream text_ui(&file_ui);
//                        form->setText(md_formdesign,text_ui.readAll());
//                        file_ui.close();
//                        openForm.remove(id_form);
//                    }
//                }
//            }
//        }
//    }
//}

///*!
// * \en
// *  Processes the user pressing button " Edit " and causes dialogue of editing of the document
// * \_en
// * \ru
// * Обрабатывает пользовательское нажатие кнопки "Редактировать" и вызывает диалог редактирования документа
// * \_ru
//*/
//void DocumentEdit::editForm_clicked()
//{
//    if (formsList->rowCount() == 0 )
//            return;
//    DomCfgItem* form = item->find(md_forms)->child(formsList->currentRow());
//    QString id_form = form->attr(mda_id);
//    QString tpl_name = "inputformcat";
//    QString ui, s, tpldir;
//
//
//
//    #ifdef Q_OS_WIN32
//            tpldir = qApp->applicationDirPath()+"/templates/";
//    #else
//            tpldir = "/usr/share/ananas/templates/";
//    #endif
//            //tpldir = ":/designer/templates/";
//    QString name_file_ui = tpldir+tpl_name+id_form+".ui";
//    QFile file_ui( name_file_ui );
//    if (!file_ui.exists()) {
//        if (file_ui.open(QIODevice::WriteOnly | QIODevice::Text)) {
//            QTextStream text_ui(&file_ui);
//            text_ui.setCodec("UTF-8");
//            text_ui << form->node().namedItem(md_formdesign).toElement().text();
//            file_ui.close();
//        }
//    }
//
//    Core::EditorManager* manager = Core::EditorManager::instance();
//
//    QString cfgName = form->cfgName();
//    Core::IEditor* editor = manager->openEditor(name_file_ui);
//    if (editor) {
//        manager->activateEditor(editor);
//        openForm.insert(id_form,name_file_ui);
//        connect(manager,SIGNAL(editorsClosed(QList<Core::IEditor*>)),this,SLOT(updateForm(QList<Core::IEditor*>)));
//    }
//
//
//}
//
///*!
// * \en
// *  Processes the user pressing button " Deletee " and deletes from metadata the form of the catalogue.
// * \_en
// * \ru
// * Обрабатывает пользовательское нажатие кнопки "Удалить" и удаляет из метаданных форму справочника.
// * \_ru
//*/
//void DocumentEdit::deleteForm_clicked()
//{
//        if (formsList->rowCount() == 0 )
//                return;
//        item->find(md_forms)->remove(formsList->currentRow());
//        GetFormsList();
////        aCfg *md = item->md;
////        aListViewItem *i = (aListViewItem *) formsList->currentItem(), *formitem;
////
////        if (formsList->currentItem()->isSelected())
////        {
////                formitem = item->findItemInMD(item, "catalogue", md->attr( item->obj, mda_name ), "form", i->text(0));
////                if ( i )
////                {
////                        if ( i->id ) {
////                                md->remove( i->obj );
////                                delete formitem;
////                        }
////                }
//
//                if (formsList->rowCount() == 0 )
//                {
//                        editForm->setEnabled( FALSE );
//                        deleteForm->setEnabled( FALSE );
//                }
////        }
//}

//
///*!
// * \en
// *  Editing of the form
// * \_en
// * \ru
// * Редактирование формы
// * \_ru
//*/
//void dEditCat::formsList_doubleClicked( QListViewItem * )
//{
//   editForm_clicked();
//}
//
//
//
//void dEditCat::eParentCat_activated( const QString & )
//{
//
//}
//
//
//void dEditCat::eOneEandGForm_stateChanged( int )
//{
//
//}
//
//
//void dEditCat::eFieldList_activated( const QString & )
//{
//
//}
//
//
///*!
// * \en
// *  Activation of buttons of moving under the list at a choice of attribute of an group
// * \_en
// * \ru
// * Активация кнопок перемещения по списку при выборе атрибута группы
// * \_ru
//*/
//void dEditCat::groupAttributesList_selectionChanged()
//{
//        aListViewItem *ai = (aListViewItem *) groupAttributesList->currentItem();
//        if ( ai->nextSibling() )
//        {
//                if (!moveDownGroupAttribute->isEnabled() ) moveDownGroupAttribute->setEnabled(TRUE);
//        } else {
//                if (moveDownGroupAttribute->isEnabled() ) moveDownGroupAttribute->setEnabled(FALSE);
//        }
//
//        if ( groupAttributesList->firstChild() == groupAttributesList->currentItem() )
//        {
//                if (moveUpGroupAttribute->isEnabled() ) moveUpGroupAttribute->setEnabled(FALSE);
//        } else {
//                if (!moveUpGroupAttribute->isEnabled() ) moveUpGroupAttribute->setEnabled(TRUE);
//        }
//
//        if (groupAttributesList->childCount() != 0 )
//        {
//                if (!editGroupAttribute->isEnabled() ) editGroupAttribute->setEnabled(TRUE);
//                if (!deleteGroupAttribute->isEnabled() ) deleteGroupAttribute->setEnabled(TRUE);
//        }
//
//}
//
//
//
//
///*!
// * \en
// *  Change of type of general show
// * \_en
// * \ru
// * Смена типа основного представления
// * \_ru
// * \param id - \en Index of the chosen element in group \_en \ru
// *	Индекс выбранного элемента в группе \_ru
//*/
//void dEditCat::eGenCodeGrp_pressed( int index)
//{
//        if (index == 0)
//        {
//                eCodeSerGrp->setEnabled(FALSE);
//                eCodeTypeGrp->setEnabled(FALSE);
//                eFieldList->setEnabled(TRUE);
//        }else {
//                eCodeSerGrp->setEnabled(TRUE);
//                eCodeTypeGrp->setEnabled(TRUE);
//                eFieldList->setEnabled(FALSE);
//        }
//}
//
//
///**
// * \ru
// *	\brief Сдвигает окно редактирования свойств в левый верхний угол родительского окна.
// *
// * 	Размеры перемещаемого окна остаются без изменений.
// * \_ru
// */
//void
//dEditCat::moveToTopLeftCorner()
//{
//        this->parentWidget()->setGeometry( 0, 0,
//                this->parentWidget()->frameSize().width(),
//                this->parentWidget()->frameSize().height());
//
//}
//

