#ifndef FIELDEDITOR_H
#define FIELDEDITOR_H

#include <QMainWindow>
#include "ananasprojectmanager/libananas/acfg.h"
#include <extensionsystem/iplugin.h>
#include <coreplugin/editormanager/ieditorfactory.h>
#include <coreplugin/ifile.h>
#include <texteditor/basetexteditor.h>
#include "ui_fieldeditor.h"

namespace Core {
class IEditor;
}

namespace TextEditor {
class FontSettings;
}

namespace FIELDEditor {


class FieldEditor : public QMainWindow, public Ui::FieldEditor
{
    Q_OBJECT

public:
    FieldEditor(QWidget* parent = 0, const char* name = 0, Qt::WindowFlags fl = Qt::Window );
    ~FieldEditor();

    QStringList otypes;
    QStringList artypes;
    DomCfgItem *item;
    //aAliasEditor *al;
    Core::IEditor *editorInterface() const { return m_ieditor; }
    void setEditorInterface(Core::IEditor *ieditor) { m_ieditor = ieditor; }
    Q_INVOKABLE void setData( DomCfgItem * o );
protected:
    void hideEvent (QHideEvent * event);
public slots:
    virtual void updateMD(QList<Core::IEditor*> editor);
    virtual void typeSelect( QStringList type );
    virtual void nameChanged();
    virtual void AARegSelect( int i );

protected slots:
    virtual void languageChange();

private:
    QString capt;
    Core::IEditor *m_ieditor;
    void destroy();
    void init();
    void setExample();
    void efVd_stateChanged( int );
    void Testline_textChanged( const QString & );
    void inputMask_textChanged( const QString & );
    void efNumerator_stateChanged( int );
    void DateFormat_stateChanged( int );
    void useOwnFofmat_stateChanged( int );
    void eWidth_valueChanged( int );
};
}
#endif // FIELDEDITOR_H
