#ifndef FINGER_H
#define FINGER_H
#include "gprs.h"
#include <sqldb.h>
#include <QSqlTableModel>
#include <QWaitCondition>
#include "dynamicgetusbcom.h"

class Finger_Gather : public QObject
{
    Q_OBJECT
public:
    Finger_Gather(QObject *parent = 0);
    ~Finger_Gather();
public:
        GlobalData *globalData;                 //全局变量
        int FingerData_Store_InModule(char *set_buf, quint8 Finger_Store_Start_Flag , quint8 Finger_num);
        void FingerData_Store_InModule(char *set_buf, quint16 partion);
        void FingerData_Delete(quint32 address, quint8 num);
private:
        quint32 Driverid;
        quint32 Driverversion;
        QWaitCondition wait;
        UsbComTool *usbComTool;
private:
    void Double_To_Array(quint8 *p , int value);
    void finger_press_process();
    void read_finger_number();
    void read_sys_para();
signals:
        //void finger_version_inquiry();
public slots:
       void finger_data_process();
};

#endif // FINGER_H
