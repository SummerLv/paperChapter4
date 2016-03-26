#ifndef ZIGBEECFG_H
#define ZIGBEECFG_H

#include "gpio.h"
#include "data.h"		//fd
#include "unistd.h"   //  等待延时函数
// open() file
#include <stdio.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <QDebug>

//20140828 kdTab 保存新的. netID . channel .启动 配置
#include "sqldb.h"
#include <QSqlTableModel>

class ZigbeeCfg 	//: public QObject
{
    //Q_OBJECT
public:
    ZigbeeCfg();
    ~ZigbeeCfg();

    //*******默认的配置函数********************
    //**定义配置函数的返回类型为 boolRst/ boolCfg/etc 返回顶层配置时bool = true,环环相扣
     void zigbeeReset( void );   // 复位
     void zigbeeCFG3s( void );
     void languageSelect( void );
     void safeCodeSend( void );
     void nodeTypeSelcet( void ); // need explict wants
     void netTypeSelect( void );
     void macCodeSelect( void );
     void sendingModeSelect( void );
     void cfgDisplay( void );         // 显示配置的数据
     void zigbeeRestart( void );    // 配置模式下的重启

     //*********需要配置时由 void enterCFG(void);进入到配置模式
     //*********再调用需要配置参数的函数   netID  or  Channel
     //*********最后调用  void exitCFG(void); 退出配置模式，重启计入工作状态
     void enterCFG(void);
     void exitCFG(void);


     //*****把上位机下发的 存储在数据库中的 ZigbeeID(NetID)  ZigbeeChannel 配置到 zigbee
     //******配置条件 ： if( sql中 zigbeeId zigbeeChannel 任意一个发生了变化）
     //******实现方案 :  if(配置条件) emit zigbeeCFG();    using SIGNAL()---SLOT() mechanisen
   //  QByteArray QString::toAscii()const;
     void zigbeeIDCfg(void);
     void zigbeeChannelCfg( void );

     void zigbeeIDCfg(const char *);
     void zigbeeChannelCfg( const char * );

     void zigbeeCFG_ID_CHANNEL(void);
     void zigbeeDisplay(void);

    /*
    *************maybe needn't*******
    void bpsSelect( void );
    void dataParity( void );
    void dataBit( void );
    void timeOut( void );
    void srcAddr( void );  //数据包 源地址 是否输出

     *************maybe needn't*******
     */
     // RS232 终端与模块通信
     int sendCommand( const  char  *str );   // command string
     void  receiveInfo(void );
//     //测试函数
//     void always_1(void);
//     int sendback( char *str, int len );
public:
      GlobalData *globalData; //引用 ZigbeeFd
      GpioCtl *gpioCtl;

};

#endif // ZIGBEECFG_H
