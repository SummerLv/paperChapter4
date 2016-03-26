#ifndef FINGER_H
#define FINGER_H
#include "gprs.h"
#include <sqldb.h>
#include <QSqlTableModel>
#include <QWaitCondition>
#include <QFile>
#include <unistd.h>
#include <QThread>
//@1209 fcntl() 方式操作fd为 非阻塞的方式
#include "sys/types.h"
#include <unistd.h>
#include <fcntl.h>
#include <gpio.h>

const int FINGERDATALEN = 512 ;

//class Finger_Gather : public QObject
class Finger_Gather:public  QThread
{
    Q_OBJECT
public:
   // Finger_Gather(QObject *parent = 0);
    Finger_Gather();
    ~Finger_Gather();

public:
    QFile file;
    qint32 fd;
    //@ 1209  打指纹次数限制
    //int fingerChance;

public:
        GlobalData *globalData;                 //全局变量
        int  checkScreenTouch();
        int FingerData_Store_InModule(char *set_buf, quint8 Finger_Store_Start_Flag , quint8 Finger_num);
        void FingerData_Store_InModule(char *set_buf, quint16 partion);
        void FingerData_Delete(quint32 address, quint8 num);
private:
        quint32 Driverid;
        quint32 Driverversion;
        QWaitCondition wait;
        QTimer* timer;
        GpioCtl *gpio;

private:
    void run();
    void Double_To_Array(quint8 *p , int value);
    int finger_press_process();
    void read_finger_number();
    void read_sys_para();
signals:
        //void finger_version_inquiry();
        void    fingerScreenTouch();
public slots:
       void finger_data_process();
       void  storeTemple();
public slots:
           void testTimer();
private:
       int readTemple();
       void   getTemple();
public:
       // add variable for  download  fingerTemple
       char fingerData[FINGERDATALEN];
};

#endif // FINGER_H
