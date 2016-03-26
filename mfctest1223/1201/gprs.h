#ifndef GPRS_H
#define GPRS_H
#include "data.h"
#include <iostream>
#include <QVector>
#include <QThread>
#include <QFile>
#include <QTextStream>
#include"udp_socket_sender.h"
#include <QTcpSocket>
#include "dialog.h"
#include "finger.h"
#include<QTimer>
#include<QDateTime>
#include<QTimeEdit>
#include<QtSql>
#include <QSqlTableModel>
using namespace std;

#define AT          0
#define ATE0       AT+1
#define CPIN       ATE0+1
#define CSQ        CPIN+1

//#define CGMM    CSQ+1
//#define CGMR    CGMM+1
//#define NWINFO  CGMR+1
//#define SYSCFG  NWINFO+1

#define CGDCONT CSQ+1
#define CFUN        CGDCONT+1
#define ATD          CFUN+1


/*****需要插入的ID和指纹模块地址******/
class Driver_ID_Address_Data
{
public:
    Driver_ID_Address_Data();
    ~Driver_ID_Address_Data();
public:
    int DriverID;
    quint32 FingerAddress;
};

/******工作循环类***********/
class WorkCycle
{
public:
    WorkCycle();
    ~WorkCycle();
public:
    char begin_t[4];
    char end_t[4];
    float max_w;
    float max_m;
    float max_h;
    float min_h;
    float max_s;
    float min_s;
    float move;
    float begin_a;
    float begin_s;
    float begin_h;
    float end_a;
    float end_s;
    float end_h;
    quint8 end_record;
    quint8 begin_record;
};


class Finger_Gather;
class GprsConnect : public QThread
{
    Q_OBJECT
public:
    static GprsConnect *Instance()
    {
        if (NULL == m_GprsConnect)             //保证只创建一个实例
            m_GprsConnect = new GprsConnect;
        return m_GprsConnect;
    }

public:
    GlobalData *globalData;                 //全局变量
//    quint8 imei_str[16];
    QTcpSocket *tcppicsender;
private:
    GprsConnect(QObject *parent = 0);
private:
    /*****转换为2字节模板************/
    template <typename T>
    void Double_To_Array(T *p , double value)
    {
            qint32 TempValue;
            TempValue = value;
            *p =((TempValue >> 8) & 0xFF);
             p++;
            *p = (TempValue & 0xFF);
    }

    void Double_To_Array2(char *p, double value);
    int Byte_To_Int(quint8 *p,quint8 num);
    int dec_to_bcd(qint32 dec_dat,quint8 *bcd ,quint32 len);
    int delete_fe(char *receive_data , quint32 len);
    int Open_Serial(quint8 U_or_S, quint32 baultrate, quint8 S_num);
private:
    /*****转换为4字节模板************/
    template <typename T>
    /*************double数据转换成4字节数组数据*********/
    void Double_To_Array4(T *p, double value)
    {
        quint32 TempValue;
        TempValue = value;
        *p =((TempValue >> 24) & 0xFF);
         p++;
        *p = ((TempValue>>16) & 0xFF);
         p++;
         *p = ((TempValue>>8) & 0xFF);
         p++;
         *p = ((TempValue) & 0xFF);
    }
private:
    void run();
    void connectGprs();
    bool Connect_Gprs();
    void read_IMEI();
    void center_data_proc(char *tem_data_receive , quint32 tem_data_receive_len);
    int analyze_Data(QString string, QByteArray data, char *tem_data_receive , quint32 tem_data_receive_len);
private:
  void send_message_to_center();//发送信息总处理程序
  void send_request_to_upper(quint8 info_type, quint32 info_version, quint32 info_index );//发送信息至上位机
    void send_mes_trans_data();
    void send_Id_to_center(char *imei);
    void send_IP_Data_ToUpper();
    void send_basic_data();
    void send_Protect_Message(char Crane_Index);
    void send_Limit_Data();
    void send_KD_Data();
    void send_CraneGroup_Data(char Crane_Index);
    void send_Moment_Data(char Crane_Index);
    void send_Reset_Data();
    void send_status_data();

    void send_finger_version_inquiry();
    void send_finger_syn_inquiry(quint32 DriverID);
    void FingerData_Store_InDataBase();
    void FingerData_Store_InModule();

    void warn_dispose();
    void collision_warn_dispose();
    void limit_Warn_Dispose();
    void barrier_Warn_Dispose();
    void forbid_Warn_Dispose();
    void overrun_Warn_Dispose();

private:
    void identity_illegal_dispose();
    void report_alarm_to_center(char alarm_type, char *detail);
    void report_illegal_to_center(char illegal_type, char *detail);
    void report_fault_to_center(char fault_type, char *detail);
    void report_work_cycle_to_center();
    void report_identity_to_center();

    void get_work_cycle_data();
    void GetTime(char *Time_str);
    void time_synchron(char *Time_str);
    void info_send(char *send_info_data , quint32 info_len, QString ipaddr, int port);
    void info_send_toUpper(char *send_info_data , quint32 info_len, QString ipaddr, int port);
    bool Strcomp_bool(char str1[],const char *str2);

    void test_display(char *set_buff);
    void send_FingerId_to_center(char *imei_num, quint8 event);

private:
    static GprsConnect *m_GprsConnect;
    char DATA_SEND_SUCCESS;//程序逻辑判断标示
    quint32 info_send_prio;//信息发送优先级确定位
    bool Upper_Or_Center;//判断信息来自上位机还是中心平台，中心平台校验结果要颠倒,为真时来自上位机
    bool Receive_IP_Sucess;//上位机成功接收IP后的回应帧
    quint8 Config_Info_Flag;//配置信息改变标志位，配置信息包括基本信息，保护区信息，限位信息,
    //前4bit分别为基本信息，保护区信息，限位信息是否发送标志位,指纹请求标志位
    quint8 Insert_Or_Change;//判断需要发送插入数据请求或是更改数据请求,为0为插入，为1为更改
    quint16 Finger_Partition_number;//分片号
    quint16 Finger_number;//指纹序号
    quint16 Driver_Num;
    quint8 DriverID_Version[10][6];
    QString Upper_IP;
    int Upper_Port;
    quint16 Call_Number;
    QSqlTableModel *Finger_Set_Data;
    QFile *file;
    quint8 I_OR_C_Num;//链表需要的指纹序号
    QList<Driver_ID_Address_Data> insertdata;
    QList<Driver_ID_Address_Data> deletedata;
    QList<Driver_ID_Address_Data> changedata;

    QTimer *timer;
    QUdpSocket *udpSocket;
    QUdpSocket *udpSocket_Upper;
    Finger_Gather *fingergather;
    EncSpl *encspl;
    WorkCycle *work_cycle;
    WorkCycle *work_cycle_send;
    //Udp_Socket_Sender *data_sender;
    QRegExp rx;

    char center_receive_time[4];

    quint8 frame[85];

    quint8 info_change_flag[1];
    QDateTime system_time;
private:
//    quint8  test_num;//测试，指纹号
//    quint8 test_sign;//测试，指纹请求发送标志位

    quint8 begin_cycle;//工作循环开始标志
    quint8 end_cycle;//工作循环结束标志
    quint8 stable1;//结束持续时间计时标志
    quint8 stable2;//开始持续时间计时标志
signals:
    void finger_change_insert();
public slots:
    void sendData();
    void receiveData();
    void receiveData_fromUpper();
    int SendPhoto();
};


#endif // GPRS_H
