#include "serialSendData.h"

#define BUFFER_SIZE 300
#define DATA_TRANSFER_COM 3
#define MAX_WRITE_NUM 5

quint8 send_data[30] = {0};                    //发送的数据帧
bool isOpen;

quint8 protocol_version = 0x04;//协议版本
quint8 factory_id = 0x08;//厂商代码
QString imei;//imei号

quint8 info_transit = 0x04;//信息传输帧
quint8 info_respond = 0x05;//信息回应帧
quint8 info_request = 0x1E;//信息请求
quint8 info_reset = 0x1F;//重启指令

int crane_inter_len;

SerialSendData::SerialSendData()
{
    globalData = GlobalData::getGlobalData();
    encSplData = new EncSpl();
}

void SerialSendData::run()
{
       // qDebug() << "*********Enter serialSendprocess";
    while(!globalData->mStop)
    {
//串口调试--by cherry
    //int send_data_len = addDataToFrame(SendBuff);          //调用数据打包函数
/**************************************/
        // qDebug() << "*********serialSendprocess    loop   "<<globalData->mStop;
        memset(crane_inter , 0 , 50);
       crane_inter_len  =  encSplData->EncCraneInter(crane_inter);
       write(globalData->ZigbeeFd, crane_inter, sizeof(crane_inter)); ////发送塔机间实时信息
        msleep(1000);
   }
    return;
}
/*
**********************************************
    **********发送基本信息*************
  参数：info_type:信息类型
           info_index：信息序号，主要用于保护区和塔群
  说明：
**********************************************
*/
void SerialSendData::sendInfo_afterRequest(quint8 info_type,quint32 info_index )
{
    quint8 info_buf[250];//信息，只包括信息字段
    quint8 info_send[250];//信息字段+帧信息+CRC+转义+帧头帧尾
    quint32 data_len;//信息字段的长度
    quint32 return_len;//混合信息的长度
    switch(info_type)
    {
/****************发送基本信息****************/
    case 0x01:
        qDebug()<<"************************SLOT:send_basic!";
        data_len = encSplData->EncBasic(info_buf);//拼接基本信息段
        break;
/****************发送保护区信息****************/
    case 0x02:
        qDebug()<<"************************SLOT:send_protectInfo !";
        //qDebug()<<"Sending proZone_"<<info_index<<" information";
        //quint32 proZoneNum = globalData->proZoneDataList.count();//保护区个数;
        data_len = encSplData->EncProZone(info_buf,info_index);//拼接保护区信息段
//        qDebug()<<"the data_len of proZone "<<info_index<<"is "<<data_len;
        break;
/****************发送限位信息****************/
    case 0x03:
        qDebug()<<"************************SLOT:send_LimitInfo !";
        data_len = encSplData->EncLimit(info_buf);//拼接保护区信息段
        break;
/****************发送K值及端口ip信息****************/
    case 0x04:
        qDebug()<<"************************SLOT:send_KD_IP_Info !";
        data_len = encSplData->EncKD_ip(info_buf);//拼接保护区信息段
        break;
/****************发送塔群信息****************/
    case 0x05:
        qDebug()<<"************************SLOT:send_CraneGroupInfo !";
        //qDebug()<<"Sending CraneGroup_"<<info_index<<" information";
        data_len = encSplData->EncCraneGroup(info_buf,info_index);//拼接塔群信息段
        break;
    default:
        break;
    }

    return_len = encSplData->add_frame_CRC_FE(protocol_version,factory_id,
                                              imei,info_buf,data_len,info_send,info_transit);//最后发送出去的数据包长度
/********test info_send[]********************/
//        qDebug()<<"*****test info_send[]******";
//            for(quint32 i = 0;i < return_len; i++)
//            {
//                qDebug()<<i<<":"<<hex<<info_send[i];
//            }
//        qDebug()<<"return_len"<<return_len;
/**********************************************/
    write(globalData->ZigbeeFd, info_send, return_len); //写串口
}

/*
**********************************************
    **********接收从上位机下发的配置信息*************
  参数：info_type:信息类型
           info_index：信息序号，主要用于保护区和塔群
  说明：
**********************************************
*/
void SerialSendData::storeInfo_afterRecv(quint8 info_type,quint32 info_version,quint32 info_index)
{
    quint8 info_buf[250];//信息，只包括信息字段
    quint8 info_send[250];//信息字段+帧信息+CRC+转义+帧头帧尾
    quint32 data_len;//信息字段的长度
    quint32 return_len;//混合信息的长度
    //bool store_success;
    quint32 m =0;
    info_buf[m] = info_type;
    m = m+1;
    quint8 info_version_hex[2];
    encSplData->Double_To_Array((char*)info_version_hex,info_version);
    info_buf[m] = info_version_hex[0];
    m = m+1;
    info_buf[m] = info_version_hex[1];
    m = m+1;
    switch(info_type)
    {
/****************配置基本信息****************/
    case 0x01:
        qDebug()<<"************************SLOT:send respond basic information to database!";

        break;
/****************配置保护区信息****************/
    case 0x02:
        qDebug()<<"************************SLOT:send respond proZone information to database!";
        info_buf[m] = info_index;//加上保护区序号
        m = m+1;
        break;
/****************配置限位信息****************/
    case 0x03:
        qDebug()<<"************************SLOT:send respond Limit information to database!";
        break;
/****************配置KD值和ip信息****************/
    case 0x04:
        qDebug()<<"************************SLOT:send respond KD_ip information to database!";
        break;
/****************配置塔群信息****************/
    case 0x05:
        qDebug()<<"************************SLOT:send respond CraneGroup information to database!";
        info_buf[m] = info_index;//加上保护区序号
        m = m+1;
        break;
/****************配置力矩曲线信息****************/
    case 0x06:
        qDebug()<<"************************SLOT:send respond torque curse information to database!";
        info_buf[m] = info_index;//加上力矩曲线倍数
        qDebug()<<"info_index="<<info_index;
        m = m+1;
        break;
    default:
        break;
    }
    data_len = m;
    return_len = encSplData->add_frame_CRC_FE(protocol_version,factory_id,
                                              imei,info_buf,data_len,info_send,info_respond);
    /********test info_send[]********************/
//            qDebug()<<"*****test info_send[]******";
//                for(quint32 i = 0;i < return_len; i++)
//                {
//                    qDebug()<<i<<":"<<hex<<info_send[i];
//                }
//            qDebug()<<"return_len"<<return_len;
    /**********************************************/
    write(globalData->ZigbeeFd, info_send, return_len); //写串口
}

/*
**********************************************
    **********重启设备*************
  参数：无
  说明：
**********************************************
*/
void SerialSendData::ResetDevice()
{
    quint8 info_buf[10] ;
    quint8 info_send[250];
    quint32 data_len = 4;
    quint32 return_len;
    //globalData->globalDataInit();
    return_len = encSplData->add_frame_CRC_FE(protocol_version,factory_id,
                                              imei,info_buf,data_len,info_send,info_reset);
    write(globalData->ZigbeeFd, info_send, return_len); //写串口
    system("./Kill_and_Restart.sh");
}




/**********************************************/
/**********将打包好的数据从串口发送出去*************/
/*参数： dataPackage[]数据包
              len：dataPackage[]的长度
              serialBuff[]: 串口缓冲区
    说明：调用此函数通常需要修改的参数为：
                    dataPackage，代表各种信息的发送数组，比如基本信息发送数组
                    len，代表dataPackage的长度
*/
/**********************************************/
bool SerialSendData::send_data(quint32 fd,quint8* serialBuff,quint8 *dataPackage, quint32 len)
{
//    qDebug()<<"isOpen="<<isOpen;
//    if(!isOpen)
//        return false;
    memset ( serialBuff , 0 , BUFFER_SIZE ); //清零操作
    write(globalData->ZigbeeFd, dataPackage, len); //写串口
    //qDebug()<<"*******Sending basic data.******** ";
    qDebug()<<"data length ="<<len;
    return true;
}
/*
**********************************************
    *******打开串口并设置参数*************
  参数：无
**********************************************
*/
//quint32 SerialSendData::openSerialPort()
//{
//    quint32 fd;
//    if ( ( fd = openPort( DATA_TRANSFER_COM) ) < 0 ) {      //打开串口
//        qDebug() << "******Open failed";
//        qDebug()<<"***************************************";
//    }
//    else
//    {
//        qDebug()<<"*****Open port success while sending data!";
//        qDebug()<<"fd="<<fd;
//        qDebug()<<"***************************************";
//    }
//    if (setComConfig( fd , 38400 , 8 , 'N' , 1 ) < 0 ){
//        qDebug() << "*****Set COM failed";
//        qDebug()<<"***************************************";
//    }
//    else
//    {
//        qDebug()<<"*****Set COM success while sending data!";
//        qDebug()<<"***************************************";
//    }
//    return fd;

//}
