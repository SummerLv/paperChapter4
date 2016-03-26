#ifndef DATAPROCESS_H
#define DATAPROCESS_H

#include "data.h"
#include <qdebug.h>
#include <termios.h>
#include "uartAPI.h"
#include <QThread>
#include "sqldb.h"
#include <QSqlTableModel>
#include "math.h"
#include "mathAPI.h"
#include "angleTable.h"
#include "encspl.h"


class DataProcess : public QThread
{
    Q_OBJECT
public:
    //explicit DataProcess(QObject *parent = 0);
    DataProcess(QObject *parent);
private:
    void run();
    void dataSerialInit();
    void localHigherJudge(quint32 i);
    void neighborHigherJudge(quint32 i);
    void equalHightJudge(quint32 i);
    quint8 limit_warn_judge();
    quint8 right_left_limit_warn_judge(float left_warn_val, float right_warn_val);
    quint8 near_far_limit_warn_judge(float near_w_val , float far_w_val, float buffer);
    quint8 high_limit_warn_judge(float warn_val,float buffer);
    quint8 low_limit_warn_judge(float warn_val,float buffer);
    quint8 overload_judge(quint16 weight , quint16 span , quint16 table_data_num);
    quint8 moment_warn_judge(float e_warn_val , float warn_val);
    quint8 wind_warn_judge(float warn_val);
    void protect_area_handle(float angle);
    quint8  trigger_left_right_warn_flag(quint8 Pzone_type,float angle,float stangle,float endangle);
    quint8 trigger_far_near_warn_flag(quint8 prtc_type,float span,float w_span_s,float w_span_e);
    int suit(float angle,float start_angle,float end_angle,float yujing_band,float baojing_band);
    char suit_baojin(float angle,float stangle,float endangl);
    void max_min_protect_area(char num,char *data ,int len,float*max_min);   //换保护区


public:
    GlobalData *globalData;                 //全局变量
    //2014.01.16
    EncSpl *encSplData;
    /////
    
signals:
    void paintSignal();                           //重绘信号


    
public slots:
    
};

#endif // DATAPROCESS_H
