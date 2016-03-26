#include "datagather.h"
const char addressValue = 0x01;
const char readNumber= 0x07;
const char functionNumber = 0x04;
const int dataLen =  readNumber *2 + 5;
const short dataSize = readNumber *2;//纯数据大小
const int buffSize = 40;
const float angleConst = 360.0/8000;
const float windConst = 30.0/8000;
const float axisConst = 25.5/4000;

char encBuff[100];
int encLen = 0;

const int DATA_TRANSFER_COM = 3;
DataGather::DataGather()
    :QThread()
{
      globaldata = GlobalData::getGlobalData();
      if ( ( fd = openPort( DATA_TRANSFER_COM,0) ) < 0 ) {      //打开串口
          qDebug() << "Open failed datagather";
          return ;
      }
      if (setComConfig( fd , 38400 , 8 , 'N' , 1 ) < 0 ){
          qDebug() << "Set COM failed datagather";
          return;
      }
}

DataGather::~DataGather(){
    close(fd);
}

void DataGather::run(){
    dataRead = new DataRead();
    dataRead->start();
    dataSend = new DataSend();
    dataSend->start();
    exec();
}
/*****读数据线程******/
DataRead::DataRead()
{

}
/****寻找帧头函数，帧头为01 functionNumber dataSize,参考M-3001 modbus手册*****/
int DataRead::FindFrameHead(char *buff , int encLen)
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

/******从端口中读入数据，进行拼接，截取符合数据报格式的数据报进行解析*******/
void DataRead::run()
{
    while(1)
    {
        msleep(100);
        char buff[buffSize];
        int len = 0;
        len = read(fd,buff,sizeof(buff));

        if(encLen < 2*dataLen)
        {
//               for (quint8 i =0;i<len;i++)
//                {
//                        qDebug()<<"****buff***"<<hex<<(quint8)buff[i];
//                }
//               qDebug()<<"****************************";
//               qDebug()<<encLen;
            memcpy(encBuff+encLen,buff,len);
            encLen += len;
        }
        else
        {
            short frameHead = 0;
            frameHead = FindFrameHead(encBuff,encLen);
           //  qDebug()<<"frameHead"<<frameHead;
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
//                    for (quint8 i =0;i<encLen;i++)
//                    {
//                        qDebug()<<"***ok***"<<hex<<(quint8)encBuff[i];
//                    }
//                    qDebug()<<"****************************";

                if((temCrc[0] == encBuff[frameHead+readNumber*2+3]) && (temCrc[1] == encBuff[frameHead+readNumber*2+4]))
                {
                    //qDebug()<<"receive ok";
                    SplThrdCommand(encBuff,frameHead);
                }
            }
           else
           {
                  for (quint8 i =0;i<len;i++)
                   {
                            qDebug()<<"****temp****"<<hex<<(quint8)buff[i];
                   }
                 qDebug()<<"**********************************************";
           }
        }
    }
}

void DataRead::SplThrdCommand(char *buff,int shift)
{
    globaldata->craneList[0].angle = (  ((unsigned int) buff[shift+3]<<8) +(unsigned int)buff[shift+4] - 2000 ) * angleConst;

    globaldata->craneList[0].windspeed = ( ( (unsigned int)buff[shift+5] << 8) + (unsigned int)buff[shift+6] - 2000) * windConst;


    unsigned int tmpADWeight =( ( (unsigned int)buff[shift+7] << 8 ) + (unsigned int)buff[shift+8] - 2000);
    unsigned int tmpADHeight = ( ( (unsigned int)buff[shift+9] << 8 ) + (unsigned int)buff[shift+10] - 2000);
    unsigned int tmpADSpan = ( ( (unsigned int)buff[shift+11]  << 8 )+ (unsigned int)buff[shift+12] - 2000);

    unsigned int tmpAxis_X = ( ( (unsigned int)buff[shift+13] << 8 ) + (unsigned int)buff[shift+14] - 2000);
    unsigned int tmpAxis_Y = ( ( (unsigned int)buff[shift+15] << 8 ) + (unsigned int)buff[shift+16] - 2000);

    Axis_Data_Proc(tmpAxis_X,tmpAxis_Y);
    Ad_Proc(tmpADWeight,tmpADHeight,tmpADSpan);
}

void DataRead::Axis_Data_Proc(unsigned int tmpAxis_X,unsigned int tmpAxis_Y){
    int axisXStandard = tmpAxis_X-4000;
    int axisYStandard = tmpAxis_Y-4000;
    float axisx,axisy;

    axisx = axisXStandard * axisConst;
    globaldata->craneList[0].tiltangleX = axisx;

    axisy =  axisYStandard * axisConst;
    globaldata->craneList[0].tiltangleY = axisy;
}

void DataRead::Ad_Proc(unsigned int tmpADweight , unsigned int tmpADheight , unsigned int tmpADpan){
    Weight_Data_Proc(tmpADweight);
    Height_Data_Proc(tmpADheight);
    Span_Data_Proc(tmpADpan);
}

void DataRead::Weight_Data_Proc(unsigned int tmpADweight){
    double w;
    w = globaldata->basicData.craneRate * globaldata->craneKD.weightK * tmpADweight + globaldata->craneKD.weightD;
    if(w < 0.09)
        w = 0;
    w = w > 99 ? 99 : w;
    globaldata->craneList[0].wight = w;

//    if(globaldata->craneList[0].wight < 0.09)
//        globaldata->craneList[0].wight = 0;
//    globaldata->craneList[0].wight = globaldata->craneList[0].wight > 99? 99 : globaldata->craneList[0].wight;
}

void DataRead::Height_Data_Proc(unsigned int tmpADheight){
    double h;
    double temcraneheight;
    temcraneheight = globaldata->basicData.preSectionHeight * globaldata->basicData.craneSectionNum+globaldata->basicData.firstSectionHeight;
    h = globaldata->craneKD.heightK * tmpADheight + globaldata->craneKD.heightD;
    if(h<0)
        h = 0;
    h = h > temcraneheight ? temcraneheight : h;
    globaldata->craneList[0].hookHeight = h;
}

void DataRead::Span_Data_Proc(unsigned int tmpADspan){
    float s;
    s = globaldata->craneKD.spanK * tmpADspan + globaldata->craneKD.spanD;
    if(s < 0)
        s = 0;
    s = s > globaldata->basicData.forearmLen ? globaldata->basicData.forearmLen : s;
    globaldata->craneList[0].span = s;
}

/***发送指令线程**************/
DataSend::DataSend()
{

}

void DataSend::run()
{
    while(1)
    {
        msleep(100);
        char thrdCommand[8];
        //qDebug()<<"send command";
        EncThrdCommand(readNumber,thrdCommand);
       // std::cout<<sendCommand.c_str()<<std::endl;
        write(fd,thrdCommand,8);
    }
}

void DataSend::EncThrdCommand(char readNumber, char *thrdCommand)
{
  //  std::string thrdCommand;
    char address = addressValue;
    char functionType = functionNumber;
    char startAddress[2] = {0x00,0x00};
    char number[2] = {0x00,readNumber};

    int addr = 0;
    addr = globaldata->split_joint_hex_data(thrdCommand,addr,&address,1);
    addr = globaldata->split_joint_hex_data(thrdCommand,addr,&functionType,1);
    addr = globaldata->split_joint_hex_data(thrdCommand,addr,startAddress,2);
    addr = globaldata->split_joint_hex_data(thrdCommand,addr,number,2);

    char crc[2];
    globaldata->dataCRC16(thrdCommand,addr,(quint8*)crc);
    addr = globaldata->split_joint_hex_data(thrdCommand,addr,crc,2);
}
