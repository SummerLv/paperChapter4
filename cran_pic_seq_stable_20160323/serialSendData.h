#ifndef SERIALSENDDATA_H
#define SERIALSENDDATA_H

#include <QThread>
#include "uartAPI.h"
#include"data.h"
#include"dataEncapsulateSpliteAPI.h"
#include "encspl.h"

class SerialSendData : public QThread
{
    Q_OBJECT
public:
    SerialSendData();

private:
    void run();
   // void addDataToFrame(void );
public:
    GlobalData *globalData ;                 //全局变量
    EncSpl *encSplData;   //全局变量，调用各分离/打包数据函数

    bool send_data(quint32 fd,quint8* serialBuff,quint8 *dataPackage, quint32 len);//发送数据
    //quint32 openSerialPort();//打开串口
    //bool send_basic(quint32 fd,quint8* serialBuff);//发送基本信息帧

private:
    quint8 crane_inter[50];
//signals:
//    void paintSignal();                           //重绘信号
public slots:
    void sendInfo_afterRequest(quint8 info_type,quint32 info_index);//发送信息帧
    void storeInfo_afterRecv(quint8 info_type,quint32 info_version,quint32 info_index);
    void ResetDevice();

};

#endif // SERIALSENDDATA_H
