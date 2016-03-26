#include "gpio.h"


GpioCtl::GpioCtl()
{
     routerFlag = 1;
     rstFlag_r = 1;
     cfgFlag_r = 1;

     GpioAllInit();
     //qDebug()<<"gpio constructor: gpioAll Init finished \n";
}


  GpioCtl::~GpioCtl()
 {
     gpio_unexport( RSTGPIO ) ;
     gpio_unexport( CFGGPIO ) ;
     gpio_unexport( CTGPIO ) ;
     gpio_unexport( DSGPIO ) ;

     gpio_unexport(FGGPIO);
     gpio_unexport(GPIO430);
  }

void GpioCtl::GpioAllInit()
{
    RstGpioInit();
    CfgGpioInit();
    DsGpioInit();
    CtGpioInit();

    FGGpioInit();
    GPIO430Init();

    //qDebug()<<"all gpio_s init finished \n";
}

////*********机器复位****************
void GpioCtl::RstGpioInit()
{
    if (rstFlag_r)
    {
        gpio_export (   RSTGPIO );
        gpio_set_dir( RSTGPIO,DIRECTION_OUT );
        gpio_set_value( RSTGPIO,VALUE_UP );
        //qDebug()<<" rst gpioinit finished\n";
     }

}

////**********CFG按键 ： 进入到配置模式 。此外还有工作模式****
void GpioCtl::CfgGpioInit()
{
    if(cfgFlag_r)
        {
            gpio_export(   CFGGPIO );
            gpio_set_dir( RSTGPIO,DIRECTION_OUT );
            gpio_set_value( RSTGPIO,VALUE_UP );
            //qDebug()<<"cfg gpioinit finished\n";
         }
}
////*********终端节点初始化**************
void GpioCtl::DsGpioInit()
{
      if(routerFlag)
          {
            gpio_export(   DSGPIO );
            gpio_set_dir( DSGPIO,DIRECTION_OUT );
            gpio_set_value( DSGPIO,VALUE_UP );
           // qDebug()<<" ds__end_device  gpioinit finished\n ";
            }
  }
////**********中心节点初始化*************
void GpioCtl::CtGpioInit()
{
    if(routerFlag)
        {
            gpio_export(  CTGPIO );
            gpio_set_dir( CTGPIO,DIRECTION_OUT );
            gpio_set_value( CTGPIO,VALUE_UP );
           // qDebug()<<" cnt_device  gpioinit finished\n ";
         }
}

////***********指纹按压触发端口初始化********
void  GpioCtl::FGGpioInit()
{
    gpio_export(FGGPIO);
    gpio_set_dir(FGGPIO,DIRECTION_IN);
    //gpio_set_value(FGGPIO,VALUE_DOWN); //输入不应该设置gpio 的值
   // qDebug()<<"FINGER_GPIO  gpioinit finished\n ";
}

////************485 数据采集控制 端口***************
//******* 210 往外发送数据时，拉低控制端口??
//******* 210 读取数据时，拉高该控制端口  ??
void GpioCtl::GPIO430Init()
{
    gpio_export(GPIO430);
    gpio_set_dir(GPIO430,DIRECTION_OUT);
    gpio_set_value(GPIO430,VALUE_UP);
}
///***********************************
//  说明：gpio 口，调用 gpio_set_value()得到 value相应的输出
//                直到下次调用 gpio_set_value ,改变他的值
//*******************************************/

///* gpio_export */
int GpioCtl::gpio_export( int gpio )
{
        int fd, len;
        char buf[MAX_BUF];

        fd = open(SYSFS_GPIO_DIR "/export", O_WRONLY);
       if (fd < 0) {
                perror("gpio/export");
                return -1;
        }
        len = snprintf(buf, sizeof(buf), "%d", gpio);
        write(fd, buf, len);
        close(fd);

        return 0;
}

///* gpio_unexport */
int GpioCtl::gpio_unexport(int gpio)
{
        int fd, len;
        char buf[MAX_BUF];

        fd = open(SYSFS_GPIO_DIR "/unexport", O_WRONLY);
        if (fd < 0) {
                perror("gpio/unexport");
                return -1;
        }
        len = snprintf(buf, sizeof(buf), "%d", gpio);
        write(fd, buf, len);
        close(fd);

        return 0;
}

///* gpio_set_dir */
int GpioCtl::gpio_set_dir( int gpio, unsigned int out_flag)
{
        int fd, len;
        char buf[MAX_BUF];

        len = snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/direction", gpio);
        fd = open(buf, O_WRONLY);
        if (fd < 0) {
                perror("gpio/direction");
                return -1;
        }

        if (out_flag)
                write(fd, "out", 4);
        else
                write(fd, "in", 3);
        close(fd);

        return 0;
}

/* gpio_set_value */
int GpioCtl::gpio_set_value( int gpio, unsigned int value)
{
        int fd, len;
        char buf[MAX_BUF];

       len = snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/value", gpio);
        fd = open(buf, O_WRONLY);
        if (fd < 0) {
                perror("gpio/set-value-error");
                return -1;
        }
        if (value)
                write(fd, "1", 2);
        else
                write(fd, "0", 2);
        close(fd);

        return 0;
}

/* gpio_get_value */
int GpioCtl::gpio_get_value( int gpio, unsigned int &value)
{
    int fd, len;
    char buf[MAX_BUF];
    char ch;

    len = snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/value", gpio);
    fd = open(buf, O_RDONLY);
    if (fd < 0) {
        perror("gpio/get-value");
        return -1;
    }
    read(fd, &ch, 1);
    if (ch != '0')
        value = 1;
    else
        value = 0;
    close(fd);

    return 0;
}

