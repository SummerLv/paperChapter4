#include "zigbeecfg.h"

#include "string.h"
#include <QDebug>
#include "gpio.h"
#include <unistd.h>

ZigbeeCfg::ZigbeeCfg()
{
    qDebug()<<"in  zigbeecfg.cpp";
    globalData =GlobalData::getGlobalData();
    //globalData->OpenSerialPort();   //在全局变量中已经打开
    gpioCtl = new GpioCtl();
}

//ZigbeeCfg::~ZigbeeCfg( close ( globalData->ZigbeeFd ) );   //close ( ZigbeeFd ) ; is there any problem ??
ZigbeeCfg::~ZigbeeCfg( )
{
    delete gpioCtl;
    //close ( globalData->ZigbeeFd);
    qDebug()<<"ZigbeeCfg::~ZigbeeCfg( )    /////   close(zigbeefd)";
}

////******zigbee  reset  低电平有效*******//        rstFlag_R      cfgFlag_r总的配置 标志位
void ZigbeeCfg::zigbeeReset(void)
{
    gpioCtl->gpio_export( RSTGPIO );
    gpioCtl->gpio_set_dir(  RSTGPIO,DIRECTION_OUT);
    gpioCtl->gpio_set_value( RSTGPIO,VALUE_DOWN );
    //sleep(1);
    sleep(1);
    gpioCtl->gpio_set_value( RSTGPIO,VALUE_UP );
     qDebug()<<" zigbee reset finished \n";
}
////******CFG  pin 5_s  低电平 然后 拉高******//
void ZigbeeCfg::zigbeeCFG3s( void )
 {
       gpioCtl->gpio_export(  CFGGPIO );
       gpioCtl->gpio_set_dir(  CFGGPIO,DIRECTION_OUT);
       gpioCtl->gpio_set_value( CFGGPIO,VALUE_DOWN );
       //sleep(3);
       sleep(4);
       gpioCtl->gpio_set_value( CFGGPIO,VALUE_UP );
        qDebug()<<" zigbee CFG_3_s  finished \n";
  }
////  1 Chinese  2 English
void ZigbeeCfg::languageSelect(void)
{
    //receiveInfo();
    sleep(1);  //处于安全考虑  ，  没有该语句也可以成功配置
    sendCommand( "2" );
    sleep(1);
    //receiveInfo(); // 暂不使用一收一发的机制
}

void ZigbeeCfg::safeCodeSend(void)
{
    sendCommand( "SHUNCOM" );
    sleep(1);
    qDebug()<<"safeCodeSend finish \n";
}

///****************************************************************************
//ZigbeeCfg::nodeTypeSelcet() // need explict wants  //  routerFlag == 0 CT DS
//{
//    if( !routerFlag)
//   {
//        if(ctFlag)        //            添加该标志
//            {
//                 gpio_set_value( CTGPIO,VALUE_DOWN );
//                 gpio_set_value( DSTGPIO,VALUE_UP );
//            }
//        else
//            {
//                gpio_set_value( CTGPIO,VALUE_UP );
//                gpio_set_value( DSTGPIO,VALUE_DOWN );
//            }
//    }
//}
//**********************************************************************************/
void ZigbeeCfg::netTypeSelect(void)
{
    sendCommand( "4" );
     sleep(1);
    sendCommand( "7" );   // peer , 对等网
    sleep(1);
    qDebug()<<"net type select finish \n";
}
///************************************
void ZigbeeCfg::macCodeSelect(void)
{
    sendCommand( "7" );
    sleep(1);
    sendCommand( "2");   //Hex ,( There is no need to set this item because of using the broadcast mode.)
}
//************************************/
void ZigbeeCfg::sendingModeSelect(void)
{
    sendCommand( "8" );
    sleep(1);     //5ms
    sendCommand( "1");   //broadcast Mode
    sleep(1);     //5ms
    qDebug()<<"sending mode select finish \n";
}

void ZigbeeCfg::cfgDisplay(void)   // 显示配置的结果----串口接收配置数据列表
{
    sendCommand( "E" );
    sleep(1);     //5ms
    qDebug()<<"run here ,display_cmd  send,but try to get data,from rs232\n";
}

 void ZigbeeCfg::zigbeeRestart( void )
 {
    sendCommand( "D" );
    sleep(1);
 }

// //--------------------------------------------------------------------
// //  QByteArray QString::toAscii()const;
   void ZigbeeCfg::zigbeeIDCfg(void)
   {
       sendCommand( "5" );
        sleep(1);
       sendCommand( "AB" );
        sleep(1);
   }
   void ZigbeeCfg::zigbeeChannelCfg( void )
   {
       sendCommand( "6" );
       sleep(1);    //5ms
       sendCommand( "C" );    //  0 -- F ,
        sleep(1);
   }
//      /////  需要参数
   void ZigbeeCfg::zigbeeIDCfg(const char *netID)
   {
       sendCommand( "5" );
        sleep(1);
       sendCommand( netID );
        sleep(1);
   }

   void ZigbeeCfg::zigbeeChannelCfg( const char *channel )
   {
       sendCommand( "6" );
       sleep(1);
       sendCommand( channel );    //  0 -- F ,
        sleep(1);
   }

// //--------------------------------------------------------------------
//// slot  // attention ,delaying for too long maybe couldn't be accepted -----500 >> 200
   void ZigbeeCfg::zigbeeCFG_ID_CHANNEL(void)
    {
        zigbeeReset();
        zigbeeCFG3s();
        sleep(2);
        languageSelect();
        safeCodeSend();
        netTypeSelect();
        sendingModeSelect();
        zigbeeIDCfg();
        zigbeeChannelCfg();
       // zigbeeRestart();

         receiveInfo( );

    }

   void ZigbeeCfg::enterCFG(void)
   {
       qDebug()<<"entering the CFG_Mode*********************************\n";
       zigbeeReset();
       zigbeeCFG3s();
       languageSelect();
       safeCodeSend();
   }

   void ZigbeeCfg::exitCFG(void)
   {
       qDebug()<<"exiting the CFG_Mode********************************\n";
                   // 接收 配置数据
                  receiveInfo();
                  sleep(1);
       zigbeeRestart();
       qDebug()<<"restart ing  *************************************\n";
   }


///*
//*************maybe needn't*******
//void ZigbeeCfg::bpsSelect(void );
//void ZigbeeCfg::dataParity(void );
//void ZigbeeCfg::dataBit(void );
//void ZigbeeCfg::timeOut(void );
//void ZigbeeCfg::srcAddr(void );  //数据包 源地址 是否输出
// *************maybe needn't*******
//*/

 int ZigbeeCfg::sendCommand(const char *str)   // command string
{
    int len = strlen(str);
    int retlen;
    qDebug()<<"zigbeeFd is :\t"<<globalData->ZigbeeFd<<"\t send cmd is :"<<str<<"\n";
    retlen = write(globalData->ZigbeeFd,str,len);
    qDebug()<<"return writen bytes :"<<retlen;
    if( retlen < 0 )
        {
            qDebug()<<"sendCommand -- write zigbeeFd  failed\n ";
            exit(1);
         }
    else
        qDebug()<<"***sendCommand -- write zigbeeFd  successed *** \n ";
        return retlen;
 }

////***************以下是测试用的函数*******************
// void ZigbeeCfg::zigbeeDisplay(void)
// {
//     char buf[50];
//     do
//     {
//         int len = 0;
//         sleep(1);
//         len = receiveInfo(buf);
//         sleep(1);
//         sendback(buf,len);
//      }while(1);
// }

void  ZigbeeCfg::receiveInfo( void)
{
    const int bufLen = 2000;
    int redLen = 0 ;
    char buf[bufLen] ;
    qDebug()<<" in the receiveInfo() function   ";
    //memset(buf,0,bufLen);
    if ((redLen = read(globalData->ZigbeeFd,buf,bufLen)) < 0)
        {
           qDebug()<<"read globalData->ZigbeeFd **failed**\n";
            exit(1);
         }
    else
        qDebug()<< " the red datas  ***from ZIGBEE*** are :   "<<buf;
}

////int ZigbeeCfg::sendback( char *str, int len )   // command string
////{

////   int retlen;
////   str [len] = 'B';
////   str [len + 1] = 'F';
////   retlen = write(globalData->ZigbeeFd,str,len+2);
////   qDebug()<<"return writen bytes :"<<retlen;
////   if( retlen < 0 )
////       {
////           qDebug()<<"sendCommand -- write zigbeeFd  failed\n ";
////           exit(1);
////        }
////   else
////       qDebug()<<"sendCommand -- write zigbeeFd  successed *** \n ";
////       return retlen;

////}

////// test write 1 to serial
////void ZigbeeCfg::always_1(void)
////{
////     int  wrlen;
////     do
////     {
////         if((wrlen = write(globalData->ZigbeeFd,"0",1) < 0))
////             qDebug()<<"write fd failed****\n";
////         else
////              qDebug()<<"write fd success ****\n";
////     }while(1);

////     return ;

////}

