#include "sqldb.h"

SqlDB::SqlDB(QObject *parent) :
    QObject(parent)
{
}

void SqlDB::connectDB()
{
    db = QSqlDatabase::addDatabase("QSQLITE");          //添加sqlite类型的数据库
    db.setDatabaseName("crane.db");                       //指定数据库名为crane.s3db
   // db.setDatabaseName(":memory:");
    if ( !db.open()) {
        QMessageBox::critical(NULL,"sqlDB","connect error");
        return;
    }else{
        //qDebug()<<"open db success";
        QSqlQuery q(db);
        /*创建基本信息表,并初始化值*/
        /*
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
        quint32 preSectionHeight;       //每节塔高
        quint32 firstSectionHeight;         //首节塔高
        QString craneIMEI;                      //塔机IMEI*/
        q.exec("create table BasicData(id integer primary key ,"
                                                      "basicVersion int,"    //信息版本
                                                      "craneName varchar(16),"      //塔吊名称（16字节）
                                                      "craneID int,"             //塔机ID
                                                      "groupID int,"            //塔群ID
                                                      "firmID int,"                 //厂商ID
                                                      "craneType int,"           //塔吊类型（4bits）
                                                      "craneRate int,"           //吊绳倍率（4bits）
                                                      "coordinateX double,"         //X坐标(-3276.8-3276.8)
                                                      "coordinateY double,"        //Y坐标(-3276.8-3276.8)
                                                      "forearmLen double,"         //前臂长(0-6553.5)
                                                      "backarmLen double,"         //后臂长(0-6553.5)
                                                      "craneHeight double,"         //塔高(0-6553.5)
                                                      "craneTopHeight double,"     // 塔顶高度(0-25.6)
                                                      "craneSectionNum int,"        //塔吊节数
                                                      "preSectionHeight double,"           //每节塔高
                                                      "firstSectionHeight double,"          //首节塔高
                                                      "craneIMEI varchar)");                    //塔机IMEI
        q.exec(tr("INSERT INTO BasicData VALUES(0,"
                  "3,"
                  "'电子科大',"
                  "0,"
                  "0,"
                  "0,"
                  "0,"
                  "2,"
                  "0,"
                  "0,"
                  "50,"
                  "10,"
                  "20,"
                  "10,"
                  "10,"
                  "2.5,"
                  "'0',"
                  "'8651910000')"));

        /*创建保护区信息并初始化*/
        /*
        quint16 proZoneVersion;                 //保护区信息版本（2Bytes）
        //quint8  ProZoneNum;                    //保护区个数（1Bytes）

        quint8 proZoneType;                     //保护区类型（1bit  [7]）
        quint8 proZoneOrder;                    //保护区序号（3bits [6:4]）
        quint8  proZoneEleNum;              //保护区元素个数（4bits [3:0]）三种合成一个Byte

        QString proZoneName;                //保护区名称（16Bytes）
        quint8 proZoneID;                       //保护区ID（1Bytes）
        quint8 proBuildType;                    //保护区建筑类型（1Bytes）
        double proZoneHeight;                 //保护区高度（2Bytes）
         QList<ElementData> elementData;        //元素信息数据*/
        q.exec("create table ProZoneData(id integer primary key ,"
                                                      "proZoneVersion int,"    //信息版本
                                                      "proZoneName varchar(16),"      //保护区名称（16字节）
                                                      "proZoneID int,"           //保护区ID（1Bytes）

                                                      "proZoneType int,"             //保护区类型（1bit  [7]）
                                                      "proZoneOrder int,"            //保护区序号（3bits [6:4]）
                                                      "proZoneEleNum int,"                 //保护区元素个数（4bits [3:0]）三种合成一个Byte

                                                      "proBuildType int,"           //保护区建筑类型（1Bytes）
                                                      "proZoneHeight double)");                    //保护区高度（2Bytes）
        /*创建保护区元素信息表*"foreign key(proZoneID) references ProZoneData)"*/
        /*
        quint8 elementType;          //元素类型（0x00点，0x01圆弧）
        double pointX;                  //X坐标(（2Bytes）-3276.8-3276.8)
        double pointY;                  //Y坐标(（2Bytes）-3276.8-3276.8)
        double radius;                 //圆半径(（2Bytes）0-6553.5)
        double startAngle;              //起始角度(（2Bytes）0-360)
        double endAngle;                //终止角度(（2Bytes）0-360)*/
        q.exec("create table ElementData(id integer primary key ,"
                                                          "proZoneID int,"       //从属于保护区ID
                                                          "elementType int,"    //元素类型
                                                          "pointX double,"      //元素X坐标
                                                          "pointY double,"           //元素Y坐标
                                                          "radius double,"             //圆半径
                                                          "startAngle int,"         //起始角度
                                                          "endAngle int)");     //终止角度
        /*塔吊群信息*/
        /*
        QString craneName;            //塔吊名称（16字节)
        quint8 craneID;                    //塔机ID
        quint8 groupID;                    //塔群ID
        quint8 firmID;                      //厂商ID
        //实时数据信息
        double angle;                        //角度
        double span;                         //变幅
        double hookHeight;                //吊钩高度
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
        */
        /*
                                                          "angle double,"               //角度
                                                          "span double,"                //变幅
                                                          "hookHeight double,"      //吊钩高度
                                                          "rotateSpeed double,"      //转速
                                                          "wight double,"               //吊重
 */
        q.exec("create table CranesData(id integer primary key ,"
                                                          "craneName varchar(16),"       //1塔吊名称
                                                          "craneMAC varchar(16),"           //2塔吊IMEI（MAC）
                                                          "craneID int,"    //3塔吊ID
                                                          "groupID int,"        //4塔群ID
                                                          "firmID int,"             //5厂商ID
                                                          "angle double,"
                                                          "span double,"
                                                          "hookHeight double,"
                                                           "windspeed double,"
                                                          "rotateSpeed double,"

                                                          "coordinateX double,"      //6 X坐标(-3276.8-3276.8)
                                                          "coordinateY double,"      //7 Y坐标(-3276.8-3276.8)
                                                          "craneHeight double,"      //8 塔吊高
                                                          "craneTopHeight double,"      //9 塔吊顶高

                                                          "forearmLen double,"           //10 前臂长(0-6553.5)
                                                          "backarmLen double,"           //11 后臂长(0-6553.5)

                                                          "DrawCircleX int,"                      //12 圆X
                                                          "DrawCircleY int,"                      //13 圆Y
                                                          "drawForeLen int,"                         //14 绘图前臂长
                                                          "drawBackLen int,"                         //15 绘图后臂长
                                                          "drawSpan int)");                           //16 绘图变幅

        q.exec("create table LimitData(id integer primary key ,"
                                                          "limitVersion int,"        //限位信息版本
                                                          "leftLimit double,"          //左限位(-3276.8-3276.8)
                                                          "rightLimit double,"    //3//右限位(-3276.8-3276.8)
                                                          "farLimit double,"        //4远限位(0-25.5)
                                                          "nearLmit double,"             //5进限位(0-25.5)

                                                          "highLimit double,"      //6 高限位(0-25.5)
                                                          "weightLimit double,"      //7 吊重限位(0-655.35)
                                                          "momentLimit double,"      //8 力矩限位值(0-655.35)
                                                          "enableBit int)");                           //传感器使能标志位
        /*  quint16 craneKDVersion;      //信息版本
          double weightK;                     //吊重K值
          double weightD;                      //吊重K值
          double spanK;                         //变幅K值
          double spanD;                         //变幅D值
          double heightK;                        //高度K值
          double heightD;                        //高度D值
          QString serverIP;                       //IP
          QString serverPort;                    //端口
          QString ZigbeeChannel;             //通信信道
          QString ZigbeeID;                      //网络ID*/
          q.exec("create table KdData(id integer primary key ,"
                                                            "weightK double,"        //1吊重K值
                                                            "weightD double,"          //2吊重D值
                                                            "spanK double,"               //3变幅K值
                                                            "spanD double,"                //4变幅D值
                                                            "heightK double,"             //5高度K值
                                                            "heightD double,"             //6高度D值
                                                            "serverIP varchar(16),"                //7IP
                                                            "serverPort varchar,"             //8端口
                                                            "ZigbeeChannel varchar,"      //9 通信信道
                                                            "ZigbeeID varchar,"//10网络ID
                                                            "rotation number)");//11转动圈数

          q.exec("create table FingerStore(id integer primary key ,"
                                                          "DriverID int,"
                                                          "DriverVersion int)");
          //添加了FingerStore(DriverID,司机ID，0-9
          //DriverVersion)版本
          //信息 13.11.18
        q.exec();
        }
}

void SqlDB::closeDB()
{
    db.close();
}
