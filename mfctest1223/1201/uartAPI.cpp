#include "uartAPI.h"


#define MAX_COM1_NUM 4       //最大四个串口
#define MAX_COM2_NUM 10       //最大四个串口

int openPort(int comPort)
{
    int fd;

// 01     #if (COM_TYPE == GNR_COM)       //普通串口

   const char *dev[] = {"/dev/ttySAC0" , "/dev/ttySAC1" , "/dev/ttySAC2" , "/dev/ttySAC3"};//此为涛哥的文件系统移植到新的210开发板后/dev下的串口设备名
//02     #else  //USB转串口
//    const char *dev[] = {"/dev/ttyUSB0" , "/dev/ttyUSB1" , "/dev/ttyUSB2" , "/dev/ttyUSB3"};
//#endif
    if (comPort < 0 || comPort > MAX_COM1_NUM)
        return -1;
    /*打开串口*/
    fd = open(dev [comPort] , O_RDWR | O_NOCTTY | O_NDELAY); //读写方式打开，该终端不会成为调用open的那个进程的控制终端
    if (fd < 0) {
        qDebug() << "open serial port error!_openPort(int comPort)_uartAPI.CPP";
        qDebug() <<"dev[comPort]"<<dev[comPort];
        return -1;
    }

    /*恢复串口为阻塞状态*/
    if (fcntl( fd , F_SETFL , 0) < 0)
        qDebug()<<"fcntl F_SETFL";

    /*测试是否为终端设备*/
    if (isatty(STDIN_FILENO) == 0)
        qDebug()<<"standard input is not a terminal device";

    return fd;
}
//===================================================================
int openPort(int comPort,int selectPort)
{
    int fd;
    //const char *dev[] = {"/dev/s3c2410_serial0" , "/dev/s3c2410_serial1" , "/dev/s3c2410_serial2" , "/dev/s3c2410_serial3"};
    const char *dev1[MAX_COM1_NUM] = {"/dev/ttySAC0", "/dev/ttySAC1","/dev/ttySAC2","/dev/ttySAC3"};
   // qDebug()<<"ttySAC0";
     const char *dev2[MAX_COM2_NUM] = {"/dev/ttyUSB0" , "/dev/ttyUSB1" , "/dev/ttyUSB2" , "/dev/ttyUSB3" , "/dev/ttyUSB4" , "/dev/ttyUSB5" ,"/dev/ttyUSB6","/dev/ttyUSB7","/dev/ttyUSB8","/dev/ttyUSB9"};
   // qDebug()<<"TTYusb0";


         if ((comPort < 0) || ((selectPort != 1) && (selectPort != 0)))
                 return -1;
          if (((selectPort == 1) && (comPort > MAX_COM2_NUM)) ||
              ((selectPort == 0) && (comPort > MAX_COM1_NUM)))
              return -1;

    /*打开串口*/
    if (selectPort == 0)
    fd = open(dev1 [comPort] , O_RDWR | O_NOCTTY | O_NDELAY); //读写方式打开，该终端不会成为调用open的那个进程的控制终端
    else if (selectPort == 1)
    {
        fd = open(dev2 [comPort] , O_RDWR | O_NOCTTY | O_NDELAY); //读写方式打开，该终端不会成为调用open的那个进程的控制终端
       // qDebug()<<"dev2 [comPort]"<<dev2 [comPort];
    }
    else qDebug()<<"selectPort error";

    if (fd < 0) {
        qDebug() << "open serial port error_openPort(int comPort,int selectPort)_uartAPI.CPP";
        return -1;
    }

    /*恢复串口为阻塞状态*/
    if (fcntl( fd , F_SETFL , 0) < 0)
        qDebug()<<"fcntl F_SETFL";

    /*测试是否为终端设备*/
    if (isatty(STDIN_FILENO) == 0)
        qDebug()<<"standard input is not a terminal device";

    return fd;
}

/*设置接口信息,fd为打开端口号，baudrate为波特率，databits为数据位，parity为奇偶校验位，stopbits为停止位*/
int setComConfig(int fd , int baudRate , int dataBits , char parity , int stopBits)
{
    struct termios newConfig , oldConfig;
    int speed;
    /*保存并测试现有串口参数设置，若串口号等出错，会有相关的出错信息*/
    if (tcgetattr( fd , & oldConfig) != 0)
    {
        qDebug() << "tcgettattr11111111";
        return -1;
    }
    /*设置字符大小*/
    newConfig = oldConfig;
    cfmakeraw( & newConfig);
    newConfig.c_cflag &= ~CSIZE;
    /*设置波特率*/
    switch (baudRate) {
    case 2400:
        speed = B2400;
        break;
    case 4800:
        speed = B4800;
        break;
    case 9600:
        speed = B9600;
        break;
    case 19200:
        speed = B19200;
        break;
    case 38400:
        speed = B38400;
        break;
    case 57600:
        speed = B57600;
        break;
    case 115200:
        speed = B115200;
        break;
    default:
        speed = B115200;
        break;
    }
    cfsetispeed ( & newConfig , speed );
    cfsetospeed ( & newConfig , speed );
    /*设置停止位*/
    switch (dataBits) {
    case 7:
        newConfig.c_cflag |= CS7;
        break;
    case 8:
        newConfig.c_cflag |= CS8;
        break;
    default:
        newConfig.c_cflag |= CS8;
        break;
    }
    /*设置奇偶位*/
    switch (parity) {
    default:
    case 'n':
    case 'N':
    {
        newConfig.c_cflag &= ~ PARENB;
        newConfig.c_iflag &= ~ INPCK;
    }
        break;

    case 'o':
    case 'O':
    {
        newConfig.c_cflag |= ~ (PARODD | PARENB);
        newConfig.c_iflag |= INPCK;
    }
        break;

    case 'e':
    case 'E':
    {
        newConfig.c_cflag &= ~ PARODD;
        newConfig.c_cflag |= PARENB;
        newConfig.c_iflag |= INPCK;
    }
        break;

    case 's':           //as no parity
    case 'S':
    {
        newConfig.c_cflag &= ~ PARENB;
        newConfig.c_cflag &= ~ CSTOPB;
    }
        break;
    }
    /*设置停止位*/
    switch (stopBits) {
    default:
    case 1:
        newConfig.c_cflag &= ~ CSTOPB;
        break;
    case 2:
        newConfig.c_cflag |= CSTOPB;
    }

    /*设置等待时间和最小接收字符*/
    newConfig.c_cc[VTIME] = 0 ;
    newConfig.c_cc[VMIN] = 1 ;
    /*处理未接受字符*/
    tcflush( fd , TCIFLUSH );
    /*激活新配置*/
    if ( tcsetattr( fd ,TCSANOW , &newConfig ) !=0 ){
        qDebug()<<"tcsetattr";
        return -1;
    }
    return 0;
}


