/****************************************************************************
** $Id: acfg.cpp,v 1.2 2008/11/08 20:16:35 leader Exp $
**
** Code file of the Ananas configuration objects of Ananas
** Designer and Engine applications
**
** Created : 20031201
**
** Copyright (C) 2003-2004 Leader InfoTech. All rights reserved.
** Copyright (C) Grigory Panov <gr1313 at mail.ru>, Yoshkar-Ola.
**
** This file is part of the Library of the Ananas
** automation accounting system.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.leaderit.ru/page=ananas or email sales@leaderit.ru
** See http://www.leaderit.ru/gpl/ for GPL licensing information.
**
** Contact org@leaderit.ru if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
//#include <string.h>
//#include <ctype.h>
//#include <stdarg.h>
#include <QtGui>
#include <qdatetime.h>
#include <qobject.h>
#include <QtXml/qdom.h>
#include <qfile.h>
//#include <qstringlist.h>
#include <QTextStream>
#include <coreplugin/messagemanager.h>
#include <coreplugin/messageoutputwindow.h>

#include "acfg.h"
//#include "alog.h"

#ifdef _MSC_VER
#define vsnprintf _vsnprintf
#endif

#define context_startid 100

/*!
* Global configuration variable.
*/

//aCfg * ANANAS_EXPORT globalCfg = 0;


/*
*/
void (*cfg_messageproc)(int , const char *)=NULL;

/*!
*\en
* Outputs error messages.
*\_en \ru
* Вывод сообщений об ошибках.
*\_ru
*/
//void
//cfg_message(int msgtype, const char *msgfmt,...){
// char msg[2048];
// char *ts=(char *)"";
// va_list args;
//
// if (msgfmt) {
// va_start(args, msgfmt);
// vsnprintf(msg, sizeof(msg)-1, msgfmt, args);
// va_end(args);
// if (cfg_messageproc) cfg_messageproc(msgtype, msg);
// else {
// if (msgtype==1) ts=(char *)"- ";
// if (msgtype==2) ts=(char *)"! ";
// if (msgtype==2) ts=(char *)"!!! ";
// printf("%s%s", ts, (const char *) QString::fromUtf8(msg));
// }
// }
//}

/*!
*\en
*\_en \ru
* Выводит отладочные сообщения, если определен макрос ANANAS_DEBUG
*\_ru
*/
//void
//debug_message(const char *msgfmt,...){
//#ifdef ANANAS_DEBUG
// char msg[2048];
// va_list args;
//
// if (msgfmt) {
// va_start(args, msgfmt);
// vsnprintf(msg, sizeof(msg)-1, msgfmt, args);
// va_end(args);
// printf("debug");
// printf("> %s", (const char *) QString::fromUtf8(msg));
// }
//#endif
//}


DomCfgItem::DomCfgItem(QDomNode &node, int row, DomCfgItem *parent):QObject(parent)
{
    domNode = node;

    rowNumber = row;
    parentItem = parent;
    if (parent==0) {
rootNode=this;
if (domNode.hasAttributes()) {
hashId.insert(domNode.attributes().namedItem(mda_id).nodeValue(),this);
}
    }
    else
{rootNode=parent->rootNode;
if (domNode.hasAttributes()) {
rootNode->hashId.insert(domNode.attributes().namedItem(mda_id).nodeValue(),this);
}
}
}

DomCfgItem::~DomCfgItem()
{
    QHash<int,DomCfgItem*>::iterator it;
    for (it = childItems.begin(); it != childItems.end(); ++it)
        delete it.value();
    QHash<QString,DomCfgItem*>::iterator id;
    for (id = hashId.begin(); id != hashId.end(); ++id)
        delete id.value();
}

QDomNode DomCfgItem::node() const
{
    return domNode;
}

DomCfgItem *DomCfgItem::parent()
{
    return parentItem;
}


bool DomCfgItem::hasChildren() const
{
    if (domNode.nodeName()==md_field)
        return false;
    if (domNode.nodeName()==md_form)
        return false;
    if (domNode.nodeName()==md_column)
        return false;
    if (domNode.nodeName()==md_group) {
        if (domNode.childNodes().count()>1) {

            return true;
}
return false;
    }
    if (domNode.hasChildNodes())
        return true;
    return false;
}
int DomCfgItem::childCount()
{
   if (domNode.childNodes().count()>0) {
    if (domNode.nodeName()==md_root || parent()==0)
        return md_row_count;

   int nodeCount=0;
   QString nodeName;

   for(int j=0;j<domNode.childNodes().count();j++) {
    nodeName = domNode.childNodes().item(j).nodeName();
   if (nodeName!=md_description && nodeName!=md_string_view && nodeName!=md_svfunction)
    nodeCount++;
   }
   return nodeCount;
      }
   return 0;
}
DomCfgItem *DomCfgItem::child(int i)
{
 DomCfgItem *childItem;
 if (childItems.contains(i)) {
        return childItems[i];
    }
    if (i >= 0 && i < domNode.childNodes().count()) {
DomCfgItem *firstChildItem = 0;

        QDomNode childNode = domNode.childNodes().item(i);
if (childNode.nodeName()==md_root)
{
QDomElement images = childNode.firstChildElement(md_image_collection);

if (!images.isNull()) {
childItem = new DomCfgItem(images, 7, this);
         childItems[7] = childItem;
}

QDomElement metadata = childNode.firstChildElement(md_metadata);
for(int j=0;j<metadata.childNodes().count();j++) {
QDomNode childNodeL = metadata.childNodes().item(j);
QString nodeName = childNodeL.nodeName();
if (nodeName==md_catalogues) {
childItem = new DomCfgItem(childNodeL, 1, this);
         childItems[1] = childItem;
firstChildItem=childItem;
};
if (nodeName==md_documents) {
childItem = new DomCfgItem(childNodeL, 2, this);
         childItems[2] = childItem;
}
if (nodeName==md_journals) {
childItem = new DomCfgItem(childNodeL, 3, this);
         childItems[3] = childItem;
}
if (nodeName==md_reports) {
childItem = new DomCfgItem(childNodeL, 4, this);
         childItems[4] = childItem;
}
if (nodeName==md_registers) {
for(int j1=0;j1<childNodeL.childNodes().count();j1++)
{
QDomNode childReg = childNodeL.childNodes().item(j1);
childItem = new DomCfgItem(childReg, 5+j1, this);
         childItems[5+j1] = childItem;
}
}
}
return firstChildItem;
}
        if (domNode.nodeName()==md_element || domNode.nodeName()==md_group)
{
        int nodeI = 0;
        QDomNodeList listNode = domNode.childNodes();
        for (int j=0;j<listNode.count();j++)

        {
            if (listNode.item(j).nodeName()==md_field)
            {
                QDomNode item = listNode.item(j);
                childItem = new DomCfgItem(item, nodeI, this);
                childItems[nodeI]=childItem;
                if (!firstChildItem)
                                firstChildItem=childItem;
                nodeI++;
            }
        }

        return childItems[i];
    }

if (domNode.nodeName()==md_columns )
{
int nodeI = 0;
QDomNode cur = domNode.firstChild();
while(!cur.isNull()) {
if (cur.nodeName()!=md_used_doc) {
childItem = new DomCfgItem(cur, nodeI, this);
childItems[nodeI]=childItem;
nodeI++;
if (!firstChildItem)
firstChildItem=childItem;
}
cur = cur.nextSibling();
}
return firstChildItem;
}
if (domNode.nodeName()==md_aregister || domNode.nodeName()==md_iregister)
{
int nodeI = 0;
QDomNode cur = domNode.firstChild();
while(!cur.isNull()) {
if (cur.nodeName()!=md_description) {
childItem = new DomCfgItem(cur, nodeI, this);
childItems[nodeI]=childItem;
nodeI++;
if (!firstChildItem)
firstChildItem=childItem;
}
cur = cur.nextSibling();
}
return firstChildItem;
}
if (domNode.nodeName()==md_form)
{
return 0;
}
        childItem = new DomCfgItem(childNode, i, this);
        childItems[i] = childItem;
        return childItem;
    }
    return 0;
}

DomCfgItem* DomCfgItem::child(QString f)
{
 for (int i=0;i<childCount();i++)
{

if (child(i)->node().nodeName()==f)
return child(i);
}
return 0;
}

DomCfgItem* DomCfgItem::child(QString f,int j)
{
 int number = 0;
 for (int i=0;i<childCount();i++)
{
if (child(i)->node().nodeName()==f) {
if (number==j)
return child(i);
else
number++;
}
}
return 0;
}

bool DomCfgItem::remove(int i)
{
 node().removeChild(child(i)->node());
 childItems.remove(i);
 return true;
}

QString DomCfgItem::cfgName() const
{
 if (domNode.nodeName()==md_catalogues)
return QObject::tr("Catalogues");
 if (domNode.nodeName()==md_documents)
return QObject::tr("Documents");
 if (domNode.nodeName()==md_journals)
return QObject::tr("Journals");
 if (domNode.nodeName()==md_reports)
return QObject::tr("Reports");
 if (domNode.nodeName()==md_iregisters)
  return QObject::tr("Information Register");
 if (domNode.nodeName()==md_aregisters)
  return QObject::tr("Accumuliation Register");
 if (domNode.nodeName()==md_image_collection)
return QObject::tr("Images");
 if (domNode.nodeName()==md_catalogue ||domNode.nodeName()==md_form || domNode.nodeName()==md_document || domNode.nodeName()==md_journal || domNode.nodeName()==md_report || domNode.nodeName()==md_aregister || domNode.nodeName()==md_iregister || domNode.nodeName()==md_column) {
QString arrt = domNode.attributes().namedItem(mda_name).nodeValue();
   return arrt;
}
 if (domNode.nodeName()==md_group)
return QObject::tr("Group");
 if (domNode.nodeName()==md_forms)
return QObject::tr("Forms");
 if (domNode.nodeName()==md_element)
return QObject::tr("Element");
 if (domNode.nodeName()==md_field) {
QString arrt = domNode.attributes().namedItem(mda_name).nodeValue();
   return arrt;
}
 if (domNode.nodeName()==md_columns)
return QObject::tr("Columns");
 if (domNode.nodeName()==md_column)
return QObject::tr("Column");
 if (domNode.nodeName()==md_tables)
return QObject::tr("Tables");
 if (domNode.nodeName()==md_resources)
return QObject::tr("Resources");
 if (domNode.nodeName()==md_dimensions)
return QObject::tr("Dimensions");
 if (domNode.nodeName()==md_information)
return QObject::tr("Information");
 if (domNode.nodeName()==md_image)
return QObject::tr("Image");
 if (domNode.nodeName()==md_svfunction)
return QObject::tr("String view");
 return domNode.nodeName();
}

int DomCfgItem::row()
{
    return rowNumber;
}
QIcon DomCfgItem::iconNode()
{
QString nodeName = domNode.nodeName();
if ( nodeName == "xml" )
return QIcon ( ":/images/project.png" );
if ( nodeName==md_catalogues )
return QIcon ( ":/images/cat_g.png" );
if (nodeName==md_catalogue)
return QIcon ( ":/images/cat.png" );
if (nodeName==md_element)
return QIcon ( ":/images/element.png" );
if (nodeName==md_field)
return QIcon ( ":/images/field.png" );
if (nodeName==md_documents)
return QIcon ( ":/images/doc.png" );
if (nodeName==md_journals)
return QIcon ( ":/images/journ_g.png" );
if (nodeName==md_journal)
return QIcon ( ":/images/journ.png" );
if (nodeName==md_reports)
return QIcon ( ":/images/report_g.png" );
if (nodeName==md_report)
return QIcon ( ":/images/report.png" );
if (nodeName==md_documents)
return QIcon ( ":/images/doc_g.png" );
if (nodeName==md_document)
return QIcon ( ":/images/doc.png" );
if (nodeName==md_iregisters)
return QIcon ( ":/images/regs.png" );
if (nodeName==md_aregisters)
return QIcon ( ":/images/regs.png" );
if (nodeName==md_form)
return QIcon ( ":/images/form.png" );
if (nodeName==md_forms)
return QIcon ( ":/images/form_g.png" );
if ( nodeName == md_header )
return QIcon ( ":/images/doc_h.png" );
if ( nodeName == md_columns )
return QIcon ( ":/images/columns.png" );
if ( nodeName == md_resources )
return QIcon ( ":/images/resourses.png" );
if ( nodeName == md_dimensions )
return QIcon ( ":/images/dimensions.png" );
if ( nodeName == md_webforms )
return QIcon ( ":/images/webform_g.png" );
if ( nodeName == md_webform )
return QIcon ( ":/images/webform.png" );
if ( nodeName == md_table )
return QIcon ( ":/images/table.png" );
if ( nodeName == md_tables )
return QIcon ( ":/images/table_g.png" );
if ( nodeName == md_information )
return QIcon ( ":/images/information.png" );
if (nodeName == md_group)
return QIcon ( ":/images/group.png" );
if (nodeName == md_image_collection)
return QIcon ( ":/images/image_g.png" );
if (nodeName == md_svfunction)
return QIcon ( ":/images/Txt-Document.png" );
if (nodeName == md_image) {
QPixmap pix;
pix.loadFromData(binary());
return QIcon(pix);}

  return QIcon();
}

QMenu *DomCfgItem::menu() const
{
if (domNode.nodeName()=="xml") {
QMenu *contextMenu = new QMenu(tr("Context menu"));
contextMenu->addAction("Open global module");
contextMenu->addAction("Property");
return contextMenu;
}
        if (domNode.nodeName()==md_catalogue || domNode.nodeName()==md_journal) {
QMenu *contextMenu = new QMenu(tr("Context menu"));
         contextMenu->addAction("New");
         contextMenu->addAction("Edit");
         contextMenu->addAction("Delete");
return contextMenu;
}
 return 0;
}

QString DomCfgItem::attr(QString attrName) const
{
 if (domNode.hasAttributes()) {
QDomNamedNodeMap attrNode = domNode.attributes();
if (attrNode.contains(attrName))
{
return attrNode.namedItem(attrName).nodeValue();
}
}
 return "";
}

QStringList DomCfgItem::types(QStringList filter)
{
QStringList l;
        //aCfgItem group, context;
if (filter.filter("base").count()!=0) {
                l << QString("\t")+QObject::tr("Unknown")
                  << QString("N %d %d\t")+QObject::tr("Numberic")
                  << QString("C %d\t")+QObject::tr("Char")
                  << QString("D\t")+QObject::tr("Date")
                  << QString("B\t")+QObject::tr("Boolean");
}
QDomNode cur = domNode;
while (cur.parentNode().nodeName()!=md_root)
cur = cur.parentNode();
cur=cur.firstChild();
while(!cur.isNull()) {
if (filter.filter(cur.nodeName()).count()!=0) {
if (filter.filter(cur.nodeName()).at(0)==md_catalogues) {
                //l << QObject::tr("Catalogues");
for (int i=0;i<cur.childNodes().count();i++)
{
QDomNode childNode = cur.childNodes().item(i);
                //l << QObject::tr("Catalogue")+"."+childNode.attributes().namedItem(mda_name).nodeValue();
                QString oid = childNode.attributes().namedItem(mda_id).nodeValue();
                l << QString("O ")+oid+QString("\t")+QObject::tr("Catalogue")+QString(".")+childNode.attributes().namedItem(mda_name).nodeValue();
}
}
if (filter.filter(cur.nodeName()).at(0)==md_documents) {
                //l << QObject::tr("Documents");
for (int i=0;i<cur.childNodes().count();i++)
{
QDomNode childNode = cur.childNodes().item(i);
                //l << QObject::tr("Document")+"."+childNode.attributes().namedItem(mda_name).nodeValue();
                QString oid = childNode.attributes().namedItem(mda_id).nodeValue();
                l << QString("O ")+oid+QString("\t")+QObject::tr("Document")+QString(".")+childNode.attributes().namedItem(mda_name).nodeValue();
}
}
if (filter.filter(cur.nodeName()).at(0)==md_registers) {
for (int i=0;i<cur.childNodes().count();i++)
{
QDomNode childNode = cur.childNodes().item(i);

if (childNode.nodeName()==md_iregisters){
for (int j=0;j<childNode.childNodes().count();j++) {
                                //l << QObject::tr("Information registers")+"."+childNode.childNodes().item(j).attributes().namedItem(mda_name).nodeValue();
                            QString oid = childNode.attributes().namedItem(mda_id).nodeValue();
                l << QString("O ")+oid+QString("\t")+QObject::tr("Information registers")+QString(".")+childNode.attributes().namedItem(mda_name).nodeValue();
}
}
if (childNode.nodeName()==md_aregisters){
for (int j=0;j<childNode.childNodes().count();j++) {
                                //l << QObject::tr("Accumulation registers")+"."+childNode.childNodes().item(j).attributes().namedItem(mda_name).nodeValue();
                                        QString oid = childNode.attributes().namedItem(mda_id).nodeValue();
                l << QString("O ")+oid+QString("\t")+QObject::tr("Accumulation registers")+QString(".")+childNode.attributes().namedItem(mda_name).nodeValue();
}
}
}
}

}
cur = cur.nextSibling();
}
return l;
}
DomCfgItem *DomCfgItem::root()
{
return rootNode;

}
DomCfgItem *DomCfgItem::find(QString f)
{
  if (nodeName()==f)
return this;
for(int i=0;i<childCount();i++) {
if (child(i)->nodeName()==f)
return child(i);
else {
if (child(i)->hasChildren()) {
DomCfgItem *fl=child(i)->find(f);
if (fl!=0)
return fl;
}
}
}
return 0;
}

DomCfgItem
*DomCfgItem::findByName(QString name)
{
QString oType, oName, omType, extName;
DomCfgItem *gobj=0, *item=0;

oType = name.section( ".", 0, 0 );
oName = name.section( ".", 1, 1 );
extName = name.section( ".", 2 );

while ( !oType.isEmpty() && !oName.isEmpty() ) {
if (oType == "Document" || oType == tr("Document") ) {
omType = md_document;
gobj = find(md_documents );
}
if (oType == "Catalogue" || oType == tr("Catalogue") ) {
omType = md_catalogue;
gobj = find( md_catalogues );
}
if (oType == "DocJournal" || oType == tr("DocJournal")) {
omType = md_journal;
gobj = find( md_journals );
}
if (oType == "Report" || oType == tr("Report")) {
omType = md_report;
gobj = find( md_reports );
}
if (oType == "InfoRegister" || oType == tr("InfoRegister") ) {
omType = md_iregister;
gobj = find( md_iregisters );
}
if (oType == "AccumulationRegister" || oType == tr("AccumulationRegister") )
{
omType = md_aregister;
gobj = find( md_aregisters );
}
if (oType == "Form" || oType == tr("Form")) {
omType = md_form;
gobj = find( md_forms );
}
if ( gobj!=0 ) {
for (int i=0;i<gobj->childCount();i++)
{
if (gobj->child(i)->cfgName()==oName) {
item=gobj->child(i);
break;
}
};}
if ( item==0 ) break;
gobj = item;
oType = extName.section( ".", 0, 0 );
if (oType=="Form")
oType=md_forms;
oName = extName.section( ".", 1, 1 );
extName = extName.section( ".", 2 );

for (int i=0;i<gobj->childCount();i++)
{
if (gobj->child(i)->nodeName()==oType) {
item = gobj->child(i);
for (int j=0;j<item->childCount();j++) {
qDebug() << "item->childCount() " << item->child(j)->cfgName() << " " << oName;
if (item->child(j)->cfgName()==oName)
return item->child(j);
}
}
}
}
return item;
}

QString DomCfgItem::configName()
{
if (domNode.nodeName()==md_catalogue)
return QObject::tr("Catalogue")+"."+domNode.attributes().namedItem(mda_name).nodeValue();
if (domNode.nodeName()==md_document)
return QObject::tr("Document")+"."+domNode.attributes().namedItem(mda_name).nodeValue();
 return "";
}
QString DomCfgItem::getNameByType(QStringList type)
{
if (type.at(1)=="C")
return QObject::tr("Char");
if (type.at(1)=="D")
return QObject::tr("Date");
if (type.at(1)=="O") {
DomCfgItem *item = findObjectById(type.at(3));
                return QObject::tr("%1").arg(item->configName());
}
if (type.at(1)=="N")
return QObject::tr("Numberic");
if (type.at(1)=="B")
return QObject::tr("Boolean");
else
return type.at(1);
}
DomCfgItem *DomCfgItem::findObjectById(int id)
{
 return findObjectById(QString::number(id));
}
DomCfgItem *DomCfgItem::findObjectById(QString id)
{
if (root()->hashId.contains(id))
return root()->hashId.value(id);
else
{
for (int i=0;i<childCount();i++)
{
//aLog::print(aLog::Debug,"DomCfgItem::findObjectById(QString id) "+id+" "+child(i)->node().attributes().namedItem(mda_id).nodeValue()+"\n");
if (child(i)->node().attributes().namedItem(mda_id).nodeValue()==id)
return child(i);
else {
if (child(i)->hasChildren()) {
DomCfgItem *f=child(i)->findObjectById(id);
if (f!=0)
return f;
}
}
}
}
return 0;
}
QString DomCfgItem::nodeName() const
{
 return domNode.nodeName();
}
QString DomCfgItem::nodeValue() const
{
 return domNode.childNodes().item(0).nodeValue();
}
long
DomCfgItem::nextID()
{
long id = 0;
QDomNode info,last_id;
info = root()->node();
QDomNode nodeInfo = info.namedItem(md_info);
if (!nodeInfo.isNull()) {
last_id = nodeInfo.namedItem(md_info_lastid);
id = last_id.nodeValue().toLong();
if (id == 0 ) id = context_startid;
++id;
last_id.setNodeValue(QString("%1").arg(id));
}
return id;
}
void DomCfgItem::insert(DomCfgItem *context,QString &otype,QString &name,long id)
{
QDomElement i;
QDomDocument xml;
if ( id==0 ) id = nextID();
i = xml.createElement(otype);
if ( id >= 100 ) i.setAttribute(mda_id,QString::number(id));
if ( !name.isNull()) i.setAttribute(mda_name,name);
context->node().appendChild( i );
}

bool DomCfgItem::moveUp()
{
    DomCfgItem* p = parent();
    int currentrow=row();
    int prevrow=row()-1;
    if (currentrow==0)
            return true;
    if (!p->node().insertBefore(node(),p->child(prevrow)->node()).isNull()) {
        p->childItems.remove(prevrow);
        p->childItems.remove(currentrow);
        return true;
    }
    return false;
}

bool DomCfgItem::moveDown()
{
    DomCfgItem* p = parent();
    int currentrow=row();
    int prevrow=row()+1;
    if (currentrow==p->childCount()-1)
            return true;

    if (!p->node().insertAfter(node(),p->child(prevrow)->node()).isNull()) {
        p->childItems.remove(prevrow);
        p->childItems.remove(currentrow);
        return true;
    }
    return false;
}

DomCfgItem* DomCfgItem::newCatalogue()
{
    if (node().nodeName()==md_catalogues) {
        QString otype=md_catalogue;
        QString name=tr("New Catalogue");
        insert(this,otype,name,0);
        DomCfgItem* catalogue=child(childCount()-1);
        otype = md_element;
        name = "";
        insert( catalogue, otype , name, -1 );
        otype = md_group;
        insert( catalogue, otype , name, -1 );
        otype = md_forms;
        insert( catalogue, otype , name, -1 );
        otype = md_webforms;
        insert( catalogue, otype , name, -1 );
        return catalogue;
    }
    return 0;
}

DomCfgItem* DomCfgItem::newJournal()
{
    if (node().nodeName()==md_journals) {
        QString otype=md_journal;
        QString name=tr("New Journal");
        insert(this,otype,name,0);
        DomCfgItem* journal=child(childCount()-1);
        otype = md_element;
        name = "";
        insert( journal, otype , name, -1 );
        otype = md_group;
        insert( journal, otype , name, -1 );
        otype = md_forms;
        insert( journal, otype , name, -1 );
        otype = md_webforms;
        insert( journal, otype , name, -1 );
        return journal;
    }
    return 0;
}

DomCfgItem* DomCfgItem::newElement()
{
    QString otype=md_field;
    QString name=tr("New");
    insert(this,otype,name,0);

    DomCfgItem *field=child(childCount()-1);
    field->setAttr(mda_type,QString("C 10\t")+QObject::tr("Char"));
    field->setAttr(mda_sort,"0");
    field->setAttr(mda_plus,"0");
    field->setAttr(mda_nz,"0");
    field->setAttr(mda_sum,"0");
    return field;

}

QByteArray
DomCfgItem::binary()
{
unsigned int i, d, blen;
bool ok;
QString vs = nodeValue();
blen = attr(mda_length).toInt();
        QByteArray b;
for ( i=0; i < blen; i++ ){
d = 0xff & vs.mid( i*2, 2 ).toInt( &ok, 16 );
                if ( ok ) //b.data()[ i ] = d;
                    b.append(d);
else b.data()[ i ] = 0;
}
return b;
}
void DomCfgItem::setModified()
{
  root()->fModified=true;
}
bool DomCfgItem::modified()
{
return root()->fModified;
}


long
DomCfgItem::getDefaultFormId( DomCfgItem *owner, int actiontype, int mode )
{
DomCfgItem *fobj, *cobj;
cobj = owner->child(md_forms);
if ( cobj==0 )
return 0;
int formtype, fa;
switch ( actiontype )
{
case md_action_new :
formtype = md_form_new;
break;
case md_action_view :
formtype = md_form_view;
break;
case md_action_edit :
formtype = md_form_edit;
break;
default :
return 0;
}
for ( uint i = 0; i < cobj->childCount(); i++ )
{
fobj = cobj->child(i);
fa = fobj->child(md_defaultmod)->nodeValue().toInt();
if ( (fa>>formtype)%2 && fobj->attr(mda_type).toInt() == mode )
return fobj->attr(mda_id).toInt();
}
return 0;
}


void
DomCfgItem::setText(const QString &name,const QString &value)
{
QDomText t;
QDomNode cur = node().namedItem(name);
while (!cur.firstChild().isNull()) {
cur.removeChild( cur.firstChild() );
}
QDomDocument xml;
t = xml.createTextNode( value );
cur.appendChild( t );
setModified();
}


void DomCfgItem::setAttr(const QString &name, const QString &value)
{
QString v = value;
if ( nodeName() == md_field && name == mda_type ) {
if ( v.section(" ", 1).isEmpty() ) v.append(" 0 0 *");
if ( v.section(" ", 2).isEmpty() ) v.append(" 0 *");
if ( v.section(" ", 3).isEmpty() ) v.append(" *");
}
  node().toElement().setAttribute( name, v );
setModified( );
}


void
DomCfgItem::setSText(const QString & subname, const QString &value)
{
DomCfgItem *i;
i = child(subname);
if ( i==0) {
//i = insert(subname);
}
//i->setText(value);
}

DomCfgItemInterfaces::DomCfgItemInterfaces(QDomNode &node, int row, DomCfgItemInterfaces *parent):DomCfgItem(node,row,parent)
{
if (parent==0) {
   domNode=node.namedItem(md_root).namedItem(md_interface);
}
}


bool DomCfgItemInterfaces::hasChildren() const
{
    if (domNode.hasChildNodes())
        return true;
    return false;
}
int DomCfgItemInterfaces::childCount()
{
if (hasChildren())
return domNode.childNodes().count();
return 0;
}
DomCfgItem *DomCfgItemInterfaces::child(int i)
{
 DomCfgItem *childItem;
    if (childItems.contains(i))
        return childItems[i];
    if (i >= 0 && i < domNode.childNodes().count()) {
//DomCfgItemInterfaces *firstChildItem = 0;

        QDomNode childNode = domNode.childNodes().item(i);

childItem = new DomCfgItemInterfaces(childNode, i, this);
         childItems[i] = childItem;
//return childItem;
}

        return childItem;
}

DomCfgItem* DomCfgItemInterfaces::child(QString f)
{
 for (int i=0;i<childCount();i++)
{
if (child(i)->nodeName()==f)
return child(i);
}
return 0;
}

DomCfgItem* DomCfgItemInterfaces::child(QString f,int j)
{
 int number = 0;
 for (int i=0;i<childCount();i++)
{
if (child(i)->nodeName()==f) {
if (number==j)
return child(i);
else
number++;
}
}
return 0;
}

DomCfgItemActions::DomCfgItemActions(QDomNode &node, int row, DomCfgItemActions *parent):DomCfgItem(node,row,parent)
{
if (parent==0) {
   domNode=node.namedItem(md_root).namedItem(md_actions);
}
}


bool DomCfgItemActions::hasChildren() const
{
    if (domNode.hasChildNodes())
        return true;
    return false;
}
int DomCfgItemActions::childCount()
{
if (hasChildren())
return domNode.childNodes().count();
return 0;
}
DomCfgItem *DomCfgItemActions::child(int i)
{
 DomCfgItem *childItem=0;
    if (childItems.contains(i))
        return childItems[i];
    if (i >= 0 && i < domNode.childNodes().count()) {
DomCfgItemActions *firstChildItem = 0;

        QDomNode childNode = domNode.childNodes().item(i);

childItem = new DomCfgItemActions(childNode, i, this);
         childItems[i] = childItem;
}

        return childItem;
}

DomCfgItem* DomCfgItemActions::child(QString f)
{
 for (int i=0;i<childCount();i++)
{
if (child(i)->nodeName()==f)
return child(i);
}
return 0;
}

DomCfgItem* DomCfgItemActions::child(QString f,int j)
{
 int number = 0;
 for (int i=0;i<childCount();i++)
{
if (child(i)->node().nodeValue()==f) {
if (number==j)
return child(i);
else
number++;
}
}
return 0;
}
