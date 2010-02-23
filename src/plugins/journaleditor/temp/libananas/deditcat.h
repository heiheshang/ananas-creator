#ifndef DEDITCAT_H
#define DEDITCAT_H

#include ".ui/ui_deditcat.h"
#include <Qt>

class dEditCat : public QMainWindow, public Ui::dEditCat
{
    Q_OBJECT

public:
    dEditCat(QWidget* parent = 0, const char* name = 0, Qt::WindowFlags fl = Qt::Window);
    ~dEditCat();

    aRoleEditor *re;
    QHash<int,DomCfgItem*> fields, fieldsg;
    aAliasEditor *al;
    DomCfgItem *item;

public slots:
    virtual void setData( DomCfgItem * o );
    virtual void updateMD();
    virtual void eSv_activated( int index );
    virtual void eSvG_activated( int index );

protected:
    int newVariable;

protected slots:
    virtual void languageChange();

private:
    void init();
    void destroy();
private slots:
    void doubleClicked ( int row, int column );
    void edit_field ();

};

#endif // DEDITCAT_H
