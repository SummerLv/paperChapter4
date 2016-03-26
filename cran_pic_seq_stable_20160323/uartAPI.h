#ifndef UARTAPI_H
#define UARTAPI_H
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <qdebug.h>


extern int openPort(int comPort,int selectPort);
extern int openPort(int comPort);
extern int setComConfig(int fd , int baudRate , int dataBits , char parity , int stopBits);



#endif // UARTAPI_H
