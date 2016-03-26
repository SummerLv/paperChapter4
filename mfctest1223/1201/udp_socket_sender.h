#ifndef UDP_SOCKET_SENDER_H
#define UDP_SOCKET_SENDER_H

#include<QUdpSocket>
#include "data.h"
#include "dialog.h"
#include<QTimer>
#include<QByteArray>
#include<QHostAddress>
#include"sqldb.h"

class Udp_Socket_Sender: public QObject
{
    Q_OBJECT
public:
    Udp_Socket_Sender(QObject *parent = 0);
    ~Udp_Socket_Sender();
    void start();
public:
    void broadcastDatagram(int messageN);
private:
    QUdpSocket *udpSocket;
    QTimer *timer;
    int messageNo;
};

class Udp_Socket_Receiver: public QObject
{
    Q_OBJECT
public:
    Udp_Socket_Receiver(QObject *parent = 0);
    ~Udp_Socket_Receiver();
//    void ProcessPendingDatagrams();
    //void start();
//private slots:
    public slots:
   // void ProcessPendingDatagrams();
private:
    QUdpSocket *udpSocket;

};

#endif // UDP_SOCKET_SENDER_H
