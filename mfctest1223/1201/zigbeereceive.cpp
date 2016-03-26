#include "zigbeereceive.h"
#define BUFFER_SIZE 200
//////2014.01.16
quint8 rx_buff[BUFFER_SIZE] ;
quint8 rx_Save_BUF[11][30];//存放无效中间数据的二维数组
quint8 set_BUF[BUFFER_SIZE];//传参传出去的去转义后的完整一帧数据
quint8 crane_inter[18];
quint32 rx_step = 0;//数据接收控制参数
quint32 rx_len = 0;//rx_buff下标控制参数
quint32 set_data_len = 0;//去转义后的一帧数据在去帧尾后的数据长度
quint8 rx_head_received = 0;//串口数据接收到协议的帧头标志
quint32 BUF_index = 0;
bool set_received = false;//一帧数据成功收到标志位
//////////////////////////////////////////////////////////
bool ZigbeeReceive::flagquit = false;
//ZigbeeReceive::ZigbeeReceive(QObject *parent) :
//    QThread(parent)
  ZigbeeReceive::ZigbeeReceive() :
      QThread()
{
    globalData = GlobalData::getGlobalData();
    encSplData = new EncSpl();

    globalData->mStop = false ;
    //qDebug()<<"zigbeeReceive's constructor  mStop is :"<<globalData->mStop;
    //kdDataTab  = new KDdataTab(NULL);
   // connect(kdDataTab,SIGNAL(IDChannelChange()),this,SLOT( zigbeeIDChannelCfg() )  );

}



void ZigbeeReceive::run()
{

     //qDebug()<<"zigbee receive thread is runing *************";
    // qDebug()<<"run()        "<<currentThreadId();
    while(!globalData->mStop)
    {
        qint8 receivedLen;
        quint8 buff [BUFFER_SIZE] ;
        //qDebug()<<"outer loop ***********  "<<globalData->mStop;
             while(set_received == false)
            {
                    //qDebug()<<"iner loop ***********  "<<globalData->mStop;
                   if (globalData->mStop)
                   {
                       qDebug()<<"inner judgement ****";
                       return ;
                   }



                memset ( buff , 0 , BUFFER_SIZE );                         //清零操作
                receivedLen = read ( globalData->ZigbeeFd , buff , 1 );  //读串口,每次只读一个字节以方便处理
                rx_buff[rx_len]= buff[0];//将每次从串口收到的1个字节提取出来
                switch(rx_step)
                  {
                  case 0 :
                        if(rx_buff[rx_len]==0xFE)
                        {
                            rx_step=1;
                        }
                        break;
                  case 1:
                        if(rx_buff[rx_len]==0xFB)
                        {
                              rx_buff[0]=0xFE;
                              rx_buff[1]=0xFB;
                              rx_len=1;
                              rx_step=0;
                              rx_head_received=1;
                        }
                    else if((rx_buff[rx_len]==0xFA)&&(rx_head_received==1))
                    {
                      //接收完成
                      rx_head_received=0;
                      rx_step=0;
                      if(rx_len<21)  //接收到的是RTU之间的通信数据(不安全，自定义信息可有12个字节+兼容数据>15字节）
                      {//接受到的是塔机间的数据
                         // qDebug()<<"$$$$crane_inter";
                          memcpy(crane_inter,rx_buff+2,10);//** rx_buf -> crane_inter**//
                          set_received = true;

                      }
                      else if(rx_len>=21&&
                              ((rx_buff[3]>>3)==0x04||
                              (rx_buff[3]>>3)==0x1E||
                              (rx_buff[3]>>3)==0x1F))  //保证接收到的是配置中心的数据
                      {
                        //接收一帧数据，不包括帧尾，也就是说set_BUF和rx_buff相比，是少了最后的两个帧尾字节
                        memcpy(set_BUF,rx_buff,rx_len-1);//** rx_buf -> set_BUF**//
                        set_received = true;//一帧数据成功收到标志位
                        set_data_len=rx_len-1;     //数据长度不包括帧尾

                      }
                    }
                    else if((rx_buff[rx_len]==0xFE)&&(rx_head_received==1))
                    {
                      rx_len=rx_len-1;
                      rx_step=0;
                    }
                    break;
                  default:
                    break;
                  }
                  rx_len++;
                  if(rx_len>=BUFFER_SIZE)
                    rx_len=0;
            }

          //  qDebug()<<"*************************YO XI***********************";
      /******测试：打印收到的字节数组*************/
//            for(quint32 i = 0; i < rx_len; i++)
//            {
//                //qDebug()<<"rx_buff_"<<i<<":"<<hex<<rx_buff[i]<<"      "<<"set_BUF_"<<i<<":"<<hex<<set_BUF[i];
//                qDebug()<<"rx_buff_"<<i<<":"<<hex<<rx_buff[i];
//            }

      /**************************************************************/

          encSplData->SplCroupInter(rx_buff);//解析塔机间状态信息
          //qDebug()<<"";
          receiveSetData(set_BUF);
          set_received = false;//数据处理完后，将一帧数据接受标志置为假，表示可以继续读串口
          msleep(200);
     }
      //  exec();
//    terminate();
//    while(1)
//        {
//            if(wait())
//                {
//                    flagquit = true;
//                    return ;
//                }
//         }
    return;

}



//////2014.01.16


/*
**************************************************************
函数：dataRecv_DeFE()
描述：收到上位机下发的命令后做出发送相应信息的动作/收到配置参数信息并存入数据库
参数：无
返回：无
日期：2013.12
**************************************************************
  */
void ZigbeeReceive::receiveSetData(quint8 *set_BUF)
{
    bool IMEI_judge = true;   //IMEI号判断标志位
    QString imei;
    quint8 str_imei[16];//imei号
    quint8 CRC[2];
    quint8 frame_type;//帧类型
    quint8 data_info[200];
    quint8 info_type;//信息类型
    quint32 info_index;//保护区，塔群序号
    quint32 info_version;
    bool store_success;
    imei = globalData->basicData.craneIMEI;//从全局变量获得塔吊的IMEI号
    encSplData->imei_to_byteArray(imei,str_imei);//将imei号转换为16字节数组
    //将收到的数据中的imei号字节段与数据库中的imei号对比
    for(quint32 i = 5; i < 20; i++)
    {
        //qDebug()<<"set_BUF["<<i-5<<"]"<<hex<<set_BUF[i];//测试打印从上位机接收到的imei字段
        if( (set_BUF[i]) != str_imei[i-5] )
        {
            IMEI_judge = false;//IMEI号不符合
          //  qDebug()<<"!!!  IMEI numbers don't match!";
            break;
        }
    }

    if(IMEI_judge == true)
    {
        globalData->dataCRC16((char*)set_BUF,set_data_len-2,CRC);//数据CRC校验,set_data_len已经是减去了帧尾的长度，再减2表示减去CRC的2个字节
      /*****************测试：CRC信息打印*****************/
//        qDebug()<<"computing CRC of the receiving data:";
//        qDebug()<<"CRC[0]="<<hex<<CRC[0]<<"CRC[1]="<<CRC[1];
//        qDebug()<<"CRC of the receiving data:";
//        qDebug()<<"set_BUF[CRC_0]="<<hex<<set_BUF[set_data_len-2]<<"set_BUF[CRC_1]="<<set_BUF[set_data_len-1];
      /***********************************/
        if( (set_BUF[set_data_len-2] == CRC[0]) && (set_BUF[set_data_len-1] == CRC[1]) )
        {
            frame_type = (set_BUF[3] >> 3);//通过命令字选择接收
            qDebug()<<"frame_type"<<hex<<frame_type;
            info_type = (set_BUF[21]);//通过判断信息段首字节接收不同类别信息
            info_index = (set_BUF[24]);//保护区序号
            qDebug()<<"info_type"<<hex<<info_type;
            //qDebug()<<"info_index"<<hex<<info_index;
//得到信息传输帧的纯数据段信息，主要用于信息传输帧0x04，第一个字节为信息类型，解析后存入数据库
            //qDebug()<<"~~~~data_info[]:";
            //qDebug()<<"pure data_len = "<<set_data_len-23;
            for (quint32 i=0; i < set_data_len-23; i++)
            {
                data_info[i] = set_BUF[21+i];
                //qDebug()<<i<<":"<<hex<<data_info[i];
            }

            info_version =  (data_info[1]<<8) + data_info[2];
            switch(frame_type)
            {

/**信息请求帧，由上位机向下位机发出，此时，下位机应向上位机发送信息**/
            case 0x1E:
                qDebug()<<"receving request infomation!";
                switch(info_type)
                {
                //基本信息上发
                case 0x01:
                    qDebug()<<"Signal:Recive reqestBasicInfo!";
                    emit reqest_Info(0x01,0x00);// 发送基本信息信号
                    break;
                //保护区信息上发
                case 0x02:
                    qDebug()<<"Signal:Recive requestProtectInfo";
                    emit reqest_Info(0x02,info_index);//发送保护区[info_index]信息信号
                    break;
                //限位区信息上发
                case 0x03:
                    qDebug()<<"Signal:Recive requestLimitInfo";
                    emit reqest_Info(0x03,0x00);//发送限位信息信号
                    break;
                //KD,ip信息上发
                case 0x04:
                    qDebug()<<"Signal:Recive requestK_ipInfo";
                    emit reqest_Info(0x04,0x00);//发送KD,ip信息信号
                    break;
                //塔群信息上发
                case 0x05:
                    qDebug()<<"Signal:Recive requestCraneGroupInfo";
                    emit reqest_Info(0x05,info_index);//发送塔群信息信号
                    break;
                default:
                    break;
                }
                break;
 /***********************************************************************************/
/***信息传输帧，由上位机向下位机发送，下位机解析并存放到数据库，存放成功后向上位机发送回应帧
        由于槽函数不能传递指针参数，只能在此调用解析函数**/
            case 0x04:
                qDebug()<<"receving data transmitting infomation!";
                switch(info_type)
                {
                case 0x01://收到基本信息帧，存到数据库
                    qDebug()<<"Signal:Recive transmit BasicInfo";
                    //由于槽函数不能传递指针参数，只能在此调用解析函数
                    store_success = encSplData->SplBasic(data_info+1);//data_info的第一个字节是信息类型
                    if (store_success)
                        emit transimit_Info(0x01,info_version,0x00);
                    break;
                case 0x02://收到保护区信息帧，存到数据库
                    qDebug()<<"Signal:Recive transmit proZoneInfo";
                    //由于槽函数不能传递指针参数，只能在此调用解析函数
                    info_index =( (data_info[4])& 0x7F) >> 4;//保护区序号
                    store_success = encSplData->SplProZone(data_info+1,info_index);//data_info的第一个字节是信息类型
                    if (store_success)
                        emit transimit_Info(0x02,info_version,info_index);
                    break;
                case 0x03://收到限位信息帧，存到数据库
                    qDebug()<<"Signal:Recive transmit LimitInfo";
                    store_success = encSplData->SplLimit(data_info+1);//data_info的第一个字节是信息类型
                    if (store_success)
                        emit transimit_Info(0x03,info_version,0x00);
                    break;
                case 0x04://收到KD,ip信息帧，存到数据库
                    qDebug()<<"Signal:Recive transmit KD_ip Info";
                    store_success = encSplData->SplKD_ip(data_info+1);//data_info的第一个字节是信息类型
                    if (store_success)
                        emit transimit_Info(0x04,info_version,0x00);
                    break;
                case 0x05://收到塔群信息帧，存到数据库
                    qDebug()<<"Signal:Recive transmit CraneGroup Info";
                    info_index =(data_info[3]);// 塔群序号
                    store_success = encSplData->SplCraneGroup(data_info+1,info_index);//data_info的第一个字节是信息类型
                    if (store_success)
                        emit transimit_Info(0x05,info_version,info_index);
                    break;
                case 0x06://收到力矩曲线信息
                    qDebug()<<"Signal:Recive transmit torque curse Info";
                    info_index =(data_info[1]);//力矩曲线序号
                    //20141113  zigbee 接收力矩曲线
                    int moment_data_len =set_data_len - 24 ;
                    //caution : the cure data,delet{fefb,3,16,1(info_type),---,crc}
                    int  writelen = 0;
                    bool   store_status = false;
                    char tem_data[100];
                    memset(tem_data,0,100);
                    memcpy(tem_data,data_info+1,moment_data_len);

                    switch(info_index)
                    {
                                case 0x01:
                                    file = new QFile;
                                    file->setFileName("momenttable2.txt");
                                    file->open(QIODevice::WriteOnly);
                                    writelen = file->write(tem_data , moment_data_len);
                                    break;

                                case 0x02:
                                    file->setFileName("momenttable4.txt");
                                    file->open(QIODevice::WriteOnly);
                                    writelen = file->write(tem_data , moment_data_len);
                                    break;

                                default:
                                    break;
                    }

                    if(writelen != -1)
                    {
                        file->close();
                        store_status =  true;
                    }
                    if(store_status == true)
                        emit transimit_Info(0x06,info_version,info_index);
                    store_status = FALSE;

                    break;

                    }
                break;




/***********************************************************************************/
/*** 重启设备**/
            case 0x1F:
                emit resetDevice();
                break;

            default:
                break;

            }
            qDebug()<<"$$$$$$$$$$$";

        }
    }
}
//20140903
 void ZigbeeReceive::zigbeeIDChannelCfg()
 {

               globalData->mStop = true;
               qDebug()<<"zigbeeIDChannelCfg()      "<<currentThreadId();
               sleep(2);//waiting for  uper thread exit , flagquit become true
                while(1)
                    {
                         if(!flagquit )
                            {
                                 qDebug()<< "zigbeeReceive is still running    globalData->mStop"<<globalData->mStop;
                                 sleep(5);
                            }
                        else
                            {
                                qDebug()<< "zigbeeReceive is finished  ";
                                zigbeeScreenCfg = new ZigbeeScreenCfg();
                                zigbeeScreenCfg->cfgNetIDChannel();
                                delete zigbeeScreenCfg;
                            }
                      }

 }

 void ZigbeeReceive::stop()
 {
     globalData->mStop = true; //不可空
     terminate();
     if( wait() )
         {
             qDebug()<<" wait(); return true;   return from run()";
             qDebug()<<"stop()      "<<currentThreadId();
         }
     else
         qDebug()<<" waiting  for  run()__return fail *****";
 }

/////////////////////////////////////////////////////////////////////////////////////////////////
