#ifndef PICTURE_SEND_TEST_H
#define PICTURE_SEND_TEST_H
#include "gprs.h"
class picture_send_test:public QThread
{
    Q_OBJECT
public:
    picture_send_test();
private:
    void run();
    void connectGprs();
private:
    QTcpSocket *tcppicsender;
    QTimer *timer;
    QByteArray picby;
private slots:
    void SendPhotoAvailable();
    void sendData();
};

#endif // PICTURE_SEND_TEST_H
