//gprs链接和调试函数
#include "gprs.h"
#include"uartAPI.h"

#define DATA_TRANSFER_COM_0 0
#define DATA_TRANSFER_COM_1 1
#define DATA_TRANSFER_COM_2 2
#define DATA_TRANSFER_COM_3 3
#define DATA_TRANSFER_COM_4 4
#define DATA_TRANSFER_COM_5 5
#define DATA_TRANSFER_COM_6 6

#define basic_info_prio 1//基本信息上发优先级
#define fpv_info_prio 2//指纹版本上发优先级
#define fpq_info_prio 4//指纹数据请求上发优先级
#define warn_info_prio 8//报警信息上发优先级
#define cycle_info_prio 16//工作循环上发优先级
#define fp_info_prio 32 //指纹验证上发优先级
#define fault_info_prio 64//故障上发优先级
#define illegal_info_prio 128//违章上发优先级
#define illegal_fp_prio 256//
#define status_info_prio 512//状态上发优先级
#define priority_init basic_info_prio+fpv_info_prio+status_info_prio//初始时需要将基本信息和指纹版本请求都上发

#define BUFFER_SIZE 200
#define tem_data_receive_maxlen 400

#define ZIGBEE_WAY 0x03
#define GPRS_WAY  0x01

const double Weight_Gap_High = 0.4;
const double Weight_Gap_Low = 0.2;
static char center_time[4];//从上位机下载的时间
static char center_receive_ID[3];
quint8 mes_trans_flag = 0;

//static quint8 DATA_SEND_SUCCESS = 0x00;
quint8 FINGER_PARTION_SEND = 0x00;

quint8 Connect_Success;
qint8 time_step = 0;

quint8 Finger_Store_Start_Flag = 0;
quint8 checkCRC[2];


static char imei_number[16];
char tem_data_receive[500];
char tem_data_receive_fromUpper[500];

char set_buf[250];//暂存指纹数据

char temp_finger_data[4][139] = {{0xef,0x01 ,0xff ,0xff ,0xff ,0xff ,0x02 ,0x00 ,0x82 ,0x03 ,
                                0x01,0x4e,0x1c,0x00,0x00,0xff,0xfe,0xff,0x0e,0xe0,
                                0x06,0xc0,0x02,0x80,0x02,0x80,0x02,0x80,0x02,0x80,
                                0x02,0x00,0x02,0x00,0x02,0x00,0x02,0x00,0x00,0x00,
                                0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x00,
                                0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                                0x00,0x00,0x00,0x00,0x00,0x21,0x0e,0x6b,0xfe,0x17,
                                0x92,0x56,0x9e,0x19,0x97,0x56,0x5e,0x26,0x9b,0x97,
                                0x1e,0x5f,0x1d,0x9b,0x1e,0x3b,0x2b,0xd6,0xbe,0x1c,
                                0x2e,0x14,0xbe,0x26,0x37,0xd2,0x9e,0x53,0x38,0x9a,
                                0xbe,0x49,0xc2,0x1c,0x5e,0x26,0x15,0x01,0xbf,0x6b,
                                0x96,0x45,0x37,0x4a,0x28,0x58,0xbf,0x33,0x31,0xd5,
                                0xff,0x36,0xb6,0xd5,0xdf,0x37,0x3e,0x14,0x1f,0x43,
                                0xbe,0xc4,0xb7,0x12,0x35,0x69,0xbc,0x00,0x00},
                                 {0xef,0x01 ,0xff ,0xff ,0xff ,0xff ,0x02 ,0x00 ,0x82 ,0x0e,
                                  0xb1,0xd1,0x75,0x54,0x23,0xda,0x9a,0x5e,0x38,0x9c,
                                  0xfb,0x6d,0x40,0x60,0xd3,0x66,0x43,0x59,0xbb,0x4f,
                                  0x24,0x59,0x99,0x53,0x0f,0x5c,0x75,0x2b,0x28,0x96,
                                  0x8c,0x44,0x11,0x9b,0x8c,0x3a,0x8f,0x5a,0xc6,0x00,
                                  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                                  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                                  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                                  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                                  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                                  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                                  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                                  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                                  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
                                 {0xef,0x01 ,0xff ,0xff ,0xff ,0xff ,0x02 ,0x00 ,0x82 ,0x03,
                                  0x01,0x3f,0x20,0x00,0x00,0xff,0xfe,0xff,0xfe,0xfe,
                                  0x7e,0xf8,0x3e,0xe0,0x0e,0xe0,0x06,0x80,0x06,0x80,
                                  0x02,0x80,0x02,0x80,0x02,0x80,0x02,0x00,0x00,0x00,
                                  0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x02,0x00,
                                  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                                  0x00,0x00,0x00,0x00,0x00,0x17,0x22,0xd6,0x96,0x2b,
                                  0xa6,0x96,0xfe,0x21,0xab,0x55,0xbe,0x74,0xb0,0xc8,
                                  0x9e,0x0a,0x32,0xa9,0x7e,0x14,0xb4,0x6a,0x7e,0x08,
                                  0x37,0x40,0xbe,0x46,0xba,0x9b,0x7e,0x65,0x9e,0xc6,
                                  0x7f,0x27,0xb1,0x55,0xdf,0x33,0xbf,0x85,0x3f,0x6f,
                                  0xc1,0x4d,0xb7,0x44,0x41,0x83,0x3f,0x5c,0xa3,0x46,
                                  0xf4,0x42,0xb3,0x1a,0xbc,0x3b,0x23,0x59,0x1d,0x44,
                                  0x9a,0x5a,0xba,0x44,0x1e,0x9a,0xfa,0x00,0x00},
                                 {0xef,0x01 ,0xff ,0xff ,0xff ,0xff ,0x08 ,0x00 ,0x82 ,0x3d,
                                  0x9f,0x9a,0x1a,0x4b,0x3e,0x40,0xba, 0x66,0xc0,0xcf,
                                  0xda,0x3c,0x12,0x84,0xf8,0x48,0x15,0x1a,0x98,0x38,
                                  0x93,0x9b,0x19,0x60,0x25,0xc7,0xd3,0x58,0xc3,0x57,
                                  0x16,0x22,0xbf,0x09,0x15,0x5f,0x41,0xd3,0x55,0x4d,
                                  0x96,0x9d,0xb3,0x55,0x37,0x1e,0xaf,0x27,0xbd,0xc6,
                                  0xaf,0x4c,0x9a,0x9f,0xa9,0x00,0x00,0x00,0x00,0x00,
                                  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                                  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                                  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                                  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                                  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                                  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                                  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}};

quint16 partion_num = 0;

quint8 check_bit = 0;
//char sendData_dd[16] = {0xef , 0x01 ,0xfe ,0xfe , 0xfe ,0xfe ,0x01 ,0x00 ,0x07 ,0x20 ,0x00 ,
//                        0x00 ,0x00 ,0xff ,0xfe, 0xfe};

quint32 receive_data_byte = 0;
quint32 frame_data_len = 0;
//quint32 set_data_len = 0;
quint8 center_receive = 0;//  if the center received the data

quint8 test_driver_ID[4] = {0};
quint8 test_driver_ID2[4] = {0x00,0x00,0x00,0x0d};

quint32 tem_data_receive_len = 0;
quint32 tem_data_receive_len_fromUpper = 0;
quint32 crc_sum = 0;
quint8 crc_char[2];

quint8 tem_data_head_receive = 0;

//quint8 identity_success = 0;
QString ipaddr1 = "125.65.110.15";//中心平台地址
//QString ipaddr1 = "222.197.174.82";//中心平台地址
int port1 = 1100;

QString ipaddr2 ;
int port2 ;


/****************GPRS数据连接和发送********/
GprsConnect *GprsConnect::m_GprsConnect = NULL;
GprsConnect::GprsConnect(QObject *parent) :
    QThread(parent),
    info_send_prio(0),Insert_Or_Change(2),
    I_OR_C_Num(11),Finger_Partition_number(0),
    Finger_number(0),Upper_Or_Center(false),
    Receive_IP_Sucess(false),DATA_SEND_SUCCESS(0x00),
    Config_Info_Flag(0x1f),stable1(0),stable2(0),
    Finger_Set_Data(new QSqlTableModel),
    timer(new QTimer),udpSocket(new QUdpSocket),
    udpSocket_Upper(new QUdpSocket),
    tcppicsender(new QTcpSocket),fingergather(new Finger_Gather),
    work_cycle(new WorkCycle),encspl(new EncSpl)
{
    globalData = GlobalData::getGlobalData();

    globalData->identity_success = 0;
    globalData->status_success = 0;

    work_cycle->begin_record = 0;
    work_cycle->end_record = 0;

    Connect_Success = 0;

    /****读取数据库中的IP*****/
    QSqlQueryModel readIP;
    readIP.setQuery("SELECT * FROM kddata WHERE id = 0");
    Upper_IP = readIP.record(0).value("serverIP").toString();
    Upper_Port = readIP.record(0).value("serverPort").toInt();
    ipaddr2 = Upper_IP;
    port2 = Upper_Port;
   // qDebug()<<"Upper_IP"<<ipaddr2;
  //  qDebug()<<"Upper_Port"<<port2;

    Driver_ID_Address_Data driver_id_address_data;
    driver_id_address_data.DriverID = 0;
    driver_id_address_data.FingerAddress = 0;
    deletedata.clear();
    changedata.clear();
    insertdata.clear();

    for(quint8 i = 0;i<10;i++)
    {
        deletedata.append(driver_id_address_data);
        changedata.append(driver_id_address_data);
        insertdata.append(driver_id_address_data);
    }

    read_IMEI();

    if (time_step == 1)
    {
  //  connectGprs();
    if ((FALSE == udpSocket ->bind( 44444)) || (FALSE == udpSocket_Upper->bind(33445)) )
       qDebug()<<"bind failed";
    else{
        //qDebug()<< "bind "<<udpSocket->localPort();
        //qDebug()<< "local IP"<< udpSocket->localAddress();
    }
    tcppicsender->connectToHost(QHostAddress(ipaddr2), port2);
    connect(timer,SIGNAL(timeout()),this,SLOT(sendData()));//UDP SOCKET发送
    connect(udpSocket,SIGNAL(readyRead()),this,SLOT(receiveData()));
    connect(udpSocket_Upper,SIGNAL(readyRead()),this,SLOT(receiveData_fromUpper()));
    //connect(this,SIGNAL(finger_change_insert()),this,SLOT(send_finger_syn_inquiry(quint32));
    }
}

Driver_ID_Address_Data::Driver_ID_Address_Data()
{

}
Driver_ID_Address_Data::~Driver_ID_Address_Data()
{

}

WorkCycle::WorkCycle()
{

}

void GprsConnect::test_display(char *set_buff)
{
    for(quint8 i = 0;i<139;i++)
    {
    qDebug()<<"set_buf"<<hex<<(quint8)*set_buff;
    set_buff++;
    }
}

/*******启动************/
void GprsConnect::run()
{

        //qDebug()<<"enter run";
        timer->start(5000);

//        for(quint8 j = 0;j<4;j++)
//     {  crc_sum = 0;
//        for (quint8 i = 6;i<137;i++)
//        {
//            crc_sum = crc_sum + temp_finger_data[j][i];
//        }
//        Double_To_Array(crc_char , crc_sum);
//        temp_finger_data[j][137] = crc_char[0];
//        temp_finger_data[j][138] = crc_char[1];
//        qDebug()<<"crc_char0"<<j<<hex<<crc_char[0];
//        qDebug()<<"crc_char1"<<j<<hex<<crc_char[1];
//     }
//            do
//        {
//            memset(set_buf , 0 , 150);
//            for (quint8 i = 0;i<139;i++)
//            {
//                set_buf[i] = temp_finger_data[partion_num][i];
//                //qDebug()<<"set_buf"<<hex<<(quint8)set_buf[i];
//            }
//               // test_display(set_buf);
//            if (partion_num == 0)
//            {
//                Finger_Store_Start_Flag = fingergather->FingerData_Store_InModule(set_buf , Finger_Store_Start_Flag);
//            }
//            else
//            {
//                fingergather->FingerData_Store_InModule(set_buf , partion_num);
//            }
//             partion_num += 1;
//            if (partion_num == 4)
//            {
//                DATA_SEND_SUCCESS = DATA_SEND_SUCCESS | 0x01;
//            }
//        }while(DATA_SEND_SUCCESS == 0x00);

            while(1)
    {
//       /*********test code***************/
//       if (globalData->identity_success == 1)
//       {
//       msleep(2000);
//       qDebug()<<"************STATUS************";
//       send_status_data();
//       qDebug()<<"test code1*******";
//       msleep(2000);
//       send_FingerId_to_center(test_driver_ID , 0x02);
//       qDebug()<<"test code2*******";
//       msleep(2000);
//       send_FingerId_to_center(test_driver_ID2 , 0x01);
//       }
                switch ( tcppicsender->state() ){
                 case 0://unconnectedstate
                    globalData->sendphotoavailable = false;
                    break;
                case 3://connectedstate
                    globalData->sendphotoavailable = true;
                    break;
                default:
                    break;
            }


         if (globalData->identity_success == 1)
        {
             send_message_to_center();
            // qDebug()<<"info_change_flag"<<info_change_flag[0];
         }
     }
 }

/***********向上位机发送数据****************/
void GprsConnect::sendData()
{


    //qDebug()<<system_time.currentDateTime().toString("yy")+ system_time.currentDateTime().toString("M")+system_time.currentDateTime().toString("d")+
    //          system_time.currentDateTime().toString("h")+system_time.currentDateTime().toString("m")+system_time.currentDateTime().toString("s");
         quint8 imei_str[16] = {0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x00};

        // if (Connect_Success == 1 )

                  //qDebug()<<"************IDENTITY************";
                  send_Id_to_center((char*)imei_str);
//}
}

/**********将本地的图片发送至中心机*********/
int GprsConnect::SendPhoto(){
    long len;
//    QBuffer buffer;
   // QByteArray data;

//    if( QPixmap("123.png").save(&buffer , "PNG") ){

//    QDataStream stream(&data , QIODevice::WriteOnly);
//    stream.setVersion(QDataStream::Qt_4_7);
//    stream<<(quint32)buffer.data().size();
//    data.append(buffer.data());
//    cout<<"send a image, size = "<<data.size();
//    tcppicsender->write(data);

//    data.resize(0);
//    return 1;
//    }
//    else{
//        qDebug()<<"Photo not exist";
//        return -1;
//    }
 //   QFile file("/sdcard/123.png");
    QFile file("/sdcard/test.h264");
    file.open(QIODevice::ReadOnly);
    QByteArray by;
    by = file.readAll();
    len = tcppicsender->write(by);
   tcppicsender->flush();

}

/***********接收中心机发送的回应帧*********/
void GprsConnect::receiveData()
{
    //qDebug()<<"enter receiveData";
    memset(tem_data_receive , 0 , 500);
    while(udpSocket->hasPendingDatagrams())//有数据返回时，进入循环
    {
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());//留出足够大小空间接收数据
        udpSocket ->readDatagram(datagram.data(),datagram.size());//读收到的数据和大小
      //  udpSocket ->readDatagram(tem_data_receive,500);
    //    qDebug()<<sizeof(tem_data_receive);
       //qDebug()<<"*******RECEDATA*******"<<datagram.size();
       qDebug()<<datagram.toHex().data();

       QString string(datagram.toHex().data());
        analyze_Data(string , datagram , tem_data_receive , tem_data_receive_len);
    }
}

/*****接收上位机发送的数据*********/
void GprsConnect::receiveData_fromUpper(){
    memset(tem_data_receive_fromUpper , 0 , 500);
    while(udpSocket_Upper->hasPendingDatagrams())//有数据返回时，进入循环
    {
        Upper_Or_Center = TRUE;
            //qDebug()<<"enter receiveData_fromUpper";
        QByteArray datagram;
        datagram.resize(udpSocket_Upper->pendingDatagramSize());//留出足够大小空间接收数据
        udpSocket_Upper ->readDatagram(datagram.data(),datagram.size());//读收到的数据和大小
       qDebug()<<datagram.toHex().data();
      // qDebug()<<"receive len"<<datagram.size();

       QString string(datagram.toHex().data());
        analyze_Data(string , datagram , tem_data_receive_fromUpper , tem_data_receive_len_fromUpper);
    }
}

/********解析接收的数据*********/
int GprsConnect::analyze_Data(QString string , QByteArray data, char *tem_data_receive , quint32 tem_data_receive_len){
    rx.setPatternSyntax(QRegExp::RegExp);
    rx.setPattern(QString("^fefb\\w*fefa$"));

    if(center_receive == 0)
    {
    if(rx.indexIn(string) != -1)
    {
        if(Upper_Or_Center == false){
         for(int i = 2;i<data.size()-2;i++)
        {
             tem_data_receive[i-2] = data.data()[i];//将接收的数据暂存在数组中 included CRC_data
         }
             tem_data_receive_len = data.size()-4;
        }
        else{
            for(int i = 0;i<data.size()-2;i++)
           {
                tem_data_receive[i] = data.data()[i];//将接收的数据暂存在数组中
            }
                tem_data_receive_len = data.size()-2;//去帧尾S
//                     for (int i = 0;i < tem_data_receive_len;i++){
//                         qDebug()<<hex<<(quint8)tem_data_receive[i];
//                     }
        }
    }
    else{
        qDebug()<<"run here";
        return -1;
    }
     //    qDebug()<<"receive len"<<tem_data_receive_len;

     tem_data_receive_len = delete_fe(tem_data_receive , tem_data_receive_len);
     center_receive = 1;
    }

 if (center_receive == 1)
 {
     center_data_proc(tem_data_receive , tem_data_receive_len);
     center_receive = 0;
 }
 return 0;
}

/***********去转义**************/
int GprsConnect::delete_fe(char *receive_data , quint32 len)
{
   // int len = len1
    for(quint32 i = 0;i<= len-1;++i)
    {
        if(i<len-1){
            if (receive_data[i] == 0xfe)
            {
                if(receive_data[i+1] == 0xfe)
                {
                    for(quint32 j = i ; j<=len-1;j++)
                    {
                        receive_data[j] = receive_data[j+1];
                    }
                  len = len -1;
                }
             }
        }
    }
    return len;
}

/********收到回应数据后的处理*******/
void GprsConnect::center_data_proc( char *tem_data_receive , quint32 tem_data_receive_len)
{
    quint8 temp[2];
    char tem_data[200];
    qint32 data_len = tem_data_receive_len;
        globalData->dataCRC16( tem_data_receive , data_len-2 , checkCRC);

       // qDebug()<<"******PRO*******";
        if (Upper_Or_Center == true){
             Upper_Or_Center = false;
//  qDebug()<<"center data len"<<data_len-2;
//            for (int i = 0;i < tem_data_receive_len;i++){
//                qDebug()<<hex<<(quint8)tem_data_receive[i];
//            }
            for(int i = 2 ; i<data_len;i++){
                tem_data[i-2] = tem_data_receive[i];//去帧头
            }
            for(int i = 0;i<data_len-2;i++){
                tem_data_receive[i] = tem_data[i];
            }
            temp[0] = checkCRC[0];
            temp[1] = checkCRC[1];
            //qDebug()<<hex<<checkCRC[0];
            //qDebug()<<hex<<checkCRC[1];
        }

        else{
        // qDebug()<<hex<<(quint8)temp[0];
        // qDebug()<<hex<<(quint8)temp[1];
        temp[0] = checkCRC[1];
        temp[1] = checkCRC[0];
        }

        if ((tem_data_receive[data_len-1] == temp[1]) && (tem_data_receive[data_len-2] == temp[0]))
        {
           // qDebug()<<"******PRO*******";
            char frame_type = tem_data_receive[1]>>3;//大类帧的类型
            char order_Num = tem_data_receive[6];//传输帧具体类型
            char Crane_Index = tem_data_receive[22];
            char finger_respon = tem_data_receive[6];//指纹回应帧类型，0x01为指纹请求，0x02为指纹同步
            char rec_style = tem_data_receive[7]>>6;//指纹类型，是指纹还是射频
            //上位机的发送帧段
            char order_Num_Upper = tem_data_receive[19];//上位机
            char info_Index_Upper = tem_data_receive[22];//保护区序号
            char info_Version_Upper = (tem_data_receive[20]<<8) + tem_data_receive[21];//信息版本
            qDebug()<<"******frame type*******"<<hex<<(quint8)frame_type;
            switch (frame_type)
            {
            case 0x01://身份验证回应帧
            {
                char CRC_result = tem_data_receive[19];
                //qDebug()<<"crc result"<<(quint8)CRC_result;
                switch (CRC_result)
                {
                case 0x00://验证成功后，将标志位identity_success置1
                    qDebug()<<"******PRO*******";

                    memset (center_receive_ID , 0 , 3);
                    memcpy (center_receive_ID ,tem_data_receive+20, 3);
                    memcpy(center_time , tem_data_receive+23 ,4);
//                    for(quint8 i = 0;i<4;i++)
//                    {
//                        qDebug()<<"receive time"<<hex<<center_time[i];
//                    }
                    time_synchron(center_time);
                    globalData->identity_success = 1;
                    timer->stop();
                    break;
                case 0x01:
                    break;
                default:
                    break;
                }      
          }
                 break;

            case 0x03://状态回应帧
                globalData->status_success = 1;
                qDebug()<<"*****status send success****";
                break;

            case 0x04://信息传输帧，为上位机下发的配置信息
            {
                qDebug()<<"Trans message from upper";
                bool store_status = FALSE;//储存状态，不为0则已经存储成功
                switch (order_Num_Upper){
                case 0x01:
                    store_status =  encspl->SplBasic((quint8 *)tem_data_receive+20);
                 if(store_status == true)
                     send_request_to_upper(0x01,info_Version_Upper,0x00);
                 store_status = FALSE;
                    break;
                case 0x02:
                    info_Index_Upper =( (tem_data_receive[23])& 0x7F) >> 4;//保护区序号
                    store_status =  encspl->SplProZone((quint8 *)tem_data_receive+20 , info_Index_Upper);
                    if(store_status == true)
                        send_request_to_upper(0x02,info_Version_Upper,info_Index_Upper);
                    store_status = FALSE;
                       break;
                case 0x03:
                    store_status =  encspl->SplLimit((quint8 *)tem_data_receive+20);
                    if(store_status == true)
                        send_request_to_upper(0x03,info_Version_Upper,0x00);
                    store_status = FALSE;
                    break;
                case 0x04:
                    store_status =  encspl->SplKD_ip((quint8 *)tem_data_receive+20);
                    if(store_status == true)
                        send_request_to_upper(0x04,info_Version_Upper,0x00);
                    store_status = FALSE;
                    break;
                case 0x05:
                    info_Index_Upper =tem_data_receive[22];// 塔群序号
                    store_status =  encspl->SplCraneGroup((quint8 *)tem_data_receive+20 , info_Index_Upper);
                    if(store_status == true)
                        send_request_to_upper(0x05,info_Version_Upper,info_Index_Upper);
                    store_status = FALSE;
                    break;
                case 0x06://力矩表--------no  testing
                {
                    info_Index_Upper =tem_data_receive[20];
                    memset(tem_data , 0 , 200);
                    for(int i = 20 ; i<data_len-2;i++){
                        tem_data[i-20] = tem_data_receive[i];
                    }
                    char moment_data_len = data_len - 24;
                    int writelen;
                    qDebug()<<"index"<<hex<<(quint8)info_Index_Upper;

                    switch(info_Index_Upper){
                    case 0x01:
                    file->setFileName("momenttable2.txt");
                    file->open(QIODevice::WriteOnly);
                    file->write(&moment_data_len,1);
                   writelen = file->write(tem_data , moment_data_len);
                    break;
                    case 0x02:
                    file->setFileName("momenttable4.txt");
                    file->open(QIODevice::WriteOnly);
                    file->write(&moment_data_len,1);
                   writelen = file->write(tem_data , moment_data_len);
                        break;
                    default:
                        break;
                    }
                    if(writelen != -1){
                        file->close();
                        store_status =  true;
                    }
                    if(store_status == true)
                        send_request_to_upper(0x06,info_Version_Upper,info_Index_Upper);
                    store_status = false ;
                }
                    break;
                default:
                    break;
                }
            }
                break;

            case 0x05://信息传输回应帧
                qDebug()<<"****Enter Mess_Trans****";
            switch (order_Num)
            {
            case 0x01:

                Config_Info_Flag = (Config_Info_Flag & 0x1e);
               // Config_Info_Flag = (Config_Info_Flag & 0x0d);
                break;
            case 0x02:
                Config_Info_Flag = (Config_Info_Flag & 0x1d);
                break;
            case 0x03:
                Config_Info_Flag = (Config_Info_Flag & 0x1b);
                //Config_Info_Flag = (Config_Info_Flag & 0x0d);
                break;
            default:
                break;
            }
                DATA_SEND_SUCCESS = DATA_SEND_SUCCESS | 0x01;
                qDebug()<<"****basreportseccuss";
                break;

            case 0x09://指纹信息回应帧
                switch(finger_respon)
                {
                case 0x01://指纹请求回应
                    switch(rec_style)
                    {
                    case 0x01://指纹
                    {
                        //j为司机个数，i为司机id和版本
                        for(quint8 j =0;j<((tem_data_receive_len-10)/6);j++)
                        {
                        for(quint8 i =0;i<6;++i)
                        {
                            DriverID_Version[j][i] = tem_data_receive[j*6+i+8];
                        }
                        }
                        Driver_Num = (tem_data_receive[7] & 0x0F);
                        qDebug()<<"driver num ++++++++"<<Driver_Num;
                        //msleep(1000);
                        Config_Info_Flag = (Config_Info_Flag & 0x17);
//                      //  emit finger_data_receive();
                    }
                        break;
                    case 0x02://射频
                        break;
                    default:
                        break;
                    }
                    break;
                case 0x02://指纹同步回应
                    switch(rec_style)
                    {
                    case 0x01://指纹
                    {
                        qDebug()<<"*******FINGER_PARTION_SEND1******"<<FINGER_PARTION_SEND;
                        switch(FINGER_PARTION_SEND)
                        {
                        case 0x00:
                            memset(set_buf,0,250);
                            for(quint8 i =12;i<tem_data_receive_len-2;i++)
                            {
                                set_buf[i-12] = tem_data_receive[i];
                            }
                            /*******测试******/
//                             Finger_Store_Start_Flag = fingergather->FingerData_Store_InModule(set_buf , Finger_Store_Start_Flag , test_num*2+Finger_number);
                           /*******测试******/
                            if (Insert_Or_Change == 0)//需要发送插入请求
                        {
                                if(I_OR_C_Num < 11)
                                {
                                    Finger_Store_Start_Flag = fingergather->FingerData_Store_InModule(set_buf , Finger_Store_Start_Flag , insertdata.at(I_OR_C_Num).FingerAddress+Finger_number);
                                }
                        }
                            else if(Insert_Or_Change == 1)//需要发送改变请求
                            {
                                if(I_OR_C_Num < 11)
                                {
                                    Finger_Store_Start_Flag = fingergather->FingerData_Store_InModule(set_buf , Finger_Store_Start_Flag , changedata.at(I_OR_C_Num).FingerAddress+Finger_number);
                                }
                            }
                            //qDebug()<<"***saveaddressdata****"<<(insertdata.at(I_OR_C_Num).FingerAddress+Finger_number);
                          if((tem_data_receive[7] == 0x40) | (tem_data_receive[7] == 0x50))//为40说明发送的是0号第0片指纹
                        {
                            FINGER_PARTION_SEND = FINGER_PARTION_SEND | 0x01;
                            Finger_Partition_number = 0x01;
                         }
                            break;
                        case 0x01:
                            memset(set_buf,0,250);
                            for(quint8 i =12;i<tem_data_receive_len-2;++i)
                            {
                                set_buf[i-12] = tem_data_receive[i];
                            }
                            fingergather->FingerData_Store_InModule(set_buf , Finger_Partition_number);
                            if((tem_data_receive[7] == 0x44) | (tem_data_receive[7] == 0x54))//为44说明发送的是第一片指纹
                            {
                                 FINGER_PARTION_SEND = FINGER_PARTION_SEND | 0x02;
                                 Finger_Partition_number = 0x02;
                            }
                            break;
                        case 0x03:
                            memset(set_buf,0,250);
                            for(quint8 i =12;i<tem_data_receive_len-2;++i)
                            {
                                set_buf[i-12] = tem_data_receive[i];
                            }
                            fingergather->FingerData_Store_InModule(set_buf, Finger_Partition_number);
                            if((tem_data_receive[7] == 0x48) | (tem_data_receive[7] == 0x58))//为48说明发送的是第er片指纹
                            {
                                FINGER_PARTION_SEND = FINGER_PARTION_SEND | 0x04;
                                Finger_Partition_number = 0x03;
                            }
                            break;
                        case 0x07:
                            memset(set_buf,0,250);
                            for(quint8 i =12;i<tem_data_receive_len-2;++i)
                            {
                                set_buf[i-12] = tem_data_receive[i];
                            }
                            fingergather->FingerData_Store_InModule(set_buf, Finger_Partition_number);
                            if(((tem_data_receive[7] == 0x4c) | (tem_data_receive[7] == 0x5c)) && (Finger_number == 0x00))//为4c说明发送的是第san片指纹
                            {  
                                Finger_Partition_number = 0x00;
                                Finger_number = 0x01;
                                FINGER_PARTION_SEND = FINGER_PARTION_SEND & 0x00;
                                Finger_Store_Start_Flag = 0;
                                qDebug()<<"**********goal***********";
                            }
                            else if(((tem_data_receive[7] == 0x4c) | (tem_data_receive[7] == 0x5c)) && (Finger_number == 0x01))
                            {
                                 FINGER_PARTION_SEND = FINGER_PARTION_SEND & 0x00;
                                Finger_Store_Start_Flag = 0;
                                Finger_Partition_number = 0x00;
                                Finger_number = 0x00;
                                if (tem_data_receive[7] == 0x5c)
                                {
                                    Insert_Or_Change = 2;
                                    I_OR_C_Num = 11;
                                    Config_Info_Flag = (Config_Info_Flag & 0x0f);
                                    qDebug()<<"**********goal***********";
                                }
                            }
                            break;
                        default:
                            break;
                        }
                    }
                    break;
                    case 0x02://射频
                        break;
                    default:
                        break;
                    }
                    break;
                default:
                    break;
                }
                break;
            case 0x14:
                Receive_IP_Sucess = true;
                break;
            case 0x1e://读取数据请求帧
             //   qDebug()<<"****Enter Mess_Trans_Upper****";
                switch(order_Num_Upper)
                {
                case 0x01:
                    Upper_Or_Center = true;
                    send_basic_data();
                    break;
                case 0x02:
                    send_Protect_Message(Crane_Index);
                    break;
                case 0x03:
                    send_Limit_Data();
                    break;
                case 0x04:
                    send_KD_Data();
                    break;
                case 0x05:
                    send_CraneGroup_Data(Crane_Index);
                    break;
                default:
                    break;
                }
                break;
            case 0x1f://重启
            {
                for(int i = 3;i>0;i--){
                    send_Reset_Data();
                    msleep(400);
                }
                GprsConnect *gprs = GprsConnect::Instance();
                gprs->exit();
                 system("./Kill_and_Restart.sh");
            }
                break;
            default:
                break;
            }
        }
}

/*************************************
数据发送流程：前提：调用脚本连接网络已经成功
先发送指纹版本对比请求（只发一次，如果和数据库中的不一致则从上位机下载数据）——>发送信息传输帧（只发一次)
*************************************/
void GprsConnect::send_message_to_center()
{


    //qDebug()<<"enter";
        send_mes_trans_data();//信息传输信息发送，为开机时必须发送的


         send_finger_version_inquiry();//指纹版本请求发送，为开机时必须发送的
         msleep(500);


        FingerData_Store_InDataBase();
        msleep(500);
        if(Config_Info_Flag == 0x00)
            DATA_SEND_SUCCESS =DATA_SEND_SUCCESS | 0x02;

        if((DATA_SEND_SUCCESS == 0x03) && (Config_Info_Flag == 0x00))//表示指纹请求和信息传输帧已发送成功
 {
    if(globalData->fingerPage[0] < 0x0f)//如果收到指纹打卡，就上发身份报告
    {
        report_identity_to_center();
    }

    if(Receive_IP_Sucess == false){//如果没收到已经解析我IP端口的回应帧就发送数据
        qDebug()<<"send ip to upper";
        send_IP_Data_ToUpper();
        msleep(2000);
    }
         warn_dispose();
         qDebug()<<"************STATUS************";
         send_status_data();
         msleep(1000);
         get_work_cycle_data();
  }
}


/************************************/
bool  GprsConnect::Strcomp_bool(char str1[],const char *str2)
{
    if(strstr(str1,str2))
        return TRUE;
    else
        return FALSE;
}


/*******调用脚本拨号****/
inline void GprsConnect::connectGprs()
{
    QString command = tr("pppd call wcdma");
    char *S;
    S = command.toAscii().data();
    system(S);
    Connect_Success = 1 ;
    qDebug()<< "Connect Success";

}

/*******读取模块IMEI号********/
void GprsConnect::read_IMEI()
{
//    qint8 time_step;
    quint32 fd;
    qint8 receivedLen;
    char sendData[100];
    char buff [BUFFER_SIZE] ;
    qDebug() << "read Imei";

    fd = Open_Serial(1,115200,0);

    memset(buff,0,BUFFER_SIZE);
    msleep(1000);
    strcpy(sendData,"AT+CGSN\r\n");
    write(fd,sendData,(sizeof(sendData)/sizeof(sendData[0])));
    memset ( buff , 0 , BUFFER_SIZE );          //清零操作
    receivedLen = read ( fd , buff , 100 );
    memset ( sendData , 0 , 100 );
    memset (imei_number , 0 , 16);
//    if (strstr(buff,"OK"))
//    {
        strcpy(imei_number , buff);
        time_step += 1;
//        qDebug()<<imei_number;

}



/**************信息发送************************/
void GprsConnect::info_send(char *send_info_data, quint32 info_len , QString ipaddr , int port)
{
    quint32 frame_len = 0;
    quint8 temp[2];

    globalData ->dataCRC16(send_info_data , info_len , checkCRC);
    temp[0] = checkCRC[1];
    temp[1] = checkCRC[0];
    memcpy(send_info_data+info_len , temp , 2);
    frame_len = info_len + 2;
    msleep(1000);
    quint64 send_size = 0;
      send_size =  udpSocket->writeDatagram( send_info_data,frame_len,QHostAddress(ipaddr),port);
   //   qDebug()<<send_size;
      // ret = udpSocket->writeDatagram(datagram.data(),datagram.size(),QHostAddress::LocalHost,44444);
}

/**************向上位机信息发送************************/
void GprsConnect::info_send_toUpper(char *send_info_data, quint32 info_len , QString ipaddr , int port)
{
    quint32 frame_len = 0;
    quint8 temp[2];

    globalData ->dataCRC16(send_info_data , info_len , checkCRC);
    temp[0] = checkCRC[1];
    temp[1] = checkCRC[0];
    memcpy(send_info_data+info_len , temp , 2);
    frame_len = info_len + 2;
    msleep(1000);
    udpSocket_Upper->writeDatagram( send_info_data,frame_len,QHostAddress(ipaddr),port);
       // ret = udpSocket->writeDatagram(datagram.data(),datagram.size(),QHostAddress::LocalHost,44444);
}

/********开串口，包括串口类型U_or_S:0为S1为U，串口号S_num，波特率baultrate,串口名********/
int GprsConnect::Open_Serial(quint8 U_or_S,quint32 baultrate,quint8 S_num)
{
    qint32 fd;

         if ( ( fd = openPort( S_num,U_or_S) ) < 0 )
        {      //打开串口,如果打开失败，会返回-1
                qDebug() << "Open USB port failed";

       }

          if (setComConfig( fd , baultrate , 8 , 'N' , 1 ) < 0 )
       {//打开端口错误，返回-1
            qDebug() << "Set COM failed";

        }
        return fd;
}


/********字节拼接为整型********************/
int GprsConnect::Byte_To_Int(quint8 *p, quint8 num)//intger为转换后 p为字节 num为字节个数
{
    quint32 intger=0;
    if(num == 4)
    {
        intger = (int) (((*(p+2) & 0xFF)<<24) | ((*(p+3) & 0xFF)<<16) | ((*(p+4) & 0xFF)<<8) | ((*(p+5) & 0xFF)<<0));
       // qDebug()<<"****p4****"<<intger;
    }
    if(num == 2)
    {
        intger = (int) ( ((0 & 0xFF)<<24) | ((0 & 0xFF)<<16) | ((*p & 0xFF)<<8) | ((*(p+1) & 0xFF)<<0) );
      //  qDebug()<<"****p2****"<<intger;
    }
    return intger;
}

///*************double数据转换成2字节无符号数组数据*********/
//void GprsConnect::Double_To_Array(quint8 *p, double value)
//{
//    qint32 TempValue;
//    TempValue = value;
//    *p =((TempValue >> 8) & 0xFF);
//     p++;
//    *p = (TempValue & 0xFF);
//}
/*************double数据转换成2字节有符号数组数据*********/
void GprsConnect::Double_To_Array2(char *p, double value)
{
    qint32 TempValue;
    TempValue = value;
    *p =((TempValue >> 8) & 0xFF);
     p++;
    *p = (TempValue & 0xFF);
}

///*************double数据转换成4字节数组数据*********/
//void GprsConnect::Double_To_Array4(quint8 *p, double value)
//{
//    quint32 TempValue;
//    TempValue = value;
//    *p =((TempValue >> 24) & 0xFF);
//     p++;
//    *p = ((TempValue>>16) & 0xFF);
//     p++;
//     *p = ((TempValue>>8) & 0xFF);
//     p++;
//     *p = ((TempValue) & 0xFF);
//}
/****************转换成字节****************/
int GprsConnect::dec_to_bcd(qint32 dec_dat, quint8 *bcd, quint32 len)
{
    quint32 i;
    qint32 temp;
    for (i = len-1;i>0;i--)
    {
        temp = dec_dat % 100;
        bcd[i] = ((temp/10)<<4) + ((temp%10) & 0x0F);
        dec_dat /=100;
    }
    return 0;
}


/******************发送身份验证************************/

void GprsConnect::send_Id_to_center(char *imei_num)
{
    quint32 addr = 0;

    addr = encspl->EncID(imei_num , frame);
    info_send((char*)frame , addr , ipaddr1,port1);
}
/*******向上位机发送数据，以便它识别本机IP端口*******/
void GprsConnect::send_IP_Data_ToUpper(){
    char Temp_IP_Mes[1] = {0x01};
    char Temp_Data[30];
        QString imei;
    quint8 addr = 1;
    addr = encspl->add_frame_CRC_FE(0x04,0x08,
                                             imei,Temp_IP_Mes,addr,Temp_Data,0x13);//最后发送出去的数据包长度
//        for (quint8 i =0;i<addr;i++)
//        {
//            qDebug()<<"**********temp********"<<hex<<(quint8)Temp_Data[i];
//        }
    info_send_toUpper(Temp_Data,addr,ipaddr2,port2);
}

/******************发送人员上报************************/

void GprsConnect::send_FingerId_to_center(char *imei_num, quint8 event)
{
    char temp[25];

    quint32 addr = 0;

    memset(temp , 0 , 25);

    GetTime(center_time);

   addr =  encspl->EncFingerID(temp , center_time , center_receive_ID , imei_num , event);

    info_send(temp , addr , ipaddr1,port1);
}

/**********指纹信息查询****************/
void GprsConnect::send_finger_version_inquiry()
{
    if((Config_Info_Flag & 0x08) == 0x08)
    {
        qDebug()<<"************FINGER INQUIRY************";
    quint32 addr = 0;
    char finger_inquiry[1] = {0x01};
    char temp_data[30];
    memset(temp_data , 0 , 30);

    addr = globalData->create_data_for_center(0x08 , (char*)center_receive_ID , temp_data , finger_inquiry , 1 , 0);
    info_send(temp_data , addr , ipaddr1,port1);
    }
//    for(quint8 i= 0;i<14;i++)
//    {
//        qDebug()<<"temp_data"<<hex<<temp_data[i];
//    }
}

/*************指纹信息同步请求帧发送**************/
void GprsConnect::send_finger_syn_inquiry(quint32 DriverID)
{
    quint32 addr = 0;
    quint8 Rec_Style = 0x01;//识别方式，0X01为指纹

    quint8 finger_inquiry[1] = {0x02};//指纹查询请求或同步请求
    quint8 Rec_Num_Par[1] = {0x00};
    quint8 temp_data[50];
    quint8 Temp_Finger_Syn[7];
    quint8 sys_driver_ID[4] = {0};

    switch(FINGER_PARTION_SEND)
    {
    case 0x00:
        Rec_Num_Par[0] = (Rec_Style<<6)| (Finger_number<<4)|(Finger_Partition_number<<2)|0x00;//第一分片号指纹，以此类推

        qDebug()<<"***00***SEND"<<hex<<Rec_Num_Par[0];
        break;
    case 0x01:
       Rec_Num_Par[0] = (Rec_Style<<6)| (Finger_number<<4)|(Finger_Partition_number<<2)|0x00;
       //qDebug()<<"*******enter 0x01******";
        qDebug()<<"***01***SEND"<<hex<<Rec_Num_Par[0];
        break;
    case 0x03:
       Rec_Num_Par[0] = (Rec_Style<<6)| (Finger_number<<4)|(Finger_Partition_number<<2)|0x00;

        qDebug()<<"***02***SEND"<<hex<<Rec_Num_Par[0];
        break;
    case 0x07:
        Rec_Num_Par[0] = (Rec_Style<<6)| (Finger_number<<4)|(Finger_Partition_number<<2)|0x00;
        qDebug()<<"*******FINGER_PARTION_SEND1******"<<FINGER_PARTION_SEND;
        qDebug()<<"***003***SEND"<<hex<<Rec_Num_Par[0];
        break;
    default:
        break;
    }

    memset(temp_data , 0 , 50);
    memset(Temp_Finger_Syn ,0 ,7);
    memset(sys_driver_ID ,0 ,4);

 //   qDebug()<<"**Rec_Num_Par***"<<hex<<Rec_Num_Par[0];
    int int_sys_driver_id = 0;
     int_sys_driver_id = DriverID;

    Double_To_Array4(sys_driver_ID,int_sys_driver_id);

    addr = globalData->split_joint_hex_data(Temp_Finger_Syn , addr ,finger_inquiry , 1);
    addr = globalData->split_joint_hex_data(Temp_Finger_Syn , addr ,Rec_Num_Par , 1);
    addr = globalData->split_joint_hex_data(Temp_Finger_Syn , addr ,sys_driver_ID , 4);
    addr = globalData->create_data_for_center(0x08 , (char*)center_receive_ID , temp_data , Temp_Finger_Syn , addr , 0);

    info_send((char*)temp_data , addr , ipaddr1,port1);
}


/*********指纹ID版本信息存储在数据库中并发送模板请求同步指令********/
void GprsConnect::FingerData_Store_InDataBase()
{
    if((Config_Info_Flag & 0x10) == 0x10 )
    {
    qDebug()<<"********FINGER SYN********";
    int driver_id[10],driver_version[10];
    for(quint8 i=0;i<Driver_Num;i++)
    {
        driver_version[i]=Byte_To_Int(DriverID_Version[i],2);
        driver_id[i] = Byte_To_Int(DriverID_Version[i] , 4);
        //qDebug()<<"****driver id****"<<driver_id[i];
    }

    /*******测试******/
//    if(test_num == 0)
//    {
//    send_finger_syn_inquiry(driver_id[0]);
//    msleep(1000);
//    }
//    else if (test_num == 1)
//    {
//        send_finger_syn_inquiry(driver_id[1]);
//        msleep(1000);
//    }
//    else if (test_num == 2)
//    {
//        send_finger_syn_inquiry(driver_id[2]);
//        msleep(1000);
//    }
    /*******测试******/

    Finger_Set_Data->setTable("FingerStore");
    Finger_Set_Data->select();
    if (Finger_Set_Data->rowCount() == 0)//数据库里为空
    {
    int row = 0;
    for(quint8 i =0;i<Driver_Num;i++)//将所有查询到的人员数目都存储在数据库中
    {
        qDebug()<<"********empty*********";
    //   qDebug()<<"***driver_version****"<<driver_version;
    //    qDebug()<<"***driver_version****"<<DriverID_Version[0][5];
        Finger_Set_Data->insertRows(row ,1);
        Finger_Set_Data->setData(Finger_Set_Data->index(row,0), i);
        Finger_Set_Data->setData(Finger_Set_Data->index(row,1), driver_id[i]);
        Finger_Set_Data->setData(Finger_Set_Data->index(row,2),driver_version[i]);
        send_finger_syn_inquiry(driver_id[i]);
        row++;
    }
      //  DATA_SEND_SUCCESS = DATA_SEND_SUCCESS | 0x02;
        Finger_Set_Data->submitAll();
    }
    else//数据库里有数据
    {
        int rowNUM = Finger_Set_Data->rowCount();
        quint8 j =0,vacant_row = 0;
         do
        {

                if((rowNUM <  Driver_Num) && (Insert_Or_Change == 2))
                {
                    Finger_Set_Data->insertRows(rowNUM ,1);
                    Finger_Set_Data->setData(Finger_Set_Data->index(rowNUM,0), rowNUM);
                    Finger_Set_Data->setData(Finger_Set_Data->index(rowNUM,1), 0);
                    Finger_Set_Data->setData(Finger_Set_Data->index(rowNUM,2), 0);
                    rowNUM++;
                    Finger_Set_Data->submitAll();
                }
                 //qDebug()<<"***rownum****"<<rowNUM;
                /***********数据库中的一条数据与请求的数据进行循环比对********/
                for(quint8 i =0;i<Driver_Num;i++)//请求下来的数目
                {
                    if((Finger_Set_Data->index(j,1).data() == driver_id[i]) &&
                                                (Finger_Set_Data->index(j,2).data() == driver_version[i])
                            && (Insert_Or_Change == 2))//若ID一致版本一致,进行下一条比对
                      {
                            break;
                     }
                    else if((Finger_Set_Data->index(j,1).data() != driver_id[i]) && (i == Driver_Num-1)
                            && (Insert_Or_Change == 2)) //如果数据库中的ID在下发的数组里
                    {
                        deletedata[j].FingerAddress = 2*j;
                        vacant_row = j+1;
                        Finger_Set_Data->revertRow(j);//删除数据库的对应行
                        qDebug()<<"***deletedata****"<<deletedata[j].FingerAddress;
                        fingergather->FingerData_Delete(deletedata.at(j).FingerAddress , 2);//发送删除模块指纹命令
                        //rowNUM--;
                        Finger_Set_Data->submitAll();
                    }
                    else if((Finger_Set_Data->index(j,1).data() == driver_id[i]) &&
                            (Finger_Set_Data->index(j,2).data() != driver_version[i])
                            && (Insert_Or_Change == 2))//若ID一致但版本不一致，则进行请求下发
                    {
                        I_OR_C_Num = j;
                        changedata[j].DriverID = Finger_Set_Data->index(j,1).data().toUInt();
                        changedata[I_OR_C_Num].FingerAddress = 2*j;
                        Finger_Set_Data->setData(Finger_Set_Data->index(j,2) , driver_version[i]);//改写数据库中的版本号
                        Insert_Or_Change = 1;
                        Finger_Set_Data->submitAll();
                    }
                }
                /*********下载的数据与数据库中的数据进行循环比对********/
                for(quint8 i = 0;i<rowNUM;i++)
                {
                    if((driver_id[j] == Finger_Set_Data->index(i,1).data()) && (Insert_Or_Change == 2))
                    {
                        if(j == Driver_Num-1)
                            Config_Info_Flag = (Config_Info_Flag & 0x0f);
                        qDebug()<<"no store finger number:"<<j;
                        break;
                    }
                    else if((driver_id[j] != Finger_Set_Data->index(i,1).data()) && (i == rowNUM-1)
                            && (Insert_Or_Change == 2)) //如果下发的数组的ID
                    {
                        if(vacant_row != 0)//如果有行数被删除 有空着的行
                        {
                            I_OR_C_Num = j;
                            insertdata[j].DriverID = driver_id[j];//在数据库中没有，则将ID存储在插入链表中
                            insertdata[I_OR_C_Num].FingerAddress = 2*j;
                            qDebug()<<"***insertdata****"<<insertdata[I_OR_C_Num].FingerAddress;
                            Finger_Set_Data->setData(Finger_Set_Data->index(vacant_row-1,0), vacant_row-1);
                            Finger_Set_Data->setData(Finger_Set_Data->index(vacant_row-1,1), driver_id[j]);
                            Finger_Set_Data->setData(Finger_Set_Data->index(vacant_row-1,2),driver_version[j]);
                            vacant_row = 0;
                            Finger_Set_Data->submitAll();
                        }
                        else//如果没有被删除的行
                        {
                            I_OR_C_Num = j;
                            insertdata[j].DriverID = driver_id[j];//在数据库中没有，则将ID存储在插入链表中
                            insertdata[I_OR_C_Num].FingerAddress = 2*j;
                            //qDebug()<<"***insertdata****"<<insertdata[I_OR_C_Num].FingerAddress;
                            Finger_Set_Data->insertRows(rowNUM ,1);
                            Finger_Set_Data->setData(Finger_Set_Data->index(rowNUM,0), rowNUM);
                            Finger_Set_Data->setData(Finger_Set_Data->index(rowNUM,1), driver_id[j]);
                            Finger_Set_Data->setData(Finger_Set_Data->index(rowNUM,2),driver_version[j]);
                            rowNUM++;
                            Finger_Set_Data->submitAll();
                        }
                      Insert_Or_Change = 0;
                    }
                }
         if((Insert_Or_Change == 1))
       {
           send_finger_syn_inquiry(changedata.at(j).DriverID);
           msleep(2000);
       }
         if((Insert_Or_Change == 0))
       {
          send_finger_syn_inquiry(insertdata.at(j).DriverID);
          msleep(2000);
       }
         if ((Insert_Or_Change == 2))
         {
            j++;
           Finger_Set_Data->submitAll();
         }
      }while(j < Driver_Num);
       //DATA_SEND_SUCCESS = DATA_SEND_SUCCESS | 0x02;
//       if(j == rowNUM)
//       {
//           DATA_SEND_SUCCESS = DATA_SEND_SUCCESS | 0x02;
//       }
    }
    }
}

/******指纹数据信息存储于模块********/
void GprsConnect::FingerData_Store_InModule()
{
//    char buff1 [13] ;
//    char buff2 [13] ;
//    quint32 fd;
//    quint16 len;
//    fd = Open_Serial(0,57600,3);

//    quint8 crc_char[2] = {0,0};
//    quint16 crc_sum = 0;

//    char sendData[14] = {0xef , 0x01, 0xff, 0xff, 0xff, 0xff, 0x01, 0x00, 0x05
//                                      , 0x32, 0x00, 0x00, 0x00, 0x00};

//    for (quint8 i = 6;i<14;i++)
//    {
//        crc_sum = crc_sum + sendData[i];
//    }

//    Double_To_Array(crc_char , crc_sum);
//    sendData[12] = crc_char[0];
//    sendData[13] = crc_char[1];

//    for(quint8 i = 0;i<14;i++)
//    {
//        qDebug()<<hex<<(quint8)sendData[i];
//    }
//    write(fd,sendData,14);

//    memset(buff1,0,BUFFER_SIZE);
//    memset(buff2,0,BUFFER_SIZE);

//    len = read(fd,buff1,12);
//    qDebug()<<"len"<<len;
//    qDebug()<<"store in module";

//    memset(sendData , 0 , 16);

//    qDebug()<<"store in module";
//    if(buff1[9] == 0x00)
//    {
//        write(fd,set_buf,16);
//        qDebug()<<"FINGER_PARTION_SEND"<<FINGER_PARTION_SEND;
//        if(FINGER_PARTION_SEND == 0x0F)
//        {
//            read(fd,buff2,12);
//        if(buff2[9] == 0x00)
//            qDebug()<<"store OK";
//        }
//    }

}

/*************状态信息******************/
void GprsConnect::send_status_data()
{
   // qDebug()<<"******STATUSDATA**********";
    char Temp_Status_Data[50];
    char temp_data[70];
    quint32 addr = 0;


memset (Temp_Status_Data , 0 , 50);
memset (temp_data , 0 , 70);

    GetTime(center_receive_time);

   addr = encspl->EncStatusData(Temp_Status_Data , center_receive_time);
addr = globalData->create_data_for_center(0x02 , (char*)center_receive_ID ,temp_data , Temp_Status_Data , addr , 0);

//for(quint8 i =0;i<addr;i++)
//{
//    qDebug()<<"send time"<<hex<<(quint8)Temp_Status_Data[i];
//}
qDebug()<<endl;
info_send((char*)temp_data , addr , ipaddr1,port1);

}
/****************获取系统时间******************/
void GprsConnect::GetTime(char *Time_str)//获取系统时间
{
    QString str;
    bool ok = false;
    quint32 i = 0,j = 0;

    str = system_time.currentDateTime().toString("yyyy")+"."+ system_time.currentDateTime().toString("M")+"."+
              system_time.currentDateTime().toString("d")+"."+system_time.currentDateTime().toString("h")+"."+
              system_time.currentDateTime().toString("m")+"."+system_time.currentDateTime().toString("s");

    i = str.indexOf(".");
    QString year = str.mid(0,i);
    quint32 int_year = year.toInt(&ok,10)-2010;
     j =str.indexOf(".",i+1);
    QString Month = str.mid(i+1,j-i-1);
    i = j;
    j = str.indexOf(".",i+1);
    quint32 int_Month = Month.toInt(&ok,10);
    QString day = str.mid(i+1,j-i-1);
    i = j;
    j = str.indexOf(".",i+1);
    quint32 int_day = day.toInt(&ok,10);
    QString hour = str.mid(i+1,j-i-1);
    i = j;
    j = str.indexOf(".",i+1);
    quint32 int_hour = hour.toInt(&ok,10);
    QString min = str.mid(i+1,j-i-1);
    i = j;
    j = str.indexOf(".",i+1);
    quint32 int_min = min.toInt(&ok,10);
    QString sec = str.mid(i+1,j-i-1);
    i = j;
    j = str.indexOf(".",i+1);
    quint32 int_sec = sec.toInt(&ok,10);

//    qDebug()<<"********year****"<<int_year;
//    qDebug()<<"********month****"<<int_Month;
//    qDebug()<<"********day****"<<int_day;
//    qDebug()<<"********hour****"<<int_hour;
//    qDebug()<<"********minute****"<<int_min;
//    qDebug()<<"********second****"<<int_sec;

    Time_str[3] = (int_min<<6) + (int_sec & 0x3F);
    Time_str[2] = ((int_hour & 0x0F)<<4) + (int_min >>2);
    Time_str[1] = ((int_Month & 0x03)<<6) + ((int_day & 0x1F)<<1) + ((int_hour & 0x1F)>>4);
    Time_str[0] = (int_year<<2) + (int_Month >> 2);
}

/************时间同步******************/
void GprsConnect::time_synchron(char *Time_str)
{
    QString str,syy,sMM,sdd,shh,smm,sss;
    quint32 yy,MM,dd,hh,mm,ss;
  //  quint8 yy_char[1]={0},MM_char[1]={0},dd_char[1]={0},hh_char[1]={0},mm_char[1]={0},ss_char[1]={0};

    yy = 2010+(Time_str[0]>>2);
    MM = ((Time_str[0] & 0x03)<<2) + ((Time_str[1] & 0xC0)>>6);
    dd = ((Time_str[1] & 0x3E) >>1);
    hh = ((Time_str[1] & 0x01)<<4) +(Time_str[2]>>4);
    mm = ((Time_str[2]<<2) & 0x3C) + (Time_str[3]>>6);
    ss = Time_str[3] & 0x3F;

    syy = QString::number(yy,10);
    sMM = QString::number(MM,10);
    sdd = QString::number(dd,10);
    shh = QString::number(hh,10);
    smm = QString::number(mm,10);
    sss = QString::number(ss,10);

   // qDebug()<<system_time.currentDateTime();
    str = "date --set \""+syy+"-"+sMM+"-"+sdd+" "+shh+":"+smm+":"+sss+"\""+" &";

    system(str.toLatin1().data());

//    system(str.toLatin1().data());
    system("hwclock -w &");

}
/********信息传输帧发送********/
void GprsConnect::send_mes_trans_data()
{
        if((Config_Info_Flag & 0x01) == 0x01)//标志位第一位为基本信息发送标志位，标志位起到保证基本信息只发送一遍
        {
           // qDebug()<<"************BASIC************";
            send_basic_data();
            msleep(500);
        }

        if((Config_Info_Flag & 0x02) == 0x02)
        {
            if(globalData->proZoneDataList.count() == 0)
            {
                DATA_SEND_SUCCESS = DATA_SEND_SUCCESS | 0x02;
                Config_Info_Flag = (Config_Info_Flag & 0x1d);
            }
            else{
             //   qDebug()<<"************PROTECT************";
                send_Protect_Message(0x01);
                msleep(500);
            }
        //保护区信息发送
        }

        if((Config_Info_Flag & 0x04) == 0x04)
        {
       // qDebug()<<"************LIMIT************";
        send_Limit_Data();
        msleep(500);
        }
}

/**************基本信息帧发送*********************/
void GprsConnect::send_basic_data()
{

    quint32 addr = 0;
    char Temp_Basic_Mes[32];
    char temp_data[70];
    QString imei;
  //  quint32 int_crane_top_height;
    qint32 len;

    memset(Temp_Basic_Mes , 0 , 32);
    memset(temp_data , 0 , 70);
    addr = encspl->EncbasicData(Temp_Basic_Mes , Upper_Or_Center);
//qDebug()<<"**********addr"<<addr;
    if((Config_Info_Flag & 0x01) == 0x01){
        addr = globalData->create_data_for_center(0x04 , (char*)center_receive_ID ,temp_data , Temp_Basic_Mes , addr ,0);
        len = addr;
        info_send((char*)temp_data , len , ipaddr1,port1);
    }
    else{
        addr = encspl->add_frame_CRC_FE(0x04,0x08,
                                                  imei,Temp_Basic_Mes,addr,temp_data,0x04);//最后发送出去的数据包长度
//        for (int i = 0 ; i < addr;i++){
//            qDebug()<<"temp data"<<hex<<temp_data[i];
//        }
        info_send_toUpper((char*)temp_data,addr,ipaddr2,port2);
    }
}
/*******保护区信息发送，其中组装部分已在EncSpl类里完成******/
void GprsConnect::send_Protect_Message(char Crane_Index)
{

    char Temp_Protect_Mes[70];
    char Temp_Data[100];
    quint8 addr = 0;
     QString imei;
    /*****测试代码，test_index为保护区序号******/
    memset(Temp_Protect_Mes , 0 , 70);
    memset(Temp_Data , 0 , 100);
    //qDebug()<<"prozone order"<<globalData->proZoneDataList[0].proZoneOrder;
    addr = encspl->EncProZone(Temp_Protect_Mes , Crane_Index);
    //qDebug()<<"addr "<<addr;

    if((Config_Info_Flag & 0x02) == 0x02){
         addr =  globalData->create_data_for_center(0x04 , (char*)center_receive_ID, Temp_Data , Temp_Protect_Mes , addr , 0);
         info_send(Temp_Data , addr , ipaddr1,port1);
    }
    else{
        // qDebug()<<"run here";
        addr = encspl->add_frame_CRC_FE(0x04,0x08,
                                                 imei,Temp_Protect_Mes,addr,Temp_Data,0x04);//最后发送出去的数据包长度
//        for (int i = 0 ; i < addr;i++)
//        {
//            qDebug()<<"proZone data"<<hex<<(quint8)Temp_Data[i];
//        }
        info_send_toUpper(Temp_Data ,addr , ipaddr2,port2);
}
}
/*******限位信息发送************/
void GprsConnect::send_Limit_Data()
{
    char Temp_Limit_Mes[100];
    char Temp_Data[70];
    quint8 addr;
    QString imei;
    addr = encspl->EncLimit(Temp_Limit_Mes);
    if((Config_Info_Flag & 0x04) == 0x04){
        addr = globalData->create_data_for_center(0x04 , (char*)center_receive_ID, Temp_Data , Temp_Limit_Mes , addr , 0);
        info_send(Temp_Data , addr , ipaddr1,port1);
    }
    else{
        addr = encspl->add_frame_CRC_FE(0x04,0x08,
                                                 imei,Temp_Limit_Mes,addr,Temp_Data,0x04);//最后发送出去的数据包长度
        info_send_toUpper(Temp_Data , addr , ipaddr2,port2);
    }
}
/*******KD值发送***********/
void GprsConnect::send_KD_Data()
{
    char Temp_KD_Mes[100];
    char Temp_Data[70];
        QString imei;
    quint8 addr;
    addr = encspl->EncKD_ip(Temp_KD_Mes);
    addr = encspl->add_frame_CRC_FE(0x04,0x08,
                                             imei,Temp_KD_Mes,addr,Temp_Data,0x04);//最后发送出去的数据包长度
    info_send_toUpper(Temp_Data,addr,ipaddr2,port2);
}
/******塔群值发送************/
void GprsConnect::send_CraneGroup_Data(char Crane_Index)
{
    char Temp_CraneGroup_Mes[100];
    char Temp_Data[70];
    QString imei;
    quint8 addr;
    addr = encspl->EncCraneGroup(Temp_CraneGroup_Mes , Crane_Index);
    addr = encspl->add_frame_CRC_FE(0x04,0x08,
                                             imei,Temp_CraneGroup_Mes,addr,Temp_Data,0x04);//最后发送出去的数据包长度
    info_send_toUpper(Temp_Data,addr,ipaddr2,port2);
}
/*****重启通告帧发送**********/
void GprsConnect::send_Reset_Data(){
    char Temp_Reset_Mes[10] ;
    char Temp_Data[40];
    QString imei;
    quint32 addr = 4;
    addr = encspl->add_frame_CRC_FE(0x04,0x08,
                                              imei,Temp_Reset_Mes,addr,Temp_Data,0x1F);
    info_send_toUpper(Temp_Data,addr,ipaddr2,port2);
}

/*****发送储存成功回应帧到上位机****/
void GprsConnect::send_request_to_upper(quint8 info_type,quint32 info_version,quint32 info_index){
    char Temp_Request_Mes[30];
    quint8 addr;
    addr = encspl->EncRequest(info_type , info_version , info_index , Temp_Request_Mes);
    info_send_toUpper(Temp_Request_Mes,addr,ipaddr2,port2);
}

///*******限位信息发送************/
//void GprsConnect::send_limit_data()
//{
//    quint32 addr = 0;
//    quint8 Temp_Limit_Mes[32];
//    quint8 temp_data[70];
//    quint8 data_style[1] ={ 0x03 };
//    quint8 limit_version[2];
//    quint8 left_limit[2];
//    quint8 right_limit[2];
//    quint8 far_limit[1];
//    quint8 near_limit[1];
//    quint8 high_limit[1];
//    quint8 weight_limit[2];
//    quint8 moment_limit[2];
//    quint8 senser_bit[1];

//    Double_To_Array(limit_version , globalData->limitData.limitVersion);
//    Double_To_Array(left_limit ,10*globalData->limitData.leftLimit);
//    Double_To_Array(right_limit , 10*globalData->limitData.rightLimit);
//    far_limit[0] = 10*globalData->limitData.farLimit;
//    near_limit[0] = 10*globalData->limitData.nearLmit;
//    high_limit[0] = 10*globalData->limitData.highLimit;
//    Double_To_Array(weight_limit , 100*globalData->limitData.weightLimit);
//    Double_To_Array(moment_limit , 100*globalData->limitData.momentLimit);
//    senser_bit[0] = globalData->limitData.enableBit;

//    memset (Temp_Limit_Mes , 0x00 , 32);
//    memset (temp_data , 0 , 70);

//     addr = globalData->split_joint_hex_data(Temp_Limit_Mes , addr ,  data_style  , 1);
//    addr = globalData->split_joint_hex_data(Temp_Limit_Mes , addr ,  limit_version  , 2);

//    addr = globalData->split_joint_hex_data(Temp_Limit_Mes , addr ,  left_limit  , 2);

//    addr = globalData->split_joint_hex_data(Temp_Limit_Mes , addr , right_limit , 2);
//    addr = globalData->split_joint_hex_data(Temp_Limit_Mes , addr , far_limit , 1);
//    addr = globalData->split_joint_hex_data(Temp_Limit_Mes , addr , near_limit , 1);
//    addr = globalData->split_joint_hex_data(Temp_Limit_Mes , addr , high_limit , 1);
//    addr = globalData->split_joint_hex_data(Temp_Limit_Mes , addr , weight_limit , 2);
//    addr = globalData->split_joint_hex_data(Temp_Limit_Mes , addr , moment_limit, 2);
//    addr = globalData->split_joint_hex_data(Temp_Limit_Mes , addr , senser_bit , 1);

//    addr = globalData->create_data_for_center(0x04 , (char*)center_receive_ID ,temp_data , Temp_Limit_Mes , addr ,0);
////qDebug()<<"**********addr"<<addr;

//    info_send((char*)temp_data , addr);
//}

/********************报警上报********************/
void GprsConnect::report_alarm_to_center(char alarm_type, char *detail)
{

    char temp_data[37];
    char alarm_message[30];

    quint32 addr = 0;

    memset(temp_data ,0 ,37 );
    memset(alarm_message , 0 ,30);
    GetTime(center_time);

   addr =  encspl->EncAlarmData(alarm_message , alarm_type , detail , center_time);

    addr = globalData->create_data_for_center(0x06 , (char*)center_receive_ID ,temp_data , alarm_message , addr ,0);

    info_send(temp_data , addr , ipaddr1,port1);
}


/*******************违章上报******************/
void GprsConnect::report_illegal_to_center( char illegal_type, char *detail)
{

    char temp_data[24];
    char illegal_message[16];
    quint32 addr = 0;

    memset(temp_data ,0 ,24 );
    memset(illegal_message , 0 ,16);

    GetTime(center_time);

    addr = encspl->EncIllegalData(illegal_message , illegal_type , detail , center_time);

    addr = globalData->create_data_for_center(0x06 , (char*)center_receive_ID ,temp_data , illegal_message , addr ,0);

    info_send((char*)temp_data , addr , ipaddr1,port1);
}

/*********************故障上报******************/
void GprsConnect::report_fault_to_center(char fault_type, char *detail)
{

    char temp_data[31];
    char fault_message[23];
    quint32 addr = 0;

    memset(temp_data ,0 ,31 );
    memset(fault_message , 0 ,23);


    GetTime(center_time);

    addr = encspl->EncFaultData(fault_message , fault_type , detail , center_time);

    addr =  globalData->create_data_for_center(0x06 , (char*)center_receive_ID ,temp_data , fault_message , addr ,0);

    info_send((char*)temp_data , addr , ipaddr1,port1);
}

/******报警处置*****************/
void GprsConnect::warn_dispose()
{
    if(globalData->ultimate_warn_flag>0)
    {
        collision_warn_dispose();
        limit_Warn_Dispose();
        barrier_Warn_Dispose();
        forbid_Warn_Dispose();
        overrun_Warn_Dispose();
        globalData->ultimate_warn_flag = 0;
    }
}

/*****相互干涉报警处置************/
void GprsConnect::collision_warn_dispose()
{
    char intefere[2] = {globalData->interfere_crane_num , globalData->interfere_Type_Flag};
    if((globalData->craneList.at(0).lowCollision == 2) ||
        (globalData->craneList.at(0).farCollision == 2) ||
        (globalData->craneList.at(0).leftCollision == 2) ||
        (globalData->craneList.at(0).rightCollision == 2) ||
        (globalData->craneList.at(0).nearCollision == 2) ||
        (globalData->craneList.at(0).bodyCollision == 2))
    {
        report_alarm_to_center(0x01, intefere);
        msleep(1000);
        report_illegal_to_center(0x04, &intefere[0]);
        msleep(1000);
    }
}


/********************限位报警处置*****************/
void GprsConnect::limit_Warn_Dispose()
{
    char limit[4] = {0x00 , 0x00 , 0x00 , 0x00};
    if(globalData->craneList.at(0).left_limit_warn_flag == 2)
    {
        limit[3] = 0x02;
        report_alarm_to_center(0x04,limit);
        msleep(1000);
        globalData->craneList[0].left_limit_warn_flag = 0;
        limit[3] = 0x00;
    }
    if(globalData->craneList.at(0).right_limit_warn_flag == 2)
    {
        limit[3] = 0x08;
        report_alarm_to_center(0x04,limit);
        msleep(1000);
        globalData->craneList[0].right_limit_warn_flag = 0;
        limit[3] = 0x00;
    }
    if(globalData->craneList.at(0).high_limit_warn_flag == 2)
    {
        limit[2] = 0x02;
        report_alarm_to_center(0x04,limit);
        msleep(1000);
        globalData->craneList[0].high_limit_warn_flag = 0;
        limit[2] = 0x00;
    }
    if(globalData->craneList.at(0).far_limit_warn_flag == 2)
    {
        limit[3] = 0x20;
        report_alarm_to_center(0x04,limit);
        msleep(1000);
        globalData->craneList[0].far_limit_warn_flag = 0;
        limit[3] = 0x00;
        char fault_type = 0x01;
        char fault_detail = 0x03;
       report_fault_to_center(fault_type,&fault_detail);
       msleep(1000);
    }
    if(globalData->craneList.at(0).near_limit_warn_flag == 2)
    {
        limit[3] = 0x80;
        report_alarm_to_center(0x04,limit);
        msleep(1000);
        globalData->craneList[0].near_limit_warn_flag = 0;
        limit[3] = 0x00;
        report_fault_to_center((char)0x01,(char*)0x04);
        msleep(1000);
    }
}

/****************障碍物报警处置*******************/
void GprsConnect::barrier_Warn_Dispose()
{
   char barrier =globalData->warn_area_num;
//    QVector<char> barrier;
//    barrier.fill(globalData->warn_area_num);
   if((globalData->low_barrier_warn_flag == 2) || (globalData->far_barrier_warn_flag == 2) ||
      (globalData->near_barrier_warn_flag == 2) || (globalData->left_barrier_warn_flag == 2) ||
      (globalData->right_barrier_warn_flag == 2))
    {
        report_alarm_to_center(0x03,&barrier);
        msleep(1000);
        report_illegal_to_center(0x04,&barrier);
    }

}
/****************禁行区报警处置*******************/
void GprsConnect::forbid_Warn_Dispose()
{
    char forbid = globalData->warn_area_num;
   if( (globalData->far_forbid_warn_flag == 2) ||
       (globalData->near_forbid_warn_flag == 2) || (globalData->left_forbid_warn_flag == 2) ||
       (globalData->right_forbid_warn_flag == 2))
     {
         report_alarm_to_center(0x02,&forbid);
         msleep(1000);
     }
}

/****************超限报警处置*******************/
void GprsConnect::overrun_Warn_Dispose()
{
    char data;
    //倾角
    if((globalData->craneList.at(0).tiltangleX == 2) || (globalData->craneList.at(0).tiltangleY == 2))
    {
        data = 0x01;
        report_alarm_to_center(0x08 , &data);
        msleep(1000);
        data = 0x00;
    }
    //风速
    if(globalData->craneList.at(0).wind_limit_warn_flag != 0)
    {
        if(globalData->craneList.at(0).wind_limit_warn_flag == 1)
     {
        data = 0x01;
        report_alarm_to_center(0x07 , &data);
        data = 0x00;
    }
        else
        {
            data = 0x02;
            report_alarm_to_center(0x07 , &data);
            data = 0x00;
        }
    }
    //吊重
    if(globalData->craneList.at(0).overweight_limit_warn_flag != 0)
    {
        if(globalData->craneList.at(0).overweight_limit_warn_flag == 1){
        data = 0x01;
        report_alarm_to_center(0x05 , &data);
        data = 0x00;
      }
        else{
            data = 0x02;
            report_alarm_to_center(0x05 , &data);
            msleep(1000);
            data = 0x01;
            report_illegal_to_center(0x01 , &data);
            data = 0x00;
        }
    }
//力矩
if(globalData->craneList.at(0).moment_limit_warn_flag != 0)
{
    if(globalData->craneList.at(0).moment_limit_warn_flag == 1){
    data = 0x01;
    report_alarm_to_center(0x06 , &data);
    data = 0x00;
  }
    else{
        data = 0x02;
        report_alarm_to_center(0x06 , &data);
        msleep(1000);
        data = 0x01;
        report_illegal_to_center(0x01 , &data);
        data = 0x00;
    }
}
}


/********************工作循环上报********************/
void GprsConnect::report_work_cycle_to_center()
{
    char event_type[1] = {0x04};
    char cycle_message[39];
    char temp_data[47];
    quint32 addr = 0;
    char temp_buff[2];//暂存2位字节数组
    GetTime(center_time);
    addr = globalData->split_joint_hex_data(cycle_message , addr , center_time, 4);
    addr = globalData->split_joint_hex_data(cycle_message , addr , event_type, 1);
    addr = globalData->split_joint_hex_data(cycle_message , addr , work_cycle->begin_t, 4);
    addr = globalData->split_joint_hex_data(cycle_message , addr , work_cycle->end_t, 4);
    Double_To_Array(temp_buff , 100*work_cycle->max_w);
    addr = globalData->split_joint_hex_data(cycle_message , addr , temp_buff , 2);
    Double_To_Array(temp_buff , 100*work_cycle->max_m);
    addr = globalData->split_joint_hex_data(cycle_message , addr , temp_buff , 2);
    Double_To_Array(temp_buff , 10*work_cycle->max_h);
    addr = globalData->split_joint_hex_data(cycle_message , addr , temp_buff , 2);
    Double_To_Array(temp_buff , 10*work_cycle->max_s);
    addr = globalData->split_joint_hex_data(cycle_message , addr , temp_buff , 2);
    Double_To_Array(temp_buff , 10*work_cycle->min_s);
    addr = globalData->split_joint_hex_data(cycle_message , addr , temp_buff , 2);
    Double_To_Array(temp_buff , 10*work_cycle->begin_a);
    addr = globalData->split_joint_hex_data(cycle_message , addr , temp_buff , 2);
    Double_To_Array(temp_buff , 10*work_cycle->begin_s);
    addr = globalData->split_joint_hex_data(cycle_message , addr , temp_buff , 2);
    Double_To_Array(temp_buff , 10*work_cycle->begin_h);
    addr = globalData->split_joint_hex_data(cycle_message , addr , temp_buff , 2);
    Double_To_Array(temp_buff , 10*work_cycle->end_a);
    addr = globalData->split_joint_hex_data(cycle_message , addr , temp_buff , 2);
    Double_To_Array(temp_buff , 10*work_cycle->end_s);
    addr = globalData->split_joint_hex_data(cycle_message , addr , temp_buff , 2);
    Double_To_Array(temp_buff , 10*work_cycle->end_h);
    addr = globalData->split_joint_hex_data(cycle_message , addr , temp_buff , 2);
    addr = globalData->create_data_for_center(0x06, (char*)center_receive_ID ,temp_data , cycle_message , addr ,0);
    info_send((char*)temp_data , addr , ipaddr1,port1);
}
/******************工作循环数据获得*****************/
void GprsConnect::get_work_cycle_data()
{
    if(globalData->craneList.at(0).wight < Weight_Gap_Low)//如果本机的吊重小于最小界
    {
        if(stable1++ > 200)//并且持续时间大于200个计时单位
        {
            begin_cycle = 1;//开始准备下一个工作循环
            end_cycle = 0;
        }
        stable2 = 0;
    }
    else if(globalData->craneList.at(0).wight > Weight_Gap_High)//如果本机吊重大于最大界
    {
        if(stable2++ > 200)//并且持续时间大于200个计时单位
        {
            end_cycle = 1;//进入工作循环当中
            begin_cycle = 0;
        }
        stable1 = 0;
    }
    if(begin_cycle == 1)
    {
        stable1 = 0;//计时标志位归零
        if(work_cycle->end_record == 0)//放下货物时开始存储状态信息
        {
            work_cycle->end_a = globalData->craneList.at(0).angle;
            work_cycle->end_s = globalData->craneList.at(0).span;
            work_cycle->end_h = globalData->craneList.at(0).craneHeight;
            GetTime(work_cycle->end_t);
            work_cycle->end_record = 1;
            if (work_cycle->begin_record == 1)//如果之前也有吊起物体，即成功完成一个工作循环，
                                                            //则向上位机发送循环信息
            {
                report_work_cycle_to_center();
            }
            work_cycle->begin_record = 0;
        }
        //最大最小值与实时状态同步
        work_cycle->max_w = globalData->craneList.at(0).wight;
        work_cycle->max_m = globalData->craneList.at(0).moment;
        work_cycle->max_h = globalData->craneList.at(0).craneHeight;
        work_cycle->min_h = globalData->craneList.at(0).craneHeight;
        work_cycle->max_s = globalData->craneList.at(0).span;
        work_cycle->min_s = globalData->craneList.at(0).span;
    }
    else if (end_cycle == 1) //如果吊起重物时间满足条件，开始记录数据
    {
        stable2 = 0;
        if(work_cycle->begin_record == 0)//之前重物有放下
        {
            work_cycle->begin_a = globalData->craneList.at(0).angle;//记录起始值
            work_cycle->begin_s = globalData->craneList.at(0).span;
            work_cycle->begin_h = globalData->craneList.at(0).craneHeight;
            GetTime(work_cycle->begin_t);
            work_cycle->end_record = 0;
            work_cycle->begin_record = 1;
        }
        //记录吊物过程中的各数据最大最小值
        work_cycle->max_w = work_cycle->max_w>globalData->craneList.at(0).wight?work_cycle->max_w:globalData->craneList.at(0).wight;
        work_cycle->max_m = work_cycle->max_m>globalData->craneList.at(0).moment?work_cycle->max_m:globalData->craneList.at(0).moment;
        work_cycle->max_h = work_cycle->max_h>globalData->craneList.at(0).craneHeight?work_cycle->max_h:globalData->craneList.at(0).craneHeight;
        work_cycle->min_h = work_cycle->min_h<globalData->craneList.at(0).craneHeight?work_cycle->min_h:globalData->craneList.at(0).craneHeight;
        work_cycle->max_s = work_cycle->max_s>globalData->craneList.at(0).span?work_cycle->max_s:globalData->craneList.at(0).span;
        work_cycle->min_s = work_cycle->min_s>globalData->craneList.at(0).span?work_cycle->min_s:globalData->craneList.at(0).span;
    }
}

/***********************人员身份上报*********************/
void GprsConnect::report_identity_to_center()
{
    char event_type[1] = {0x05};
    char identity_message[11];
    char temp_data[30];
    char sys_driver_ID[4];
    char rec_style_event[1];
    quint32 addr = 0;
    quint32 int_driver_id = 0;
    int select_id = 0;

    select_id = globalData->fingerPage[0]/2;
     qDebug()<<"*****select_id***"<<select_id;

    globalData->fingerPage[0] = 0x0f;

    Finger_Set_Data->setTable("FingerStore");
    Finger_Set_Data->setFilter(QString("id = %1").arg(select_id));
    Finger_Set_Data->select();
    if (Finger_Set_Data->rowCount() == 0)
    {
        qDebug()<<"wrong driver";
        rec_style_event[0] = 0x50;
    }
    else if (Finger_Set_Data->rowCount() == 1)
    {
        int_driver_id = Finger_Set_Data->record(0).value(1).toInt();
        qDebug()<<"****int_driver_id***"<<int_driver_id;
        rec_style_event[0] = 0x48;
    }
    memset(sys_driver_ID ,0 ,4);
    memset(temp_data , 0 ,30);
    memset(identity_message , 0 , 11);
    Double_To_Array4(sys_driver_ID,int_driver_id);
    GetTime(center_time);

    addr = globalData->split_joint_hex_data(identity_message , addr , center_time ,4);
    addr = globalData->split_joint_hex_data(identity_message , addr , event_type ,1);
    addr = globalData->split_joint_hex_data(identity_message , addr , rec_style_event ,1);
    addr = globalData->split_joint_hex_data(identity_message , addr , sys_driver_ID ,4);
    addr = globalData->create_data_for_center(0x06, (char*)center_receive_ID ,temp_data , identity_message , addr ,0);
    info_send((char*)temp_data , addr , ipaddr1,port1);
//    globalData ->dataCRC16(temp_data , addr , checkCRC);
//    temp_data1[0] = checkCRC[1];
//    temp_data1[1] = checkCRC[0];
//    memcpy(temp_data+addr , temp_data1 , 2);
    //    qDebug()<<"addr"<<addr;
//            for(quint8 i= 0;i<20;i++)
//            {
//                qDebug()<<"temp_data"<<hex<<temp_data[i];
//            }
}

/******************************/
//void Gprs_Data_Process::info_send (quint8 *send_info_data , quint32 info_len , quint8 send_way)
//{
//    quint8 send_request_data[250];
//    quint8 temp[2];
//    quint32 frame_len = 0;
//    switch (send_way)
//    {
//    case ZIGBEE_WAY:
////        send_request_data[0] = Frame_Head[0];
////        send_request_data[1] = Frame_Head[1];
////        memcpy(send_request_data+2 , send_info_data , info_len);
////        globalData ->dataCRC16(checkCRC , send_request_data ,info_len+2);
////        temp[0] = checkCRC[1];
////        temp[1] = checkCRC[0];
////        memcpy ()
//        break;
//    case GPRS_WAY:
//        globalData ->dataCRC16(checkCRC , send_info_data ,info_len+2);
//        memcpy(send_info_data + info_len , checkCRC , 2);
//        frame_len = info_len + 2;
//    }
//}

bool GprsConnect::Connect_Gprs()
{
    qint32 fd0,fd1,fd2,fd3,fd4,fd5,fd,Sel_Num=5;
    qint8 receivedLen;
    qint32 GprsConnect_Step=0;
    char sendData[100];
    //char at_gprs[]={0x4B,0xC9,0x01,0x00,0x20,0xA5,0x7E};
    //char at_gprs[]={0x4B,0xC8,0x07,0x00,0x01,0x00,0x00,0x00,0x05,0x00,0x00,0x00,0x00,0x00,0x89,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x89,0x00,0x00,0x00,0xE1,0x80,0x7E};
    //quint16 ADCData[DATA_LEN];
    char buff [BUFFER_SIZE] ;
    //quint16 chackCC16;
   // GlobalData *data = GlobalData::getGlobalData();
    Call_Number = 0;
    Connect_Success = 0;

    qDebug() << "enter dataprocess";

    if ( ( fd0 = openPort( DATA_TRANSFER_COM_0,1) ) < 0 ) {      //打开串口,如果打开失败，会返回-1
        qDebug() << "Open USB port failed";
        return FALSE;
    }

    if ( ( fd1 = openPort( DATA_TRANSFER_COM_1,1) ) < 0 ) {      //打开串口,如果打开失败，会返回-1
        qDebug() << "Open USB port failed";
        return FALSE;
    }

    if (setComConfig( fd0 , 115200 , 8 , 'N' , 1 ) < 0 ){//打开端口错误，返回-1
        qDebug() << "Set COM failed";
        return FALSE;
    }

    if (setComConfig( fd1 , 115200 , 8 , 'N' , 1 ) < 0 ){//打开端口错误，返回-1
        qDebug() << "Set COM failed";
        return FALSE;
    }

    if ( ( fd2 = openPort( DATA_TRANSFER_COM_2,1) ) < 0 ) {      //打开串口,如果打开失败，会返回-1
        qDebug() << "Open USB port failed";
        return FALSE;
    }

    if (setComConfig( fd2 , 115200 , 8 , 'N' , 1 ) < 0 ){//打开端口错误，返回-1
        qDebug() << "Set COM failed";
        return FALSE;
    }

    if ( ( fd3 = openPort( DATA_TRANSFER_COM_3,1) ) < 0 ) {      //打开串口,如果打开失败，会返回-1
        qDebug() << "Open USB port failed";
        return FALSE;
    }

    if (setComConfig( fd3 , 115200 , 8 , 'N' , 1 ) < 0 ){//打开端口错误，返回-1
        qDebug() << "Set COM failed";
        return FALSE;
    }

    if ( ( fd4 = openPort( DATA_TRANSFER_COM_4,1) ) < 0 ) {      //打开串口,如果打开失败，会返回-1
        qDebug() << "Open USB port failed";
        return FALSE;
    }

    if (setComConfig( fd4 , 115200 , 8 , 'N' , 1 ) < 0 ){//打开端口错误，返回-1
        qDebug() << "Set CO0x00M failed";
        return FALSE;
    }

    if ( ( fd5 = openPort( DATA_TRANSFER_COM_4,1) ) < 0 ) {      //打开串口,如果打开失败，会返回-1
        qDebug() << "Open USB port failed";
        return FALSE;
    }

    if (setComConfig( fd5 , 115200 , 8 , 'N' , 1 ) < 0 ){//打开端口错误，返回-1
        qDebug() << "Set COM failed";
        return FALSE;
    }
/*************************/


//    QString command = tr("pppd call wcdma");
//    char *S;
//    S = command.toAscii().data();
//    system(S);




    /*******************************/
    if( Sel_Num == 0 ) fd = fd0;
    else if( Sel_Num == 2 ) fd = fd2;
    else if( Sel_Num == 4 ) fd = fd4;
    else if( Sel_Num == 5 ) fd = fd5;
    else fd = fd1;


        qDebug()<<"Connect_Success:"<<Connect_Success;
    //Udp_Socket_Sender data_sender;
    /*************************************/
    do
    {
        //qDebug()<< "Connect_Success:"<<Connect_Success;
        switch(GprsConnect_Step)
      {
        case DSFLOWQRY://关闭回显
            memset(buff,0,BUFFER_SIZE);
            msleep(1000);
            qDebug()<< "input flowqry";
           strcpy(sendData,"AT+CGSN\r\n");

            //memset(buff,0,BUFFER_SIZE);
            write(fd,sendData,strlen(sendData));

            memset ( buff , 0 , BUFFER_SIZE );          //清零操作

            receivedLen = read ( fd , buff , 100 );
            //qDebug() << "receivedLen :"<< receivedLen ;
            qDebug()<< buff;
//            qDebug()<< "********************";
            memset ( sendData , 0 , 100 );

            if (strstr(buff,"OK"))
            {
                msleep(1000);
                GprsConnect_Step+=1;
            }
            else if (strstr(buff,"ERROR"))
            {
                msleep(1000);
                GprsConnect_Step=DSFLOWQRY;

           }
            break;

        case CMEE://返回错误信息，为0时只返回ERROR
            memset(buff,0,BUFFER_SIZE);
             msleep(1000);
            qDebug()<<"Input CMEE command";
            strcpy(sendData,"AT+CMEE=1\r\n");
            //strcpy(sendData,"DIAG");
            //memset(buff,0,BUFFER_SIZE);
           // write(fd1,at_gprs,sizeof(at_gprs));
            write(fd,sendData,strlen(sendData));
            memset ( buff , 0 , BUFFER_SIZE );          //清零操作
            receivedLen = read ( fd , buff , 100 );
            qDebug() << "receivedLen :"<< receivedLen ;
            /*
            for(qint8 i=0;i<receivedLen;i++)
            {
                qDebug()<<hex<< (quint32) buff[i];
            }
    */
            qDebug()<<buff;
            memset ( sendData , 0 , 100 );
            if (Strcomp_bool(buff,"OK"))
            {
                msleep(1000);
                GprsConnect_Step+=1;
            }
            else if (Strcomp_bool(buff,"ERROR"))
            {
                qDebug()<< "NO SIM CARD";
                msleep(1000);
                GprsConnect_Step=CMEE;
           }

            break;

        case ATE0://关闭回显
            memset(buff,0,BUFFER_SIZE);
            msleep(1000);
            qDebug()<<"Input ATE0 command";
            strcpy(sendData,"ATE1\r\n");
            //memset(buff,0,BUFFER_SIZE);
            write(fd,sendData,strlen(sendData));
            memset ( buff , 0 , BUFFER_SIZE );          //清零操作
            receivedLen = read ( fd , buff , 100 );
            //qDebug() << "receivedLen :"<< receivedLen ;
            qDebug()<< buff;
            memset ( sendData , 0 , 100 );

            if (strstr(buff,"OK"))
            {
                msleep(1000);
                GprsConnect_Step+=1;
            }
            else if (strstr(buff,"ERROR"))
            {
                msleep(1000);
                GprsConnect_Step=CPIN;

           }
            break;

        case AT://检查端口是否正常
         memset(buff,0,BUFFER_SIZE);
       //   msleep(1000);
        qDebug()<<"Input AT command";
        strcpy(sendData,"AT\r\n");
        //memset(buff,0,BUFFER_SIZE);
        write(fd,sendData,strlen(sendData));
       // qDebug() << "xxx";
        memset ( buff , 0 , BUFFER_SIZE );          //清零操作
        receivedLen = read ( fd , buff , 20 );

        qDebug()<< buff;
        memset(sendData,0,100);
        if (Strcomp_bool(buff,"OK"))
        {
            qDebug() << "SIM PORT OK";
            msleep(1000);
            GprsConnect_Step+=1;
        }
        else  {
           msleep(1000);
            GprsConnect_Step=AT;
       }
        break;

        case CPIN://检查SIM卡是否连接好
            memset(buff,0,BUFFER_SIZE);
            // msleep(1000);
            qDebug()<<"Input AT+CPIN command";
            strcpy(sendData,"AT+CPIN?\r\n");
            //memset(buff,0,BUFFER_SIZE);
            write(fd,sendData,strlen(sendData));
            memset ( buff , 0 , BUFFER_SIZE );          //清零操作
            read ( fd , buff , 30 );
            //qDebug() << "receivedLen :"<< receivedLen ;
            qDebug()<< buff;
            memset ( sendData , 0 , 100 );

            if (Strcomp_bool(buff,"READY"))
            {
                msleep(1000);
                GprsConnect_Step+=1;
            }
            else if (Strcomp_bool(buff,"ERROR"))
            {
                if (Strcomp_bool(buff,"13"))
                qDebug()<< "SIM CARD FAILURE";
                msleep(1000);
                GprsConnect_Step=CPIN;
           }
            break;

        case CSQ://检查网络信号强度
            memset(buff,0,BUFFER_SIZE);
           //  msleep(1000);
            qDebug()<<"Input AT+CSQ command";
            strcpy(sendData,"AT+CSQ=?\r\n");
            //memset(buff,0,BUFFER_SIZE);

            // qDebug()<<"PORT2 Display:";
             write(fd,sendData,strlen(sendData));
             memset ( buff , 0 , BUFFER_SIZE );          //清零操作
             receivedLen = read ( fd , buff , 30 );
              qDebug()<< buff;

            memset ( sendData , 0 , 100 );

            if (Strcomp_bool(buff,"OK"))
            {
                msleep(1000);
                GprsConnect_Step+=3;
            }
            else if (Strcomp_bool(buff,"ERROR"))
            {
                msleep(1000);
                GprsConnect_Step=CSQ;

           }
            break;


        case CREG://注册网络
            memset(buff,0,BUFFER_SIZE);
             msleep(1000);
            qDebug()<<"Input AT+CREG command";
            strcpy(sendData,"AT+CREG=1\r\n");
            //memset(buff,0,BUFFER_SIZE);
            write(fd,sendData,strlen(sendData));
            memset ( buff , 0 , BUFFER_SIZE );          //清零操作
            receivedLen = read ( fd , buff , 30 );
            //qDebug() << "receivedLen :"<< receivedLen ;
            qDebug()<< buff;
            memset ( sendData , 0 , 100 );

            if (Strcomp_bool(buff,"OK"))
            {
                msleep(1000);
                GprsConnect_Step+=1;
            }
            else if (Strcomp_bool(buff,"ERROR"))
            {
                msleep(1000);
                GprsConnect_Step=CREG;

           }
            break;

        case CGREG://注册网络
            memset(buff,0,BUFFER_SIZE);
             msleep(1000);
            qDebug()<<"Input AT+CGREG command";
            strcpy(sendData,"AT+CGREG=1\r\n");
            //memset(buff,0,BUFFER_SIZE);
            write(fd,sendData,strlen(sendData));
            memset ( buff , 0 , BUFFER_SIZE );          //清零操作
            receivedLen = read ( fd , buff , 30 );
            //qDebug() << "receivedLen :"<< receivedLen ;
            qDebug()<< buff;
            memset ( sendData , 0 , 100 );

            if (Strcomp_bool(buff,"OK"))
            {
                msleep(1000);
                GprsConnect_Step+=1;
            }
            else if (Strcomp_bool(buff,"ERROR"))
            {
                msleep(1000);
                GprsConnect_Step=CGREG;

           }
            break;

        case COPS://查询网络服务商种类
             memset(buff,0,BUFFER_SIZE);
             // msleep(1000);
            qDebug()<<"Net status";
            strcpy(sendData,"AT+COPS?\r\n");
            memset(buff,0,BUFFER_SIZE);
            write(fd,sendData,strlen(sendData));
            memset ( buff , 0 , BUFFER_SIZE );          //清零操作
             receivedLen = read ( fd , buff , 100 );
           // qDebug() << "receivedLen :"<< receivedLen ;
            qDebug()<< buff;
           memset ( sendData , 0 , 100 );
           if(strstr(buff,"46001"))
           {
                qDebug()<<"China United Telecom link succeed";
               msleep(1000);
               GprsConnect_Step+=1;
           }
           else if (strstr(buff,"ERROR"))
           {
               msleep(1000);
               GprsConnect_Step=COPS;
          }
            break;

        case CGDCOND:
            memset(buff,0,BUFFER_SIZE);
           //  msleep(1000);
            qDebug()<<"Input AT+CGDCOND command";
            strcpy(sendData,"AT+CGDCONT=1,\"PPP\",\"3gnet\",,0,0\r\n");
           // strcpy(sendData,"AT+CMEE\r\n");
            memset(buff,0,BUFFER_SIZE);
           // qDebug()<< sendData;
            write(fd,sendData,strlen(sendData));
            qDebug()<< buff;
            memset ( buff , 0 , BUFFER_SIZE );          //清零操作
            receivedLen = read ( fd , buff , 100 );
            //qDebug() << "receivedLen :"<< receivedLen ;
            qDebug()<< buff;
            memset ( sendData , 0 , 100 );
           // msleep(1000);
            if (strstr(buff,"OK"))
            {
                msleep(1000);
                GprsConnect_Step+=1;
                //GprsConnect_Step+=2;
              //  GprsConnect_Step+=3;
               // GprsConnect_Step+=5;
            }
            else if (strstr(buff,"ERROR"))
            {
                msleep(1000);
                GprsConnect_Step=CGDCOND;
           }
            break;

        case CFUN://联网命令，为0为飞行模式，为1为连接运营商模式
            memset(buff,0,BUFFER_SIZE);
             msleep(1000);
            qDebug()<<"Input AT+CFUN command";
            strcpy(sendData,"AT+CFUN=1\r\n");
            memset(buff,0,BUFFER_SIZE);
            write(fd,sendData,strlen(sendData));
            memset ( buff , 0 , BUFFER_SIZE );          //清零操作
            receivedLen = read ( fd , buff , 100 );
            //qDebug() << "receivedLen :"<< receivedLen ;
            qDebug()<< buff;
           memset ( sendData , 0 , 100 );
             if (strstr(buff,"OK"))
             {
                 msleep(1000);
                 GprsConnect_Step+=1;
             }
             else if (strstr(buff,"ERROR"))
             {
                 msleep(1000);
                 GprsConnect_Step=CFUN;
            }
                 break;

        case CGMR://联网命令，为0为飞行模式，为1为连接运营商模式
            memset(buff,0,BUFFER_SIZE);
             msleep(1000);
            qDebug()<<"Input AT+CGMR command";
            strcpy(sendData,"AT+CGMR\r\n");
            memset(buff,0,BUFFER_SIZE);
            write(fd,sendData,strlen(sendData));
            memset ( buff , 0 , BUFFER_SIZE );          //清零操作
            receivedLen = read ( fd , buff , 100 );
            //qDebug() << "receivedLen :"<< receivedLen ;
            qDebug()<< buff;
           memset ( sendData , 0 , 100 );
             if (strstr(buff,"OK"))
             {
                 msleep(1000);
               //  GprsConnect_Step+=1;
             }
             else if (strstr(buff,"ERROR"))
             {
                 msleep(1000);
                 GprsConnect_Step=CGMR;
            }
                 break;


    //222.197.174.69
    #ifdef CALL
        case ATD:
            memset(buff,0,BUFFER_SIZE);
             msleep(1000);
            qDebug()<<"Input phone number command";
            strcpy(sendData,"ATD15528312533;\r\n");
          //  memset(buff,0,BUFFER_SIZE);
            write(fd,sendData,strlen(sendData));
            memset ( buff , 0 , BUFFER_SIZE );          //清零操作
             receivedLen = read ( fd , buff , 100 );
           // qDebug() << "receivedLen :"<< receivedLen ;
            qDebug()<< buff;
           memset ( sendData , 0 , 100 );
            msleep(1000);
            Call_Number =+1;
//            while(Call_Number>3)
//            {
//                memset(buff,0,BUFFER_SIZE);
//                strcpy(sendData,"AT+CHUP\r\n");
//                memset ( sendData , 0 , 100 );
//            }
            //GprsConnect_Step+=1;
            break;

    #endif

    #ifdef GPRS
        case IPINIT:
            memset(buff,0,BUFFER_SIZE);
             msleep(1000);
            qDebug()<<"Connection Initialization";
             //write(fd,at_gprs,sizeof(at_gprs));
             //qDebug()<<"at_gprs:"<<hex<< at_gprs;
            //strcpy(sendData,"ATD%Rr\n");
           strcpy(sendData,"AT%RSSI");
            memset(buff,0,BUFFER_SIZE);
            write(fd,sendData,strlen(sendData));

             memset ( buff , 0 , BUFFER_SIZE );          //清零操作
             receivedLen = read ( fd , buff , 100 );
           // qDebug() << "receivedLen :"<< receivedLen ;
            qDebug()<< buff;
           memset ( sendData , 0 , 100 );


            if (strstr(buff,"CONNECT") )
            {
                msleep(1000);
             //   return TRUE;
               // data->Connect_Success = 1;
                //Connect_Success = 1;
                GprsConnect_Step+=3;
            }
            else if (strstr(buff,"ERROR") || strstr(buff,"NO CARRIER"))
            {
                msleep(1000);
                //Connect_Success = 0;
                return FALSE;
                GprsConnect_Step=IPINIT;

           }
                break;

        case IPSTART:
            memset(buff,0,BUFFER_SIZE);
             msleep(1000);
            qDebug()<<"Connection Start";
             //write(fd,at_gprs,sizeof(at_gprs));
             //qDebug()<<"at_gprs:"<<hex<< at_gprs;
            strcpy(sendData,"AT+CGACT=1,1\r\n");
          //  memset(buff,0,BUFFER_SIZE);
            write(fd,sendData,strlen(sendData));
            memset ( buff , 0 , BUFFER_SIZE );          //清零操作
             receivedLen = read ( fd , buff , 100 );
           // qDebug() << "receivedLen :"<< receivedLen ;
            qDebug()<< buff;
           memset ( sendData , 0 , 100 );
            msleep(1000);
            if (strstr(buff,"OK") )
            {
                msleep(1000);
                     Connect_Success = 1;
            }

          //  GprsConnect_Step+=1;
            break;

    #endif

        default:
            break;
    }
    }  while (Connect_Success == 0);

    qDebug()<<"Connect_Success:"<<Connect_Success;
    close ( fd0 );
    close ( fd1 );
    close ( fd2 );
    close ( fd3 );
    close ( fd4 );
    close ( fd5 );
    return TRUE;
}
