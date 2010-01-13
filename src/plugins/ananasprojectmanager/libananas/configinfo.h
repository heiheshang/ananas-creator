#ifndef CONFIGINFO_H
#define CONFIGINFO_H
#include <QDialog>
#include <Qt>
#include "acfg.h"
#include "ui_configinfo.h"


class configInfo : public QDialog, public Ui::configInfo
{
    Q_OBJECT

public:
    configInfo(DomCfgItem *item,QWidget * parent = 0, Qt::WindowFlags f = 0 );
    ~configInfo();
private :
 virtual void accept();
private:
  DomCfgItem *node;
};

#endif // CONFIGINFO_H
