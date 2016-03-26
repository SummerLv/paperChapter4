#ifndef DATAENCAPSULATESPLITEAPI_H
#define DATAENCAPSULATESPLITEAPI_H

#include<qglobal.h>
#include"data.h"
#include<QString>
#include<string.h>
extern int addDataToFrame(quint8 *send_data);
extern void splitDataframe(quint8 *recv_buff);

#endif // DATAENCAPSULATESPLITEAPI_H
