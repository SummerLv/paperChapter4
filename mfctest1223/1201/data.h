#ifndef DATA_H
#define DATA_H

#include "parameter.h"
#include <qstring.h>
#include <QColor>
#include <qpoint.h>
#include <QSqlTableModel>
#include "sqldb.h"
#include "math.h"
#include <QPainter>
#include "mathAPI.h"
#include "angleTable.h"
#include "uartAPI.h"
class BasicData                         //基本信息
{
public:
     BasicData();

      ~BasicData();
public:
    quint16 basicVersion;                 //信息版本
    QString craneName;            //塔吊名称（16字节）
    quint8 craneID;                    //塔机ID
    quint8 groupID;                    //塔群ID
    quint8 firmID;                      //厂商ID
    quint8 craneType;                   //塔吊类型（4bits）
    quint8 craneRate;                   //吊绳倍率（4bits）
    double coordinateX;             //X坐标(-3276.8-3276.8)
    double coordinateY;             //Y坐标(-3276.8-3276.8)
    double forearmLen;             //前臂长(0-6553.5)
    double backarmLen;            //后臂长(0-6553.5)
    double craneHeight;            //塔高(0-6553.5)
    double craneTopHeight;      // 塔顶高度(0-25.6)
    quint32 craneSectionNum;     //塔吊节数
    double preSectionHeight;       //每节塔高
    double firstSectionHeight;         //首节塔高
    QString craneIMEI;                      //塔机IMEI
};




class ElementData
{
public:
    quint8 elementType;          //元素类型（0x00点，0x01圆弧）
    double pointX;                  //X坐标(（2Bytes）-3276.8-3276.8)
    double pointY;                  //Y坐标(（2Bytes）-3276.8-3276.8)
    double radius;                 //圆半径(（2Bytes）0-6553.5)
    double startAngle;              //起始角度(（2Bytes）0-360)
    double endAngle;                //终止角度(（2Bytes）0-360)
    qint32 drawPointX;              //画图数据
    qint32 drawPointY;
    qint32 drawRadius;
};


class ProZoneData                  //保护区信息
{
   // Q_OBJECT
public:
    ProZoneData();
    ~ProZoneData();
public:
    /*
      struct ElementPoint //元素位置坐标
    {
        double PointX;                  //X坐标(（2Bytes）-3276.8-3276.8)
        double PointY;                  //Y坐标(（2Bytes）-3276.8-3276.8)
        double Radius;                 //圆半径(（2Bytes）0-6553.5)
        double StartAngle;              //起始角度(（2Bytes）0-360)
        double EndAngle;                //终止角度(（2Bytes）0-360)
    };

     struct Element                     //元素信息
    {
        quint8 ElementType;                   //元素类型（0x00点，0x01圆弧）
        ElementPoint elementPoint;      //元素位置信息
    };
    */
    quint16 proZoneVersion;                 //保护区信息版本（2Bytes）
    //quint8  ProZoneNum;                    //保护区个数（1Bytes）

    quint8 proZoneType;                     //保护区类型（1bit  [7]）
    quint8 proZoneOrder;                    //保护区序号（3bits [6:4]）
    quint8  proZoneEleNum;              //保护区元素个数（4bits [3:0]）三种合成一个Byte

    QString proZoneName;                //保护区名称（16Bytes）
    quint8 proZoneID;                       //保护区ID（1Bytes）
    quint8 proBuildType;                    //保护区建筑类型（1Bytes）
    double proZoneHeight;                 //保护区高度（2Bytes）
     QList<ElementData> elementData;        //元素信息数据
     QPainterPath drawPath;                         //保护区绘图路径
     QColor currentColor;                               //当前绘图颜色
     QColor originalColor;                              //原本绘图颜色（green-禁行区；cyan-障碍物）

     float proZoneStartAngle;   //保护区起始角度
     float proZoneEndAngle;         //保护区终止角度
     float min_span;                //保护区最近距离
     float max_span;                //保护区最远距离
     float point_perangle[360][2];
     //double min_max_span[2];				//max & min span;

     unsigned char left_barrier;
     unsigned char right_barrier;
     unsigned char far_barrier;
     unsigned char near_barrier;
     unsigned char low_barrier;

     unsigned char left_forbid;
     unsigned char right_forbid;
     unsigned char far_forbid;
     unsigned char near_forbid;





   //typedef struct Element element;         //保护区元素信息

};



class LimitData                            //限位信息
{
public:
    LimitData();

    ~LimitData();
public:
    quint16 limitVersion;                   //限位信息版本
    double leftLimit;                          //左限位(-3276.8-3276.8)
    double rightLimit;                        //右限位(-3276.8-3276.8)
    double farLimit;                           //远限位(0-25.5)
    double nearLmit;                         //进限位(0-25.5)
    double highLimit;                         //高限位(0-25.5)
    double weightLimit;                     //吊重限位(0-655.35)
    double momentLimit;                     //力矩限位值(0-655.35)
    quint8 enableBit;                         //传感器使能标志位
};



class CraneKD                          //KD值
{
public:
    CraneKD();
    ~CraneKD();
public:
    quint16 craneKDVersion;      //信息版本
    double weightK;                     //吊重K值
    double weightD;                      //吊重K值
    double spanK;                            //变幅K值or水平K值
    double spanD;                            //变幅D值or水平d值
    double heightK;                          //高度K值or垂直k值
    double heightD;                              //高度D值or垂直d值
    QString serverIP;                            //IP
    QString serverPort;                          //端口
    QString ZigbeeChannel;                      //通信信道
    QString ZigbeeID;                                   //网络ID
    double pitchHeight;                  //塔吊节高
    double initHeight;//塔吊初始高度
    double pitchNum;//塔吊节数
    double proHeightEW;//保护区高度预紧
    double proHeightW;//保护区高度报警

};

class CraneData                       //塔吊实时信息
{
public:
    CraneData();
    ~CraneData();
public:
    QString craneName;            //塔吊名称（16字节)
    quint8 craneID;                    //塔机ID
    quint8 groupID;                    //塔群ID
    quint8 firmID;                      //厂商ID
    quint8 moment; //力矩
    //实时数据信息
    double angle;                        //角度
    double span;                         //变幅
    double hookHeight;                //吊钩高度,起升
    double windspeed;  //风速
    float tiltangleX;//倾斜角度
    float tiltangleY;
    double rotateSpeed;            //转速
    double wight;                           //吊重
    //固定信息
    double coordinateX;             //X坐标(-3276.8-3276.8)
    double coordinateY;             //Y坐标(-3276.8-3276.8)
    double craneHeight;
    double craneTopHeight;      // 塔顶高度(0-25.6)
    double forearmLen;             //前臂长(0-6553.5)
    double backarmLen;            //后臂长(0-6553.5)
    QString craneMAC;                      //塔机IMEI
    //绘图信息
    QPoint drawCenter;                  //绘图圆心
    //qint32 DrawCircleX;                 //绘图圆心X
   // qint32 DrawCircleY;                 //绘图圆形Y
    qint32 drawForeLen;                //绘图前臂长
    qint32 drawBackLen;               //绘图后臂长
    qint32 drawSpan;                    //绘图变幅
    QColor paintColor;                  //绘图颜色
    QPainterPath drawPath;          //绘图路径
    quint32 relatedCraneNum;      //相干塔吊个数
    //防碰撞信息
    double bottomEnvelope;          //底部包络边界 = 塔臂高 - 1米；
    double topEnvelope;                 //顶部包络边界 = 塔臂高+塔帽高度
    double forearmPointX;               //实际前端坐标X（用于防碰撞判断）
    double forearmPointY;               //实际前端坐标Y（用于防碰撞判断）
    double backarmPointX;               //实际后端坐标X（用于防碰撞判断）
    double backarmPointY;               //实际后端坐标Y（用于防碰撞判断）

    //报警标志位
    quint8 leftCollision;                   //左碰撞
    quint8 rightCollision;                  //右碰撞

    quint8 farCollision;                     //远碰撞
    quint8 nearCollision;                   //近碰撞
    quint8 lowCollision;                     //低碰撞

    quint8 tailCollision;                      //尾部碰撞
    quint8 bodyCollision;                    //塔身碰撞

    quint8 left_limit_warn_flag;        //左限位报警标志
    quint8 right_limit_warn_flag;       //右限位报警标志
    quint8 far_limit_warn_flag;         //远限位报警
    quint8 near_limit_warn_flag;        //近限位报警]
    quint8 high_limit_warn_flag;        //高限位报警
    quint8 low_limit_warn_flag;         //低限位报警
    quint8 overweight_limit_warn_flag;  //超重限位报警标志
    quint8 moment_limit_warn_flag;  //力矩限位报警标志
    quint8 wind_limit_warn_flag;    //风俗限位报警标志
    quint8 axis_x_warn_flag;
    quint8 axis_y_warn_flag;

};

//class GroupInter//塔机相干信息，协议：塔机监控终端间通信协议
//{
//public:
//    GroupInter();
//    ~GroupInter();
//public:
//    quint8 craneID;                    //塔机ID
//    quint8 groupID;                    //塔群ID
//    quint8 firmID;                      //厂商ID
//    double angle_DOUBLE;               //回转
//    double rotateSpeed_DOUBLE;       //转速
//    double span_DOUBLE;                 //变幅
//    double height_DOUBLE;               //起升

//};

class GlobalData
{
public:

   // Q_OBJECT

    static GlobalData *getGlobalData()
    {
        if (NULL == m_GlobalData)             //保证只创建一个实例
            m_GlobalData = new GlobalData;
        return m_GlobalData;
    }
    //static QGlobalData *getGlobalData();

     BasicData basicData;             //基本信息
     ProZoneData proZoneData;      //保护区信息
     LimitData limitData;                //限位信息
     CraneKD craneKD;                  //K&D值
//     GroupInter groupInter;               //塔群相干
     QList<CraneData> craneList;   //塔吊群实时信息list
     QList<ProZoneData> proZoneDataList;   //塔吊保护区信息list
    QTextCodec *tc;//界面中文转换

public:
     void globalDataInit();                 //数据初始化
     void calculateScale();                 //计算画图数据
     void calculateProZoneData();		//pro_zone_data  for  anti-collision
     void flagInit();                            //标志位初始化
     void OpenSerialPort();           // 打开串口并进行初始化设置

    // void Stop_Gprs(GprsConnect *m);
     void drawPathInit();                       //绘图路径初始化

     quint16 dataCRC16(char *pucSendBuf , quint32 usLen , quint8 *CRC);    //CRC16校验
    /****** 数据拼接类*****/
public:
     template <typename T>
     quint32 split_joint_hex_data( T *carrier, quint32 joint_addr, T *segment, quint32 data_len)
     {
         quint32 next_addr;
         memcpy (carrier + joint_addr , segment ,data_len);
         next_addr = joint_addr +data_len;
         return next_addr;
     }
public:
//     quint32 split_joint_hex_data(quint8 *carrier, quint32 joint_addr, quint8 *segment, quint32 data_len);//数据拼接
     quint32 split_joint_hex_data2(char *carrier, quint32 joint_addr, char *segment, quint32 data_len);//数据拼接
     quint32 create_data_for_center(quint16 frame_type, char *crane_id, quint8 *frame, quint8 *segment, quint32 data_len, quint8 gprs_or_site);//加入帧头帧尾
     quint32 create_data_for_center(quint16 frame_type , char *crane_id , char *frame , char *segment , quint32 data_len , quint8 gprs_or_site);
public:
     char fingerPage[2];                   //指纹页码
     int    fingerChance;
     quint32 paintWidth;                    //画图宽
     quint32 paintHight;                    //画图高
     quint32 paintGapX;                      //画图X方向间隙（相对间隙）
     quint32 paintGapY;                      //画图Y方向间隙（相对间隙）
     double scaleValue;                     //画图缩放尺寸
     double protect_start_end[6][2];		//start angle & end angle
     qint32 ZigbeeFd;                  //Zigbee通过串口发送数据的文件设备号
     qint32 isPortOpen;//串口打开标志
     bool sendphotoavailable;//图片是否可以发送标志
    // double min_max_span[6][2];				//max & min span;

     unsigned char warn_area_num;//障碍物或禁行区序号
     //保护区报警状态标志位
    quint8 left_forbid_warn_flag;
    quint8 right_forbid_warn_flag;
    quint8 near_forbid_warn_flag;
    quint8 far_forbid_warn_flag;

    quint8 left_barrier_warn_flag;
    quint8 right_barrier_warn_flag;
    quint8 near_barrier_warn_flag;
    quint8 far_barrier_warn_flag;
    quint8 low_barrier_warn_flag;

    //保护区报警语音标志位
   quint8 left_forbid_warn_voice_flag;
   quint8 right_forbid_warn_voice_flag;
   quint8 near_forbid_warn_voice_flag;
   quint8 far_forbid_warn_voice_flag;

   quint8 left_barrier_warn_voice_flag;
   quint8 right_barrier_warn_voice_flag;
   quint8 near_barrier_warn_voice_flag;
   quint8 far_barrier_warn_voice_flag;
   quint8 low_barrier_warn_voice_flag;

    //相干塔吊碰撞报警状态标志位
    quint8 interfere_crane_num;
    quint8 interfere_Type_Flag;

    quint8 left_collision_warn_flag;
    quint8 right_collision_warn_flag;
    quint8 far_collision_warn_flag;
    quint8 near_collision_warn_flag;
    quint8 tail_collision_warn_flag;
    quint8 body_collision_warn_flag;

    //相干塔吊碰撞报警语音标志位
    quint8 left_collision_warn_voice_flag;
    quint8 right_collision_warn_voice_flag;
    quint8 far_collision_warn_voice_flag;
    quint8 near_collision_warn_voice_flag;
    quint8 tail_collision_warn_voice_flag;
    quint8 body_collision_warn_voice_flag;

    //最总报警状态
    quint8 final_warn_1;
    quint8 final_warn_2;
    quint8 ultimate_warn_flag;


     quint8 identity_success;
     quint8 status_success;
   // quint8 Connect_Success;             //---------------

     //SqlDB *db;                      //数据库类

public:
     bool mStop;  //20140904

private:
    GlobalData();
    static GlobalData *m_GlobalData;


    class CGarbo
    {
    public:
        ~CGarbo()                                     //在析构函数的时候删除实例
        {
            if(GlobalData::m_GlobalData)
                delete GlobalData::m_GlobalData;
        }

    };
    static CGarbo Garbo;                           //静态成员，在程序结素时会调用它的析构函数


};


#endif // DATA_H
