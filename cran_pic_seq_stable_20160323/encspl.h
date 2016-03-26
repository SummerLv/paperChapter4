#ifndef ENCSPL_H
#define ENCSPL_H

#include<QtGlobal>
#include"data.h"
#include<QSqlTableModel>
 #include <QVector>
#include<QMessageBox>
#include"sqldb.h"
#include<QChar>
#include <QMutex>

class EncSpl
{
public:
    EncSpl();

    quint8 center_receive_ID[3];


    GlobalData *globalData;                 //全局变量
    QSqlTableModel *crane_group_inter;//塔群相干信息
    /*****int转换为数组模板******/
private:
    QMutex sql_mutex;
public:
        template <typename T>
        void Double_To_Array(T *p, double value)//入口参数为整型
        {
            qint32 TempValue;
            TempValue = (int)value;
            *p =((TempValue >> 8) & 0xFF);
             p++;
            *p = (TempValue & 0xFF);
        }
    /****保护区模板函数*****/
public:
    template <typename T>  /*quint32 EncProZone(T *pro_info_buf, quint32 proZone_index);//封装保护区信息*/
    /***************************************/
    /***********保护区信息拼接**********/
    /***************************************/
     quint32 EncProZone(T *pro_info_buf,quint32 proZone_index)
    {
        quint32 addr = 0;
        quint32 len = 0;

        T  info_type[1] = {0x02};//信息类别
        T  proZone_version[2];  //信息版本，2字节
        T  proZone_num[1];//保护区个数
        T  proZone_typ_ord_elenum[1];//保护区类型+序号+元素个数，拼接字节 , 1字节
        quint8 proZone_type_int = 0;
        quint8 proZone_elenum_int = 0;//元素个数
        T  proZone_name[16];//保护区名称
        QByteArray proZone_name_byte;
        T  proZone_id[1];//保护区id
        T  proZone_build_type[1];//建筑类别
        T  proZone_height[2];//保护区高度
        T  proZone_ele_type[1];//保护区元素类型
        T  proZone_ele_locX[2];//保护区元素X坐标
        T  proZone_ele_locY[2];//保护区元素Y坐标
        T  proZone_ele_rad[2];//保护区元素圆半径
        T  proZone_ele_start_ang[2];//保护区元素起点角度
        T  proZone_ele_end_ang[2];//保护区元素终点角度

        //将保护区拼接数组清空
        memset(pro_info_buf , 0 , 70);

    /******将数据库中的各个类型的数据转换为字节数组，以便拼接********/
        //qDebug()<<"proZone_num = "<<globalData->proZoneDataList.count();
        char prozone_num_tem;
       prozone_num_tem = globalData->proZoneDataList.count();
        proZone_num[0] = (T)prozone_num_tem;//保护区个数，只有这个变量是依赖QList的成员函数求得
        //qDebug()<<"proZone_num = "<<proZone_num[0];
        if (proZone_index > proZone_num[0] )
        {
            qDebug()<<"No proZoneData!";
            addr = globalData->split_joint_hex_data(pro_info_buf , addr , info_type , 1);
            for(int i = addr; i < 25;i++){
                pro_info_buf[i] = 0;
            }
            addr = 25;
            len = addr;
        }
        else{
        Double_To_Array(proZone_version, globalData->proZoneDataList.at(proZone_index - 1 ).proZoneVersion);//保护区信息版本

        //保护区类型+序号+元素个数
        //qDebug()<<"proZoneOrder = "<<globalData->proZoneDataList.at(proZone_index -1 ).proZoneOrder;
        proZone_typ_ord_elenum[0] = ((globalData->proZoneDataList.at(proZone_index -1).proZoneType << 7) & 0x80)
                                                    +((globalData->proZoneDataList.at(proZone_index -1 ).proZoneOrder << 4) & 0x70)
                                                      +((globalData->proZoneDataList.at(proZone_index -1).proZoneEleNum) & 0x0F);
        //保护区类型，如果为禁行区（0），则只是保护区建筑类别有效；
        //若为障碍物（1），则只是保护区高度有效
        proZone_type_int = globalData->proZoneDataList.at(proZone_index -1).proZoneType;
        proZone_elenum_int = globalData->proZoneDataList.at(proZone_index -1).proZoneEleNum;//该保护区元素个数

        //保护区名称
        String_to_GBK(proZone_name,16,globalData->proZoneDataList.at(proZone_index -1).proZoneName);
    //    proZone_name_byte = globalData->proZoneDataList.at(proZone_index -1).proZoneName.toAscii();
    //    for (quint8 i = 0;i<16;i++)
    //    {
    //        proZone_name[i] = proZone_name_byte.data()[i];
    //    }
        //保护区id
        proZone_id[0] = globalData->proZoneDataList.at(proZone_index -1).proZoneID;
        proZone_build_type[0] = (globalData->proZoneDataList.at(proZone_index -1).proBuildType);//建筑类型

        Double_To_Array(proZone_height, 10*globalData->proZoneDataList.at(proZone_index -1).proZoneHeight);//保护区高度
        //将以上各字段拼接为数组
        addr = globalData->split_joint_hex_data(pro_info_buf , addr ,  info_type  , 1);//信息类别
        addr = globalData->split_joint_hex_data(pro_info_buf , addr ,  proZone_version  , 2);//保护区信息 版本
        addr = globalData->split_joint_hex_data(pro_info_buf , addr ,  proZone_num  , 1);//保护区个数
        addr = globalData->split_joint_hex_data(pro_info_buf , addr ,  proZone_typ_ord_elenum  , 1);//3合1
        addr = globalData->split_joint_hex_data(pro_info_buf , addr , proZone_name , 16);
        addr = globalData->split_joint_hex_data(pro_info_buf , addr , proZone_id , 1);
        addr = globalData->split_joint_hex_data(pro_info_buf , addr , proZone_build_type , 1);
        addr = globalData->split_joint_hex_data(pro_info_buf , addr , proZone_height , 2);
        //以上25字节，接下来拼接元素信息
       // qDebug()<<"**********************proZone_elenum_int"<<proZone_elenum_int;
        if (proZone_elenum_int > 10)
        {
            qDebug()<<"Something wrong with your element number, larger than 10!Check your database!";
            return -1;
        }
        for(quint8 i =0;i<proZone_elenum_int;i++)
        {
            proZone_ele_type[0] = globalData->proZoneDataList.at(proZone_index -1).elementData.at(i).elementType;
            //判断为点类型还是圆弧类型，并作各自的拼接
            if( proZone_ele_type[0] == 0x00)//点类型
            {
                Double_To_Array(proZone_ele_locX ,10*globalData->proZoneDataList.at(proZone_index -1).elementData.at(i).pointX);
                Double_To_Array(proZone_ele_locY ,10*globalData->proZoneDataList.at(proZone_index -1).elementData.at(i).pointY);
                addr = globalData->split_joint_hex_data(pro_info_buf , addr , proZone_ele_type , 1);
                addr = globalData->split_joint_hex_data(pro_info_buf , addr , proZone_ele_locX , 2);
                addr = globalData->split_joint_hex_data(pro_info_buf , addr , proZone_ele_locY, 2);
            }
            else if(proZone_ele_type[0] == 0x01)//圆弧类型
            {
                Double_To_Array(proZone_ele_locX, 10*globalData->proZoneDataList.at(proZone_index -1).elementData.at(i).pointX);
                Double_To_Array(proZone_ele_locY,10*globalData->proZoneDataList.at(proZone_index -1).elementData.at(i).pointY);
                Double_To_Array(proZone_ele_rad,10*globalData->proZoneDataList.at(proZone_index -1).elementData.at(i).radius);
                Double_To_Array(proZone_ele_start_ang,10*globalData->proZoneDataList.at(proZone_index -1).elementData.at(i).startAngle);
                Double_To_Array(proZone_ele_end_ang,10*globalData->proZoneDataList.at(proZone_index -1).elementData.at(i).endAngle);
                addr = globalData->split_joint_hex_data(pro_info_buf , addr , proZone_ele_type , 1);
                addr = globalData->split_joint_hex_data(pro_info_buf , addr , proZone_ele_locX , 2);
                addr = globalData->split_joint_hex_data(pro_info_buf , addr , proZone_ele_locY , 2);
                addr = globalData->split_joint_hex_data(pro_info_buf , addr , proZone_ele_rad , 2);
                addr = globalData->split_joint_hex_data(pro_info_buf , addr , proZone_ele_start_ang , 2);
                addr = globalData->split_joint_hex_data(pro_info_buf , addr , proZone_ele_end_ang , 2);
            }
        }
        len = addr;
        }
      // qDebug()<<"the data_len of proZone "<<proZone_index<<"is "<<len;
        return len;
    }
     /********限位模板函数******/
public:
     template <typename T>
     /***************************************/
     /***********限位信息拼接**********/
     /***************************************/
     quint32 EncLimit(T *limit_info_buf)
     {
         quint32 addr = 0;
         quint32 len=0;
         //quint8 limit_info_buf[200];//限位信息拼接数组
         T info_type[1] = {0x03};//信息类别
         T limit_info_type[2];//限位信息版本
         T left_limit[2];//左限位(-3276.8-3276.8)
         T right_limit[2];//右限位(-3276.8-3276.8)
         T far_limit[1];//远限位(0-25.5)
         T near_limit[1];//近限位(0-25.5)
         T height_limit[1];//高限位(0-25.5)
         T weight_limit[2];//吊重限位(0-655.35)
         T torque_limit[2];//力矩限位值(0-655.35)
         T sensor_enable[1];//传感器使能标志

         //将限位信息拼接数组清空
         memset(limit_info_buf , 0 , 100);

     /******将数据库中的各个类型的数据转换为字节数组，以便拼接********/
        // qDebug()<<"limitVersion = "<<globalData->limitData.limitVersion;
         Double_To_Array(limit_info_type, globalData->limitData.limitVersion);
         //qDebug()<<"leftLimit = "<<globalData->limitData.leftLimit;
         Double_To_Array(left_limit, 10*globalData->limitData.leftLimit);
         Double_To_Array(right_limit, 10*globalData->limitData.rightLimit);
         far_limit[0] = 10*globalData->limitData.farLimit;
         near_limit[0]=10*globalData->limitData.nearLmit;
         height_limit[0]=10*globalData->limitData.highLimit;
         Double_To_Array(weight_limit,100*globalData->limitData.weightLimit);
         Double_To_Array(torque_limit,100*globalData->limitData.momentLimit);
         sensor_enable[0]=globalData->limitData.enableBit;

         //将以上各字段拼接为数组
         addr = globalData->split_joint_hex_data(limit_info_buf , addr ,  info_type  , 1);
         addr = globalData->split_joint_hex_data(limit_info_buf , addr ,  limit_info_type  , 2);
         addr = globalData->split_joint_hex_data(limit_info_buf , addr ,  left_limit  , 2);
         addr = globalData->split_joint_hex_data(limit_info_buf , addr ,  right_limit  , 2);
         addr = globalData->split_joint_hex_data(limit_info_buf , addr ,  far_limit  , 1);
         addr = globalData->split_joint_hex_data(limit_info_buf , addr ,  near_limit  , 1);
         addr = globalData->split_joint_hex_data(limit_info_buf , addr ,  height_limit  , 1);
         addr = globalData->split_joint_hex_data(limit_info_buf , addr ,  weight_limit  , 2);
         addr = globalData->split_joint_hex_data(limit_info_buf , addr ,  torque_limit  , 2);
         addr = globalData->split_joint_hex_data(limit_info_buf , addr ,  sensor_enable  , 1);

         len=addr;
         return len;
     }
public:
    /***************************************/
    /****************KD值及端口信息拼接***************/
    /***************************************/
    template <typename T>
    quint32 EncKD_ip(T *KDip_info_buf)
    {
        qDebug()<<"kDKDKDKDKDK";
        quint32 addr = 0;
        quint32 len=0;
    //    quint8 Temp_KD_Mes[100];//KD值及端口信息拼接数组
    //    quint8 temp_data[150];//加协议版本 厂商代码 帧类型 信息段长 设备代码
       // quint8 info_type[1] = {0x03};//信息类别
        T info_type[1] = {0x04};
        T KD_info_version[2];//KD值及端口信息版本,2字节
        T weight_k[2];//吊重k值,2字节
        T weight_d[2];//吊重d值，2字节
        T hor_k[2];//水平k值，2字节
        T hor_d[2];//水平d值，2字节
        T vert_k[2];//垂直k值，2字节
        T vert_d[2];//垂直d值，2字节
        T server_ip[15];//服务器ip，15字节
        QByteArray server_ip_byte;
        T server_port[5];//服务器端口，5字节
        QByteArray server_port_byte;
        T com_channel[1];//信道id，1字节
        QByteArray com_channel_byte;
        T net_id[2];//网络id，2字节
        QByteArray net_id_byte;
        T pitch_height[1];//塔吊节高，1字节
        T init_height[1];  //塔吊初始高度，1字节
        T pitch_num[1];//塔吊节数，1字节
        T pro_high_e_w[1];//保护区高度预警，1字节
        T pro_high_w[1];//保护区高度报警，1字节

        //将拼接数组清空
        memset(KDip_info_buf , 0 , 100);
    //    memset(temp_data , 0 , 150);
    /******将数据库中的各个类型的数据转换为字节数组，以便拼接********/
        Double_To_Array(KD_info_version,globalData->craneKD.craneKDVersion);
        Double_To_Array(weight_k,10000*globalData->craneKD.weightK);
        Double_To_Array(weight_d,globalData->craneKD.weightD);
        Double_To_Array(hor_k,10000*globalData->craneKD.spanK);
        Double_To_Array(hor_d,100*globalData->craneKD.spanD);
        Double_To_Array(vert_k,10000*globalData->craneKD.heightK);
        Double_To_Array(vert_d,10*globalData->craneKD.heightD);

        //服务器IP,15字节
        String_to_GBK(server_ip,15,globalData->craneKD.serverIP);
    //    server_ip_byte = globalData->craneKD.serverIP.toAscii();
    //    for (quint8 i = 0;i<15;i++)
    //    {
    //        server_ip[i] = server_ip_byte.data()[i];
    //    }

        //服务器端口，5字节
        String_to_GBK(server_port,5,globalData->craneKD.serverPort);
    //    server_port_byte = globalData->craneKD.serverPort.toAscii();
    //    for (quint8 i = 0;i<5;i++)
    //    {
    //        server_port[i] = server_port_byte.data()[i];
    //    }

        //信道ID,1字节
        String_to_GBK(com_channel,1,globalData->craneKD.ZigbeeChannel);
    //    com_channel_byte= globalData->craneKD.ZigbeeChannel.toAscii();
    //    for (quint8 i = 0;i<1;i++)
    //    {
    //        com_channel[i] = com_channel_byte.data()[i];
    //    }

        //网络ID，2字节
        String_to_GBK(net_id,2,globalData->craneKD.ZigbeeID);
    //    net_id_byte= globalData->craneKD.ZigbeeID.toAscii();
    //    for (quint8 i = 0;i<2;i++)
    //    {
    //        net_id[i] = net_id_byte.data()[i];
    //    }

        pitch_height[0] = globalData->craneKD.pitchHeight;
        init_height[0] = globalData->craneKD.initHeight;
        pitch_num[0] = globalData->craneKD.pitchNum;
        pro_high_e_w[0] = globalData->craneKD.proHeightEW;
        pro_high_w[0] = globalData->craneKD.proHeightW;

        //将以上各字段拼接为数组
        addr = globalData->split_joint_hex_data(KDip_info_buf , addr ,  info_type  , 1);//信息类别
        addr = globalData->split_joint_hex_data(KDip_info_buf , addr ,  KD_info_version  , 2);//KD值及端口信息版本,2字节
        addr = globalData->split_joint_hex_data(KDip_info_buf , addr ,  weight_k  , 2);//吊重k值,2字节
        addr = globalData->split_joint_hex_data(KDip_info_buf , addr ,  weight_d  , 2);//吊重d值，2字节
        addr = globalData->split_joint_hex_data(KDip_info_buf , addr ,  hor_k  , 2);//水平k值，2字节
        addr = globalData->split_joint_hex_data(KDip_info_buf , addr ,  hor_d  , 2);//水平d值，2字节
        addr = globalData->split_joint_hex_data(KDip_info_buf , addr ,  vert_k  , 2);//垂直k值，2字节
        addr = globalData->split_joint_hex_data(KDip_info_buf , addr ,  vert_d  , 2);//垂直d值，2字节
        addr = globalData->split_joint_hex_data(KDip_info_buf , addr ,  server_ip  , 15);//服务器ip，15字节
        addr = globalData->split_joint_hex_data(KDip_info_buf , addr ,  server_port  , 5);//服务器端口，5字节
        addr = globalData->split_joint_hex_data(KDip_info_buf , addr ,  com_channel  , 1);//信道id，1字节
        addr = globalData->split_joint_hex_data(KDip_info_buf , addr ,  net_id  , 2);//网络id，2字节
        addr = globalData->split_joint_hex_data(KDip_info_buf , addr ,  pitch_height  , 1);//塔吊节高，1字节
        addr = globalData->split_joint_hex_data(KDip_info_buf , addr ,  init_height  , 1);//塔吊初始高度，1字节
        addr = globalData->split_joint_hex_data(KDip_info_buf , addr ,  pitch_num  , 1);//塔吊节数，1字节
        addr = globalData->split_joint_hex_data(KDip_info_buf , addr ,  pro_high_e_w  , 1);//保护区高度预警，1字节
        addr = globalData->split_joint_hex_data(KDip_info_buf , addr ,  pro_high_w  , 1);//保护区高度报警，1字节

//        for(quint32 i=0; i < addr; i++)
//        {
//            qDebug()<<i<<":"<<hex<<KDip_info_buf[i];
//        }
        len= addr;
//        qDebug()<<"kd data length = "<<len;
        return len;//信息段长度
    }
public:
    /*
      ********************************************************
        函数：EncCraneGroup（quint32）
        描述：拼接塔群配置信息
        参数：
        日期：2013.12
      ********************************************************
      */
    template <typename T>
    quint32 EncCraneGroup(T *group_info_buf,quint32 Crane_index)
    {
        quint32 addr = 0;
        quint32 len=0;
        T info_type[1] = {0x05};//信息类别，
        T group_info_version[2];//塔群信息版本
        T crane_index[1] = {Crane_index};
        T groupID[1];
        T relatedCraneNum[1];//相干塔吊个数
        T relatedCraneID[1];//相干塔吊ID
        QString craneIMEI;
        T str_imei[16];//imei号转换的字节数组
        T coordinateX[2];//X
        T coordinateY[2];//Y
        T forearmLen[2];//前臂长
        T backarmLen[2];//后臂长
        T craneHeight[2];//塔臂高
        T craneTopHeight[2];//塔帽高

        //将拼接数组清空
        memset(group_info_buf , 0 , 100);
        qDebug()<<"Crane_index = "<<Crane_index;
        qDebug()<<"Crane list count"<<globalData->craneList.count();
        if(Crane_index < globalData->craneList.count()){
    /******将数据库中的各个类型的数据转换为字节数组，以便拼接********/
        Double_To_Array(group_info_version,globalData->basicData.basicVersion);//塔群信息版本即基本信息版本
        groupID[0] = globalData->craneList.at(Crane_index).groupID;
        relatedCraneNum[0] = globalData->craneList.at(Crane_index).relatedCraneNum;
        relatedCraneID[0] = globalData->craneList.at(Crane_index).craneID;
        craneIMEI = globalData->craneList.at(Crane_index).craneMAC;
        if (craneIMEI.size() < 15)
        {
            qDebug()<<"Something wrong with your IMEI length of the CraneGroup_"<<Crane_index;
            return -1;
        }
        imei_to_byteArray(craneIMEI,str_imei);//imei号转换为16字节数组
        Double_To_Array(coordinateX,globalData->craneList.at(Crane_index).coordinateX * 10);
        Double_To_Array(coordinateY,globalData->craneList.at(Crane_index).coordinateY * 10);
        Double_To_Array(forearmLen,globalData->craneList.at(Crane_index).forearmLen * 10);
        Double_To_Array(backarmLen,globalData->craneList.at(Crane_index).backarmLen * 10);
        Double_To_Array(craneHeight,globalData->craneList.at(Crane_index).craneHeight * 10);
        Double_To_Array(craneTopHeight,globalData->craneList.at(Crane_index).craneTopHeight * 10);



        addr = globalData->split_joint_hex_data(group_info_buf , addr ,  info_type  , 1);//信息类别
        addr = globalData->split_joint_hex_data(group_info_buf , addr ,  group_info_version  , 2);
        addr = globalData->split_joint_hex_data(group_info_buf , addr ,  crane_index  , 1);//塔吊index
        addr = globalData->split_joint_hex_data(group_info_buf , addr ,  groupID  , 1);
        addr = globalData->split_joint_hex_data(group_info_buf , addr ,  relatedCraneNum  , 1);
        addr = globalData->split_joint_hex_data(group_info_buf , addr ,  relatedCraneID  , 1);
        addr = globalData->split_joint_hex_data(group_info_buf , addr ,  str_imei  , 16);
        addr = globalData->split_joint_hex_data(group_info_buf , addr ,  coordinateX  , 2);
        addr = globalData->split_joint_hex_data(group_info_buf , addr ,  coordinateY  , 2);
        addr = globalData->split_joint_hex_data(group_info_buf , addr ,  forearmLen  , 2);
        addr = globalData->split_joint_hex_data(group_info_buf , addr ,  backarmLen  , 2);
        addr = globalData->split_joint_hex_data(group_info_buf , addr ,  craneHeight  , 2);
        addr = globalData->split_joint_hex_data(group_info_buf , addr ,  craneTopHeight  , 2);

        len = addr;
        }
        else{
            addr = globalData->split_joint_hex_data(group_info_buf , addr , info_type , 1);
            addr = globalData->split_joint_hex_data(group_info_buf , addr ,  group_info_version  , 2);
            addr = globalData->split_joint_hex_data(group_info_buf , addr ,  crane_index  , 1);//塔吊index
            for(int i = addr ; i <35;i++){
                group_info_buf[i] = 0;
            }
            addr = 35;
            len = addr;
        }
        return len;
    }
public:
    /*****回复帧拼接模板***********/
    template <typename T>
    int EncRequest(quint8 info_type,quint32 info_version,quint32 info_index , T *info_send){
        T info_buf[250];//信息，只包括信息字段
        quint32 data_len;//信息字段的长度
        quint32 m =0;
        QString imei;
        info_buf[m] = info_type;
        m = m+1;
        T info_version_hex[2];
        Double_To_Array(info_version_hex,info_version);
        info_buf[m] = info_version_hex[0];
        m = m+1;
        info_buf[m] = info_version_hex[1];
        m = m+1;
        switch(info_type)
        {
    /****************配置基本信息****************/
        case 0x01:
            qDebug()<<"************************SLOT:send respond basic information to database!";

            break;
    /****************配置保护区信息****************/
        case 0x02:
            qDebug()<<"************************SLOT:send respond proZone information to database!";
            info_buf[m] = info_index;//加上保护区序号
            m = m+1;
            break;
    /****************配置限位信息****************/
        case 0x03:
            qDebug()<<"************************SLOT:send respond Limit information to database!";
            break;
    /****************配置KD值和ip信息****************/
        case 0x04:
            qDebug()<<"************************SLOT:send respond KD_ip information to database!";
            break;
    /****************配置塔群信息****************/
        case 0x05:
            qDebug()<<"************************SLOT:send respond CraneGroup information to database!";
            info_buf[m] = info_index;//加上保护区序号
            m = m+1;
            break;
    /****************配置力矩曲线信息****************/
        case 0x06:
            qDebug()<<"************************SLOT:send respond torque curse information to database!";
            info_buf[m] = info_index;//加上力矩曲线倍数
            qDebug()<<"info_index="<<hex<<info_index;
            m = m+1;
            break;
        default:
            break;
        }
        data_len = m;
        data_len = add_frame_CRC_FE(0x04,0x08,
                                                  imei,info_buf,data_len,info_send,0x05);
        return data_len;
    }
public:
    quint32 EncStatusData(char *Temp_Status_Data, char *center_receive_time);
    quint32 EncBasic(quint8* basic_info_buf);//封装基本信息
    quint32 EncbasicData(char *Temp_Basic_Mes , bool Enc_Meathod);//GPRS模块调用的基本信息封装
    quint32 EncAlarmData(char *alarm_message, char alarm_type, char *detail, char *center_time);//报警封装
    quint32 EncIllegalData(char *illegal_message, char illegal_type, char *detail, char *center_time);//违章封装
    quint32 EncFaultData(char *fault_message, char fault_type, char *detail, char *center_time);//故障封装
    int EncCraneInter(quint8* Group_info_buf);//塔机间互发
    quint32 EncReport();//封装事件上报
    /****封装ID数据，入口参数为IMEI号和要存放的数组***/
    int EncID(char *imei_num , quint8 *frame);
    /****封装指纹ID数据，入口参数为要存放的数组, 发送时间,上位机分配的ID,事件类型***/
    int EncFingerID(char *frame, char *center_time , char *center_receive_ID , char *imei_num,quint8 event);
/*********数据解析，将接收到的16进制字节数组解析为协议的各字段*******/
    bool SplCroupInter(quint8 *recv_buff);//解析塔群相干
    bool SplBasic(quint8 *recv_buff);//解析基本信息
    bool SplLimit(quint8 * recv_buff);//解析限位信息
    bool SplKD_ip(quint8 * recv_buff);//解析KD值和端口信息
    bool SplProZone(quint8 *recv_buff,quint8 proZone_index);//解析保护区信息
    bool SplCraneGroup(quint8 *recv_buff,quint8 group_index);


/****Qstring类型（unicode编码）转换为GBK编码模板********/
public:
    template <typename T>
    void String_to_GBK(T *temp, quint32 len, QString str_from_global)
    {
        QByteArray qbyte_temp;
        memset(temp , 0 , len);
        QTextCodec *code = QTextCodec::codecForName("GBK");
        qbyte_temp = code->fromUnicode(str_from_global);
        for (quint8 i = 0;i<qbyte_temp.length();i++)
        {
            temp[i] = qbyte_temp.data()[i];
           //qDebug()<<i<<hex<<temp[i];
        }
    }
public:
    //void Double_To_Array(quint8 *p,double value);
    double Hex_to_Double(quint8 *temp);
    //将源数组sourceArray从下标为sourceIndex起，长度为len的数组段复制到目标数组destinationArray从下标为destinationIndex的地方
    void Array_copy(quint8 *sourceArray, quint32 sourceIndex, quint8 *destinationArray,quint32 destinationIndex,quint32 len);
    QString Array_to_String(quint8 *temp,quint32 len);//将字节数组temp[]转化为字符串
    QString Array_to_String01(quint8 *temp,quint32 len);
    void String_to_GBK(quint8 *temp, quint32 len, QString str_from_global);//将全局变量中的Qstring类型（unicode编码）转换为GBK编码

    int delete_fe(char *receive_data,quint32 len);//去转义
public:
    template <typename T>
    /***************************************/
    /*******加转义,**********************/
    /*参数：info[]包括CRC校验，包括帧头，所以在for循环的循环变量初始值为2，要排除掉帧头
                 send_data[]是在info[]的基础上加来转义，加帧尾的数组，也就是待发送的数据数组
                 len为不包括帧头，包括【协议版本/厂商代码】，【帧类型/信息段长】，【设备代码】
                     【信息段】，【CRC】的长度
       返回：len+重复的0xFE个数
    */
    /***************************************/
    quint32 add_fe(T *info, T *send_data, quint32 len)
    {
        T frame_header[2] = {0xfe , 0xfb};
        T frame_end[2] = {0xfe , 0xfa};
        int repeat_num = 0;
        quint32 addr = 0;
        //int addr=0;
        for(quint32 i = 2; i<len;i++)
            {
                if(info[i] == 0xFE)//如果遇到0xFE则在其后再增加一个0xFE
                {
                    repeat_num++;
                    send_data[i+ repeat_num - 1] = info[i];
                    send_data[i+ repeat_num] = 0xFE;
                }
                else
                {
                    send_data[i+ repeat_num] = info[i];
                }
            }
        globalData->split_joint_hex_data(send_data , addr ,  frame_header  , 2);//加帧头
        //Array_copy(frame_header,0,send_data,0,2);//加帧头
        addr =globalData->split_joint_hex_data(send_data , len+repeat_num ,  frame_end  , 2);//加帧尾
        //Array_copy(frame_end,0,send_data,len+2+repeat_num,2);//加帧尾
        /***测试：打印加0xFE的个数****/
    //    qDebug()<<"**add_fe()***repeat_num="<<repeat_num;
    //    qDebug()<<"len+repeat_num="<<len+repeat_num;
        return addr;//返回len+重复的0xFE个数
    }
    //最后的发送格式，向上位机发送各信息，加帧头帧尾，校验
public:

    template <typename T>
    /********************************************************/
    /*********最后的发送格式，向上位机发送各信息，加帧头帧尾，校验**********************/
    /*参数：protocol_version为协议版本
                 manufactory_id厂商代码
                 imei为imei号，string类型
                 frame_type帧类型
                 data_len信息段长度
                 info[]为以Enc开头的将各字段打包为16进制字节数组的函数拼接的数组，即信息段数组
                 send_data[]为最后拼接完成，发送出去的数组
        返回：send_data[]的长度
    */
    /********************************************************/
    quint32 add_frame_CRC_FE(char protocol_version, char manufactory_id, QString imei, T *info,
                         quint32 data_len, T *send_data, char frame_type)
    {

        T frame_header[2] = {0xfe , 0xfb};
        T frame_end[2] = {0xfe , 0xfa};
        quint32 addr = 0;
        bool imei_flag = false;
        T return_info[250]={0};

        addr = globalData->split_joint_hex_data(return_info , addr ,  frame_header  , 2);//拼接帧头
        return_info[2]= ((protocol_version<<4)+ manufactory_id) & 0xFF;//将协议版本和厂商代码合成一个字节
        return_info[3]=(  (frame_type<<3)+data_len/256  ) & 0xFF;
        return_info[4]= data_len & 0xFF;
        addr = addr +3;
        T str_imei[16];
        imei = globalData->basicData.craneIMEI;//从全局变量获得塔吊的IMEI号

        imei_flag=imei_to_byteArray(imei,str_imei);//将imei号转换为16字节数组
        addr = globalData->split_joint_hex_data(return_info , addr ,  str_imei  , 16);//将imei字节数组拼接
        addr = globalData->split_joint_hex_data(return_info , addr ,  info  , data_len);//将信息段数组拼接
        /****CRC校验，不包括帧头，所以return_info要加2，CRC的低字节在前，高字节在后****/
        quint8 CRC[2];
        globalData->dataCRC16((char*)return_info, 21+data_len, CRC);//CRC校验要包括帧头，不包括帧尾

        addr = globalData->split_joint_hex_data(return_info ,addr ,(T *)CRC ,2);//将CRC数组拼接
        addr = globalData->split_joint_hex_data(return_info ,addr ,frame_end  ,2);//将帧尾拼接

        quint32 return_len;

        return_len=add_fe(return_info,send_data,2+19+data_len+2);

        return return_len;//最后拼接完成，发送出去的数组的长度
    }

public:
    /********************************************************/
    /*********imei号码转换为十六进制**********************/
    /*参数：crane_imei字符串形式的imei号
                str_imei字节数组形式的imei
    */
    /********************************************************/
    template <typename T>
    bool imei_to_byteArray(QString crane_imei, T *str_imei)
    {
        QByteArray imei_Qbyte;
        quint32 len_imei ;
        imei_Qbyte = crane_imei.toAscii();
        len_imei= crane_imei.size();
        //qDebug()<<"len_imei"<<len_imei;
        if(crane_imei.length() != 15)
        {
            qDebug()<<"Something is wrong with the length of imei number!";
            return false;
        }

        for (quint32 i = 0;i<15;i++)
        {
            str_imei[i] = imei_Qbyte.data()[i];
            //qDebug()<<"str_imei["<<i<<"]"<<hex<<str_imei[i];//测试打印本机imei字段
        }
        str_imei[15] = 0x00;// 最后一位补0
        return true;

    }
};



#endif // ENCSPL_H
