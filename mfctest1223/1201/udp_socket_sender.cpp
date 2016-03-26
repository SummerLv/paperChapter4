#include "udp_socket_sender.h"


    Udp_Socket_Sender::Udp_Socket_Sender(QObject *parent):
        QObject(parent)
    {
        //timer = new QTimer(this);
        udpSocket = new QUdpSocket(this);

        messageNo = 1;
        // timer ->start(1000);
    }

    void Udp_Socket_Sender::start()
    {
        //QTimer *timer = new QTimer(this);
        //timer ->start(1000);
    }

    void Udp_Socket_Sender::broadcastDatagram(int messageN)
    {

    }


Udp_Socket_Sender::  ~Udp_Socket_Sender()
{

}

/****************************************/


Udp_Socket_Receiver::Udp_Socket_Receiver(QObject *parent)
    :QObject(parent)
{

    udpSocket = new QUdpSocket(this);
    udpSocket ->bind(QHostAddress::LocalHost,44444);
//    connect(udpSocket,SIGNAL(readyRead()),this,SLOT(ProcessPendingDatagrams()));

}


Udp_Socket_Receiver::  ~Udp_Socket_Receiver()
{

}
