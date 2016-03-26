#include"data.h"
#include <qglobal.h>

#define DATA_TRANSFER_COM 2
//基本信息
BasicData::BasicData()
{
//     basicVersion =0;                 //信息版本
//     craneName="电子科大";            //塔吊名称（16字节）
//     craneType=0;                   //塔吊类型（4bits）
//     craneRate=0;                   //吊绳倍率（4bits）
//     coordinateX=200;             //X坐标
//     coordinateY=340;             //Y坐标
//     forearmLen=100;             //前臂长
//     backarmLen=10;            //后臂长
//     craneHeight=50;            //塔高
//     craneTopHeight=10;      // 塔顶高度
}

BasicData::~BasicData()
{

}


//保护区信息
ProZoneData:: ProZoneData()
{

}
ProZoneData::~ProZoneData()
{

}


//限位信息
LimitData::LimitData()
{

}
LimitData::~LimitData()
{

}

//塔吊群塔信息
CraneData::CraneData()
{

}
CraneData::~CraneData()
{

}
//KD值信息
CraneKD::CraneKD()
{

}
CraneKD::~CraneKD()
{

}
//全局变量
GlobalData *GlobalData::m_GlobalData = NULL;//初始化
GlobalData::GlobalData()
{

    OpenSerialPort();
    globalDataInit();    //调用初始化函数

}

//void GlobalData::Stop_Gprs(GprsConnect *m)
//{
//    m->exit();
//}

void GlobalData::globalDataInit()
{
    //指纹序号初始化 将初始值定为0x0f 这样就不会和模块中已有的数据重合
    fingerPage[0] = 0x0f;
    fingerPage[1] = 0x00;
    sendphotoavailable = false;
    //数据初始化，从数据库中读取数据并对数据赋值
        craneList.clear();
    //模拟本机塔吊
    CraneData craneData;
    //从数据库中取出数据
   // SqlDB *db = new SqlDB;                                     //打开已链接的数据库
    QSqlTableModel *tableData = new QSqlTableModel;
    bool ok;

    interfere_crane_num = 0;
    /*实时信息及画图初始信息*/
    craneData.drawCenter = QPoint(0,0);
    craneData.drawForeLen = 0;
    craneData.drawBackLen = 0;
    craneData.drawSpan = 0;

    craneData.angle = 0;
    craneData.span = 40;
    craneData.hookHeight = 20;
    craneData.rotateSpeed = 0;

    craneData.moment = 0;
    craneData.windspeed = 0;
    craneData.tiltangleX = 0;
    craneData.tiltangleY = 0;

    craneData.paintColor = Qt::black;
    /*分别从数据库BasicData和CranesData中读出数据信息存入LIST*/
    tableData->setTable("BasicData");
    tableData->setFilter("id = 0");
    tableData->select();
    if (tableData->rowCount() == 1) {
        //基本信息，本机信息
         basicData.basicVersion = tableData->record(0).value("basicVersion").toInt(&ok);
        craneData.craneID = tableData->record(0).value("craneID").toInt(&ok);
        basicData.craneID = tableData->record(0).value("craneID").toInt(&ok);
        craneData.groupID = tableData->record(0).value("groupID").toInt(&ok);
        basicData.groupID = tableData->record(0).value("groupID").toInt(&ok);
        craneData.firmID = tableData->record(0).value("firmID").toInt(&ok);
        basicData.firmID = tableData->record(0).value("firmID").toInt(&ok);
        basicData.craneType = tableData->record(0).value("craneType").toInt(&ok);
        basicData.craneRate = tableData->record(0).value("craneRate").toInt(&ok);

        craneData.coordinateX = tableData->record(0).value("coordinateX").toDouble(&ok);
        basicData.coordinateX = tableData->record(0).value("coordinateX").toDouble(&ok);
        craneData.coordinateY =tableData->record(0).value("coordinateY").toDouble(&ok);
        basicData.coordinateY =tableData->record(0).value("coordinateY").toDouble(&ok);
        craneData.backarmLen =tableData->record(0).value("backarmLen").toDouble(&ok);
        basicData.forearmLen = tableData->record(0).value("forearmLen").toDouble(&ok);
        basicData.backarmLen =tableData->record(0).value("backarmLen").toDouble(&ok);
        craneData.forearmLen = tableData->record(0).value("forearmLen").toDouble(&ok);
        craneData.craneHeight = tableData->record(0).value("craneHeight").toDouble(&ok);
        basicData.craneHeight = tableData->record(0).value("craneHeight").toDouble(&ok);
        craneData.craneTopHeight = tableData->record(0).value("craneTopHeight").toDouble(&ok);
        basicData.craneTopHeight = tableData->record(0).value("craneTopHeight").toDouble(&ok);
        basicData.firstSectionHeight = tableData->record(0).value("firstSectionHeight").toDouble(&ok);
        basicData.craneSectionNum = tableData->record(0).value("craneSectionNum").toInt(&ok);
        basicData.preSectionHeight = tableData->record(0).value("preSectionHeight").toDouble(&ok);
        craneData.craneMAC = tableData->record(0).value("craneIMEI").toString();
        basicData.craneIMEI = tableData->record(0).value("craneIMEI").toString();
        craneList.append(craneData);
        //相干塔吊信息

        tableData->setTable("CranesData");
        tableData->select();
        for (quint8 i = 0 ; i < tableData->rowCount() ; i++) {
            //qDebug()<<"enter cranesdata";
            //craneList[0].angle = 0;
            craneData.craneID = tableData->record(i).value("craneID").toInt(&ok);
            craneData.groupID = tableData->record(i).value("groupID").toInt(&ok);
            craneData.firmID = tableData->record(i).value("firmID").toInt(&ok);
           // craneData.angle = tableData->record(i).value("angle").toInt(&ok);
            craneData.coordinateX = tableData->record(i).value("coordinateX").toDouble(&ok);
            craneData.coordinateY =tableData->record(i).value("coordinateY").toDouble(&ok);
            craneData.backarmLen =tableData->record(i).value("backarmLen").toDouble(&ok);
            craneData.forearmLen = tableData->record(i).value("forearmLen").toDouble(&ok);
            craneData.craneHeight = tableData->record(i).value("craneHeight").toDouble(&ok);
            craneData.craneTopHeight = tableData->record(i).value("craneTopHeight").toDouble(&ok);
            craneData.craneMAC = tableData->record(i).value("craneMAC").toString();
            craneData.relatedCraneNum = tableData->record(i).value("relatedCraneNum").toInt(&ok);//相干塔吊个数，MQY 201312添加
            craneList.append(craneData);
        }
    }

    //qDebug()<<"angle"<<craneList[0].angle;
    /*添加保护区，先在数据库中找到保护区
     *然后根据找到的数据的主键KEY
     *去元素表中查找与主键KEY对应的元素信息
        最后将信息整合放入proZoneDataList中*/
     proZoneDataList.clear();///清空保护区,添加保护区
     ProZoneData proData;
     QSqlTableModel *proZoneTable = new QSqlTableModel;
     QSqlTableModel *elementTable = new QSqlTableModel;
     QSqlQueryModel *findID = new QSqlQueryModel;//位了在表中数据中查找数据对应的主键ID
     tc = QTextCodec::codecForName("UTF-8");
     quint32 keyID = 0;
     ElementData ele;
     elementTable->setTable("ElementData");
     proZoneTable->setTable("ProZoneData");
     proZoneTable->select();
     for (qint8 i = 0 ; i < proZoneTable->rowCount() ; i++) {
         proData.proBuildType = proZoneTable->record(i).value("proBuildType").toInt(&ok);
         proData.proZoneHeight = proZoneTable->record(i).value("proZoneHeight").toDouble(&ok);
         proData.proZoneID = proZoneTable->record(i).value("proZoneID").toInt(&ok);
         proData.proZoneName = proZoneTable->record(i).value("proZoneName").toString();
         proData.proZoneVersion = proZoneTable->record(i).value("proZoneVersion").toInt(&ok);
         proData.proZoneType = proZoneTable->record(i).value("proZoneType").toInt(&ok);
         proData.proZoneOrder = proZoneTable->record(i).value("proZoneOrder").toInt(&ok);
         proData.proZoneEleNum = elementTable->rowCount();

         findID->setQuery(QString("SELECT id FROM ProZoneData WHERE proZoneID = %1").arg(proData.proZoneID));
         if (findID->rowCount() >0) {                                                   //提取元素信息
             keyID = findID->record(0).value(0).toInt();
             elementTable->setFilter(QString("proZoneID = %1").arg(keyID));
             elementTable->select();
             if (elementTable->rowCount() > 2 ) {                            //有2条以上元素信息才提取该条保护区于LIST中
                 proData.proZoneEleNum = elementTable->rowCount();
                 for (quint8 j = 0 ; j < elementTable->rowCount() ; j++) {                          //元素信息保存
                     ele.elementType = elementTable->record(j).value("elementType").toInt(&ok);
                     ele.pointX = elementTable->record(j).value("pointX").toDouble(&ok);
                     ele.pointY = elementTable->record(j).value("pointY").toDouble(&ok);
                     ele.startAngle = elementTable->record(j).value("startAngle").toInt(&ok);
                     ele.endAngle = elementTable->record(j).value("endAngle").toInt(&ok);
                     ele.radius = elementTable->record(j).value("radius").toDouble(&ok);
                     proData.elementData.append(ele);
                 }
                 proZoneDataList.append(proData);

                 proData.elementData.clear();           //清除数据，否则下一次会添加进来上次的元素数据
             }
         }
     }


     /************初始化限位信息*************/
          //qDebug()<<"%%%%%%%%******Initialize Limit Data!!";
          QSqlTableModel *LimitData = new QSqlTableModel;
          LimitData->setTable("LimitData");
          LimitData->setFilter("id = 0");
          LimitData->select();
          if (LimitData->rowCount() == 1) {
              //

              //qDebug()<<"%%%%%%%%******Initialize Limit Data!!";
              limitData.limitVersion = LimitData->record(0).value("limitVersion").toInt(&ok);
              //qDebug()<<"limitVersion = "<<limitData.limitVersion;
              limitData.leftLimit = LimitData->record(0).value("leftLimit").toDouble(&ok);
              //qDebug()<<"leftLimit = "<<limitData.leftLimit;
              limitData.rightLimit = LimitData->record(0).value("rightLimit").toDouble(&ok);
              //qDebug()<<"rightLimit = "<<limitData.rightLimit;
              limitData.farLimit = LimitData->record(0).value("farLimit").toDouble(&ok);
              limitData.nearLmit = LimitData->record(0).value("nearLmit").toDouble(&ok);
              limitData.highLimit = LimitData->record(0).value("highLimit").toDouble(&ok);
              limitData.weightLimit = LimitData->record(0).value("weightLimit").toDouble(&ok);
              limitData.momentLimit = LimitData->record(0).value("momentLimit").toDouble(&ok);
              limitData.enableBit = LimitData->record(0).value("enableBit").toInt(&ok);
          }

     /************初始化KD值及端口信息*************/
          QSqlTableModel *KD_IpData = new QSqlTableModel;
          KD_IpData->setTable("KdData");
          KD_IpData->setFilter("id = 0");
          KD_IpData->select();
          if (KD_IpData->rowCount() == 1) {
              //
              craneKD.craneKDVersion = KD_IpData->record(0).value("craneKDVersion").toInt(&ok);
              craneKD.weightK = KD_IpData->record(0).value("weightK").toDouble(&ok);
              craneKD.weightD = KD_IpData->record(0).value("weightD").toDouble(&ok);
              craneKD.spanK = KD_IpData->record(0).value("spanK").toDouble(&ok);
              craneKD.spanD = KD_IpData->record(0).value("spanD").toDouble(&ok);
              craneKD.heightK = KD_IpData->record(0).value("heightK").toDouble(&ok);
              craneKD.heightD = KD_IpData->record(0).value("heightD").toDouble(&ok);
              craneKD.serverIP = KD_IpData->record(0).value("serverIP").toString();
              craneKD.serverPort = KD_IpData->record(0).value("serverPort").toString();
              craneKD.ZigbeeChannel = KD_IpData->record(0).value("ZigbeeChannel").toString();
              craneKD.ZigbeeID = KD_IpData->record(0).value("ZigbeeID").toString();
              craneKD.pitchHeight = KD_IpData->record(0).value("pitchHeight").toDouble(&ok);
              craneKD.initHeight = KD_IpData->record(0).value("initHeight").toDouble(&ok);
              craneKD.pitchNum = KD_IpData->record(0).value("pitchNum").toDouble(&ok);
              craneKD.proHeightEW = KD_IpData->record(0).value("proHeightEW").toDouble(&ok);
              craneKD.proHeightW = KD_IpData->record(0).value("proHeightW").toDouble(&ok);
          }
/***读取力矩表数据到数组里******/
    GetMomentData();
    calculateScale();
    calculateProZoneData();
    flagInit();
    drawPathInit();

}

void GlobalData::GetMomentData()
{
    //QFile file("momenttable2.txt");
    //if(!file.open(QIODevice::ReadOnly |))
}

void GlobalData::flagInit()                            //标志位初始化
{
    craneList[0].leftCollision = 0;
    craneList[0].rightCollision = 0;
    craneList[0].nearCollision =0;
    craneList[0].farCollision =0;
    craneList[0].tailCollision = 0;
    craneList[0].bodyCollision = 0;
    craneList[0].lowCollision = 0;

    left_forbid_warn_flag = 0;
    right_forbid_warn_flag = 0;
    near_forbid_warn_flag = 0;
    far_forbid_warn_flag = 0;

    left_barrier_warn_flag = 0;
    right_barrier_warn_flag = 0;
    near_barrier_warn_flag = 0;
    far_barrier_warn_flag = 0;
    low_barrier_warn_flag = 0;

    left_collision_warn_flag = 0;
    right_collision_warn_flag = 0;
    far_collision_warn_flag = 0;
    near_collision_warn_flag = 0;
    tail_collision_warn_flag = 0;
    body_collision_warn_flag = 0;

    ultimate_warn_flag = 0;


}

void GlobalData::calculateScale()
{

    //计算画图比例尺
    float xMin = 0,xMax = 0,yMin= 0 ,yMax = 0;//塔群值
    float proXMin = 0,proXMax = 0,proYMin = 0,proYMax = 0;  //保护区值
    float width,height;
    float startX,startY;

    if (craneList.count()<1)
        return;

    xMin = (craneList[0].coordinateX - craneList[0].forearmLen);
    xMax = (craneList[0].coordinateX + craneList[0].forearmLen);
    yMin = (craneList[0].coordinateY - craneList[0].forearmLen);
    yMax = (craneList[0].coordinateY + craneList[0].forearmLen);

    for (quint8 i = 1 ; i<craneList.count() ; i++)
    {
        if ((craneList[i].coordinateX - craneList[i].forearmLen) < xMin)
            xMin = (craneList[i].coordinateX - craneList[i].forearmLen);
        if ((craneList[i].coordinateX + craneList[i].forearmLen) >xMax)
            xMax = craneList[i].coordinateX + craneList[i].forearmLen;
        if ((craneList[i].coordinateY - craneList[i].forearmLen) < yMin)
            yMin = craneList[i].coordinateY - craneList[i].forearmLen;
        if ((craneList[i].coordinateY + craneList[i].forearmLen) > yMax)
           yMax = craneList[i].coordinateY + craneList[i].forearmLen;
    }

    //保护区坐标计算,计算吃最大最小的XY坐标
    QPointF point;
    float tmpX,tmpY;
    for (quint8 i =0 ; i < proZoneDataList.count() ; i++) {
        for (quint8 j =0 ; j < proZoneDataList.at(i).elementData.count() ; j++ ) {
            //点元素，判断最大最小XY
            if (0 == proZoneDataList.at(i).elementData.at(j).elementType) {
                proXMin = proXMin > proZoneDataList.at(i).elementData.at(j).pointX ? proZoneDataList.at(i).elementData.at(j).pointX : proXMin;
                proXMax = proXMax >proZoneDataList.at(i).elementData.at(j).pointX ? proXMax : proZoneDataList.at(i).elementData.at(j).pointX;
                proYMin = proXMin > proZoneDataList.at(i).elementData.at(j).pointY ? proZoneDataList.at(i).elementData.at(j).pointY : proYMin;
                proYMax = proXMax >proZoneDataList.at(i).elementData.at(j).pointY ? proYMax : proZoneDataList.at(i).elementData.at(j).pointY;
            //圆弧,只计算起点终点的值用与判断最大最小XY
            } else if (1 == proZoneDataList.at(i).elementData.at(j).elementType) {
                tmpX = proZoneDataList.at(i).elementData.at(j).pointX \
                        + proZoneDataList.at(i).elementData.at(j).radius *cos(proZoneDataList.at(i).elementData.at(j).startAngle);
                tmpY = proZoneDataList.at(i).elementData.at(j).pointY \
                        + proZoneDataList.at(i).elementData.at(j).radius *sin(proZoneDataList.at(i).elementData.at(j).startAngle);
                proXMin = proXMin > tmpX ? tmpX : proXMin;
                proXMax = proXMax > tmpX ? proXMax : tmpX;
                proYMin = proYMin > tmpY ? tmpY : proYMin;
                proYMax = proYMax > tmpY ? proYMax : tmpY;

                tmpX = proZoneDataList.at(i).elementData.at(j).pointX \
                        + proZoneDataList.at(i).elementData.at(j).radius *cos(proZoneDataList.at(i).elementData.at(j).endAngle);
                tmpY = proZoneDataList.at(i).elementData.at(j).pointY \
                        + proZoneDataList.at(i).elementData.at(j).radius *sin(proZoneDataList.at(i).elementData.at(j).endAngle);
                proXMin = proXMin > tmpX ? tmpX : proXMin;
                proXMax = proXMax > tmpX ? proXMax : tmpX;
                proYMin = proYMin > tmpY ? tmpY : proYMin;
                proYMax = proYMax > tmpY ? proYMax : tmpY;
        }
    }
}
    //因为保护区坐标是相对坐标，所以位了与塔群计算出来的坐标比较大小需要加上本机塔吊的原点坐标
    proXMin = proXMin + craneList.at(0).coordinateX;
    proXMax = proXMax + craneList.at(0).coordinateX;
    proYMin = proYMin + craneList.at(0).coordinateY;
    proYMax = proYMax + craneList.at(0).coordinateY;
    //将计算的最大最小值与之前塔群计算出来的最大最小比较，找出缩放的最大最小值
    xMin = xMin > proXMin ? proXMin : xMin;
    xMax = xMax > proXMax ? xMax : proXMax;
    yMin = yMin > proYMin ? proYMin : yMin;
    yMax = yMax > proYMax ? yMax : proYMax;

    //根据最大最小的XY坐标计算缩放尺寸
    width = (PAINT_W - 2*PAINT_GAPX) / (xMax - xMin);           //水平宽缩放尺寸
    height = (PAINT_H -2*PAINT_GAPY) / (yMax - yMin);            //垂直高缩放尺寸

    startX = (xMax + xMin) / 2;                                             //用于计算画图坐标
    startY = (yMax + yMin) / 2;

    scaleValue = width > height ? height : width;                   //屏幕缩放尺寸
    //塔群的画图数据
    for (quint8 i = 0 ; i < craneList.count() ; i++ )                       //更新全局变量中与画图有关的数据
    {
        craneList[i].drawForeLen = craneList[i].forearmLen * scaleValue;    //前臂同比缩小
        craneList[i].drawBackLen = craneList[i].backarmLen * scaleValue;    //后臂同比缩小
        craneList[i].drawCenter.setX(PAINT_CENTER_X + (craneList[i].coordinateX - startX) * scaleValue);
        craneList[i].drawCenter.setY(PAINT_CENTER_Y - (craneList[i].coordinateY - startY) * scaleValue);
        craneList[i].drawSpan = craneList[i].span * scaleValue;
    }
    //保护区的画图数据
    for (quint8 i =0 ; i < proZoneDataList.count() ; i++) {
        for (quint8 j =0 ; j < proZoneDataList.at(i).elementData.count() ; j++ ) {
            proZoneDataList[i].elementData[j].drawPointX = craneList.at(0).drawCenter.x() + proZoneDataList.at(i).elementData.at(j).pointX * scaleValue;
            proZoneDataList[i].elementData[j].drawPointY = craneList.at(0).drawCenter.y() - proZoneDataList.at(i).elementData.at(j).pointY * scaleValue;
            proZoneDataList[i].elementData[j].drawRadius = proZoneDataList.at(i).elementData.at(j).radius * scaleValue;
        }
    }
}

void GlobalData::calculateProZoneData()
{
    //calculate prozone data
    struct float_point point[20];
    double crane_r = craneList.at(0).forearmLen;
    double min = crane_r,max = 0;
    char first=0,second=0,first_end=0;
    //first = second = first_end = 0;
    char b_span=0;
    int ele_num=0;
    double angle_protect[2][2];
    int proZone_count = proZoneDataList.count();
  //  qDebug() <<"proZoneDataList.count()"<< proZone_count;
    for (int num = 0 ; num < proZone_count ; num++) {
        first=0;
        second=0;
        first_end=0;
        //qDebug()<< "enter for" << num;
        protect_start_end[num][0] = 0;
        protect_start_end[num][1] = 0;
        ele_num = proZoneDataList.at(num).elementData.count();
        //qDebug() << "ele_num" << ele_num;
        for (int i = 0 ; i< 360 ; i++) {
            struct float_point startdian,endian;
            startdian.x = 0;
            startdian.y = 0;
            endian.x = (crane_r*cos(i*pi/180));
            endian.y = (crane_r*sin(i*pi/180));
            struct float_point cross_point[6];
            int n = 0,m = 0;
            float temp[2];

            for (int j = 0 ; j < ele_num ; j++) {

                if(proZoneDataList.at(num).elementData.at(j).elementType == 0) {
                  //  qDebug() << "enter elementType=0";
                    point[m].x = (proZoneDataList.at(num).elementData.at(j).pointX);
                    point[m].y = (proZoneDataList.at(num).elementData.at(j).pointY);
                  //  qDebug()<< "point[m].x"<<point[m].x<<"point[m].y"<<point[m].y;
                    if( j != 0) {
                       // int a =0;
                        int a = cacu_cross(point[m-1],point[m],startdian,endian,temp);
                       // qDebug() << "a="<<a;
                        if (a) {
                            cross_point[n].x=temp[0];
                            cross_point[n].y=temp[1];
                            n++;
                            }
                        }
                    m++;
                    if(j == ele_num -1) {
                       // qDebug()<<"enter j==ele_num"<<j;
                       // int a =0;
                        int a = cacu_cross(point[m-1],point[0],startdian,endian,temp);
                        if(a) {
                            cross_point[n].x=temp[0];
                            cross_point[n].y=temp[1];
                            n++;
                        }
                        break;
                    }
                }
                else if (proZoneDataList.at(num).elementData.at(j).elementType == 1) {
                   // qDebug() << "enter elementType=1";
                    float x,y,r,starangle,endangle;
                    x = proZoneDataList.at(num).elementData.at(j).pointX;
                    y = proZoneDataList.at(num).elementData.at(j).pointY;
                    r = proZoneDataList.at(num).elementData.at(j).radius;
                    starangle = proZoneDataList.at(num).elementData.at(j).startAngle;
                    endangle = proZoneDataList.at(num).elementData.at(j).endAngle;
                    struct float_point temp1[2];
                    int k=cacu_line_circle(startdian,endian,x,y,r,starangle,endangle,temp1);
                    if (k==1)
                    {
                        cross_point[n].x=temp1[0].x;
                        cross_point[n].y=temp1[0].y;
                        n++;
                    }
                    else if (k==2)
                    {
                        cross_point[n].x=temp1[0].x;
                        cross_point[n].y=temp1[0].y;
                        n++;

                        cross_point[n].x=temp1[1].x;
                        cross_point[n].y=temp1[1].y;
                        n++;
                    }

                    point[m].x=x+r*cos(starangle*pi/180);
                   // point[m].x=x+r*cos_tab[(unsigned int)(starangle*TVECT)];
                    point[m].y=y+r*sin(starangle*pi/180);
                   // point[m].y=y+r*sin_tab[(unsigned int)(starangle*TVECT)];

                    m++;

                    point[m].x=x+r*cos(endangle*pi/180);
                    point[m].y=y+r*sin(endangle*pi/180);

                    m++;
                    if (j==ele_num-1){
                        int a=cacu_cross(point[m-1],point[0],startdian,endian,temp);
                        if (a){
                            cross_point[n].x=temp[0];
                            cross_point[n].y=temp[1];
                            n++;
                        }
                        break;
                    }

                }

            }
         //  qDebug() << "i"<< i;

            if( n != 0 ) {
                if ((!first)&&!(first_end))
                {
                    angle_protect[0][0]=i;
                    first=1;
                }
                if (first&&(!first_end))
                {
                    angle_protect[0][1]=i;
                }
                if (first_end&&(!second))
                {
                    angle_protect[1][0]=i;
                    second=1;
                }
                if (second)
                {
                    angle_protect[1][1]=i;
                }
               float float_data[7];
               int k = 0;
              // int k=(int)float_data[0];
                if (n==1)
                {

                    float d1=pow(cross_point[0].x,2)+pow(cross_point[0].y,2);
                    float d2=pow(crane_r,2);

                    float_data[k*2+1]=sqrt(d1);
                    float_data[k*2+2]=sqrt(d2);

                    float_data[0]+=1;
                                        if(!b_span)
                                        {
                                              min=sqrt(d1);
                                              max=sqrt(d2);
                                              b_span=1;
                                        }
                                        min=min<sqrt(d1)?min:sqrt(d1);
                                        max=max>sqrt(d2)?max:sqrt(d2);
                }
                else if (n==2)
                {

                    float d1=pow(cross_point[0].x,2)+pow(cross_point[0].y,2);
                    float d2=pow(cross_point[1].x,2)+pow(cross_point[1].y,2);
                    float temp1=fmin(d1,d2);
                    float temp2=fmax(d1,d2);
                    float_data[k*2+1]=sqrt(temp1);
                    float_data[k*2+2]=sqrt(temp2);

                                        float_data[0]+=1;
                                        if(!b_span)
                                        {
                                              min=sqrt(temp1);
                                              max=sqrt(temp2);
                                              b_span=1;
                                        }
                                         min=min<sqrt(temp1)?min:sqrt(temp1);
                                         max=max>sqrt(temp2)?max:sqrt(temp2);

                }
                if (n==3)
                {

                    float d[3];
                    for(int z=0;z<3;z++)
                    {
                        d[z]=pow(cross_point[z].x,2)+pow(cross_point[z].y,2);
                    }
                    select_sort(d,3);
                    float_data[k*2+1]=sqrt(d[0]);
                    float_data[k*2+2]=sqrt(d[2]);
                                        float_data[0]+=1;
                                        if(!b_span)
                                        {
                                              min=sqrt(d[0]);
                                              max=sqrt(d[2]);
                                              b_span=1;
                                        }
                                         min=min<sqrt(d[0])?min:sqrt(d[0]);
                                         max=max>sqrt(d[2])?max:sqrt(d[2]);

                }
                else if (n==4)
                {
                    float d[4];
                    for(int v=0;v<4;v++)
                    {
                        d[v]=pow(cross_point[v].x,2)+pow(cross_point[v].y,2);
                    }
                    select_sort(d,4);
                    for(int v=0;v<4;v++)
                    {
                        float_data[k*2+1+v]=sqrt(d[v]);
                    }
                    float_data[0]+=2;

                    if(!b_span){
                         min=sqrt(d[0]);
                         max=sqrt(d[3]);
                         b_span=1;
                         }
                    min=min<sqrt(d[0])?min:sqrt(d[0]);
                    max=max>sqrt(d[3])?max:sqrt(d[3]);
                    }

                for (int l = 0; l < 7 ; l++) {
                    proZoneDataList[num].point_perangle[i][l] =float_data[l];
                }
               // qDebug()<<"end 1234";
            }
            if (first&&(!first_end)) {
                if ((i-angle_protect[0][1])>=3) {
                    first_end=1;             //
                    }
                }
            }

//        qDebug() << "end 360";
//        qDebug()<<"angle_protect[0][0]"<<angle_protect[0][0];
//        qDebug()<<"angle_protect[0][1]"<<angle_protect[0][1];
//        qDebug()<<"angle_protect[1][0]"<<angle_protect[1][0];
//        qDebug()<<"angle_protect[1][1]"<<angle_protect[1][1];
        if (!second) {
            proZoneDataList[num].proZoneStartAngle = angle_protect[0][0];
            //protect_start_end[num][0]=angle_protect[0][0];
            proZoneDataList[num].proZoneEndAngle = angle_protect[0][1];
            //protect_start_end[num][1]=angle_protect[0][1];
            proZoneDataList[num].min_span = min;
            proZoneDataList[num].max_span = max;
            //min_max_span[num][0]=min;
           // min_max_span[num][1]=max;
            }

        else if (first_end&&(second)) {
            proZoneDataList[num].proZoneStartAngle = angle_protect[1][0];
           // protect_start_end[num][0]=angle_protect[1][0];
            proZoneDataList[num].proZoneEndAngle = angle_protect[1][1];
            //protect_start_end[num][1]=angle_protect[0][1];
            proZoneDataList[num].min_span = min;
            proZoneDataList[num].max_span = max;
           // min_max_span[num][0]=min;
           // min_max_span[num][1]=max;
            }
        }
}






/*绘图路径初始化*/
void GlobalData::drawPathInit()
{
    //保护区绘图路径保存
    for (quint8 i = 0 ; i < proZoneDataList.count() ; i++) {
        proZoneDataList[i].drawPath.moveTo(proZoneDataList.at(i).elementData.at(0).drawPointX,
                                                                proZoneDataList.at(i).elementData.at(0).drawPointY);

        for (quint8 j = 0 ; j < proZoneDataList.at(i).elementData.count() ; j++) {
            if (0 == proZoneDataList.at(i).elementData.at(j).elementType )   //点元素
                proZoneDataList[i].drawPath.lineTo(proZoneDataList.at(i).elementData.at(j).drawPointX,
                                                                    proZoneDataList.at(i).elementData.at(j).drawPointY);
            else if (1 == proZoneDataList.at(i).elementData.at(j).elementType) {      //圆弧
                proZoneDataList[i].drawPath.arcTo(proZoneDataList.at(i).elementData.at(j).drawPointX-
                                                                    proZoneDataList.at(i).elementData.at(j).drawRadius,

                                                                    proZoneDataList.at(i).elementData.at(j).drawPointY-
                                                                    proZoneDataList.at(i).elementData.at(j).drawRadius,

                                                                    proZoneDataList.at(i).elementData.at(j).drawRadius,

                                                                    proZoneDataList.at(i).elementData.at(j).drawRadius,

                                                                    proZoneDataList.at(i).elementData.at(j).startAngle,

                                                                    (proZoneDataList.at(i).elementData.at(j).endAngle -
                                                                    proZoneDataList.at(i).elementData.at(j).startAngle));
            }

        }
        proZoneDataList[i].drawPath.closeSubpath();

        //初始化绘图颜色
        if (0 == proZoneDataList.at(i).proZoneType) {               //禁行区用green标志
            proZoneDataList[i].originalColor = Qt::yellow;
            proZoneDataList[i].currentColor = Qt::yellow;
        }else if (1 == proZoneDataList.at(i).proZoneType) {
            proZoneDataList[i].originalColor = Qt::cyan;
            proZoneDataList[i].currentColor = Qt::cyan;
        }
    }

    for (quint8 i = 0 ; i < craneList.count() ; i++) {              //塔群圆绘图路径
        craneList[i].drawPath.addEllipse(craneList.at(i).drawCenter,
                                            craneList.at(i).drawForeLen,
                                            craneList.at(i).drawForeLen);
    }


    /*防碰撞数据初始化*/
    //计算包络数据
    for (quint8 i = 0 ; i < craneList.count() ; i++) {
        craneList[i].bottomEnvelope = craneList.at(i).craneHeight - 1 ;//底部包络边界 = 塔臂高 - 1米；
        craneList[i].topEnvelope = craneList.at(i).craneHeight + craneList.at(i).craneTopHeight; //顶部包络边界 = 塔臂高+塔帽高度
    }

}


quint16 GlobalData::dataCRC16(const char *pucSendBuf , quint32 usLen , quint8 *CRC)     //CRC16校验算法,低字节在前
{
    quint16  i, j ;
    quint16 usCrc = 0xFFFF;
    for ( i = 0 ; i < usLen ; i++ ){
        usCrc ^= pucSendBuf[i];
        for ( j = 0 ; j < 8 ; j++ ) {
        if ( usCrc & 1 ){
            usCrc >>=1;
            usCrc ^=0xA001;
        }else{
            usCrc >>= 1;
        }
        }
    }
   // return usCrc;
    CRC [1] = (usCrc >> 8) & 0xFF;
    CRC [0] = (usCrc & 0xFF);
    return usCrc;
}


///****************数据拼接***********************/

//quint32 GlobalData::split_joint_hex_data(quint8 *carrier, quint32 joint_addr, quint8 *segment, quint32 data_len)
//{
//    quint32 next_addr;
//    memcpy (carrier + joint_addr , segment ,data_len);
//    next_addr = joint_addr +data_len;
//    return next_addr;
//}
/****************有符号数组数据拼接***********************/

quint32 GlobalData::split_joint_hex_data2(char *carrier, quint32 joint_addr, char *segment, quint32 data_len)
{
    quint32 next_addr;
    memcpy (carrier + joint_addr , segment ,data_len);
    next_addr = joint_addr +data_len;
    return next_addr;
}


/*******************************/

quint32 GlobalData::create_data_for_center(quint16 frame_type , char *crane_id , quint8 *frame , quint8 *segment , quint32 data_len , quint8 gprs_or_site)
{
    quint32 addr = 0;
    quint8  tem_id = 0x05;
    quint8 id_len;
    quint8 device_code[16];
//    qDebug()<<"**************create**************";
    if ( (frame_type == 0x00) || (frame_type == 0x05) || (gprs_or_site == 1))
    {
//        qDebug()<<"****************************";
        id_len = 16;
        memset (device_code , 0 , 16);
        memcpy (device_code , crane_id , 16);
    }
    else
    {
        id_len = 3;
        memset (device_code , 0 , 16);
        memcpy (device_code ,crane_id ,3);
    }
    quint8 v_and_f[1];
    quint8 f_type_and_data_len[2];
    v_and_f[0] = (VERSION << 4) | tem_id;
    f_type_and_data_len[0] = (frame_type << 3) | (data_len >> 8);
    f_type_and_data_len[1] = data_len & 0xFF;
//qDebug()<<"**************create**************";
    addr = split_joint_hex_data(frame , addr , v_and_f , 1);
    addr = split_joint_hex_data(frame , addr , f_type_and_data_len , 2);
    addr = split_joint_hex_data(frame , addr , device_code ,id_len);
   // qDebug()<<"***********addr********"<<addr;
    addr = split_joint_hex_data(frame , addr , segment ,data_len);
    //   qDebug()<<"***********addr********"<<addr;
    return addr;
}

/*******************************/
quint32 GlobalData::create_data_for_center(quint16 frame_type , char *crane_id , char *frame , char *segment , quint32 data_len , quint8 gprs_or_site)
{
    quint32 addr = 0;
    quint8  tem_id = 0x05;
    quint8 id_len;
    char device_code[16];
//    qDebug()<<"**************create**************";
    if ( (frame_type == 0x00) || (frame_type == 0x05) || (gprs_or_site == 1))
    {
//        qDebug()<<"****************************";
        id_len = 16;
        memset (device_code , 0 , 16);
        memcpy (device_code , crane_id , 16);
    }
    else
    {
        id_len = 3;
        memset (device_code , 0 , 16);
        memcpy (device_code ,crane_id ,3);
    }
    char v_and_f[1];
    char f_type_and_data_len[2];
    v_and_f[0] = (VERSION << 4) | tem_id;
    f_type_and_data_len[0] = (frame_type << 3) | (data_len >> 8);
    f_type_and_data_len[1] = data_len & 0xFF;
//qDebug()<<"**************create**************";
    addr = split_joint_hex_data2(frame , addr , v_and_f , 1);
    addr = split_joint_hex_data2(frame , addr , f_type_and_data_len , 2);
    addr = split_joint_hex_data2(frame , addr , device_code ,id_len);
   // qDebug()<<"***********addr********"<<addr;
    addr = split_joint_hex_data2(frame , addr , segment ,data_len);
    //   qDebug()<<"***********addr********"<<addr;
    return addr;
}
/*
**************************************************************
**************************************************************
*/
void GlobalData::OpenSerialPort()
{
    if ( ( ZigbeeFd = openPort( DATA_TRANSFER_COM) ) < 0 ) {      //打开串口
        qDebug() << "******Open failed";
        qDebug()<<"***************************************";
    }
    else
    {
        qDebug()<<"*****Open port success !";
        qDebug()<<"ZigbeeFd="<<ZigbeeFd;
        qDebug()<<"***************************************";
    }
    if (setComConfig( ZigbeeFd , 9600 , 8 , 'N' , 1 ) < 0 ){
        qDebug() << "*****Set COM failed";
        qDebug()<<"***************************************";
    }
    else
    {
        qDebug()<<"*****Set COM success !";
        qDebug()<<"***************************************";
    }
}


