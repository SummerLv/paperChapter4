#ifndef DATAGATHER_H
#define DATAGATHER_H
#include "gprs.h"
class DataRead;
class DataSend;
class DataGather : public QThread
{
    Q_OBJECT
public:
    GlobalData *globaldata;
    DataGather();
    virtual ~DataGather();
private:
    void run();
    DataRead *dataRead;
    DataSend *dataSend;
public:
    int fd;
};
//读线程
class DataRead : public DataGather
{
    Q_OBJECT
public:
    DataRead();
private:
    void run();
    void SplThrdCommand(char *buff, int shift);
    void Axis_Data_Proc(unsigned int tmpAxis_X, unsigned int tmpAxis_Y);
    void Ad_Proc(unsigned int tmpADweight , unsigned int tmpADheight , unsigned int tmpADpan);
    void Weight_Data_Proc(unsigned int tmpADweight);
    void Height_Data_Proc(unsigned int tmpADheight);
    void Span_Data_Proc(unsigned int tmpADspan);
    int FindFrameHead(char *buff, int encLen);//不安全版
};
//写线程
class DataSend : public DataGather
{
    Q_OBJECT
public:
    DataSend();
private:
    void run();
    void EncThrdCommand(char readNumber, char *thrdCommand);
};
#endif // DATAGATHER_H
