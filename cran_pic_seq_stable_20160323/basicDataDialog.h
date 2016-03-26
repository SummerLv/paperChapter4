#ifndef BASICDATADIALOG_H
#define BASICDATADIALOG_H
#include <QLayout>//布局库
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
 #include <QComboBox>//下拉框
#include "parameter.h"
#include "data.h"
#include <QSqlTableModel>
#include "sqldb.h"

/*基本信息Tab标签页，基本信息布局*/

class BasicDataTab : public QDialog
{
    Q_OBJECT
public:
    GlobalData *globalData;
    explicit BasicDataTab(QWidget *parent = 0);
    ~BasicDataTab();
   // bool eventFilter(QObject *watched, QEvent *event);      //事件过滤器函数
private:
    void interfaceInit();

    QLineEdit *version;     //信息版本输入框
    QLineEdit *firmID;      //厂商ID
    //QLineEdit *craneType;
    QLineEdit *craneName;

    QLineEdit *groupID;
    QLineEdit *craneID;
    QLineEdit *craneTopheight;
    //QLineEdit *CraneRate;

    QLineEdit *craneX;
    QLineEdit *craneY;
    QLineEdit *foreArmLen;
    QLineEdit *backArmLen;

    QLineEdit *craneHeight;
    QLineEdit *sectionNum;
    QLineEdit *preHeight;
    QLineEdit *firstSectionH;        //初始首节高度

    QComboBox *CraneRate;
    QComboBox *craneType;

    QLineEdit *IMEI;

    QPushButton *saveButton;
    QPushButton *cancelButton;


    SqlDB *db;                      //数据库类
    //QSqlQuery *basicData;   //数据库表操作类
    //QList<QLineEdit *> basicLineEdit;

signals:


private slots:
   // void cancelButtonClicked(QWidget *parent);
    void saveButtonClicked();


};
#endif // BASICDATADIALOG_H
