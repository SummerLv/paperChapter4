#ifndef TABDIALOG_H
#define TABDIALOG_H
#include <QTabWidget>
#include <QLayout>
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
 #include <QComboBox>
//#include <QtGui>
#include "parameter.h"
#include "data.h"
#include "basicDataDialog.h"
#include "proZoneDataTab.h"
#include "limitdatatab.h"
#include "interferecranetab.h"
#include "kdDataTab.h"
#include "temtab.h"
#include "gprs.h"
class TabDialog : public QDialog
{
    Q_OBJECT
public:
    GlobalData *globalData;
    explicit TabDialog(QWidget *parent = 0);
    ~TabDialog();
private:
    //QTabWidget *tabWidget;
    //GlobalData *globalData;        //全局变量
signals:
    
public slots:
    void windowClose( ) ;
    
};






#endif // TABDIALOG_H
