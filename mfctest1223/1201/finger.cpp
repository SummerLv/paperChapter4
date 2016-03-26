#include "finger.h"
#include <QTimer>
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

/*****指纹收集构造函数******/
//Finger_Gather::Finger_Gather(QObject *parent) :
//    QObject(parent)
Finger_Gather::Finger_Gather():QThread()
{
        globalData = GlobalData::getGlobalData();


        if ( ( fd = openPort( FINGER_COM) ) < 0 )
        { //打开串口,如果打开失败，会返回-1
            qDebug() << "Open Serial port failed,FINGER_COM";
        }
        else
        {
            qDebug()<<"open FINGER_COM success***      ****"<<fd;
        }

//        int flags = fcntl(fd, F_GETFL, 0);
//        if( fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0 )
//        {
//            qDebug()<<"*** change the FINGER_COM to NONBLOCK  failed *** " ;
//        }

        if (setComConfig( fd , 57600 , 8 , 'N' , 1 ) < 0 )
        {//打开端口错误，返回-1
            qDebug() << "Set COM failed,FINGER_COM";
        }
         qDebug()<<"set    FINGER_COM success***      ****";

         //@1128 指纹 线程
         timer = new QTimer();
         connect(timer, SIGNAL(timeout()),this, SLOT(testTimer()));

         //gpio 操作  @1216
         gpio =new GpioCtl();


}

Finger_Gather::~Finger_Gather()
{
    close(fd);
    delete  gpio;
}

void Finger_Gather::run()
{
    // 1 -- timeout() , fingerprint TEST
    // 2 -- begin 3 times test

    while(1)
        {
                msleep(1000);
                testTimer();
                  //use  timer to fullfill the target
                  //之后，每隔一段时间采集一次指纹
                  //  timer->start(1000);//1 min  START() ms

         }

}

void Finger_Gather::testTimer()
{
    //qDebug()<<"testTimer  use  timeout---slot    5  second";
    //qDebug()<<"*****************";
                    if(checkScreenTouch() == 1)
                    {
                            finger_data_process();              //对模块读写
                            if (!finger_press_process())        //返回值处理
                            {
                                qDebug()<<"ok  PASS!!!!!";
                                globalData->fingerChance = 5;
                                return ;
                                //fingerChance = 0;
                            }
                            else
                            {
                                qDebug()<<"Error  Finger  Process ";
                            }
                    }

}


int Finger_Gather::checkScreenTouch()
{
    unsigned int x=0;
    unsigned int x2 = 0;


         gpio->gpio_get_value(FGGPIO,x) ;
        //qDebug()<<"the x value is :\t"<<x;
        if(x == 1)
           {
                msleep(200);
                gpio->gpio_get_value(FGGPIO,x2) ;
                //qDebug()<<"the x2 value is :\t"<<x2;
                if(x2 == 1)
                {
                    qDebug()<<"somebody  touch the  fingerScreen";
                    return 1;
                }
            }

        return 0;
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

    qDebug()<<"**********read_finger_number*****************";

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

   if( write(fd,sendData,12) > 0)
       qDebug()<<"write success ***";
   else
       qDebug()<<"write fail**";

    memset(buff,0,BUFFER_SIZE);

    for(quint8 i = 0;i<14;i++)
    {
        read(fd,&buff[i],1);
       len++;
    }
    memset(sendData , 0 , 12);
    for(quint8 i = 0 ; i<14;i++)
    {
        qDebug()<<i<<"\t"<<hex<<(quint8)buff[i];
    }
    //qDebug()<<"len****"<<len;
}
/****************读系统参数*********************/
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

        //read_finger_number(); // readFingerNumber  in  the  module

        char sendData[16] = {0xef , 0x01, 0xff, 0xff, 0xff, 0xff, 0x01, 0x00, 0x07
                                          , 0x20, 0x00, 0x00, 0x00, 0xa0, 0x00, 0x00};
        //使用此指令自动完成搜索，若搜索到则返回序号
        quint8 crc_char[2] = {0,0};

        memset(buff,0,BUFFER_SIZE);

        for (quint8 i = 6;i<14;i++)
        {
            crc_sum = crc_sum + sendData[i];
        }

        Double_To_Array(crc_char , crc_sum);
        sendData[14] = crc_char[0];
        sendData[15] = crc_char[1];

        if(write(fd,sendData,16)<0)
            qDebug("0x20  search cmd send failed ");
        else
            qDebug("0x20  search cmd send successed");

        memset(buff,0,BUFFER_SIZE);

        for(quint8 i = 0;i<16;i++)
        {
            read(fd,&buff[i],1);
           len++;
        }
        qDebug()<<"0x20 readAnswer   0 got it ,1  bad package ,9 no found    \t\t"<<quint8(buff[9]);

        for(quint8 i = 0 ; i<16;i++)
        { 
            qDebug()<<i<<"\t\t"<<hex<<(quint8)buff[i];
        }

        //@1208  注释掉
       // finger_press_process();
}

/******检查是否按键********/
int Finger_Gather::finger_press_process()
{
    quint8 buff_request[2] = {0x00};
    buff_request[0] = buff[9];
    buff_request[1] = buff[8];

    qDebug()<<"bufff 11"<<(quint8)buff[9];
    switch (buff_request[0])
    {
        case 0x00:
        if(buff_request[1] == 0x07)
        {
            globalData->fingerPage[1] = (quint8)buff[10];
            globalData->fingerPage[0] = (quint8)buff[11];
        }
            qDebug()<<"****receive ok****PASS";
            //dummy(); // 播放提示指纹验证语音
            //break;
        return  0;

        /*ase 0x01:
             qDebug()<<"******receive error****Package Error";
             //dummy(); // 播放提示指纹验证语音
             //break;
             return 1;
        case 0x02:
            qDebug()<<"***no finger****";
            //dummy(); // 播放提示指纹验证语音
            //break;
             return 2;*/
        default:
            qDebug()<<"******receive error****";
            //dummy(); // 播放提示指纹验证语音
           // break;
            globalData->fingerPage[0] = 0xa0;
             return 3;
    }
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

    while(Finger_Store_Flag == 0)
    {
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

           if((buff1[9] == 0x00) && (buff1[6] == 0x07))//如果发送指纹储存请求回应为OK,标志位置1
           {
               qDebug()<<"***store begin***";
               Finger_Store_Flag = 1;
           }
        }

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


int Finger_Gather::readTemple()
{
        int fd;
        fd = open("FGTemplet0.mb",O_RDONLY);
        if(fd<0)
            qDebug()<<"open FAILED"<<fd<<endl;
        else
            qDebug()<<"open successed"<<fd<<endl;

        int rdNum;
        //char fingerData[FINGERDATALEN];
        rdNum = read(fd,fingerData,FINGERDATALEN);
        if(rdNum<0)
            qDebug()<<"read  failed"<<rdNum<<endl;
        else
            qDebug()<<"read successed"<<rdNum<<endl;

//        for(quint16 i=0;i<512;++i)
//       {
//            qDebug()<<quint8(fingerData[i])<<"\t\t"<<i;
//        }

    return rdNum;

}



void  Finger_Gather::storeTemple()
{
    char answer1[13] ;
    char answer2[13];
    char answer[13];
    quint16 len=0;
    quint16 len2=0;

    quint8 crc_char[2] = {0,0};
    quint16 crc_sum = 0;
    int crc_sum2=0;

    read_finger_number();

    //0X32  。。上位机下载特征文件到模块的一个缓冲区,并存入指定位置
    char sendData_store[14] = {0xef , 0x01, 0xff, 0xff, 0xff, 0xff, 0x01, 0x00, 0x05
                                      , 0x32, 0x00, 0x00, 0x00, 0x00};

    for (quint8 i = 6;i<12;i++)
    {
        crc_sum = crc_sum + sendData_store[i];
    }

    Double_To_Array(crc_char , crc_sum);
    sendData_store[12] = crc_char[0];
    sendData_store[13] = crc_char[1];

    if(write(fd,sendData_store,14)<0)
        qDebug()<<"ox32 write cmd  failed ";
    else
        qDebug()<<"ox32 write cmd  successed ";
   //******************up  1*************************************
    memset(answer1,0,13);

    for(quint8 i = 0;i<12;i++)
    {
       read(fd,&answer1[i],1);
       len++;
       qDebug()<<i<<"\t\t"<<hex<<quint8(answer1[i])<<"\t\t"<<fd;
    }

   if((answer1[9] == 0x00) && (answer1[6] == 0x07))//如果发送指纹Template储存请求回应为OK时
   {
       qDebug()<<"***answer1    ok   store begin***";
        qDebug()<<quint8(answer1[6])<<"\t\t"<<quint8(answer1[9]);
   }
   else
      qDebug()<<"***answer1    problem accured***";

//******************down  1*************************************
    if(readTemple()<0)
        qDebug("read temple data  fail");
    else
        qDebug("read temple data    success");


   quint16 crcSun=0;
   quint16   pFingerData=0;

   for(quint8 n=0;n<3;++n)
   {
       quint8   length=0;
       qDebug()<<"pFingerData\t\t"<<pFingerData<<"crcSum\t\t"<<crcSun;
       qDebug("enter the loop ");
       char sendData[139] = {0xef , 0x01, 0xff, 0xff, 0xff, 0xff, 0x02, 0x00, 0x82};
       for(quint8 m=0;m<128;++m)
       {
            sendData[9+m] = fingerData[pFingerData+m];
            crcSun = crcSun + fingerData[pFingerData+m];
       }
       pFingerData =pFingerData + 128;
       crcSun  = crcSun  + 132;
       memset(crc_char,0,2);
       Double_To_Array(crc_char,crcSun);
       sendData[137] = crc_char[0];
       sendData[138] = crc_char[1];

       qDebug()<<"write  data        "<<write(fd,sendData,139);

       memset(sendData,0,139);
       crcSun = 0;
   }

   qDebug()<<"pFingerData\t\t"<<pFingerData<<"crcSum\t\t"<<crcSun;
   char sendData4[139] = {0xef , 0x01, 0xff, 0xff, 0xff, 0xff, 0x08, 0x00, 0x82};
   for(quint8 m=0;m<128;++m)
   {
        sendData4[9+m] = fingerData[pFingerData+m];
        crcSun = crcSun + fingerData[pFingerData+m];
   }

   crcSun  = crcSun  + 138;
   memset(crc_char,0,2);
   Double_To_Array(crc_char,crcSun);
   sendData4[137] = crc_char[0];
   sendData4[138] = crc_char[1];

   qDebug()<<"write  data4        "<<write(fd,sendData4,139);


//******************up  2*************************************
   memset(answer2,0,13);

   for(quint8 i = 0;i<12;i++)
   {
      read(fd,&answer2[i],1);
      len2++;
      qDebug()<<quint8(answer2[i])<<"\t\t"<<i;
   }

  if((answer2[9] == 0x00) && (answer2[6] == 0x07))//如果发送指纹储存请求回应为OK时
  {
       qDebug()<<"***answer2    ok   store finished  ???   ***";
       qDebug()<<quint8(answer2[6])<<"\t\t"<<quint8(answer2[9]);
  }
  else
     qDebug()<<"***answer2    problem accured***";
//******************down 2*************************************

  read_finger_number();
  getTemple();
}

void  Finger_Gather::getTemple()
{
    int  crcSum;
    quint8  crcChar[2] = {0x00,0x00};
    char  readModule[15]={0xef , 0x01, 0xff, 0xff, 0xff, 0xff, 0x01, 0x00, 0x06,
                                  0x07,0x01,0x00,0x00,0x00,0x00};
    for(quint8 i=6;i<13;++i)
        crcSum = crcSum + readModule[i];

    Double_To_Array(crcChar,crcSum);
    readModule[13] = crcChar[0];
    readModule[14] = crcChar[1];

    if(  write(fd,readModule,15) < 0   )
        qDebug()<<"write  cmd__readModule  failed  ";
    else
        qDebug()<<"write  cmd__readModule  successed  ";
    //=================================================================
    char  answer1[12] ;
    //int len;
    for(int  i=0;i<12;++i)
        {
                read(fd,&answer1[i],1);
                qDebug()<<i<<"\t\t"<<hex<<quint8(answer1[i]);
        }
    if( ! ( ( answer1[6] == 0x07 ) &&  (answer1[9] == 0x00 )))
            {
                qDebug()<<"read temple from module  failed ";
                qDebug()<<"6\t\t"<<hex<<quint8(answer1[6]);
                qDebug()<<"9\t\t"<<hex<<quint8(answer1[9]);
            }
    else
            qDebug()<<"read temple from module  successed ";
     //=================================================================
    crcSum=0;
    memset(crcChar,0,2);
    char  readBuf1[13]={0xef , 0x01, 0xff, 0xff, 0xff, 0xff, 0x01, 0x00, 0x04,
                                  0x08,0x01,0x00,0x00};
    for(quint8 i=6;i<13;++i)
        crcSum = crcSum + readBuf1[i];

    Double_To_Array(crcChar,crcSum);
    readBuf1[11] = crcChar[0];
    readBuf1[12] = crcChar[1];

    if(  write(fd,readBuf1,13) < 0   )
        qDebug()<<"write  cmd__readbuf1  failed  ";
    else
        qDebug()<<"write  cmd__readbuf1  successed  ";

     //=================================================================
    char  returnTemple[512];
    char  receiveBuf[139];
    int pReturnTemple = 0;
    memset(crcChar,0,2);
    int crc;
    for(int i=0;i<4;++i)
    {
        memset(returnTemple,0,512);
        memset(receiveBuf,0,139);
        memset(crcChar,0,2);
        crc = 0;

        read(fd,receiveBuf,139);
        for(int m=6 ;m<137;++m)
            {
                crc = crc + receiveBuf[m];
            }
        if(i==3)
                {
                    crc= crc+6;
                }
        crc = 0xFFFF & crc;
        Double_To_Array(crcChar,crc);
        if(! (( crcChar[0]==receiveBuf[137] ) && (crcChar[1]==receiveBuf[138]) ))
               {
                     qDebug()<<"crc   NO  pass !!!"<<"\t\t"<<i;
                   //  return ;
               }
        else
               {
                    qDebug()<<"crc   pass !!!""\t\t"<<i;
                    for(int n=9;n<137;++n)
                        returnTemple[pReturnTemple+n-9]=receiveBuf[n];
               }
          pReturnTemple=pReturnTemple + 128;
    }
    //===================receive 4  ====================================

    for(int j=0;j<50;++j)
        qDebug()<<j<<"\t\t"<<quint8(returnTemple[j]) ;

}


