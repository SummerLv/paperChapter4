#ifndef ZIGBEERECEIVE_H
#define ZIGBEERECEIVE_H

#include <QThread>
#include "data.h"
#include "encspl.h"
#include <qdebug.h>
class ZigbeeReceive : public QThread
{
    Q_OBJECT
public:
   // explicit ZigbeeReceive(QObject *parent = 0);
    ZigbeeReceive(QObject *parent);

public:
    void run();

private:
    GlobalData *globalData;
    //2014.01.16
    EncSpl*encSplData;
    //2014.01.16

    void receiveSetData(quint8 *set_BUF);//收到上位机下发的命令后做出发送相应信息的动作/收到配置参数信息并存入数据库
    
signals:
    void resetDevice();                          //重启设备信号
    void reqest_Info(quint8 info_type,quint32 info_index);// 发送信息信号，参数info_type表示发送类型参数,参数info_index表示保护区序号
    void transimit_Info(quint8 info_type,quint32 info_version,quint32 info_index);
};

#endif // ZIGBEERECEIVE_H
