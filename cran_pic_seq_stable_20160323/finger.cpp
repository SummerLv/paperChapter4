#include "finger.h"
#define BUFFER_SIZE 200
#define DATA_TRANSFER_COM_0 0
#define DATA_TRANSFER_COM_1 1
#define DATA_TRANSFER_COM_2 2
#define DATA_TRANSFER_COM_3 3
#define DATA_TRANSFER_COM_4 4
#define DATA_TRANSFER_COM_5 5
#define DATA_TRANSFER_COM_6 6
#define DATA_TRANSFER_COM_7 7
#define DATA_TRANSFER_COM_8 8
#define FINGER_COM 2

char buff [BUFFER_SIZE] ;
qint32 fd;

/*****指纹收集构造函数******/
Finger_Gather::Finger_Gather(QObject *parent) :
    QObject(parent)
{
    globalData = GlobalData::getGlobalData();

   // usbComTool = new UsbComTool;
   // usbComTool->GetUsbCom();

    QVector<QString>::size_type size;
   //const char **temp_finger = usbComTool->GetComNameFromFile(size);
   qDebug()<<"size:"<<size;
    //qDebug()<<"data"<<temp_finger[0];
  // fprintf(stderr,"data0:%s\n",temp_finger[0]);
 //  fprintf(stderr,"data1:%s\n",temp_finger[1]);
 //  fprintf(stderr,"data2:%s\n",temp_finger[2]);

    if ( ( fd = openPort( FINGER_COM,0) ) < 0 )
  // if((fd = usbComTool->dynamicOpenPort(temp_finger ,size-1 , size))<0)
    {      //打开串口,如果打开失败，会返回-1
        qDebug() << "Open Finger Serial port failed";

    }

    if (setComConfig( fd , 57600 , 8 , 'N' , 1 ) < 0 ){//打开端口错误，返回-1
        qDebug() << "Set COM failed";

    }
}

Finger_Gather::~Finger_Gather()
{
    close(fd);
}

/*************double数据转换成数组数据*********/
void Finger_Gather::Double_To_Array(quint8 *p, int value)
{
    quint32 TempValue;
    TempValue = value;
    *p =((TempValue >> 8) & 0xFF);
     p++;
    *p = (TempValue & 0xFF);
}
/**************删除模板************/
void Finger_Gather::FingerData_Delete(quint32 address, quint8 num)
{
    quint16 crc_sum = 0;
    quint8 len = 0;

    char sendData[16] = {0xef , 0x01, 0xff, 0xff, 0xff, 0xff, 0x01, 0x00, 0x07
                                      , 0x0c, 0x00, 0x00,0x00,0x00,0x00,0x00};
    char buff [BUFFER_SIZE] ;

    quint8 crc_char[2] = {0,0};
    quint8 address_char[2];
    quint8 num_char[2];

    Double_To_Array(address_char , address);
    sendData[10] = address_char[0];
    sendData[11] = address_char[1];

    Double_To_Array(num_char , num);
    sendData[12] = num_char[0];
    sendData[13] = num_char[1];

    for (quint8 i = 6;i<14;i++)
    {
        crc_sum = crc_sum + sendData[i];
    }

    Double_To_Array(crc_char , crc_sum);
    sendData[14] = crc_char[0];
    sendData[15] = crc_char[1];

    write(fd,sendData,16);

    memset(buff,0,BUFFER_SIZE);

    for(quint8 i = 0;i<12;i++)
    {
       read(fd,&buff[i],1);
       len++;
    }
    qDebug()<<"len****"<<len;
}

/***********读储存指纹数目****************/
void Finger_Gather::read_finger_number()
{
    quint16 crc_sum = 0;
    quint8 len = 0;

    char sendData[12] = {0xef , 0x01, 0xff, 0xff, 0xff, 0xff, 0x01, 0x00, 0x03
                                      , 0x1d, 0x00, 0x00};

    char buff [BUFFER_SIZE] ;
    quint8 crc_char[2] = {0,0};


    for (quint8 i = 6;i<10;i++)
    {
        crc_sum = crc_sum + sendData[i];
    }

    Double_To_Array(crc_char , crc_sum);
    sendData[10] = crc_char[0];
    sendData[11] = crc_char[1];

   // msleep(1000);
//    for (quint8 i = 0;i<12;i++)
//    {
//        qDebug()<< "senddata"<<hex<<(quint8)sendData[i];
//    }

    write(fd,sendData,12);

    memset(buff,0,BUFFER_SIZE);

    for(quint8 i = 0;i<14;i++)
    {
       read(fd,&buff[i],1);
       len++;
    }
    memset(sendData , 0 , 12);
    for(quint8 i = 0 ; i<14;i++)
    {
        qDebug()<<"buff"<<hex<<(quint8)buff[i];
    }
    //qDebug()<<"len****"<<len;
}
/****************读参数*********************/
void Finger_Gather::read_sys_para()
{
    quint16 crc_sum = 0;
    quint8 len = 0;

    char sendData[12] = {0xef , 0x01, 0xff, 0xff, 0xff, 0xff, 0x01, 0x00, 0x03
                                      , 0x0f, 0x00, 0x00};

    char buff [BUFFER_SIZE] ;
    quint8 crc_char[2] = {0,0};


    for (quint8 i = 6;i<10;i++)
    {
        crc_sum = crc_sum + sendData[i];
    }

    Double_To_Array(crc_char , crc_sum);
    sendData[10] = crc_char[0];
    sendData[11] = crc_char[1];

   // msleep(1000);
    for (quint8 i = 0;i<12;i++)
    {
        qDebug()<< "senddata"<<hex<<(quint8)sendData[i];
    }

    write(fd,sendData,12);

    memset(buff,0,BUFFER_SIZE);

    for(quint8 i = 0;i<28;i++)
    {
       read(fd,&buff[i],1);
       len++;
    }
    memset(sendData , 0 , 12);
    for(quint8 i = 0 ; i<28;i++)
    {
        qDebug()<<"buff"<<hex<<(quint8)buff[i];
    }
    //qDebug()<<"len****"<<len;
}

/******************指纹按键触发后信息处理**********/
void Finger_Gather::finger_data_process()
{

        quint16 crc_sum = 0;
        quint8 len = 0;

        char sendData[16] = {0xef , 0x01, 0xff, 0xff, 0xff, 0xff, 0x01, 0x00, 0x07
                                          , 0x20, 0x00, 0x00, 0x00, 0xa0, 0x00, 0x00};

        quint8 crc_char[2] = {0,0};


       // read_sys_para();
        read_finger_number();

        memset(buff,0,BUFFER_SIZE);

        for (quint8 i = 6;i<14;i++)
        {
            crc_sum = crc_sum + sendData[i];
        }

        Double_To_Array(crc_char , crc_sum);
        sendData[14] = crc_char[0];
        sendData[15] = crc_char[1];

        write(fd,sendData,16);

        memset(buff,0,BUFFER_SIZE);

        for(quint8 i = 0;i<16;i++)
        {
           read(fd,&buff[i],1);
           len++;
        }

        memset(sendData , 0 , 16);

        for(quint8 i = 0 ; i<16;i++)
        {
            qDebug()<<hex<<(quint8)buff[i];
        }

       // qDebug()<<"len****"<<len;
        finger_press_process();
}
/************将下发的指纹版本ID存储在数据库里,先清空，再发请求储存帧*****************/
int Finger_Gather::FingerData_Store_InModule(char *set_buf , quint8 Finger_Store_Start_Flag , quint8 Finger_num)
{
    char buff1 [13] ;

    quint16 len;

    quint8 Finger_Store_Flag = Finger_Store_Start_Flag;

    quint8 crc_char[2] = {0,0};
    quint16 crc_sum = 0;

    char sendData_store[14] = {0xef , 0x01, 0xff, 0xff, 0xff, 0xff, 0x01, 0x00, 0x05
                                      , 0x32, 0x00, 0x00, 0x00, 0x00};
    //char sendData_clean[12] = {0xef , 0x01, 0xff, 0xff, 0xff, 0xff, 0x01, 0x00, 0x03
    //                                  , 0x0d,  0x00, 0x00};

    while(Finger_Store_Flag == 0)
    {
//        switch(Send_Data_Flag)
//        {
//            case 0://清空指纹库
//            if(Finger_num == 0x00)
//        {
//            crc_sum = 0;
//            len = 0;
//            for (quint8 i = 6;i<10;i++)
//            {
//                crc_sum = crc_sum + sendData_clean[i];
//            }

//            Double_To_Array(crc_char , crc_sum);
//            sendData_clean[10] = crc_char[0];
//            sendData_clean[11] = crc_char[1];

//           // qDebug()<<"clean crc"<<hex<<crc_char[0]<<crc_char[1];

//                 write(fd,sendData_clean,12);

//                 memset(buff1,0,13);
//                 for(quint8 i = 0;i<12;i++)
//                 {
//                    read(fd,&buff1[i],1);
//                    len++;
//                 }
//                   for(quint8 i =0;i<12;i++)
//                    {
//                      qDebug()<<hex<<(quint8)buff1[i];
//                    }

//                if((buff1[9] == 0x00) && (buff1[6] ==0x07))
//                {
//                    qDebug()<<"***clean***";
//                }
//                else
//                   qDebug()<<"*****clean error*****";
//        }
//           Send_Data_Flag += 1;
//                break;
//        case 1://存储请求指令
//        {
            crc_sum = 0;
            len = 0;

            sendData_store[11] = Finger_num;

            qDebug()<<"***sendData_store[11]***"<<(quint8)sendData_store[11];

            for (quint8 i = 6;i<12;i++)
            {
                crc_sum = crc_sum + sendData_store[i];
            }

            Double_To_Array(crc_char , crc_sum);
            sendData_store[12] = crc_char[0];
            sendData_store[13] = crc_char[1];

            write(fd,sendData_store,14);

            memset(buff1,0,13);

            for(quint8 i = 0;i<12;i++)
            {
               read(fd,&buff1[i],1);
               len++;
            }

           if((buff1[9] == 0x00) && (buff1[6] == 0x07))//如果发送指纹储存请求回应为OK时，将标志位置一
           {
               qDebug()<<"***store begin***";
               Finger_Store_Flag = 1;
           }
        }
//            break;
//        default:
//            break;
//        }
//    }

    if ( Finger_Store_Flag == 1)
    {
         write(fd,set_buf,139);
        Finger_Store_Flag = 2;
    }
    return Finger_Store_Flag;
}

/************将下发的指纹版本ID存储在数据库里 后续*****************/
 void Finger_Gather::FingerData_Store_InModule(char *set_buf,quint16 partion)
{
    char buff [13] ;
    quint16 len = 0;
    quint8 part = partion;//指纹片号
    qDebug()<<"store in module";

        write(fd,set_buf,139);
        memset(buff,0,13);
        if (part == 3)
        {
         for(quint8 i = 0;i<12;i++)
           {
            read(fd,&buff[i],1);
            len++;
          }

 //        qDebug()<<"len"<<len;
         for(quint8 i =0;i<12;i++)
         {
             qDebug()<<hex<<(quint8)buff[i];
         }

         if(buff[9] == 0x00)
         {
             qDebug()<<"store OK";
         }
      }

}

/******检查是否按键********/
void Finger_Gather::finger_press_process()
{
    quint8 buff_request[2] = {0x00};
    buff_request[0] = buff[9];
    buff_request[1] = buff[8];

    qDebug()<<"bufff 11"<<(quint8)buff[11];
    switch (buff_request[0])
    {
        case 0x00:
        if(buff_request[1] == 0x07)
        {
            globalData->fingerPage[1] = (quint8)buff[10];
            globalData->fingerPage[0] = (quint8)buff[11];
        }
            qDebug()<<"****receive ok****";
            break;
        case 0x01:
             qDebug()<<"******receive error****";
             break;
        case 0x02:
            qDebug()<<"***no finger****";
            break;
        default:
            qDebug()<<"******receive error****";
            break;
    }
}
