
#include "fieldeditor.h"
#include <QDate>
#include <QString>
#include <QPalette>
#include <QVariant>
#include <QDebug>
#include <coreplugin/editormanager/editormanager.h>

using namespace FIELDEditor;

/*
 *  Constructs a FieldEditor as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 */
FieldEditor::FieldEditor(QWidget* parent, const char* name, Qt::WindowFlags fl)
    : QMainWindow(parent, fl)
{
    setupUi(this);
    init();
}

/*
 *  Destroys the object and frees any allocated resources
 */
FieldEditor::~FieldEditor()
{
    destroy();
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void FieldEditor::languageChange()
{
    retranslateUi(this);
}

void
setText(){

}


void FieldEditor::destroy()
{
    //updateMD();
    //( (MainForm*)this->topLevelWidget() )->wl->remove( this );
    //( (MainForm*)this->topLevelWidget() )->removeTab(name());
}



void FieldEditor::init()
{
// eModule->setInterpreter(new QSInterpreter());
        //delete statusBar();
	otypes.clear();
        artypes.clear();
        eType->clear();
	otypes.append(" ");
        eType->insertItems(0,QStringList() << tr("Unknown"));
        tabWidget->setCurrentIndex(0);

}


void FieldEditor::setData( DomCfgItem *o )
{
        item = o;
// 	aCfg *md = o->md;
// 	aCfgItem obj = o->obj;
// 
        QString ts;
        QString t=" ";
        int w=0, d=0, oid, idx=0;
        unsigned int i;
// 
// 
// 	aAliasEditor *a = new aAliasEditor( md, obj, tAliases );
// 	al = a;
// 	al->setData();
// 	// setCaption( tr("Field:") + md->attr( obj, mda_name ) );
        eName->setText( item->cfgName() );
 	nameChanged();
 	// eID->setText( md->attr( obj, mda_id ) );
        ts = item->attr(mda_type );
        if(item->parent()->node().nodeName()==md_element)
 	{
 		saldoTextLabel->show();
 		combo_rests->show();
 //		updateGeometry();
 	}
 	else
 	{
 		saldoTextLabel->hide();
 		combo_rests->hide();
 //		layout()->remove(comboBox2);
 //		layout()->remove(saldoTextLabel);
 
 //		updateGeometry();
 	}
 	// eType0->setText( ts );
 	// eModule->setText( md->sText( obj, md_sourcecode ) );
        eDescription->setText( item->attr( md_description ) );
// 
        if( item->attr( mda_sort ) == "1" ) efSort->setChecked( true );
 	else efSort->setChecked( false );
        if( item->attr( mda_plus ) == "1" ) efPlus->setChecked( true );
 	else efPlus->setChecked( false );
        if( item->attr( mda_nz ) == "1" ) efNZ->setChecked( true );
 	else efNZ->setChecked( false );
            efSum->setChecked(o->attr( mda_sum ) == "1");
 
 	QStringList tlist;
        if (item->parent()->node().nodeName()==md_resources)
        {
                tlist.append("\t"+QObject::tr("Unknown"));
                tlist.append("N %d %d\t"+QObject::tr("Numberic"));
        }
        else
        {
                tlist = item->types(QStringList() << "base" << md_catalogues << md_documents);
        }
        otypes.clear();

        QStringList type = item->attr("type").split(QRegExp("\\b"));

        eType->clear();
	
        //eType->addItems(item->types(QStringList() << "base" << md_catalogues << md_documents));
        QListIterator<QString> it(tlist);
        while (it.hasNext()) {
            QString sec = it.next();
            eType->addItem(sec.section("\t",1,1));
            otypes.append(sec.section( "\t", 0, 0 ));
        }

        int index =eType->findText(item->root()->getNameByType(type));
	if (index!=-1) {eType->setCurrentIndex(index);}

        if ( !ts.isEmpty() )
        {
                 //sscanf( ts, "%c %d %d", &t, &w, &d );
                 QStringList ts_list=ts.split(" ");
                 t=ts_list[0];
                 w=ts_list[1].toInt();
                 d=ts_list[2].toInt();
        }
        else
        {
                t = "N";
                w = 10;
        }

        if ( t=="O" ) {
                for( i = 0 ; i < otypes.count(); i++ ) {
                        if( otypes[i][0] == 'O' ) {
                                //sscanf( (const char *)otypes[ i ], "O %d", &oid );
                                QStringList ts_list = otypes[i].split(" ");
                                oid = ts_list[0].toInt();
                                if ( oid == w ) {
                                         eType->setCurrentIndex( i );
                                         break;
                                }
                        }
                }
        } else {
                 if ( t == " " ) eType->setCurrentIndex( 0 );
 	 	 if ( type.at(1) == "N" )
 		 {
                        eWidth->setMaximum(20);
                        eDec->setMaximum(99);
// 			eType->setCurrentItem( 1 );
 		 }
 	 	 if ( type.at(1) == "C" )
 		 {
 
                        eWidth->setMaximum(254);
                        eDec->setMaximum(99);
// 			eType->setCurrentItem( 2 );
 		 }
                 if ( t == "D" ) eType->setCurrentIndex( 3 );
                 if ( t == "B" ) eType->setCurrentIndex( 4 );
                 eWidth->setValue( w );
                 eDec->setValue( d );
        }
// 	aCfgItem obj2;
        int n, n1,aregid, aregfid;
        QString str;
// 	QStringList l_name;
//  //long oid;
// 	aCfgItem context = md->findChild(md->find(mdc_metadata),md_registers);
        combo_rests->clear();

        combo_rests->addItems(QStringList() << " " <<item->types(QStringList() << md_registers));
        artypes.clear();
        artypes.append(" ");
        DomCfgItem* reg = item->root()->find(md_aregisters);
        n = reg->childCount();//count( context, md_aregister );
 // printf("n=%d name = %s\n",n, md->attr(context,mda_name).ascii());
        for (i=0; i<n; i++)
        {
                DomCfgItem* obj = reg->child(md_aregister,i);
                if ( !obj )
                {
                        aregid=obj->attr(mda_id).toInt();
                        str = QObject::tr("AccumulationRegister.")+obj->attr(mda_name );
                        //printf("`%s'\n",str.ascii());
                        obj = obj->child(md_resources);
                        n1 = obj->childCount();
 //   printf("n=%d name = %s\n",n, md->attr(obj,mda_name).ascii());
                        for (uint j=0; j<n1; j++)
                        {
                                DomCfgItem* obj2 = obj->child(j);
                                if ( !obj2)
                                {
                                        aregfid = obj2->attr( mda_id).toInt();
                                        artypes.append(QString(" %1 %2").arg(aregid).arg(aregfid));
                                        combo_rests->insertItem(0,str+"."+obj2->attr(mda_name));
                                        if(w==aregid && d==aregfid) combo_rests->setCurrentIndex(combo_rests->count()-1);
                                }
                        }
                }
        }
 	typeSelect( type );
}

void FieldEditor::updateMD(QList<Core::IEditor*> editor)
{
  QString st;
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
// 
//  aCfg *md = item->md;
//  aCfgItem obj = item->obj;
// 
//  al->updateMD();
//  item->setText( 0, eName->text().stripWhiteSpace() );
    item->setAttr( mda_name, eName->text().trimmed() );
    item->setText(md_description,eDescription->toPlainText().trimmed());
//  md->setAttr( obj, mda_name, eName->text().stripWhiteSpace() );
//  md->setSText( obj, md_description, eDescription->text() );
// // md->setSText( obj, md_sourcecode, eModule->text() );
// 
//  if ( md->objClass(md->parent(obj)) == md_resources )
//   efSort->setChecked( true );
  if( efSort->isChecked() ) item->setAttr(mda_sort, "1" );
  else item->setAttr(mda_sort, "0" );
  if( efPlus->isChecked() ) item->setAttr(mda_plus, "1" );
  else item->setAttr(mda_plus, "0" );
  if( efNZ->isChecked() ) item->setAttr(mda_nz, "1" );
  else item->setAttr(mda_nz, "0" );
  if( efSum->isChecked() )
      item->setAttr(mda_sum, "1" );
  else
      item->setAttr(mda_sum, "0" );
  if(eType->currentIndex()==0)
  {
         st.append(QString("%1").arg(artypes[combo_rests->currentIndex()]));
  }
  else
  {
        st.append(QString("%1 %2").arg(otypes[eType->currentIndex()]).arg(eDec->value()));
        st.replace("%d",QString::number(eWidth->value()));
        //printf("type is %s\n",st.ascii());
        if ( efSort->isChecked() )
                st.append(" I");
  }
  //printf("%s\n",st.ascii());
  item->setAttr(mda_type, st );
}

void FieldEditor::hideEvent(QHideEvent* event)
{
    //Core::MessageManager::instance()->printToOutputPane(tr("DirectoryEditor"));
    //updateMD();
}

void FieldEditor::typeSelect( QStringList type )
{
	if ( type.at(1) == "N" )
	{
                eWidth->setMaximum(20);
		eWidth->show();
                eDec->setMaximum(99);
		eDec->show();
		tWidth->show();
		tDec->show();
		efSum->show();
		combo_rests->setEnabled(false);
	}
	else
	{
		if ( type.at(1) == "C" )
		{
                        eWidth->setMaximum(254);
			eWidth->show();
			eDec->hide();
			tWidth->show();
			tDec->hide();
			efSum->hide();
			efSum->setChecked(false);
			combo_rests->setEnabled(false);
		}
		else
		{
			if( type.at(1) == "")
			{
				combo_rests->setEnabled(true);
				eWidth->hide();
				eDec->hide();
				tWidth->hide();
				tDec->hide();
				efSum->hide();
				efSum->setChecked(false);
			}
  			else
			{
				combo_rests->setEnabled(false);
				eWidth->hide();
				eDec->hide();
				tWidth->hide();
				tDec->hide();
				efSum->hide();
				efSum->setChecked(false);
  //combo->setEnabled(false);
  			}
		}
	}
}


void FieldEditor::nameChanged()
{
        setWindowTitle( tr("Field:") + eName->text() );
}


void FieldEditor::AARegSelect( int i )
{

}

void FieldEditor::setExample()
{
     QDate date = QDate::currentDate();
     QString curdate, dateformat, numerator;

     if (DateFormat->isChecked() )
     {
         if (useOwnFofmat->isChecked() )
         {
             dateformat =eOwnFormat->text();
         }else{
             dateformat =aDFormatBox->currentText();
         }
        curdate = date.toString(dateformat);
     }

     if (lzcheckBox->isChecked())
     {
        numerator = QString("789").rightJustified( 8, '0' );
     }
     else {
        numerator = QString("789");
     }
    eXample->setText(QString("%1%2%3%4").arg(ePrefix->text()).arg(numerator).arg(eSuffix->text()).arg(curdate));
    eWidth->setValue(eXample->text().length());
}


void FieldEditor::efVd_stateChanged( int )
{
    if (efVd->isChecked() )
    {
        ValidateGroupBox->show();
    } else {
        ValidateGroupBox->hide();
    }
}


void FieldEditor::Testline_textChanged( const QString & )
{
    QRegExp rx( VdRegEx->text() );
    QRegExpValidator v( rx, 0 );
    int pos = 0;
    QString s = Testline->text();
    QPalette pal = Testline->palette();

    if(v.validate(s, pos ) == QValidator::Invalid)
    {
        pal.setColor(QPalette::Highlight, Qt::red);
        Testline->setPalette(pal);
    }
    if(v.validate(s, pos ) == QValidator::Intermediate)
    {
        if (two_state->isChecked() ) {
            pal.setColor(QPalette::Highlight, Qt::red);
        }else{
            pal.setColor(QPalette::Highlight, Qt::yellow);
        }
         Testline->setPalette(pal);
    }
     if(v.validate(s, pos ) == QValidator::Acceptable)
    {
         pal.setColor(QPalette::Highlight, Qt::green);
         Testline->setPalette(pal);
    }
}

void FieldEditor::inputMask_textChanged( const QString & )
{
    Testline->setInputMask(inputMask->text());
}


void FieldEditor::efNumerator_stateChanged( int )
{
    if (efNumerator->isChecked() )
    {
        NumeratorGroupBox->show();
        eWidth->setValue(10);
    } else {
        NumeratorGroupBox->hide();
    }
}


void FieldEditor::DateFormat_stateChanged( int )
{
    if (DateFormat->isChecked() )
    {
        aDFormatBox->setEnabled(TRUE);
        textLabel2->setEnabled(TRUE);
        useOwnFofmat->setEnabled(TRUE);
        eOwnFormat->setEnabled(TRUE);
    } else {
        aDFormatBox->setEnabled(FALSE);
        textLabel2->setEnabled(FALSE);
        useOwnFofmat->setEnabled(FALSE);
        eOwnFormat->setEnabled(FALSE);
    }
}


void FieldEditor::useOwnFofmat_stateChanged( int )
{
        if (useOwnFofmat->isChecked() )
        {
                aDFormatBox->setEnabled(FALSE);
        }else{
                aDFormatBox->setEnabled(TRUE);
        }

}


void FieldEditor::eWidth_valueChanged( int )
{
    if (efNumerator->isChecked() ) setExample();
}
