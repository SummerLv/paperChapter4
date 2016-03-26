#ifndef GPIO_H
#define GPIO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>

#include <QDebug>

#define SYSFS_GPIO_DIR "/sys/class/gpio"
#define MAX_BUF 64


#define DIRECTION_OUT 1
#define DIRECTION_IN  0

#define VALUE_UP 1
#define VALUE_DOWN 0

#define  RSTGPIO 147
#define  CFGGPIO 148
#define  DSGPIO  149    // end_device
#define  CTGPIO  155     // center

#define  FGGPIO  156
#define  GPIO430 157

class GpioCtl
{

    public :
            GpioCtl();
            ~GpioCtl();

          void  GpioAllInit();
          void   RstGpioInit();
          void   CfgGpioInit();
          void   DsGpioInit();
          void   CtGpioInit();
          //////0915   FINGER TRAGER
          void  FGGpioInit();
          void  GPIO430Init();

            int gpio_export( int gpio);
            int gpio_unexport( int gpio);
            int gpio_set_dir( int gpio, unsigned int out_flag);
            int gpio_set_value( int gpio, unsigned int value);
            int gpio_get_value(int gpio, unsigned int &value);
          //  the operation to RST / CFG / DS /CT is writing a value to  the  GPIO
    private :
            unsigned int gpio;
            unsigned int out_flag;
            unsigned int value;
           // unsigned int * value;    //gpio_set_value()

            int routerFlag;  // 标志位为零，配置成 中心节点 OR 终端节点 ,
            int rstFlag_r;    // 标志位为零，zigbee 复位
            int cfgFlag_r;    // 标志位为零，zigbee 配置
 };


#endif // GPIO_H

