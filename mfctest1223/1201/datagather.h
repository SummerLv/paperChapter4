//===========  version_2

#ifndef DATAGATHER_H
#define DATAGATHER_H
#include "gprs.h"
#include "gpio.h"
#include  <qmutex.h>
#include <QSemaphore>

//class DataRead;
//class DataSend;
class DataGather : public QThread
{
    Q_OBJECT
public:
    GlobalData *globaldata; //global data :moment file -> span . weight Array
    DataGather();
    virtual ~DataGather();
    GpioCtl  *gpioCtl;
    QMutex  mutex;
    QSemaphore *sem;
private:
    void run();

    double weightRate;  // return for the real_time display
    quint8 spanSearch(void);
    double momentCaculate(quint8 index);
    int  dataSafer(int data);
     int FindFrameHead(char *buff , int encLen);

public:
    int fd;
public:
    void readrun();
    void SplThrdCommand(char *buff, int shift);
    void Axis_Data_Proc(unsigned int tmpAxis_X, unsigned int tmpAxis_Y);
    void Ad_Proc(unsigned int tmpADweight , unsigned int tmpADheight , unsigned int tmpADpan);
    void Weight_Data_Proc(unsigned int tmpADweight);
    void Height_Data_Proc(unsigned int tmpADheight);
    void Span_Data_Proc(unsigned int tmpADspan);

public:
    void writerun();
    void EncThrdCommand(char readNumber, char *thrdCommand);

    //@0123 check electric
    bool checkElectry(void);
    void shutDown(void);

};

#endif // DATAGATHER_H

//============version_1

//#ifndef DATAGATHER_H
//#define DATAGATHER_H
//#include "gprs.h"
//#include "gpio.h"

//class DataGather : public QThread
//{
//    Q_OBJECT
//public:
//    GlobalData *globaldata;
//    DataGather();
//    ~DataGather();
//    GpioCtl   *gpioCtl;
//    void Send_Data_To_430();
//private:
//    void run();
//    void Ad_Proc(unsigned int tmpADweight, unsigned int tmpADheight, unsigned int tmpADpan);
//    void Weight_Data_Proc(unsigned int tmpADweight);
//    void Height_Data_Proc(unsigned int tmpADheight);
//    void Span_Data_Proc(unsigned int tmpADspan);
//    void Axis_Data_Proc(char *axisdata);
//private:
//    int fd;
//};

//#endif // DATAGATHER_H
