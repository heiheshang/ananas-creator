#include "acfg.h"
#include "configinfo.h"

configInfo::configInfo(DomCfgItem *item,QWidget * parent , Qt::WindowFlags f ): QDialog(parent,f),node(item)
{
    setupUi(this);
    node = item;
    QDomNode info = node->root()->node().namedItem(md_root).namedItem(md_info);
    editInfo->setText(info.namedItem(md_info_name).toElement().text());
    autorEdit->setText(info.namedItem(md_info_author).toElement().text());
    dateEdit->setDate(QDate::fromString(info.namedItem(md_info_date).toElement().text()));
    commentEdit->setText(info.namedItem(md_info_remark).toElement().text());
}


configInfo::~configInfo()
{
 
}

void configInfo::accept()
{
QDomNode element = node->root()->node().namedItem(md_root);
QDomNode oldInfoElement =node->root()->node().namedItem(md_root ).namedItem(md_info);//.setNodeValue(editInfo//->text());
QDomDocument domDocument = QDomDocument();
domDocument.setContent(QString("<?xml version = '1.0' encoding = 'UTF-8'?>\n"));
QDomElement newInfoElement = domDocument.createElement(md_info);
QDomElement name = domDocument.createElement(md_info_name);
name.appendChild(domDocument.createTextNode(editInfo->text()));
newInfoElement.appendChild(name);
QDomElement autor = domDocument.createElement(md_info_author);
autor.appendChild(domDocument.createTextNode(autorEdit->text()));
newInfoElement.appendChild(autor);

QDomElement date = domDocument.createElement(md_info_date);
date.appendChild(domDocument.createTextNode(dateEdit->date().toString()));
newInfoElement.appendChild(date);

QDomElement comment = domDocument.createElement(md_info_remark);
comment.appendChild(domDocument.createTextNode(commentEdit->toPlainText()));
newInfoElement.appendChild(comment);


element.replaceChild(newInfoElement, oldInfoElement);
QDialog::accept();
}

