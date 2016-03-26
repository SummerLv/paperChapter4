#ifndef DIALOG_H
#define DIALOG_H
#include <QDialog>
#include <QPen>
#include <QtCore>
#include<QtGui>
#include <QMutexLocker>
#include "parameter.h"
#include"data.h"
#include "dataprocess.h"
#include "tabdialog.h"
#include "qpainter.h"
#include <linux/fs.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <QtDebug>
#include "voiceplay.h"

#include <QSqlTableModel>
#include "sqldb.h"
#include "math.h"

#include"serialSendData.h"
#include "zigbeereceive.h"
#include "camtab.h"
#include "finger.h"
#include "datagather.h"
class Finger_Gather;
class CamTab;

class Dialog : public QDialog
{
    Q_OBJECT
    
public:

    static Dialog *getDiaglogData()
    {
        if (NULL == m_DialogData)             //保证只创建一个实例
            m_DialogData = new Dialog;
        return m_DialogData;
    }

    QLabel *itemsLabelLinkdis;
    CamTab *camTab;

private:
     Dialog(QWidget *parent = 0);
    static Dialog *m_DialogData;


    class CGarbo
    {
    public:
        ~CGarbo()                                     //在析构函数的时候删除实例
        {
            if(Dialog::m_DialogData)
                delete Dialog::m_DialogData;
        }

    };
    static CGarbo Garbo;                           //静态成员，在程序结素时会调用它的析构函数

    void interfaceInit();                    //界面初始化

    QPen pen;                               //绘图画笔



    QLCDNumber *LCDNumberAngle;     //LCDnumber显示回转
    QLCDNumber *LCDNumberWeight;     //LCDnumber显示吊重
    QLCDNumber *LCDNumberSpan;     //LCDnumber显示变幅
    QLCDNumber *LCDNumberHeight;     //LCDnumber显示高度
    QLCDNumber *LCDNumberX;     //LCDnumber显示X倾角
    QLCDNumber *LCDNumberY;     //LCDnumber显示Y倾角
    QLCDNumber *LCDNumberWind;     //LCDnumber显示风速
    //QProgressBar *momentBar;        //力矩条
    QLCDNumber *LCDNumberMoment;

    QLCDNumber *LCDNumberChannel;   //信道的LCD显示
    QLCDNumber *LCDNumberID;   //ID的LCD显示
    QLCDNumber *LCDNumberAddr;   //节点地址的LCD显示
    QLCDNumber *LCDNumberLink;   //连接状态的LCD显示

    QPushButton *setButton;              //设置按键
    QPushButton *fingerButton;                  //指纹按键

    QPushButton *ChangeTab;//视频切换键

    Finger_Gather *fingergather;

    GlobalData *globalData;              //全局变量
    QMutex mutex;             //互斥锁

    QTableView *table;
    //QPainterPath *proZonePath;          //保护区绘图路径
    QList<QPainterPath> proZonePath;        //保护区绘图路径

    //SqlDB *db;                                //数据库打开指针
    //QSqlTableModel *userModel;      //数据库打开模式

public slots:
    void timer1UpDate();                 //定时器1刷新槽函数
    void timer2UpDate();                 //定时器2槽函数
    void setButtonClicked();            //设置按钮槽函数
    void ChangeTabButtonClicked();
    void ChangeCamButtonClicked();
    void fingerButtonClicked();         //指纹按键槽函数

protected:

    void paintEvent(QPaintEvent *event);

};

#endif // DIALOG_H
