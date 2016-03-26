//==============  version_3  430_Zhang


//==============  version_2   430_Zhou  ( Modlebus )
#include "datagather.h"
const char addressValue = 0x01;
const char readNumber= 0x07;
const char functionNumber = 0x04;
const int dataLen =  readNumber *2 + 5;
const short dataSize = readNumber *2;//纯数据大小
const int buffSize = 40;  //zhao zhen tou
const float angleConst = 360.0/8000;
const float windConst = 30.0/8000;
const float axisConst = 25.5/4000;

char encBuff[100];
int  encLen = 0;

const int DATA_TRANSFER_COM = 1;    // now, the dataGather Port is com_1

static int  flag = 0;
char buffarray[buffSize];
//@0123
const int checkConst = 8;
static int  flagCheck = 0;
char checkBuff[checkConst];

int timeSend = 0;
int timeRec = 0;

DataGather::DataGather()
    :QThread()
{
      globaldata = GlobalData::getGlobalData();
      gpioCtl = new GpioCtl();

      if ( ( fd = openPort( DATA_TRANSFER_COM,0) ) < 0 )
      {      //打开串口
          qDebug() << "Open failed datagather";
          return ;
      }
      else
          qDebug()<<"open com_1  dataGather success  dataGather_Fd is ******\t\t" <<  fd;
      if (setComConfig( fd , 38400 , 8 , 'N' , 1 ) < 0 )
      {
          qDebug() << "Set COM failed datagather";
          return;
      }
}

DataGather::~DataGather()
{
    close(fd);
    delete gpioCtl;
}

void DataGather::run()
{

        int loop = 0;
        bool checkResult = true;  // 1--true: Normal;      0--false:  Abnormal
        while(1)
        {
            for(loop=0; loop<100; loop++)
            {
                writerun();
            }
            //qDebug()<<"^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^";


           checkResult = checkElectry();
           qDebug()<<"check  result :\t"<<checkResult;
            if(false==checkResult)  // Abnormal
            {
                //还需要增加断电现场保护
                /*
                 *实时传感器数据
                 *图像数据
                */
                shutDown();
                checkResult = true;
            }

           msleep(1000);


    }

}
/*****读数据线程******/


/******从端口中读入数据，进行拼接，截取符合数据报格式的数据报进行解析*******/
void DataGather::readrun()
{
}

void DataGather::SplThrdCommand(char *buff,int shift)
{
//    globaldata->craneList[0].angle = (  ((unsigned int) buff[shift+3]<<8) +(unsigned int)buff[shift+4] - 2000 ) * angleConst;

//    globaldata->craneList[0].windspeed = ( ( (unsigned int)buff[shift+5] << 8) + (unsigned int)buff[shift+6] - 2000) * windConst;

//    unsigned int tmpADWeight =( ( (unsigned int)buff[shift+7] << 8 ) + (unsigned int)buff[shift+8] - 2000);
//    unsigned int tmpADHeight = ( ( (unsigned int)buff[shift+9] << 8 ) + (unsigned int)buff[shift+10] - 2000);
//    unsigned int tmpADSpan = ( ( (unsigned int)buff[shift+11]  << 8 )+ (unsigned int)buff[shift+12] - 2000);

//    unsigned int tmpAxis_X = ( ( (unsigned int)buff[shift+13] << 8 ) + (unsigned int)buff[shift+14] - 2000);
//    unsigned int tmpAxis_Y = ( ( (unsigned int)buff[shift+15] << 8 ) + (unsigned int)buff[shift+16] - 2000);

    //01 angle
    int angleData =   (buff[shift+3]<<8) +buff[shift+4] - 2000 ;
     globaldata->craneList[0].angle= dataSafer(angleData) * angleConst;
    //02  speed
     int  windSpeed = ( buff[shift+5] << 8) + buff[shift+6] - 2000;
     globaldata->craneList[0].windspeed = dataSafer(windSpeed)  * windConst;
    //03  weight
    unsigned int tmpADWeight =dataSafer(( buff[shift+7] << 8 ) + buff[shift+8] - 2000);
    //04  height
    unsigned int tmpADHeight = dataSafer(( buff[shift+9] << 8 ) + buff[shift+10] - 2000);
    //05  span
    unsigned int tmpADSpan = dataSafer(( buff[shift+11]  << 8 )+ buff[shift+12] - 2000);
    //06--1  axis_x
    unsigned int tmpAxis_X = dataSafer(( buff[shift+13] << 8 ) + buff[shift+14] - 2000);
    //06--2 axis_y
    unsigned int tmpAxis_Y = dataSafer(( buff[shift+15] << 8 ) + buff[shift+16] - 2000);

    Axis_Data_Proc(tmpAxis_X,tmpAxis_Y);
    Ad_Proc(tmpADWeight,tmpADHeight,tmpADSpan);
}

void DataGather::Axis_Data_Proc(unsigned int tmpAxis_X,unsigned int tmpAxis_Y)
{
    int axisXStandard = tmpAxis_X-4000;
    int axisYStandard = tmpAxis_Y-4000;
    float axisx,axisy;

    axisx = axisXStandard * axisConst;
    globaldata->craneList[0].tiltangleX = axisx;

    axisy =  axisYStandard * axisConst;
    globaldata->craneList[0].tiltangleY = axisy;
}

void DataGather::Ad_Proc(unsigned int tmpADweight , unsigned int tmpADheight , unsigned int tmpADpan){
    Weight_Data_Proc(tmpADweight);
    Height_Data_Proc(tmpADheight);
    Span_Data_Proc(tmpADpan);
}

void DataGather::Weight_Data_Proc(unsigned int tmpADweight){
    double w;
    w = globaldata->basicData.craneRate * globaldata->craneKD.weightK * tmpADweight + globaldata->craneKD.weightD;
    if(w < 0.09)
        w = 0;
    w = w > 99 ? 99 : w;
    globaldata->craneList[0].wight = w;    //先查看实时传感器数据

    if(globaldata->craneList[0].wight < 0.09)
        globaldata->craneList[0].wight = 0;
    globaldata->craneList[0].wight = globaldata->craneList[0].wight > 99? 99 : globaldata->craneList[0].wight;
}

void DataGather::Height_Data_Proc(unsigned int tmpADheight){
    double h;
    double temcraneheight;
    temcraneheight = globaldata->basicData.preSectionHeight * globaldata->basicData.craneSectionNum+globaldata->basicData.firstSectionHeight;
    h = globaldata->craneKD.heightK * tmpADheight + globaldata->craneKD.heightD;
    if(h<0)
        h = 0;
    h = h > temcraneheight ? temcraneheight : h;
    globaldata->craneList[0].hookHeight = h;    //先查看实时传感器数据
}

void DataGather::Span_Data_Proc(unsigned int tmpADspan){
    float s;
    s = globaldata->craneKD.spanK * tmpADspan + globaldata->craneKD.spanD;
    if(s < 0)
        s = 0;
    s = s > globaldata->basicData.forearmLen ? globaldata->basicData.forearmLen : s;
    globaldata->craneList[0].span = s;       //先查看实时传感器数据
}

/***发送命令***接收数据***解析数据************************************/

void DataGather::writerun()
{


        gpioCtl->gpio_set_value(GPIO430,VALUE_DOWN);
        char thrdCommand[8];
        EncThrdCommand(readNumber,thrdCommand);
    //    if(timeSend++ > 100)
       // {
           // qDebug()<<"flag 0\t"<<flag;
          //  timeSend = 0;
      //  }
        if(flag == 0)
            {
                      //  int n= -5 ;

                        memset(buffarray,0,buffSize);
                        write(fd,thrdCommand,8);   //0.2ms

                        flag = 1;
                        msleep(2);          //  19bytes * 8 bits = 160   / 38400 bit/s  = 5 ms
                        gpioCtl->gpio_set_value(GPIO430,VALUE_UP);

            }
            //qDebug()<<"flag 1\t"<<flag;
        if(flag == 1)
            {

                                int len = 0;
                                msleep(80);

                                //这个时间需要足够长，否则不能收到有效的数据  at least 8ms<<
                                //@1127       80 这个值能够正常运行

                                len = read(fd,buffarray,40);  //0.4ms

                                //qDebug()<<"len"<<len;
                                //  @1201
                                //-----------------------------------------------------------------------------------------------
                                if(encLen< 2*dataLen)
                                {
                                    memcpy(encBuff+encLen,buffarray,len);
                                    encLen += len;
                                }
                                else
                                {
                                    short frameHead = 0;
                                    frameHead = FindFrameHead(encBuff,encLen);
                                     //qDebug()<<"frameHead"<<frameHead;
                                    encLen = 0;
                                    if(encBuff[frameHead+1]  == 0x84)
                                    {
                                        qDebug()<<"receive error";
                                    }
                                    else if(encBuff[frameHead+1] == functionNumber)
                                    {
                                        char temCrc[2];
                                   // qDebug()<<"len"<<len;
                                          globaldata->dataCRC16(encBuff+frameHead,readNumber*2+3,(quint8 *)temCrc);


                                        if((temCrc[0] == encBuff[frameHead+readNumber*2+3]) && (temCrc[1] == encBuff[frameHead+readNumber*2+4]))
                                        {

//                                                                                      for (quint8 i =frameHead;i<frameHead+readNumber*2+4;i++)
//                                                                                       {
//                                                                                         qDebug()<<"****receive****"<<hex<<(quint8)encBuff[i];
//                                                                                       }
//                                                                                     qDebug()<<"*****************************";
                                           // qDebug()<<"receive ok";                       
                                            SplThrdCommand(encBuff,frameHead);
                                        }
                                    }
                                   else
                                   {
//                                          for (quint8 i =0;i<len;i++)
//                                           {
//                                             qDebug()<<"****temp****"<<hex<<(quint8)buffarray[i];
//                                           }
//                                         qDebug()<<"*****************************";
                                   }
                                }
                                //  @1201
                                //-----------------------------------------------------------------------------------------------
                                flag=0;
                                gpioCtl->gpio_set_value(GPIO430,VALUE_DOWN);
            }

}

void DataGather::EncThrdCommand(char readNumber, char *thrdCommand)
{

    char *buff;
    char address = addressValue; //0x01
    char functionType = functionNumber;//0x04
    char startAddress[2] = {0x00,0x00};
    char number[2] = {0x00,readNumber}; //0x07

    int addr = 0;
    addr = globaldata->split_joint_hex_data(thrdCommand,addr,&address,1);
    addr = globaldata->split_joint_hex_data(thrdCommand,addr,&functionType,1);
    addr = globaldata->split_joint_hex_data(thrdCommand,addr,startAddress,2);
    addr = globaldata->split_joint_hex_data(thrdCommand,addr,number,2);

    //  01 04 00 00 00 07

    char crc[2];
    globaldata->dataCRC16(thrdCommand,addr,(quint8*)crc);
    addr = globaldata->split_joint_hex_data(thrdCommand,addr,crc,2);

   buff=thrdCommand;
//    for (quint8 i =0;i<8;i++)
//     {
//             qDebug()<<"****buff***"<<hex<<(quint8)*(buff+i);
//     }
//    qDebug()<<"****************************";

}

int  DataGather::dataSafer(int data)
{
      int reData = 0;

       if(data > 8000)
          reData = 8000;
      else if(data < 0)
          reData = 0;
      else
          reData = data;

      return reData;
}

/****寻找帧头函数，帧头为01 functionNumber dataSize,参考M-3001 modbus手册*****/
int DataGather::FindFrameHead(char *buff , int encLen)
{
    int head = 0;
    bool findSuccess = false;
    while(findSuccess == false)
    {
        if( (buff[head] == 0x01) && (buff[head+1] == functionNumber) && buff[head+2] == dataSize)
        {
            findSuccess = true;
        }
        else
        {
            if( (buff[head] == 0x01) && (buff[head+1] == functionNumber) && buff[head+2] != dataSize)
                head += 2;
           else
                head += 1;
            if(head >= encLen){
                head = -1;
                break;
            }
        }
    }
    return head;
}

bool DataGather::checkElectry(void)
{
    //gpioCtl->gpio_set_value(GPIO430,VALUE_DOWN);
    char checkCommand[8] = {0xFA,0x02,0x55,0x21,0x30,0x41};
    char crc[2] = {0x00};
    globaldata->dataCRC16(checkCommand,6,(quint8*)crc);
    globaldata->split_joint_hex_data(checkCommand,6,crc,2);

  //  qDebug()<<"check command :";
//    int index = 0;
//    for(; index<8; ++index)
//    {
//        qDebug()<<"\t"<<index<<"\t"<<hex<<(quint8)checkCommand[index];
//    }
   // qDebug()<<"*************  check command  ***************";
    //qDebug()<<"flagCheck\t"<<flagCheck;
    if(flagCheck == 0)
        {
                   flagCheck = 1;
                    // memset(checkBuff,0,checkConst);
                    gpioCtl->gpio_set_value(GPIO430,VALUE_DOWN);
                    int lenwrite;
                    lenwrite=write(fd,checkCommand,8);   //0.2ms
                    /*qDebug()<<"len****          "<<lenwrite;
                    if(lenwrite==8)
                    {
                         qDebug()<<"&&&&&&&&&&&&&&         write success";
                    }*/

                    //msleep(2);          //   <2ms
                    /*int i,j,i1;
                    for(i=0;i<1000;i++)
                    {
                        for(i1=0;i1<1000;i1++)
                        {
                            j=0;
                        }
                    }*/
                    usleep(2000);

                    gpioCtl->gpio_set_value(GPIO430,VALUE_UP);

        }

    if(flagCheck == 1)
        {
                            flagCheck=0;
                            int len = 0;
                            //msleep(20);
                            //这个时间需要足够长，否则不能收到有效的数据  at least 8ms<<
                            //@1127       80 这个值能够正常运行

                            //gpioCtl->gpio_set_value(GPIO430,VALUE_UP);
                            int reIndex = 0;

//                            for(; reIndex<8; ++reIndex)
//                            {
//                            qDebug()<<"before  read   return value\t"<<reIndex<<"\t"<<hex<<(quint8)checkBuff[reIndex];
//                            }
                            msleep(2);
                            len = read(fd,checkBuff,8);  //0.4ms
//                            qDebug()<<"len"<<len;
//                            //-----------------------------------------------------------------------------------------------
//                             reIndex = 0;
//                            for(; reIndex<len; ++reIndex)
//                            {
//                            qDebug()<<"return value\t"<<reIndex<<"\t"<<hex<<(quint8)checkBuff[reIndex];
//                            }
                            qDebug()<<"stop  ?\t"<<hex<<(quint8)checkBuff[2];

                            globaldata->dataCRC16(checkBuff,6,(quint8*)crc);
                          //  qDebug()<<"\n \t\t\t checkbuf crc:\t"<<hex<<(quint8)crc[0]<<"\t"<<(quint8)crc[1];

                            //+++++++++++++++++++++++++++++++++++++++++++
                            //if((crc[0]==checkBuff[6])&& (crc[1]==checkBuff[7]))
                            //{
                            //    qDebug()<<"check return crc pass";
                                //if( (checkBuff[0]==0xFA) && (checkBuff[1]==0x02) )
                                if(checkBuff[2]==0xFE)
                                    {
                                        qDebug()<<"electry NORMAL";
                                        return true;
                                    }
                                else if(checkBuff[2]==0xFF)
                                    {
                                        qDebug()<<"electry Abnormal";
                                        return false;
                                    }
                                else
                                    {
                                        qDebug()<<"NOT  ( FE | FF )";
                                        qDebug()<<"!@#$%^&*()";
                                        return true;
                                    }

                               memset(checkBuff,0,8);

//                            }
//                            else
//                            {
//                                qDebug()<<"check return crc Failed   ***********";
//                                return false;
//                            }
                            //++++++++++++++++++++++++++++++++++++++++++

                            //gpioCtl->gpio_set_value(GPIO430,VALUE_DOWN);
        }
}



void DataGather::shutDown(void)
{
    gpioCtl->gpio_set_value(GPIO430,VALUE_DOWN);
    char shutdownCommand[8] = {0xFA,0x00,0x55,0x20,0x30,0x40};
    char crc[2] = {0x00};
    globaldata->dataCRC16(shutdownCommand,6,(quint8*)crc);
    globaldata->split_joint_hex_data(shutdownCommand,6,crc,2);

    qDebug()<<"shut down  command :";
    int index = 0;
    for(; index<8; ++index)
    {
        qDebug()<<"\t"<<index<<"\t"<<hex<<(quint8)shutdownCommand[index];
    }
    qDebug()<<"*************  check command  ***************";

   //memset(checkBuff,0,checkConst);
   write(fd,shutdownCommand,8);   //0.2ms

}




//===========================================================================
//================== version_1  430_YuJia
//===========================================================================
//#include "datagather.h"
//const int MAX_COUNT = 10;
//const int DATA_TRANSFER_COM = 1; //数据端口是  __1
//const int BUFFER_SIZE = 200;
//const int DATA_LEN = 8;
//const float Angle_From_Down = 360.0000/1024;

//DataGather::DataGather()
//    :QThread()
//{
//      globaldata = GlobalData::getGlobalData();
//      gpioCtl = new GpioCtl();

//      if ( ( fd = openPort( DATA_TRANSFER_COM,0) ) < 0 )                 //打开串口
//      {
//          qDebug() << "Open failed datagather***datagether ";
//          return ;
//      }
//      else
//          {
//           qDebug() << "open DATA_TRANSFER_COM  DataGather  : COM1     "<<fd ;
//           qDebug()<<"Open successed datagather***datagether";
//          }

//      ///******************test  close fd here ************
////      close(fd);
////      qDebug()<<"close (fd ) ; here  ,to confirm that   串口占用，导致无法读写，但是可以获得不同的文件描述符";

//      if (setComConfig( fd , 9600 , 8 , 'N' , 1 ) < 0 )                             //设置串口
//      {
//          qDebug() << "Set COM failed ***datagather";
//          return;
//      }
//      else
//       {
//           qDebug()<<"Set COM successed  ***datagather";
//       }
//}

//DataGather::~DataGather()
//{
//    close(fd);
//    qDebug()<<"in the ~DataGather().....close ( fd ); ";

//    delete gpioCtl;
//}

////void DataGather::run(){
////    qDebug()<<"in the datagather::run()";
////    qint8 receivedLen;
////    //char ADCData[DATA_LEN];
////    char buff [10] ;
////    quint8 chackCRC16[2];
////    //quint32 datalen=8;
////    bool set_received = false;
////    int rx_len = 0;
////    int rx_step = 0;
////    int rx_head_received =0;
////    int set_data_len = 0;
////    char ADCdata[BUFFER_SIZE];
////    char rx_buff[BUFFER_SIZE];
////    unsigned int tmpADSpan[MAX_COUNT];
////    unsigned int tmpADWeight[MAX_COUNT];
////    unsigned int tmpADHeight[MAX_COUNT];
//////    unsigned int spanMax = 0;
//////    unsigned int spanMin = 5000;
//////    unsigned int heightMax = 0;
//////    unsigned int heightMin = 5000;
//////    unsigned int weightMax = 0;
//////    unsigned int weightMin = 5000;

//////    unsigned int spanData = 0;
//////    unsigned int heightData = 0;
//////    unsigned int weightData = 0;
//////    unsigned int span=0;
//////    unsigned int height = 0;
//////    unsigned int weight =0;

////    int count=0;
////    //qDebug() << "enter dataprocess";


////    memset ( rx_buff , 0 , BUFFER_SIZE );                         //清零操作
////    memset ( tmpADSpan , 0 , MAX_COUNT );                         //清零操作
////    memset ( tmpADWeight , 0 , MAX_COUNT );                         //清零操作
////    memset ( tmpADHeight , 0 , MAX_COUNT );                         //清零操作

////    while(1)    //使用 while(1) 取代 exec();函数 实现 事件循环
////    {
////    while(set_received == false)
////    {
////        memset ( buff , 0 , 10 );                         //清零操作
////        //memset( ADCdata , 0 , BUFFER_SIZE);
////       // qDebug()<<"run here";
////        receivedLen = read ( fd , buff , 1 );                         //读串口,每次只读一个字节以方便处理
////       // qDebug()<<"receive";
////       // qDebug()<<hex<<(quint8)buff[0];
////       // qDebug()<<rx_len;
////        rx_buff[rx_len]= buff[0];//将每次从串口收到的1个字节提取出来
////        switch(rx_step)
////          {
////          case 0 :
////            if(rx_buff[rx_len]==0xFE)
////            {
////              rx_step=1;
////            }
////            break;
////          case 1:
////            if(rx_buff[rx_len]==0xFB){
////              rx_buff[0]=0xFE;
////              rx_buff[1]=0xFB;
////              rx_len=1;
////              rx_step=0;
////              rx_head_received=1;
////            }
////            else if((rx_buff[rx_len]==0xFA)&&(rx_head_received==1))
////            {
////              //   qDebug()<<"receive ONE";
////              //接收完成
////              rx_head_received=0;
////              rx_step=0;

////              memcpy(ADCdata,rx_buff,rx_len+1);
////              set_received = true;//一帧数据成功收到标志位
////              set_data_len=rx_len+1;
////              rx_len = 0;
////            }
//////            else if((rx_buff[rx_len]==0xFE)&&(rx_head_received==1))
//////            {
//////              rx_len=rx_len-1;
//////              rx_step=0;
//////            }
////            break;
////          default:
////            break;
////          }
////        if( set_received == false){
////          rx_len++;
////          if(rx_len>=BUFFER_SIZE)
////            rx_len=0;
////        }
////    }

////     if(set_received == true) {
////          globaldata->dataCRC16( ADCdata+2, set_data_len-6 ,chackCRC16);              //CRC16校验
////          //   qDebug()<<"chackCRC16[0]"<<hex<<chackCRC16[0];
////        //     qDebug()<<"chackCRC16[1]"<<hex<<chackCRC16[1];
//////             qDebug()<<"ADCdata[set_data_len-4]"<<hex<<(quint8)ADCdata[set_data_len-4];
//////             qDebug()<<"ADCdata[set_data_len-3]"<<hex<<(quint8)ADCdata[set_data_len-3];
////          if(chackCRC16[0]==ADCdata[set_data_len-4] && chackCRC16[1]==ADCdata[set_data_len-3]) {
////             // qDebug()<<"CRC OK";
////              //qDebug()<<hex<<(quint8)ADCdata[6];
////              //qDebug()<<hex<<(quint8)ADCdata[7];
////              globaldata->craneList[0].windspeed = (unsigned int)ADCdata[5] * 0.88;

////              globaldata->craneList[0].angle =( ((unsigned int)ADCdata[6]<<8)+(unsigned int)ADCdata[7] ) * Angle_From_Down;


////              Axis_Data_Proc(ADCdata);

////              tmpADWeight[count] = ( ((unsigned int)ADCdata[14] << 16)+( (unsigned int)ADCdata[15] << 8 ) + (unsigned int)ADCdata[16]);
////              tmpADHeight[count] = ( ((unsigned int)ADCdata[17] << 16 )+( (unsigned int)ADCdata[18] << 8 ) + (unsigned int)ADCdata[19]);
////              tmpADSpan[count] = ( ((unsigned int)ADCdata[20] << 16 ) +( (unsigned int)ADCdata[21]  << 8 )+ (unsigned int)ADCdata[22]);



////              // qDebug() <<(quint8)count<<"tmpADHeight[count]"<<(quint32)tmpADWeight[count];
////              Ad_Proc(tmpADWeight[count] , tmpADHeight[count] , tmpADSpan[count]);
////              //***        tmpADSpan[count] = ( ((unsigned int)ADCdata[7]<<8)+(unsigned int)ADCdata[8]);  //变幅AD值
////            //  qDebug()<<"ADCdata[9]"<<hex<<(quint8)ADCdata[9];
////            //  qDebug()<<"ADCdata[10]"<<hex<<(quint8)ADCdata[10];
////      //***        tmpADHeight[count] = ( ((unsigned int)ADCdata[9]<<8)+(unsigned int)ADCdata[10]); //高度AD值
////          //    qDebug() <<"tmpADHeight[count]"<<tmpADHeight[count];
////        //***      tmpADWeight[count] = ( ((unsigned int)ADCdata[11]<<8)+(unsigned int)ADCdata[12]); //吊重AD值
////             // globaldata->craneList[0].windspeed = ( ((unsigned int)ADCdata[13]<<8)+(unsigned int)ADCdata[14])*0.1;
////              count++;
////              if(count >= MAX_COUNT-1) {
////                  count = 0;
////              }
////          //    qDebug() << "count" << count;
////          }
////     set_received = false;
////    // spanData =0;
////     //heightData= 0;
////     //weightData =0;
//////     spanMin = tmpADSpan[0];
//////     heightMin = tmpADHeight[0];
//////     weightMin = tmpADWeight[0];
//////     spanMax =0;
//////     heightMax =0;
//////     weightMax =0;
//////     for (int i = 0 ; i< MAX_COUNT ; i++) {
//////         spanMax = spanMax > tmpADSpan[i] ? spanMax:tmpADSpan[i];
//////         spanMin = spanMin < tmpADSpan[i] ? spanMin:tmpADSpan[i];
//////         heightMax = heightMax > tmpADHeight[i] ? heightMax:tmpADHeight[i];
//////         heightMin = heightMin < tmpADHeight[i] ? heightMin:tmpADHeight[i];
//////         weightMax = spanMax > tmpADWeight[i] ? weightMax:tmpADWeight[i];
//////         weightMin = weightMin < tmpADWeight[i] ? weightMin:tmpADWeight[i];
//////     }
//////     for (int k = 0 ; k < MAX_COUNT;k++) {
//////         spanData +=tmpADSpan[k] ;
//////         heightData += tmpADHeight[k];
//////         weightData += tmpADWeight[k];
//////        // qDebug() << "heightData"<<heightData;
//////     }


//////    // qDebug()<<"heightData" <<heightData;
//////     span = (spanData - spanMax - spanMin)>>3;
//////     height = (heightData - heightMax - heightMin)>>3;
//////     weight = (weightData - weightMax - weightMin)>>3;

////  //   qDebug() << "height"<<height;
//////     data->craneList[0].span = span*0.5;
//////     data->craneList[0].wight = weight*0.001;
//////     data->craneList[0].hookHeight = (unsigned int)((height*2)/100);

////     }
////     //msleep(200);
////    }
////   close(fd);
////  // exec();   //20140901  using in this method  ,  any problem ?
////}

////void DataGather::run()
////{
////    while(1)
////    {
////        sleep(1);
////        qDebug()<<"in the dataGather -- RUN() ";
////        const int readBufLen = 10;
////        int readNum= 0;
////        int writeNum= 0;
////        char  readBuf[readBufLen];
////        char  writeBuf[10]="ABCDEFabc";
////        memset(readBuf,0,readBufLen);
////       // memset(writeBuf,0,10);

//////        if((readNum=read(fd,readBuf,readBufLen))>0)    //这种写法会造成 读阻塞
//////            qDebug()<<"readData :  "<<readBuf;
//////        else
//////            qDebug()<<"readData From 485 failed";


////        qDebug()<<"readNum is before read"<<readNum;
////        readNum=read(fd,readBuf,readBufLen);
////        qDebug()<<"readNum is after read"<<readNum;
////        qDebug()<<"readBuf is "<<readBuf;

////        //writeNum;
//////        if((writeNum=write(fd,writeBuf,10))>0)
//////        {
//////            qDebug()<<"writen  bytes"<<writeNum;
//////        }
//////        else
//////        {
//////            qDebug()<<"writen  failed";
//////        }

////     }
////}

//void DataGather::run()
//{
//    while(1)
//    {
//        qDebug()<<"in the loop";
//        const int len = 20;
//        char  buf[len];
//        memset(buf,0,len);
//        int  readNum = 0;
//        int writeNum = 0;

//        gpioCtl->gpio_set_value(GPIO430,VALUE_UP);//控制为读操作
//        readNum = read(fd,buf,len);
//        if(readNum>0)
//        {
//                qDebug()<<"read bytes  :    "<<readNum;
//                qDebug()<<"the  data  is  :  "<<buf;
//        }
//        else
//        {
//                  qDebug()<<"read bytes failed";
//        }

//        buf[readNum]  = 'A';
//        buf[readNum+1]  = 'A';

//        gpioCtl->gpio_set_value(GPIO430,VALUE_DOWN);//控制为写操作
//        writeNum = write(fd,buf,readNum+2);
//        msleep(500);  //这里需要等待时间，因为write 非阻塞，需要485把数据从缓冲区全部移走再开始下一次的读
//        if(writeNum > 0)
//        {
//            qDebug()<<"write bytes to memory "<<writeNum;
//            qDebug()<<"return back       "<<buf;
//        }
//        else
//        {
//                qDebug()<<"write failed  ";
//        }
//    }
//}

//void DataGather::Axis_Data_Proc(char *axisdata){
//    float axisx,axisy;
//    axisx = ( axisdata[9] >>4 )*10 + ( axisdata[9] & 0x0f ) + ( axisdata[10] >>4 ) * 0.1;
//    if ( axisdata[8] >> 4 == 0x01){
//        axisx = - axisx;
//    }
//    globaldata->craneList[0].tiltangleX = axisx;
//    axisy =  ( axisdata[12] >>4 )*10 + ( axisdata[12] & 0x0f ) + ( axisdata[13] >>4 ) * 0.1;
//    if ( axisdata[11] >> 4 == 0x01){
//        axisy = -axisy;
//    }
//    globaldata->craneList[0].tiltangleY = axisy;
//}

//void DataGather::Ad_Proc(unsigned int tmpADweight , unsigned int tmpADheight , unsigned int tmpADpan){
//    Weight_Data_Proc(tmpADweight);
//    Height_Data_Proc(tmpADheight);
//    Span_Data_Proc(tmpADpan);
//}

//void DataGather::Weight_Data_Proc(unsigned int tmpADweight){
//    double w;
//    //qDebug()<<"weightd"<<globaldata->craneKD.weightD;
//    //qDebug()<<"cranerate"<< globaldata->basicData.craneRate;
//    w = globaldata->basicData.craneRate * globaldata->craneKD.weightK * tmpADweight + globaldata->craneKD.weightD;
//    if(w < 0.09)
//        w = 0;
//    w = w > 99 ? 99 : w;
//    globaldata->craneList[0].wight = w;

////    if(globaldata->craneList[0].wight < 0.09)
////        globaldata->craneList[0].wight = 0;
////    globaldata->craneList[0].wight = globaldata->craneList[0].wight > 99? 99 : globaldata->craneList[0].wight;
//}

//void DataGather::Height_Data_Proc(unsigned int tmpADheight){
//    double h;
//    double temcraneheight;
//    temcraneheight = globaldata->basicData.preSectionHeight * globaldata->basicData.craneSectionNum+globaldata->basicData.firstSectionHeight;
//    h = globaldata->craneKD.heightK * tmpADheight + globaldata->craneKD.heightD;
//    if(h<0)
//        h = 0;
//    h = h > temcraneheight ? temcraneheight : h;
//    globaldata->craneList[0].hookHeight = h;
//}

//void DataGather::Span_Data_Proc(unsigned int tmpADspan){
//    float s;
//    s = globaldata->craneKD.spanK * tmpADspan + globaldata->craneKD.spanD;
//    if(s < 0)
//        s = 0;
//    s = s > globaldata->basicData.forearmLen ? globaldata->basicData.forearmLen : s;
//    globaldata->craneList[0].span = s;
//}

//void DataGather::Send_Data_To_430(){
//    int datasize = 17;
//    char data2430[datasize];
//    data2430[0] = 0xfe;
//    data2430[1] = 0xfb;
//    data2430[datasize-1] = 0xfa;
//    data2430[datasize-2] = 0xfe;

//}
