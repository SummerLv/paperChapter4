#ifndef TEMTAB_H
#define TEMTAB_H
#include <QDialog>
#include <QtGui>
#include <QTableWidget>
#include "data.h"
#include <QSqlTableModel>
#include "sqldb.h"
class temtab:public QDialog
{
    Q_OBJECT
public:
    GlobalData *globaldata;
    temtab(QWidget *parent);
    ~temtab();
private:
  void  InterfaceInit();
private:
    QLineEdit *temprature;
    QLineEdit *voltage;
    QPushButton *read_ad;
    QPushButton *fitting;
    QPushButton *save;
    QPushButton *delete_line;
    QTableWidget *vol_ad;
};

#endif // TEMTAB_H
