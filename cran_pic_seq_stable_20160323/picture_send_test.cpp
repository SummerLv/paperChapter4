#include "picture_send_test.h"

picture_send_test::picture_send_test()
{
    tcppicsender = new QTcpSocket();
    timer = new QTimer();
    quint16 port = 10150;
    tcppicsender->connectToHost(QHostAddress("202.115.11.128"), port);
    connect(tcppicsender , SIGNAL(connected()) , this , SLOT(SendPhotoAvailable()));
    connect(timer,SIGNAL(timeout()),this,SLOT(sendData()));//UDP SOCKET发送
}

void picture_send_test::run(){
    connectGprs();
}

void picture_send_test::connectGprs(){
    QString command = tr("pppd call wcdma");
    char *S;
    S = command.toAscii().data();
    system(S);
    qDebug()<< "Connect Success";
}

void picture_send_test::SendPhotoAvailable(){

    QFile file("/123.png");
   file.open(QIODevice::ReadOnly);
   picby = file.readAll();
   file.close();
    timer->start(3000);
}

void picture_send_test::sendData(){
    qDebug()<<"send photo"<<picby.size();
      tcppicsender->write(picby);
      tcppicsender->flush();
}
