#ifndef KDDATATAB_H
#define KDDATATAB_H

#define CORRECT_WEIGHT 0
#define CORRECT_SPAN 1
#define CORRENT_HEIGHT 2
#define SET_ZIGBEE 3
#define SET_SERVER_NET 4
#define SET_ANGLE 5

#include <QDialog>
#include <QtGui>
#include <QTableWidget>
#include "data.h"
#include <QSqlTableModel>
#include "sqldb.h"

class KDdataTab : public QDialog
{
    Q_OBJECT
public:
    GlobalData *globalData;
    KDdataTab(QWidget *parent);
    ~KDdataTab();
private:
    void interfaceInit();
private:
    QPushButton *saveButton;
    QPushButton *quitButton;
    QButtonGroup *setButtons;

    QLineEdit *weightK;
    QLineEdit *weightD;
    QLineEdit *spanK;
    QLineEdit *spanD;
    QLineEdit *heightK;
    QLineEdit *heightD;

    QLineEdit *zigbeeChannel;
    QLineEdit *zigbeeID;

    QLineEdit *serverIP;
    QLineEdit *serverPort;

    QLineEdit *angle;


    //SqlDB *db;                      //数据库类
   // CorrectDialog *correctDialog;
    qint8 seclectNum;

public slots:
    void saveButtonClicked();      //保存槽函数
    void setButtonsSelect(int i);       //设置按钮
    //void valueRefresh(double kData,double dData);

};
/*
class CorrectDialog : public QDialog
{
    Q_OBJECT
public:
    CorrectDialog(QWidget *parent);
private:
    void dialogInit();
private:
    QPushButton *getValueButton;
    QPushButton *calculateButton;
    QPushButton *cancelButton;
    QPushButton *saveValueButton;
    QPushButton *addValueButton;
    QPushButton *delValueButton;

    QLineEdit *actualValue;
    QLineEdit *ADValue;
    QLineEdit *KValue;
    QLineEdit *DValue;

    QTableWidget *valueTab;
signals:
    void dataChange(double,double);


public slots:
    void getValueButtonClicked();
    void addValueButtonClicked();
    void delValueButtonClicked();
    void calculateButtonClicked();
    void saveValueButtonClicked();
    void cancelButtonClicked();


};

*/
#endif // KDDATATAB_H
