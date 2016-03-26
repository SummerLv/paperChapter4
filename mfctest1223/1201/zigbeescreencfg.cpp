#include "zigbeescreencfg.h"

ZigbeeScreenCfg::ZigbeeScreenCfg()
{
    //sqldb = new SqlDB();
    zigbeeCfg = new ZigbeeCfg();
    qDebug()<<"in zigbeescreencfg.cpp";
}

ZigbeeScreenCfg::~ZigbeeScreenCfg()
{
    delete zigbeeCfg ;
}

 // 保证 存入到数据库中的数据是正确的    channel:0-F   netID:00-FF
void ZigbeeScreenCfg::getNetIDChannel(void)
{
    QSqlTableModel *readkdData = new QSqlTableModel;
    readkdData->setTable("KdData");
    readkdData->select();

    channel = readkdData->record(0).value("ZigbeeChannel").toString().toAscii();
    if(!channel.isEmpty())
        qDebug()<<"channel is        :       "<<channel;
    else
        qDebug()<<"failed to get ***channel value from KadaData_table*";

    netId =  readkdData->record(0).value("ZigbeeID").toString().toAscii();
    if(!netId.isEmpty())
        qDebug()<<"netID is         :         "<<netId;
    else
        qDebug()<<"failed to get ***netID  value from KadaData_table*";

}

 void  ZigbeeScreenCfg::cfgNetIDChannel(void)
 {
    //globalData->mStop = false;

     getNetIDChannel();

     zigbeeCfg->enterCFG();
     sleep(1);
     zigbeeCfg->zigbeeIDCfg(netId);
     zigbeeCfg->zigbeeChannelCfg(channel);
     zigbeeCfg->exitCFG();
 }


