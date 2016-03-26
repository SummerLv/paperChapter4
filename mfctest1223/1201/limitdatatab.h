#ifndef LIMITDATATAB_H
#define LIMITDATATAB_H

#include <QDialog>
#include <QtGui>
#include "data.h"
#include <QSqlTableModel>
#include "sqldb.h"
class LimitDataTab : public QDialog
{
    Q_OBJECT
public:
    GlobalData *globalData;
    LimitDataTab(QWidget *parent);
    ~LimitDataTab();
private:
    void interfaceInit();    //限位值界面初始化
private:
    QLineEdit *version;
    QLineEdit *leftLimit;
    QLineEdit *rightLimit;
    QLineEdit *farLimit;
    QLineEdit *closeLimit;
    QLineEdit *heightLimit;
    QLineEdit *weightLimit;
    QLineEdit *momentLimit;

    QLineEdit *limit_test;

    QPushButton *saveButton;
    QPushButton *quitButton;
    QButtonGroup *buttonGroup;

    QCheckBox *sensor[8];        //传感器使能选中

    //GlobalData *globalData;        //全局变量
   // SqlDB *db;                      //数据库类

public slots:
    void saveButtonClicked();      //保存槽函数

};

#endif // LIMITDATATAB_H
