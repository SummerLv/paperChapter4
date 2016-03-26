#ifndef INTERFERECRANETAB_H
#define INTERFERECRANETAB_H

#include <QDialog>
#include <QtGui>
#include "data.h"
#include <QSqlTableModel>
#include "sqldb.h"
class InterfereCraneTab : public QDialog
{
    Q_OBJECT
public:
    GlobalData *globalData;
    InterfereCraneTab(QWidget *parent);
    ~InterfereCraneTab();
private:
    void interfaceInit();
    void editRefresh();
private:
    QPushButton *saveButton;
    QPushButton *quitButton;
    QPushButton *addCraneButton;
    QPushButton *delCraneButton;

    QButtonGroup *craneGroup;

    QLineEdit *craneName;
    QLineEdit *craneGroupID;
    QLineEdit *craneID;

    QLineEdit *craneX;
    QLineEdit *craneY;
    QLineEdit *craneTopHeight;

    QLineEdit *foreArmLen;
    QLineEdit *backArmLen;
    QLineEdit *craneHeight;

    QLineEdit *IMEI;

    //GlobalData *globalData;
    quint8 currentCrane;                    //选中的塔吊代号
    //SqlDB *db;                      //数据库类
public slots:
    void saveButtonClicked();               //保存按键槽函数
    void craneSelect(int i);                    //相干塔吊选择按钮
    void delButtonClicked();                    //删除按键槽函数



};

#endif // INTERFERECRANETAB_H
