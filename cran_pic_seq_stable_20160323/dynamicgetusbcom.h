#ifndef DYNAMICGETUSBCOM_H
#define DYNAMICGETUSBCOM_H
/****2014.07.04****/
#include <QString>
#include <stdlib.h>
#include <QVector>
#include <QFile>
#include <QTextStream>
#include <QByteArray>
#include <QDebug>
#include <fcntl.h>
#include <map>
//using namespace std;
//map<int ,vector<int> > tem;

class UsbComTool{
public:
  //  UsbComTool();
    void GetUsbCom();
    const char**  GetComNameFromFile(QVector<QString>::size_type &);
    int dynamicOpenPort(const char ** &dev, int comPort , int size);
};

#endif // DYNAMICGETUSBCOM_H
