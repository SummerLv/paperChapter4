#include "encspl.h"

static quint8 frame_header[2] = {0xFE, 0xFB};
static quint8 frame_end[2] = {0xFE, 0xFA};

EncSpl::EncSpl()
{
    globalData = GlobalData::getGlobalData();
}
///*************double数据转换成2字节数组数据*********/
//void EncSpl::Double_To_Array(quint8 *p, double value)//入口参数为整型
//{
//    qint32 TempValue;
//    TempValue = (int)value;
//    *p =((TempValue >> 8) & 0xFF);
//     p++;
//    *p = (TempValue & 0xFF);
//}
/****************十六进制转double型*******************/
double EncSpl::Hex_to_Double(quint8 *temp)
{
    return (qint16)((temp[0] << 8) + temp[1]);
}
/**将源数组sourceArray从下标为sourceIndex起，
      将长度为len的数组段复制到目标数组destinationArray从下标为destinationIndex的地方**/
void EncSpl::Array_copy(quint8 *sourceArray, quint32 sourceIndex,
                      quint8 *destinationArray,quint32 destinationIndex,quint32 len)
{

    for( quint32 i=destinationIndex;i<len;i++)
    {
        destinationArray[i] = sourceArray[sourceIndex+i];
    }
}
/**将字节数组temp[]转化为字符串*****/
/****参数len为temp数组的长度*/
QString EncSpl::Array_to_String(quint8 *temp,quint32 len)
{
    QString temp_str;
    char temp_char[len+1];//char数组
    bool ok;
    for(quint32 i=0;i<len;i++)
    {
        temp_str = QString::number(temp[i],16);
        temp_char[i] = (char)(temp_str.toShort(&ok,16));
    }
    temp_char[len] = 0x00;
    QTextCodec *code = QTextCodec::codecForName("GBK");
    QString str_temp = code->toUnicode(temp_char);
    //code->fromUnicode()
    qDebug()<<str_temp;
    return str_temp;
//    QString string_temp;
//    for (quint32 i=0;i<len;i++)
//    {
//        string_temp += QString::number(temp[i],10);//将len字节的数组转换为字符串
//    }
//    return string_temp;
}
QString EncSpl::Array_to_String01(quint8 *temp,quint32 len)
{
        QString string_temp;
        for (quint32 i=0;i<len;i++)
        {
            string_temp += QString::number(temp[i],16);//将len字节的数组转换为字符串
        }
        return string_temp;
//    QString temp_str;
//    char temp_char[len];
//    bool ok;
//    for(quint32 i=0;i<len;i++)
//    {
//        temp_str = QString::number(temp[i],16);
//        temp_char[i] = (char)(temp_str.toShort(&ok,16));
//    }
//    QTextCodec *code = QTextCodec::codecForName("GBK");
//    QString str_temp = code->toUnicode(temp_char);
//    return str_temp;

//    quint8 uestc[15] = {0xb5,0xe7,0xd7,0xd3,0xbf,0xc6,0xb4,0xf3,0x30,0x31,0x69,0x6b,0x00,0x00,0x00};

//    QString uestc_str;
//    char uestc_char[15];
//    bool ok;
//    for (quint32 i=0;i<15;i++)
//    {
//        uestc_str = QString::number(uestc[i],16);
//         qDebug()<<"===================="<<i<<uestc_str;
//        uestc_char[i] = (char)(uestc_str.toShort(&ok,16));
//    }

//    QTextCodec * code = QTextCodec::codecForName("GBK");
//    QString string_uestc = code->toUnicode(uestc_char);
//    qDebug()<<"string_uestc="<<string_uestc;
//    return string_uestc;
}
void EncSpl::String_to_GBK(quint8 *temp, quint32 len, QString str_from_global)
{
    QByteArray qbyte_temp;
    QTextCodec *code = QTextCodec::codecForName("GBK");
    qbyte_temp = code->fromUnicode(str_from_global);
    for (quint8 i = 0;i<len;i++)
    {
        temp[i] = qbyte_temp.data()[i];
        qDebug()<<i<<hex<<temp[i];
    }
}

/***************************************/
/***********基本信息拼接**********/
/***************************************/
quint32 EncSpl::EncBasic(quint8* basic_info_buf)
{
    qDebug()<<"***Enter EncBasic()";
    quint32 addr = 0;
    //quint8 basic_info_buf[32];
    //quint8 temp_data[70];
    quint8 info_type[1] ={ 0x01 };                              //信息类别，1字节
    quint8 crane_type_rate[1];                                    //塔机类型和倍率
    quint8 basic_version[2];                                       //信息版本，1字节
    quint8 crane_name[16];                                       //塔机名称，16字节
    QByteArray crane_name_byte;
   // const char* crane_name;
    quint8 crane_ID[1];                                              //塔机ID
    quint8 group_ID[1];                                              //塔群ID
    quint8 coordinateX[2];                                          //坐标X
    quint8 coordinateY[2];                                          //坐标Y
 //   quint32 int_coordinateY;
    quint8 forearmlen[2];                                           //前臂长度
   // quint32 int_forearmlen;
    quint8 backarmlen[2];                                          //后臂长度
   // quint32 int_backarmlen;
    quint8 crane_height[2];                                        //塔臂高度
  //  quint32 int_crane_height;
    quint8 crane_top_height[1];                                  //塔帽高度
  //  quint32 int_crane_top_height;
    qint32 len;
    //qDebug()<<"basicData.craneType";
    crane_type_rate[0] = (globalData->basicData.craneType << 4) + (globalData->basicData.craneRate);//拼接帧类型和倍率
    //qDebug()<<"crane_type_rate="<<crane_type_rate[0];
    //qDebug()<<"globalData->basicData.basicVersion="<<globalData->basicData.basicVersion;
    Double_To_Array(basic_version , globalData->basicData.basicVersion);


    QString TempBasic;//用来将cranename转换为unicode
    QByteArray TempArray;
    TempArray =globalData->basicData.craneName.toLocal8Bit();
    TempBasic = globalData->tc->toUnicode(TempArray);
    //TempBasic = globalData->basicData.craneName.toUtf8();
   String_to_GBK(crane_name , 16 , TempBasic);
//    QTextCodec *code = QTextCodec::codecForName("GBK");
//    crane_name_byte = code->fromUnicode(globalData->basicData.craneName);
//    for (quint8 i = 0;i<16;i++)
//    {
//        crane_name[i] = TempArray.data()[i];
//        qDebug()<<"crane_name"<<hex<<crane_name[i];
//    }

//    qDebug()<<"*****test crane_name[]******";
//        for(quint32 i = 0;i < 16; i++)
//        {
//            qDebug()<<hex<<crane_name[i];
//        }
    crane_ID[0] = globalData->basicData.craneID;
    group_ID[0] = globalData->basicData.groupID;

    Double_To_Array(coordinateX ,10*globalData->basicData.coordinateX);

    Double_To_Array(coordinateY ,10*globalData->basicData.coordinateY);

    Double_To_Array(forearmlen, 10*globalData->basicData.forearmLen);

    Double_To_Array(backarmlen,10*globalData->basicData.backarmLen);

    Double_To_Array(crane_height,10*globalData->basicData.craneHeight);

    Double_To_Array(crane_top_height,10*globalData->basicData.craneHeight);

    memset (basic_info_buf , 0x00 , 250);


    addr = globalData->split_joint_hex_data(basic_info_buf , addr ,  info_type  , 1);//信息类别

    addr = globalData->split_joint_hex_data(basic_info_buf , addr ,  basic_version  , 2);//

    addr = globalData->split_joint_hex_data(basic_info_buf , addr , crane_name , 16);
    addr = globalData->split_joint_hex_data(basic_info_buf , addr , crane_ID , 1);
    addr = globalData->split_joint_hex_data(basic_info_buf , addr , group_ID , 1);
    addr = globalData->split_joint_hex_data(basic_info_buf , addr , crane_type_rate , 1);
    addr = globalData->split_joint_hex_data(basic_info_buf , addr , coordinateX , 2);
    addr = globalData->split_joint_hex_data(basic_info_buf , addr , coordinateY, 2);
    addr = globalData->split_joint_hex_data(basic_info_buf , addr , forearmlen , 2);
    addr = globalData->split_joint_hex_data(basic_info_buf , addr , backarmlen, 2);
    addr = globalData->split_joint_hex_data(basic_info_buf , addr , crane_height, 2);
    addr = globalData->split_joint_hex_data(basic_info_buf , addr , crane_top_height, 1);
    len = addr;
    return  len;
// addr = globalData->create_data_for_center(0x04 , (char*)center_receive_ID ,temp_data , basic_info_buf , len ,0);
//    len = addr;
}

/****基本信息封装*****/
quint32 EncSpl::EncbasicData(char *Temp_Basic_Mes, bool Enc_Meathod){
    char data_style[1] ={ 0x01 };
    char crane_type_rate[1];
    char basic_version[2];
    char crane_name[16];
    QByteArray crane_name_byte;

    quint32 addr = 0;
   // const char* crane_name;
    char crane_ID[1];
    char group_ID[1];
    char coordinateX[2];
   //
    char coordinateY[2];
 //   quint32 int_coordinateY;
    char forearmlen[2];
   // quint32 int_forearmlen;
    char backarmlen[2];
   // quint32 int_backarmlen;
    char crane_height[2];
  //  quint32 int_crane_height;
    char crane_top_height[1];


    crane_type_rate[0] = (globalData->basicData.craneType << 4)|(globalData->basicData.craneRate);//拼接帧类型和倍率

    Double_To_Array(basic_version , globalData->basicData.basicVersion);

    if(Enc_Meathod == false){//拼接发送给中心平台的name
    crane_name_byte = globalData->basicData.craneName.toAscii();
    for (quint8 i = 0;i<globalData->basicData.craneName.size();i++)
    {
        crane_name[i] = crane_name_byte.data()[i];
      //  qDebug()<<crane_name[i];
    }
    }
    else{//拼接发送给上位机的name
        qDebug()<<"BASICBASICBASIC";
        QString TempBasic;//用来将cranename转换为unicode
        QByteArray TempArray;
        TempArray =globalData->basicData.craneName.toLocal8Bit();
        TempBasic = globalData->tc->toUnicode(TempArray);
        //TempBasic = globalData->basicData.craneName.toUtf8();
        String_to_GBK(crane_name , 16 , TempBasic);
    }

    crane_ID[0] = globalData->basicData.craneID;
    group_ID[0] = globalData->basicData.groupID;

    Double_To_Array(coordinateX ,10*globalData->basicData.coordinateX);
//    int_coordinateX = 10*globalData->basicData.coordinateX;
//    coordinateX[0] = int_coordinateX && 0xFF;
//    coordinateX[1] = (int_coordinateX >> 8) && 0xFF;
    Double_To_Array(coordinateY ,10*globalData->basicData.coordinateY);
//    int_coordinateY = 10*globalData->basicData.coordinateY;
//    coordinateY[0] = int_coordinateY && 0xFF;
//    coordinateY[1] = (int_coordinateY >> 8) && 0xFF;
    Double_To_Array(forearmlen, 10*globalData->basicData.forearmLen);
//    int_forearmlen = 10*globalData->basicData.forearmLen;
//    forearmlen[0] = int_forearmlen && 0xFF;
//    forearmlen[1] = (int_forearmlen >> 8) && 0xFF;
    Double_To_Array(backarmlen,10*globalData->basicData.backarmLen);
//    int_backarmlen = 10*globalData->basicData.backarmLen;
//    backarmlen[0] = int_backarmlen && 0xFF;
//    backarmlen[1] = (int_backarmlen >> 8) && 0xFF;
    Double_To_Array(crane_height,10*globalData->basicData.craneHeight);
//    int_crane_height = 10*globalData->basicData.craneHeight;
//    crane_height[0] = int_crane_height && 0xFF;
//    crane_height[1] = (int_crane_height >> 8) && 0xFF;
    Double_To_Array(crane_top_height,10*globalData->basicData.craneHeight);
//    int_crane_top_height = 10*globalData->basicData.craneHeight;
//    crane_top_height[0] = int_crane_top_height;
  //  qDebug()<<"************addr";

    addr = globalData->split_joint_hex_data(Temp_Basic_Mes , addr ,  data_style  , 1);

    addr = globalData->split_joint_hex_data(Temp_Basic_Mes , addr ,  basic_version  , 2);

    addr = globalData->split_joint_hex_data(Temp_Basic_Mes , addr , crane_name , 16);
    addr = globalData->split_joint_hex_data(Temp_Basic_Mes , addr , crane_ID , 1);
    addr = globalData->split_joint_hex_data(Temp_Basic_Mes , addr , group_ID , 1);
    addr = globalData->split_joint_hex_data(Temp_Basic_Mes , addr , crane_type_rate , 1);
    addr = globalData->split_joint_hex_data2(Temp_Basic_Mes , addr , coordinateX , 2);
    addr = globalData->split_joint_hex_data2(Temp_Basic_Mes , addr , coordinateY, 2);
    addr = globalData->split_joint_hex_data(Temp_Basic_Mes , addr , forearmlen , 2);
    addr = globalData->split_joint_hex_data(Temp_Basic_Mes , addr , backarmlen, 2);
    addr = globalData->split_joint_hex_data(Temp_Basic_Mes , addr , crane_height, 2);
    addr = globalData->split_joint_hex_data(Temp_Basic_Mes , addr , crane_top_height, 1);
    return addr;
}

///***************************************/
///***********保护区信息拼接**********/
///***************************************/
// quint32 EncSpl::EncProZone(T* pro_info_buf,quint32 proZone_index)
//{
//    quint32 addr = 0;
//    quint32 len = 0;

//    quint8 info_type[1] = {0x02};//信息类别
//    quint8 proZone_version[2];  //信息版本，2字节
//    quint8 proZone_num[1];//保护区个数
//    quint8 proZone_typ_ord_elenum[1];//保护区类型+序号+元素个数，拼接字节 , 1字节
//    quint8 proZone_type_int = 0;
//    quint8 proZone_elenum_int = 0;//元素个数
//    quint8 proZone_name[16];//保护区名称
//    QByteArray proZone_name_byte;
//    quint8 proZone_id[1];//保护区id
//    quint8 proZone_build_type[1];//建筑类别
//    quint8 proZone_height[2];//保护区高度
//    quint8 proZone_ele_type[1];//保护区元素类型
//    quint8 proZone_ele_locX[2];//保护区元素X坐标
//    quint8 proZone_ele_locY[2];//保护区元素Y坐标
//    quint8 proZone_ele_rad[2];//保护区元素圆半径
//    quint8 proZone_ele_start_ang[2];//保护区元素起点角度
//    quint8 proZone_ele_end_ang[2];//保护区元素终点角度

//    //将保护区拼接数组清空
//    memset(pro_info_buf , 0 , 200);

///******将数据库中的各个类型的数据转换为字节数组，以便拼接********/
//    proZone_num[0] = globalData->proZoneDataList.count();//保护区个数，只有这个变量是依赖QList的成员函数求得
//    qDebug()<<"proZone_num = "<<proZone_num[0];
//    if (proZone_index > proZone_num[0] )
//    {
//        qDebug()<<"proZoneDataList out of range!";
//    }
//    Double_To_Array(proZone_version, globalData->proZoneDataList.at(proZone_index - 1 ).proZoneVersion);//保护区信息版本

//    //保护区类型+序号+元素个数
//    qDebug()<<"proZoneOrder = "<<globalData->proZoneDataList.at(proZone_index -1 ).proZoneOrder;
//    proZone_typ_ord_elenum[0] = ((globalData->proZoneDataList.at(proZone_index -1).proZoneType << 7) & 0x80)
//                                                +((globalData->proZoneDataList.at(proZone_index -1 ).proZoneOrder << 4) & 0x70)
//                                                  +((globalData->proZoneDataList.at(proZone_index -1).proZoneEleNum) & 0x0F);
//    //保护区类型，如果为禁行区（0），则只是保护区建筑类别有效；
//    //若为障碍物（1），则只是保护区高度有效
//    proZone_type_int = globalData->proZoneDataList.at(proZone_index -1).proZoneType;
//    proZone_elenum_int = globalData->proZoneDataList.at(proZone_index -1).proZoneEleNum;//该保护区元素个数

//    //保护区名称
//    String_to_GBK(proZone_name,16,globalData->proZoneDataList.at(proZone_index -1).proZoneName);
////    proZone_name_byte = globalData->proZoneDataList.at(proZone_index -1).proZoneName.toAscii();
////    for (quint8 i = 0;i<16;i++)
////    {
////        proZone_name[i] = proZone_name_byte.data()[i];
////    }
//    //保护区id
//    proZone_id[0] = globalData->proZoneDataList.at(proZone_index -1).proZoneID;
//    proZone_build_type[0] = (globalData->proZoneDataList.at(proZone_index -1).proBuildType);//建筑类型

//    Double_To_Array(proZone_height, 10*globalData->proZoneDataList.at(proZone_index -1).proZoneHeight);//保护区高度
//    //将以上各字段拼接为数组
//    addr = globalData->split_joint_hex_data(pro_info_buf , addr ,  info_type  , 1);//信息类别
//    addr = globalData->split_joint_hex_data(pro_info_buf , addr ,  proZone_version  , 2);//保护区信息 版本
//    addr = globalData->split_joint_hex_data(pro_info_buf , addr ,  proZone_num  , 1);//保护区个数
//    addr = globalData->split_joint_hex_data(pro_info_buf , addr ,  proZone_typ_ord_elenum  , 1);//3合1
//    addr = globalData->split_joint_hex_data(pro_info_buf , addr , proZone_name , 16);
//    addr = globalData->split_joint_hex_data(pro_info_buf , addr , proZone_id , 1);
//    addr = globalData->split_joint_hex_data(pro_info_buf , addr , proZone_build_type , 1);
//    addr = globalData->split_joint_hex_data(pro_info_buf , addr , proZone_height , 2);
//    //以上25字节，接下来拼接元素信息
//    qDebug()<<"**********************proZone_elenum_int"<<proZone_elenum_int;
//    if (proZone_elenum_int > 10)
//    {
//        qDebug()<<"Something wrong with your element number, larger than 10!Check your database!";
//        return -1;
//    }
//    for(quint8 i =0;i<proZone_elenum_int;i++)
//    {
//        proZone_ele_type[0] = globalData->proZoneDataList.at(proZone_index -1).elementData.at(i).elementType;
//        //判断为点类型还是圆弧类型，并作各自的拼接
//        if( proZone_ele_type[0] == 0x00)//点类型
//        {
//            Double_To_Array(proZone_ele_locX ,10*globalData->proZoneDataList.at(proZone_index -1).elementData.at(i).pointX);
//            Double_To_Array(proZone_ele_locY ,10*globalData->proZoneDataList.at(proZone_index -1).elementData.at(i).pointY);
//            addr = globalData->split_joint_hex_data(pro_info_buf , addr , proZone_ele_type , 1);
//            addr = globalData->split_joint_hex_data(pro_info_buf , addr , proZone_ele_locX , 2);
//            addr = globalData->split_joint_hex_data(pro_info_buf , addr , proZone_ele_locY, 2);
//        }
//        else if(proZone_ele_type[0] == 0x01)//圆弧类型
//        {
//            Double_To_Array(proZone_ele_locX, 10*globalData->proZoneDataList.at(proZone_index -1).elementData.at(i).pointX);
//            Double_To_Array(proZone_ele_locY,10*globalData->proZoneDataList.at(proZone_index -1).elementData.at(i).pointY);
//            Double_To_Array(proZone_ele_rad,10*globalData->proZoneDataList.at(proZone_index -1).elementData.at(i).radius);
//            Double_To_Array(proZone_ele_start_ang,10*globalData->proZoneDataList.at(proZone_index -1).elementData.at(i).startAngle);
//            Double_To_Array(proZone_ele_end_ang,10*globalData->proZoneDataList.at(proZone_index -1).elementData.at(i).endAngle);
//            addr = globalData->split_joint_hex_data(pro_info_buf , addr , proZone_ele_type , 1);
//            addr = globalData->split_joint_hex_data(pro_info_buf , addr , proZone_ele_locX , 2);
//            addr = globalData->split_joint_hex_data(pro_info_buf , addr , proZone_ele_locY , 2);
//            addr = globalData->split_joint_hex_data(pro_info_buf , addr , proZone_ele_rad , 2);
//            addr = globalData->split_joint_hex_data(pro_info_buf , addr , proZone_ele_start_ang , 2);
//            addr = globalData->split_joint_hex_data(pro_info_buf , addr , proZone_ele_end_ang , 2);
//        }
//    }
//    len = addr;
//    qDebug()<<"the data_len of proZone "<<proZone_index<<"is "<<len;
//    return len;
////            addr = globalData->create_data_for_center(0x04 , (char*)center_receive_ID ,temp_data , Temp_ProZone_Mes , addr ,0);
//}
///***************************************/
///***********限位信息拼接**********/
///***************************************/
//quint32 EncSpl::EncLimit(quint8* limit_info_buf)
//{
//    quint32 addr = 0;
//    quint32 len=0;
//    //quint8 limit_info_buf[200];//限位信息拼接数组
//    quint8 info_type[1] = {0x03};//信息类别
//    quint8 limit_info_type[2];//限位信息版本
//    quint8 left_limit[2];//左限位(-3276.8-3276.8)
//    quint8 right_limit[2];//右限位(-3276.8-3276.8)
//    quint8 far_limit[1];//远限位(0-25.5)
//    quint8 near_limit[1];//近限位(0-25.5)
//    quint8 height_limit[1];//高限位(0-25.5)
//    quint8 weight_limit[2];//吊重限位(0-655.35)
//    quint8 torque_limit[2];//力矩限位值(0-655.35)
//    quint8 sensor_enable[1];//传感器使能标志

//    //将限位信息拼接数组清空
//    memset(limit_info_buf , 0 , 200);

///******将数据库中的各个类型的数据转换为字节数组，以便拼接********/
//    //qDebug()<<"limitVersion = "<<globalData->limitData.limitVersion;
//    Double_To_Array(limit_info_type, globalData->limitData.limitVersion);
//    //qDebug()<<"leftLimit = "<<globalData->limitData.leftLimit;
//    Double_To_Array(left_limit, 10*globalData->limitData.leftLimit);
//    Double_To_Array(right_limit, 10*globalData->limitData.rightLimit);
//    far_limit[0] = 10*globalData->limitData.farLimit;
//    near_limit[0]=10*globalData->limitData.nearLmit;
//    height_limit[0]=10*globalData->limitData.highLimit;
//    Double_To_Array(weight_limit,100*globalData->limitData.weightLimit);
//    Double_To_Array(torque_limit,100*globalData->limitData.momentLimit);
//    sensor_enable[0]=globalData->limitData.enableBit;

//    //将以上各字段拼接为数组
//    addr = globalData->split_joint_hex_data(limit_info_buf , addr ,  info_type  , 1);
//    addr = globalData->split_joint_hex_data(limit_info_buf , addr ,  limit_info_type  , 2);
//    addr = globalData->split_joint_hex_data(limit_info_buf , addr ,  left_limit  , 2);
//    addr = globalData->split_joint_hex_data(limit_info_buf , addr ,  right_limit  , 2);
//    addr = globalData->split_joint_hex_data(limit_info_buf , addr ,  far_limit  , 1);
//    addr = globalData->split_joint_hex_data(limit_info_buf , addr ,  near_limit  , 1);
//    addr = globalData->split_joint_hex_data(limit_info_buf , addr ,  height_limit  , 1);
//    addr = globalData->split_joint_hex_data(limit_info_buf , addr ,  weight_limit  , 2);
//    addr = globalData->split_joint_hex_data(limit_info_buf , addr ,  torque_limit  , 2);
//    addr = globalData->split_joint_hex_data(limit_info_buf , addr ,  sensor_enable  , 1);

//    len=addr;
//    return len;
//}
/***************************************/
/****************KD值及端口信息拼接***************/
/***************************************/
//quint32 EncSpl::EncKD_ip(quint8* KDip_info_buf)
//{
//    qDebug()<<"kDKDKDKDKDK";
//    quint32 addr = 0;
//    quint32 len=0;
////    quint8 Temp_KD_Mes[100];//KD值及端口信息拼接数组
////    quint8 temp_data[150];//加协议版本 厂商代码 帧类型 信息段长 设备代码
//   // quint8 info_type[1] = {0x03};//信息类别
//    quint8 info_type[1] = {0x04};
//    quint8 KD_info_version[2];//KD值及端口信息版本,2字节
//    quint8 weight_k[2];//吊重k值,2字节
//    quint8 weight_d[2];//吊重d值，2字节
//    quint8 hor_k[2];//水平k值，2字节
//    quint8 hor_d[2];//水平d值，2字节
//    quint8 vert_k[2];//垂直k值，2字节
//    quint8 vert_d[2];//垂直d值，2字节
//    quint8 server_ip[15];//服务器ip，15字节
//    QByteArray server_ip_byte;
//    quint8 server_port[5];//服务器端口，5字节
//    QByteArray server_port_byte;
//    quint8 com_channel[1];//信道id，1字节
//    QByteArray com_channel_byte;
//    quint8 net_id[2];//网络id，2字节
//    QByteArray net_id_byte;
//    quint8 pitch_height[1];//塔吊节高，1字节
//    quint8 init_height[1];  //塔吊初始高度，1字节
//    quint8 pitch_num[1];//塔吊节数，1字节
//    quint8 pro_high_e_w[1];//保护区高度预警，1字节
//    quint8 pro_high_w[1];//保护区高度报警，1字节

//    //将拼接数组清空
//    memset(KDip_info_buf , 0 , 100);
////    memset(temp_data , 0 , 150);
///******将数据库中的各个类型的数据转换为字节数组，以便拼接********/
//    Double_To_Array(KD_info_version,globalData->craneKD.craneKDVersion);
//    Double_To_Array(weight_k,10000*globalData->craneKD.weightK);
//    Double_To_Array(weight_d,globalData->craneKD.weightD);
//    Double_To_Array(hor_k,10000*globalData->craneKD.spanK);
//    Double_To_Array(hor_d,100*globalData->craneKD.spanD);
//    Double_To_Array(vert_k,10000*globalData->craneKD.heightK);
//    Double_To_Array(vert_d,10*globalData->craneKD.heightD);

//    //服务器IP,15字节
//    String_to_GBK(server_ip,15,globalData->craneKD.serverIP);
////    server_ip_byte = globalData->craneKD.serverIP.toAscii();
////    for (quint8 i = 0;i<15;i++)
////    {
////        server_ip[i] = server_ip_byte.data()[i];
////    }

//    //服务器端口，5字节
//    String_to_GBK(server_port,5,globalData->craneKD.serverPort);
////    server_port_byte = globalData->craneKD.serverPort.toAscii();
////    for (quint8 i = 0;i<5;i++)
////    {
////        server_port[i] = server_port_byte.data()[i];
////    }

//    //信道ID,1字节
//    String_to_GBK(com_channel,1,globalData->craneKD.ZigbeeChannel);
////    com_channel_byte= globalData->craneKD.ZigbeeChannel.toAscii();
////    for (quint8 i = 0;i<1;i++)
////    {
////        com_channel[i] = com_channel_byte.data()[i];
////    }

//    //网络ID，2字节
//    String_to_GBK(net_id,2,globalData->craneKD.ZigbeeID);
////    net_id_byte= globalData->craneKD.ZigbeeID.toAscii();
////    for (quint8 i = 0;i<2;i++)
////    {
////        net_id[i] = net_id_byte.data()[i];
////    }

//    pitch_height[0] = globalData->craneKD.pitchHeight;
//    init_height[0] = globalData->craneKD.initHeight;
//    pitch_num[0] = globalData->craneKD.pitchNum;
//    pro_high_e_w[0] = globalData->craneKD.proHeightEW;
//    pro_high_w[0] = globalData->craneKD.proHeightW;

//    //将以上各字段拼接为数组
//    addr = globalData->split_joint_hex_data(KDip_info_buf , addr ,  info_type  , 1);//信息类别
//    addr = globalData->split_joint_hex_data(KDip_info_buf , addr ,  KD_info_version  , 2);//KD值及端口信息版本,2字节
//    addr = globalData->split_joint_hex_data(KDip_info_buf , addr ,  weight_k  , 2);//吊重k值,2字节
//    addr = globalData->split_joint_hex_data(KDip_info_buf , addr ,  weight_d  , 2);//吊重d值，2字节
//    addr = globalData->split_joint_hex_data(KDip_info_buf , addr ,  hor_k  , 2);//水平k值，2字节
//    addr = globalData->split_joint_hex_data(KDip_info_buf , addr ,  hor_d  , 2);//水平d值，2字节
//    addr = globalData->split_joint_hex_data(KDip_info_buf , addr ,  vert_k  , 2);//垂直k值，2字节
//    addr = globalData->split_joint_hex_data(KDip_info_buf , addr ,  vert_d  , 2);//垂直d值，2字节
//    addr = globalData->split_joint_hex_data(KDip_info_buf , addr ,  server_ip  , 15);//服务器ip，15字节
//    addr = globalData->split_joint_hex_data(KDip_info_buf , addr ,  server_port  , 5);//服务器端口，5字节
//    addr = globalData->split_joint_hex_data(KDip_info_buf , addr ,  com_channel  , 1);//信道id，1字节
//    addr = globalData->split_joint_hex_data(KDip_info_buf , addr ,  net_id  , 2);//网络id，2字节
//    addr = globalData->split_joint_hex_data(KDip_info_buf , addr ,  pitch_height  , 1);//塔吊节高，1字节
//    addr = globalData->split_joint_hex_data(KDip_info_buf , addr ,  init_height  , 1);//塔吊初始高度，1字节
//    addr = globalData->split_joint_hex_data(KDip_info_buf , addr ,  pitch_num  , 1);//塔吊节数，1字节
//    addr = globalData->split_joint_hex_data(KDip_info_buf , addr ,  pro_high_e_w  , 1);//保护区高度预警，1字节
//    addr = globalData->split_joint_hex_data(KDip_info_buf , addr ,  pro_high_w  , 1);//保护区高度报警，1字节

//    for(quint32 i=0; i < addr; i++)
//    {
//        qDebug()<<i<<":"<<hex<<KDip_info_buf[i];
//    }
//    len= addr;
//    qDebug()<<"kd data length = "<<len;
//    return len;//信息段长度

////    addr = globalData->create_data_for_center(0x04 , (char*)center_receive_ID ,temp_data , KDip_info_buf , addr ,0);
//}
/*
  ********************************************************
    函数：EncCraneGroup（quint32）
    描述：拼接塔群配置信息
    参数：
    日期：2013.12
  ********************************************************
  */
//quint32 EncSpl::EncCraneGroup(quint8* group_info_buf,quint32 Crane_index)
//{
//    quint32 addr = 0;
//    quint32 len=0;
//    quint8 info_type[1] = {0x05};//信息类别，
//    quint8 group_info_version[2];//塔群信息版本
//    quint8 crane_index[1] = {Crane_index};
//    quint8 groupID[1];
//    quint8 relatedCraneNum[1];//相干塔吊个数
//    quint8 relatedCraneID[1];//相干塔吊ID
//    QString craneIMEI;
//    quint8 str_imei[16];//imei号转换的字节数组
//    quint8 coordinateX[2];//X
//    quint8 coordinateY[2];//Y
//    quint8 forearmLen[2];//前臂长
//    quint8 backarmLen[2];//后臂长
//    quint8 craneHeight[2];//塔臂高
//    quint8 craneTopHeight[2];//塔帽高

//    //将拼接数组清空
//    memset(group_info_buf , 0 , 200);
//    qDebug()<<"Crane_index = "<<Crane_index;
///******将数据库中的各个类型的数据转换为字节数组，以便拼接********/
//    Double_To_Array(group_info_version,globalData->basicData.basicVersion);//塔群信息版本即基本信息版本
//    groupID[0] = globalData->craneList.at(Crane_index).groupID;
//    relatedCraneNum[0] = globalData->craneList.at(Crane_index).relatedCraneNum;
//    relatedCraneID[0] = globalData->craneList.at(Crane_index).craneID;
//    craneIMEI = globalData->craneList.at(Crane_index).craneMAC;
//    if (craneIMEI.size() < 15)
//    {
//        qDebug()<<"Something wrong with your IMEI length of the CraneGroup_"<<Crane_index;
//        return -1;
//    }
//    imei_to_byteArray(craneIMEI,str_imei);//imei号转换为16字节数组
//    Double_To_Array(coordinateX,globalData->craneList.at(Crane_index).coordinateX * 10);
//    Double_To_Array(coordinateY,globalData->craneList.at(Crane_index).coordinateY * 10);
//    Double_To_Array(forearmLen,globalData->craneList.at(Crane_index).forearmLen * 10);
//    Double_To_Array(backarmLen,globalData->craneList.at(Crane_index).backarmLen * 10);
//    Double_To_Array(craneHeight,globalData->craneList.at(Crane_index).craneHeight * 10);
//    Double_To_Array(craneTopHeight,globalData->craneList.at(Crane_index).craneTopHeight * 10);



//    addr = globalData->split_joint_hex_data(group_info_buf , addr ,  info_type  , 1);//信息类别
//    addr = globalData->split_joint_hex_data(group_info_buf , addr ,  group_info_version  , 2);
//    addr = globalData->split_joint_hex_data(group_info_buf , addr ,  crane_index  , 1);//塔吊index
//    addr = globalData->split_joint_hex_data(group_info_buf , addr ,  groupID  , 1);
//    addr = globalData->split_joint_hex_data(group_info_buf , addr ,  relatedCraneNum  , 1);
//    addr = globalData->split_joint_hex_data(group_info_buf , addr ,  relatedCraneID  , 1);
//    addr = globalData->split_joint_hex_data(group_info_buf , addr ,  str_imei  , 16);
//    addr = globalData->split_joint_hex_data(group_info_buf , addr ,  coordinateX  , 2);
//    addr = globalData->split_joint_hex_data(group_info_buf , addr ,  coordinateY  , 2);
//    addr = globalData->split_joint_hex_data(group_info_buf , addr ,  forearmLen  , 2);
//    addr = globalData->split_joint_hex_data(group_info_buf , addr ,  backarmLen  , 2);
//    addr = globalData->split_joint_hex_data(group_info_buf , addr ,  craneHeight  , 2);
//    addr = globalData->split_joint_hex_data(group_info_buf , addr ,  craneTopHeight  , 2);

//    len = addr;
//    return len;

//}

/***************************************/
/***********塔机间互发信息拼接**********/
/*参数：
           proZone_index一定为0，本机塔吊
***/
/***************************************/
int EncSpl::EncCraneInter(quint8 *Crane_info_buf)
{
    //qDebug()<<"groupID = "<<globalData->craneList.at(0).groupID;
    //qDebug()<<"enter  enccraneinter";
    /***********************************
    //                   拼接数据
    //塔群识别码（6bits）+塔机识别码（6bits）+厂商（3bits) +
       回转（12bits）+转速（7bits）+变幅（11bits）+起升（13bits）
    */
    /**/

    //quint32 addr = 0;
    //quint32 len=0;
    //quint8 Crane_info_buf[30];//KD值及端口信息拼接数组
    //quint8 temp_data[50];//加协议版本 厂商代码 帧类型 信息段长 设备代码
    //quint8 info_type[1] = {0x05};
    quint8 wirelessM[15] = {0};                    //拼接的字节数组
    quint8 groupID;                          //塔群ID
    quint8 craneID;                          //塔机ID
    quint8 firmID;                            //厂商号
    double angle_DOUBLE;               //中间变量，回转
    double rotateSpeed_DOUBLE;         //中间变量，转速
    double span_DOUBLE;                 //中间变量，变幅
    double height_DOUBLE;               //中间变量，起升
    quint16 angle;                                       //回转
    quint16 rotateSpeed;                              //转速
    quint16 span;                                        //变幅
    qint16 height;                                      //起升
    quint8 checkCRC[2];
    int send_data_len;
    //将2个拼接数组清空
    //memset(Crane_info_buf , 0 , 30);
    //memset(temp_data , 0 , 50);
 /******将数据库中的数据转换为中间变量，以便拼接********/
    //全局变量中的craneList的第0条数据为本机信息，从第1条数据开始为相干塔吊信息
 //   qDebug()<<"********************************************";

    //qDebug()<<"groupID = "<<globalData->basicData.groupID;
    groupID=globalData->basicData.groupID;

    craneID=globalData->basicData.craneID;
  //  qDebug()<<"craneID = "<<craneID;
    firmID=globalData->basicData.firmID;
   //qDebug()<<"firmID = "<<firmID;

   // qDebug()<<"angle_DOUBLE = "<<globalData->craneList.at(0).angle;
    angle_DOUBLE=globalData->craneList.at(0).angle;
    //qDebug()<<"angle_DOUBLE = "<<angle_DOUBLE;
    rotateSpeed_DOUBLE=globalData->craneList.at(0).rotateSpeed;
  //  qDebug()<<"rotateSpeed_DOUBLE = "<<rotateSpeed_DOUBLE;
    span_DOUBLE=globalData->craneList.at(0).span;
  //  qDebug()<<"span_DOUBLE = "<<span_DOUBLE;
    height_DOUBLE=globalData->craneList.at(0).hookHeight;
 //   qDebug()<<"height_DOUBLE = "<<height_DOUBLE;

    /*将double型的回转，转速，变幅，起升4个变量强制转换为无符号16位整型*/
    angle = (quint16)(angle_DOUBLE/0.08789);
    rotateSpeed = (quint16)(rotateSpeed_DOUBLE*10);
    span = (quint16)(span_DOUBLE*10);
    height = (qint16)(height_DOUBLE*10);          //先将double型的height_DOUBLE转换为无符号的
 //   qDebug()<<"height"<<height;
    if (height < 0)                                              //正数直接用，负数则做补码运算
    {
        height = abs(height);
        //qDebug()<<"height"<<height;
        height = ((~height) & 0xFFF);                      //再做按位取反运算，并且屏蔽掉13位以上的每一bit
        height = (height | 0x1000) + 1;                   //置符号位，并+1得到补码
    }

    /*按照协议 塔机终端间的通信协议 开始拼接*/    
    wirelessM[0] = frame_header[0];
    wirelessM[1] = frame_header[1];
    wirelessM[2] = (groupID & 0x3F) + ((craneID << 6) & 0xC0);
    wirelessM[3] = ((craneID >> 2) & 0x0F) + ((firmID << 4) & 0x70) + ((angle << 7) & 0x80);
    wirelessM[4] = ((angle >> 1) & 0xFF);
    wirelessM[5] = ((angle >> 9) & 0x07) + ((rotateSpeed << 3) & 0xF8);
    wirelessM[6] = ((rotateSpeed >> 5) & 0x03) + ((span << 2) & 0xFC);
    wirelessM[7] = ((span >> 6) & 0x1F) + ((height << 5) & 0xE0);
    wirelessM[8] = ((height >> 3) & 0xFF);
    wirelessM[9] = ((height >> 11) & 0x03);
    //附加信息为0
    //CRC校验不包括帧头和帧尾
    globalData->dataCRC16((char*)wirelessM + 2, 8, checkCRC);
    wirelessM[10] = checkCRC[0];                         //CRC的低字节
    wirelessM[11] = checkCRC[1];                         //CRC的高字节
    //////////////////////////////////////////////////////////////////////////////////////////
   /****按照协议要求 在从帧头到CRC校验码共12个字节长的信息段中 如果有 FE的情况 则插入一个FE****/
   //////////////////////////////////////////////////////////////////////////////////////////
       quint8 repeat_num = 0;                                  //0xFE重复的次数
       quint8 info_size = 12;                                    //信息总长度，包括帧头和CRC


       Crane_info_buf[0] = wirelessM[0];                         //添加帧头
       Crane_info_buf[1] = wirelessM[1];


       for (quint8 i = 2; i < info_size; i++)
       {
           if(wirelessM[i] == 0xFE)
           {
               repeat_num++;
               Crane_info_buf[i + repeat_num-1 ] = 0xFE;
           }
           Crane_info_buf[ i + repeat_num ] = wirelessM[i];
       }
       Crane_info_buf[ info_size + repeat_num] = frame_end[0];
       Crane_info_buf[ info_size + repeat_num + 1] = frame_end[1];

        send_data_len = info_size + repeat_num +2;


    return send_data_len;
}
/*****封装ID数据************/
int EncSpl::EncID(char *imei_num, quint8 *frame){
    quint8 User[32];
    quint8 PasWord[32];
//    quint8 frame[85];
//    quint32 len;
    quint32 addr = 0;
    quint8 Temp_ID_Mes[64];

    for(quint8 i = 0;i <32;i++)
       {
        User[i] = 0x30;
        PasWord[i] = 0x30;
    }

//    addr = split_joint_hex_data(Temp_ID_Mes , addr , User , sizeof(User)/sizeof(User[0]));
    addr = globalData->split_joint_hex_data(Temp_ID_Mes , addr , User , 32);
    addr = globalData->split_joint_hex_data(Temp_ID_Mes , addr , PasWord , sizeof(PasWord)/sizeof(PasWord[0]));
    addr = globalData->create_data_for_center(0x00 , imei_num ,frame , Temp_ID_Mes , addr , 1);
    return addr;
}

/*****封装指纹ID数据*********/
int EncSpl::EncFingerID(char *frame, char *center_time , char *center_receive_ID , char *imei_num,quint8 event){
    char Temp_FingerId_Mes[13];

    char rec_style_event1[1] = {0x00};
    char event_type[1] = {0x05};
    int addr = 0;

    rec_style_event1[0] = (0x01 << 6) | (event << 3) | 0x00;
    //qDebug()<<"rec_style_event1"<<hex<<rec_style_event1[0];

    memset(Temp_FingerId_Mes , 0 , 13);
    addr = globalData->split_joint_hex_data(Temp_FingerId_Mes , addr , center_time ,4);

    addr = globalData->split_joint_hex_data(Temp_FingerId_Mes , addr , event_type ,1);
    addr = globalData->split_joint_hex_data(Temp_FingerId_Mes , addr , rec_style_event1 , 1);
    addr = globalData->split_joint_hex_data(Temp_FingerId_Mes , addr , imei_num , 4);
    addr = globalData->create_data_for_center(0x06 , center_receive_ID ,frame ,Temp_FingerId_Mes , addr , 0);
    return addr;
}

/***************************************/
/***********塔群相干信息解析**********/
/*参数：recv_buff[]为已经去除帧头帧尾，去转义，并且CRC校验后的数据段
 *          crane_index为craneList中的第n条数据的下标
*/
/***************************************/
bool EncSpl::SplCroupInter(quint8 *recv_buff)
{

    //quint8 recvLen = 14;                                     //塔机间通讯接受到的14个字节
    /******************从接受的数据帧分拆得到的中间整型变量********************/
    quint8 groupID_r;                                     //塔群ID
    quint8 craneID_r;                                     //塔机ID
    quint8 firmID_r;                                       //厂商号

    quint16 angle_r;                                       //回转
    quint16 rotateSpeed_r;                              //转速
    quint16 span_r;                                        //变幅
    qint16 lifting_height_r;                              //起升
    /******************将后面4个整型变量转换为double型变量********************/
    double angle_DOUBLE_r;                                       //回转
    double rotateSpeed_DOUBLE_r;                              //转速
    double span_DOUBLE_r;                                        //变幅
    double liftingHeight_DOUBLE_r;                                      //起升
    /**************************************************************/
    //quint8 CRCdataLen= recvLen - 6;                   //校验帧头到CRC出现之前的数据段，不算帧头帧尾及CRC
    //测试用数据，即为上面测试数据正确打包后的发送
    //quint8 recv_buff[14] = {0XFE, 0XFB, 0X81, 0X10, 0X58, 0X02, 0X28, 0XE6, 0XCD, 0X02, 0XD1, 0XB8, 0XFE, 0XFA};

    quint8 recvCRC[2] = {0};

    globalData->dataCRC16((char*)recv_buff+2, 8, recvCRC); //对数据段进行CRC校验并得到CRC校验码
                                       //判断CRC校验码是否正确，正确才进行下一步的数据段解析，并得到各个数据信息
//    qDebug()<<"CRC[0]="<<hex<<recvCRC[0];
//    qDebug()<<"CRC[1]="<<hex<<recvCRC[1];
    if ( (recv_buff[10] == recvCRC[0]) && (recv_buff[11] == recvCRC[1]))
    {
        groupID_r= recv_buff[2] & 0x3F;
        craneID_r = ( (recv_buff[2] >> 6) & 0x03 ) + ( (recv_buff[3] << 2) & 0x3C );
        firmID_r = (recv_buff[3] >> 4) & 0x07;

        angle_r = (( (quint16)recv_buff[3] >> 7 ) & 0x0001 ) + (( (quint16)recv_buff[4] << 1 ) & 0x01FE )
                   + (( (quint16)recv_buff[5] << 9 ) & 0x0E00 );
        rotateSpeed_r = (( (quint16)recv_buff[5] >> 3 ) & 0x001F ) + (( (quint16)recv_buff[6] << 5 ) & 0x0060 );
        span_r = (( (quint16)recv_buff[6] >> 2 ) & 0x003F ) + (( (quint16)recv_buff[7] << 6 ) & 0x07C0 );
        lifting_height_r = (( (quint16)recv_buff[7] >> 5 ) & 0x0007 ) + (( (quint16)recv_buff[8] << 3 ) & 0x07F8 )
                    + (( (quint16)recv_buff[9] << 11 ) & 0x1800 );
        if( (lifting_height_r & 0x1000) == 0x1000 )                                          //最高位为1，即为负数
        {
            lifting_height_r =~ (lifting_height_r - 1);                                                    //补码逆运算
            lifting_height_r = (lifting_height_r & 0xFFF);                                               //屏蔽掉13位以上符号位及无效位
            lifting_height_r = 0 - lifting_height_r;                                                        //取反，正数变成负数
        }

        angle_DOUBLE_r = (double)angle_r * 0.08789;
        rotateSpeed_DOUBLE_r = (double)rotateSpeed_r / 10;
        span_DOUBLE_r = (double)span_r / 10;
        liftingHeight_DOUBLE_r = (double)lifting_height_r / 10;

        qDebug()<< "groupID"<<groupID_r;
        qDebug()<< "craneID"<<craneID_r;
        qDebug()<< "firmID"<<firmID_r;
        qDebug()<< "angle"<<angle_DOUBLE_r;
        qDebug()<< "rotateSpeed"<<rotateSpeed_DOUBLE_r;
        qDebug()<< "span"<<span_DOUBLE_r;
        qDebug()<< "height"<<liftingHeight_DOUBLE_r;

/**********存全局变量***********/

        for(qint32 i = 0; i < globalData->craneList.size(); i++)
        {
            quint8 craneID_temp = globalData->craneList.at(i).craneID;
            qDebug()<<"craneID_temp in the globalData = "<<craneID_temp;
            if (craneID_temp == craneID_r)
            {
                globalData->craneList[i].groupID =  groupID_r;
                globalData->craneList[i].firmID =  firmID_r;
                globalData->craneList[i].angle =  angle_DOUBLE_r;
                globalData->craneList[i].rotateSpeed =  rotateSpeed_DOUBLE_r;
                globalData->craneList[i].span =  span_DOUBLE_r;
                globalData->craneList[i].hookHeight =  liftingHeight_DOUBLE_r;
                break;//跳出循环
            }
            else
            {
                qDebug()<<"What we received is not the interfaced crane!! ";

            }

        }
    }
    else
    {
        qDebug()<<"CRC desn't match when receiving crane inter information!";
    }
    return true;

}

/*****封装状态信息***********/
quint32 EncSpl::EncStatusData(char *Temp_Status_Data , char *center_receive_time){
    char angle_char[2];
    char span_char[2];
    char height_char[2];
    char weight_char[2];
    char moment_char[1];

    char windspeed_char[2];
    char tileangleX_char[1];
    char tileangleY_char[1];
    quint32 addr = 0;
    char stcar_angle_char[1];
char limitalarm_char[4];
char otheralarm_char[4];
char collidealarm_char[4];
char forbidalarm_char[4];
char obstaclealarm_char[4];
char relay_char[4];

    Double_To_Array(angle_char , 10*globalData->craneList.at(0).angle);
    //int_angle = 10*globalData->craneList.at(0).angle;
    //angle_char[0] = (int_angle) && 0xFF;
    //angle_char[1] = (int_angle >> 8) && 0xFF;

    Double_To_Array(span_char ,10*globalData->craneList.at(0).span);
    //int_span = 10*globalData->craneList.at(0).span;
    //span_char[0] = (int_span) && 0xFF;
    //span_char[1] = (int_span >>8) && 0xFF;

    Double_To_Array(height_char,10*globalData->craneList.at(0).craneHeight);
    //int_height = 10*globalData->craneList.at(0).craneHeight;
    //height_char[0] = (int_height ) && 0xFF;
    //height_char[1] = (int_height >>8) && 0xFF;

    Double_To_Array(weight_char,100*globalData->craneList.at(0).wight);
    //int_weight = 100*globalData->craneList.at(0).wight;
    //weight_char[0] = (int_weight ) && 0xFF;
    //weight_char[1] = (int_weight >>8) && 0xFF;

    moment_char[0] = globalData->craneList.at(0).moment;
    stcar_angle_char[0] = 0;

    Double_To_Array(windspeed_char,10*globalData->craneList.at(0).windspeed);
    //int_windspeed = 10*globalData->craneList.at(0).windspeed;
    //windspeed_char[0] = (int_windspeed ) && 0xFF;
    //windspeed_char[1] = (int_windspeed >>8) && 0xFF;
    static char tmpchar[2];
    Double_To_Array(tmpchar,10*globalData->craneList.at(0).tiltangleX);
    tileangleX_char[0] = tmpchar[1];
    Double_To_Array(tmpchar,10*globalData->craneList.at(0).tiltangleY);
    tileangleY_char[0] = tmpchar[1];

    /*******限位报警标志封装*********/
    limitalarm_char[0] = 0x00;
    limitalarm_char[1] = globalData->craneList.at(0).high_limit_warn_flag + (globalData->craneList.at(0).low_limit_warn_flag << 4);
    limitalarm_char[2] = globalData->craneList.at(0).far_limit_warn_flag + (globalData->craneList.at(0).near_limit_warn_flag<<4);
    limitalarm_char[3] = globalData->craneList.at(0).left_limit_warn_flag+(globalData->craneList.at(0).right_limit_warn_flag<<4);

    /******其他报警标志封装*******/
    otheralarm_char[0] = 0x00;
    otheralarm_char[1] = globalData->craneList.at(0).axis_y_warn_flag;
    otheralarm_char[2] = globalData->craneList.at(0).wind_limit_warn_flag+(globalData->craneList.at(0).axis_x_warn_flag << 4);
    otheralarm_char[3] = globalData->craneList.at(0).overweight_limit_warn_flag+(globalData->craneList.at(0).moment_limit_warn_flag<<4);

    /*****防碰撞报警标志封装******/
    collidealarm_char[0] = 0x00;
    collidealarm_char[1] = globalData->craneList.at(0).lowCollision;
    collidealarm_char[2] = globalData->craneList.at(0).farCollision+ (globalData->craneList.at(0).nearCollision<<4);
    collidealarm_char[3] = globalData->craneList.at(0).leftCollision + (globalData->craneList.at(0).rightCollision<<4);

    /******禁行区报警标志封装******/
    forbidalarm_char[0] = 0x00;
    forbidalarm_char[1] = 0x00;
    forbidalarm_char[2] = globalData->far_forbid_warn_flag+(globalData->near_forbid_warn_flag<<4);
    forbidalarm_char[3] = globalData->left_forbid_warn_flag + (globalData->right_forbid_warn_flag<<4);

    /*****障碍物报警标志封装******/
    obstaclealarm_char[0] = 0x00;
    obstaclealarm_char[1] = globalData->low_barrier_warn_flag;
    obstaclealarm_char[2] = globalData->far_barrier_warn_flag+(globalData->near_barrier_warn_flag<<4);
    obstaclealarm_char[3] = globalData->left_barrier_warn_flag+(globalData->right_barrier_warn_flag<<4);

    //for(quint8 i =0;i<4;i++)
    //{
    //    qDebug()<<"send time"<<hex<<center_receive_time[i];
    //}
    addr = globalData->split_joint_hex_data(Temp_Status_Data , addr ,  center_receive_time  , 4);
    addr = globalData->split_joint_hex_data2((char*)Temp_Status_Data , addr ,  angle_char  , 2);
    addr = globalData->split_joint_hex_data(Temp_Status_Data , addr ,  span_char  , 2);
    addr = globalData->split_joint_hex_data2((char*)Temp_Status_Data , addr ,  height_char  , 2);
    addr = globalData->split_joint_hex_data(Temp_Status_Data , addr ,  weight_char  , 2);
    addr = globalData->split_joint_hex_data(Temp_Status_Data , addr ,  moment_char  , 1);
    addr = globalData->split_joint_hex_data(Temp_Status_Data , addr ,  stcar_angle_char  , 1);
    addr = globalData->split_joint_hex_data(Temp_Status_Data , addr ,  windspeed_char  , 2);
    addr = globalData->split_joint_hex_data(Temp_Status_Data , addr ,  tileangleX_char  , 1);
    addr = globalData->split_joint_hex_data(Temp_Status_Data , addr ,  tileangleY_char  , 1);
    addr = globalData->split_joint_hex_data(Temp_Status_Data , addr ,  limitalarm_char  , 4);
    addr = globalData->split_joint_hex_data(Temp_Status_Data , addr ,  otheralarm_char  , 4);
    addr = globalData->split_joint_hex_data(Temp_Status_Data , addr ,  collidealarm_char  , 4);
    addr = globalData->split_joint_hex_data(Temp_Status_Data , addr ,  forbidalarm_char  , 4);
    addr = globalData->split_joint_hex_data(Temp_Status_Data , addr ,  obstaclealarm_char  , 4);
    addr = globalData->split_joint_hex_data(Temp_Status_Data , addr ,  relay_char  , 4);
    return addr;
}

/***************************************/
/***********基本信息解析**********/
/*参数：recv_buff[]为已经去除帧头帧尾，去转义，并且CRC校验后的数据段
*/
/***************************************/
bool EncSpl::SplBasic(quint8 *recv_buff)
{
    sql_mutex.lock();
    bool storeSuccess = false;//存数据库成功标志位，作为此函数返回值

    /*******存数据库前的中间数据变量*******/
    quint16 basic_version_r;//信息版本
    QString craneName_r;//塔吊名称（16字节）
    quint8 craneID_r;//塔机ID
    quint8 groupID_r;//塔群ID
    quint8 craneType_r;//塔吊类型（4bits）
    quint8 craneRate_r;//塔吊倍率（4bits）
    double coordinateX_r;//X坐标（-3276.8～3276.8）
    double coordinateY_r;//Y坐标（-3276.8～3276.8）
    double foreArmLen_r;//前臂长（0～6553.5）
    double backArmLen_r;//后臂长（0～6553.5）
    double craneArmHeight_r;            //塔臂高(0-6553.5)
    double craneTopHeight_r;      // 塔顶高度(0-25.6)

    /************从字节数组中分离字段储存到中间变量中**********************/

    quint32 m=0;
    basic_version_r = (recv_buff[0]<<8) + recv_buff[1];//信息版本，范围0～65535
    qDebug()<<"basic_version_r = "<<basic_version_r;
    m = m + 2;
    quint8 crane_name[16];
    Array_copy(recv_buff,m,crane_name,0,16);
    craneName_r = Array_to_String(crane_name,16);//转换为字符串

    qDebug()<<"%%%%%%%%%%%%% craneName_r = "<<craneName_r;
//    for (int i=0;i<m+16;i++)//将
//    {
//        crane_name[i] = recv_buff[i+m];
//        craneName_r += QString::number(crane_name[i],10);//将16字节的数组转换为字符串
//    }
    //qDebug()<<"craneName_r = "<<craneName_r;
    m = m+16;
    craneID_r = recv_buff[m];//塔机ID
    qDebug()<<"craneID_r = "<<craneID_r;
    m = m+1;
    groupID_r = recv_buff[m];//塔群ID
    m=m+1;
    craneType_r = recv_buff[m]>>4;//塔机类型，为高4bits
    if(craneType_r >= 2)
    {
        craneType_r =0;
    }
    craneRate_r =( recv_buff[m] & 0x0F ) * 2;//塔机倍率
    if(craneRate_r >= 9)  //按照协议的2，4，6，8来写，与罗浮云的程序data_pro.cs的hex_to_basic_info()不同
    {
        craneRate_r =0;
    }
    m=m+1;
    quint8 temp[2];
    Array_copy(recv_buff,m,temp,0,2);
    coordinateX_r = 0.1*Hex_to_Double(temp);//坐标X
    m=m+2;
    Array_copy(recv_buff,m,temp,0,2);
    coordinateY_r = 0.1*Hex_to_Double(temp);//坐标Y
    m=m+2;
    Array_copy(recv_buff,m,temp,0,2);
    foreArmLen_r = 0.1*Hex_to_Double(temp);//前臂长度
    m=m+2;
    Array_copy(recv_buff,m,temp,0,2);
    backArmLen_r = 0.1*Hex_to_Double(temp);//后臂长度
    m=m+2;
    Array_copy(recv_buff,m,temp,0,2);
    craneArmHeight_r = 0.1*Hex_to_Double(temp);//塔臂高度
    m=m+2;
    craneTopHeight_r=0.1*recv_buff[m];//塔帽高度
    m=m+1;
/******************将数据存到数据库**********************/
    QSqlTableModel *saveBasicData = new QSqlTableModel();
    saveBasicData->setTable("BasicData");//打开数据库表
    saveBasicData->setFilter("id = 0");
    saveBasicData->select();
//用setData的方法依赖于数据库字段的顺序
    if (saveBasicData->rowCount() == 0) {            //表中不存在数据,则新建
        int row = 0 ;
        saveBasicData->insertRows(row,1);
        saveBasicData->setData(saveBasicData->index(row,0),0);//id = 0
        saveBasicData->setData(saveBasicData->index(row,1),basic_version_r);
        saveBasicData->setData(saveBasicData->index(row,2),craneName_r);
        saveBasicData->setData(saveBasicData->index(row,4),groupID_r);
        saveBasicData->setData(saveBasicData->index(row,3),craneID_r);

        saveBasicData->setData(saveBasicData->index(row,6),craneType_r);
        saveBasicData->setData(saveBasicData->index(row,7),craneRate_r);
        saveBasicData->setData(saveBasicData->index(row,8),coordinateX_r);
        saveBasicData->setData(saveBasicData->index(row,9),coordinateY_r);
        saveBasicData->setData(saveBasicData->index(row,10),foreArmLen_r);
        saveBasicData->setData(saveBasicData->index(row,11),backArmLen_r);
        saveBasicData->setData(saveBasicData->index(row,12),craneArmHeight_r);
        saveBasicData->setData(saveBasicData->index(row,13),craneTopHeight_r);
        saveBasicData->submitAll();
        if (FALSE == saveBasicData->submitAll())
        {
            qDebug()<<"Basic data failed to be stored in database!!!";
            return storeSuccess ;
        }
        else
            storeSuccess = true;
    }
    else if (saveBasicData->rowCount() == 1) //如果有一条数据则更新表数据
    {
        QSqlRecord record = saveBasicData->record(0);
        record.setValue("basicVersion",basic_version_r);// 保存基本信息版本
        record.setValue("craneName",craneName_r);
        record.setValue("groupID",groupID_r);
        record.setValue("craneID",craneID_r);
        record.setValue("craneType",craneType_r);
        record.setValue("craneRate",craneRate_r);
        record.setValue("coordinateX",coordinateX_r);
        record.setValue("coordinateY",coordinateY_r);
        record.setValue("forearmLen",foreArmLen_r);
        record.setValue("backarmLen",backArmLen_r);
        record.setValue("craneHeight",craneArmHeight_r);
        record.setValue("craneTopheight",craneTopHeight_r);
        saveBasicData->setRecord(0,record);//在第0行保存上面刚设置好的记录

        if (FALSE == saveBasicData->submitAll())
        {
            qDebug()<<"基本信息在数据库中保存失败";
            return storeSuccess;
        }
        else
            storeSuccess = true;

    }
    sql_mutex.unlock();
    return storeSuccess;
}
/***************************************/
/***********限位信息解析**********/
/*参数：recv_buff[]为已经去除帧头帧尾，去转义，并且CRC校验后的数据段
*/
/***************************************/
bool EncSpl::SplLimit(quint8 * recv_buff)
{
    sql_mutex.lock();
    bool storeSuccess = false;//存数据库成功标志位，作为此函数返回值
    /*******存数据库前的中间数据变量*******/
    quint16 limit_version_r;//信息版本
    double left_limit_r;//左限位（-3276.8～3276.8）
    double right_limit_r;//右限位（-3276.8～3276.8）
    double far_limit_r;//远限位（0～25.5）
    double near_limit_r;//近限位（0～25.5）
    double hight_limit_r; //高限位（0～25.5）
    double weight_limit_r;      // 起重量高度(0-655.35)
    double torque_limit_r; // 力矩限位(0-655.35)
    quint8 enable_bit_r;//传感器使能标志
    /************从字节数组中分离字段储存到中间变量中**********************/
    quint32 m=0;
    limit_version_r = (recv_buff[0]<<8) + recv_buff[1];//信息版本，范围0～65535
    m = m + 2;

    quint8 temp[2];

    Array_copy(recv_buff,m,temp,0,2);
    left_limit_r = 0.1*Hex_to_Double(temp);//左限位
    m=m+2;

    Array_copy(recv_buff,m,temp,0,2);
    right_limit_r = 0.1*Hex_to_Double(temp);//右限位
    m=m+2;

    far_limit_r=0.1*recv_buff[m];
    m=m+1;

    near_limit_r=0.1*recv_buff[m];
    m=m+1;

    hight_limit_r=0.1*recv_buff[m];
    m=m+1;

    Array_copy(recv_buff,m,temp,0,2);
    weight_limit_r = 0.1*Hex_to_Double(temp);
    m=m+2;

    Array_copy(recv_buff,m,temp,0,2);
    torque_limit_r = 0.1*Hex_to_Double(temp);
    m=m+2;

    enable_bit_r=recv_buff[m];
/******************将数据存到数据库**********************/
    QSqlTableModel *saveLimitData = new QSqlTableModel;
    saveLimitData->setTable("LimitData");
    saveLimitData->setFilter("id = 0");
    saveLimitData->select();
    if (saveLimitData->rowCount() == 0) {            //表中不存在数据,则新建
        int row = 0 ;
        saveLimitData->insertRows(row,1);
        saveLimitData->setData(saveLimitData->index(row,0),0);
        saveLimitData->setData(saveLimitData->index(row,1),limit_version_r);
        saveLimitData->setData(saveLimitData->index(row,2),left_limit_r);
        saveLimitData->setData(saveLimitData->index(row,3),right_limit_r);
        saveLimitData->setData(saveLimitData->index(row,4),far_limit_r);
        saveLimitData->setData(saveLimitData->index(row,5),near_limit_r);
        saveLimitData->setData(saveLimitData->index(row,6),hight_limit_r);
        saveLimitData->setData(saveLimitData->index(row,7),weight_limit_r);
        saveLimitData->setData(saveLimitData->index(row,8),torque_limit_r);
        saveLimitData->setData(saveLimitData->index(row,9),enable_bit_r);
        saveLimitData->submitAll();
        if (FALSE == saveLimitData->submitAll())
        {
            qDebug()<<"限位信息添加失败";
            return storeSuccess;
        }
    }
    else if (saveLimitData->rowCount() == 1) //如果有一条数据则更新表数据
    {
        QSqlRecord record = saveLimitData->record(0);
        record.setValue("limitVersion",limit_version_r);
        record.setValue("leftLimit",left_limit_r);
        record.setValue("rightLimit",right_limit_r);
        record.setValue("farLimit",far_limit_r);
        record.setValue("nearLmit",near_limit_r);
        record.setValue("highLimit",hight_limit_r);
        record.setValue("weightLimit",weight_limit_r);
        record.setValue("momentLimit",torque_limit_r);
        record.setValue("enableBit",enable_bit_r);
        saveLimitData->setRecord(0,record);

        if (FALSE == saveLimitData->submitAll())
        {
            qDebug()<<"限位信息添加失败";
            return storeSuccess;
        }
        else
            storeSuccess = true;
    }
    sql_mutex.unlock();
    return storeSuccess;
}
/***************************************/
/***********KD值及端口信息解析**********/
/*参数：recv_buff[]为已经去除帧头帧尾，去转义，并且CRC校验后的数据段
*/
/***************************************/
bool EncSpl::SplKD_ip(quint8 * recv_buff)
{
    sql_mutex.lock();
    bool storeSuccess = false;//存数据库成功标志位，作为此函数返回值

    /*******存数据库前的中间数据变量*******/
    quint16 kd_ip_version_r;//信息版本
    double weightK_r;                     //吊重K值
    double weightD_r;                      //吊重D值
    double spanK_r;                            //变幅K值
    double spanD_r;                            //变幅D值
    double heightK_r;                          //高度K值
    double heightD_r;                              //高度D值
    QString serverIP_r;                            //IP
    QString serverPort_r;                          //端口
    QString ZigbeeChannel_r;                      //通信信道
    QString ZigbeeID_r;                                   //网络ID
    double pitchHeight_r;                  //塔吊节高
    double initHeight_r;//塔吊初始高度
    double pitchNum_r;//塔吊节数
    double proHeightEW_r;//保护区高度预紧
    double proHeightW_r;//保护区高度报警

    /************从字节数组中分离字段储存到中间变量中**********************/
    quint32 m=0;
    kd_ip_version_r = (recv_buff[0]<<8) + recv_buff[1];//信息版本，范围0～65535
    m = m + 2;
    quint8 temp[2];
    Array_copy(recv_buff,m,temp,0,2);
    weightK_r = 0.0001*Hex_to_Double(temp);//吊重K值
    m=m+2;
    Array_copy(recv_buff,m,temp,0,2);
    weightD_r = Hex_to_Double(temp);//吊重D值
    m=m+2;
    Array_copy(recv_buff,m,temp,0,2);
    spanK_r = 0.0001*Hex_to_Double(temp);//变幅K值
    m=m+2;
    Array_copy(recv_buff,m,temp,0,2);
    spanD_r = 0.01*Hex_to_Double(temp);//变幅D值
    m=m+2;
    Array_copy(recv_buff,m,temp,0,2);
    heightK_r = 0.0001*Hex_to_Double(temp);//塔臂高度
    m=m+2;
    Array_copy(recv_buff,m,temp,0,2);
    heightD_r = 0.1*Hex_to_Double(temp);//塔臂高度
    m=m+2;

    quint8 serverIP[15];        //IP
    Array_copy(recv_buff,m,serverIP,0,15);
    serverIP_r = Array_to_String(serverIP,15);//转换为字符串
    m=m+15;

    quint8 serverPort[5];//端口
    Array_copy(recv_buff,m,serverPort,0,5);
    serverPort_r = Array_to_String(serverPort,5);//转换为字符串
    m=m+5;

    quint8 ZigbeeChannel[1];//通信信道
    Array_copy(recv_buff,m,ZigbeeChannel,0,1);
    ZigbeeChannel_r = Array_to_String(ZigbeeChannel,1);//转换为字符串
    m=m+1;

    quint8 ZigbeeID[2];//网络ID
    Array_copy(recv_buff,m,ZigbeeID,0,2);
    ZigbeeID_r = Array_to_String(ZigbeeID,2);//转换为字符串
    m=m+2;

    pitchHeight_r = 0.1 * recv_buff[m];//塔吊节高
    m=m+1;
    initHeight_r = 0.1 * recv_buff[m];//塔吊初始高度
    m=m+1;
    pitchNum_r = recv_buff[m];//塔吊节数
    m=m+1;
    proHeightEW_r = 0.1 * recv_buff[m];//保护区高度预紧
    m=m+1;
    proHeightW_r = 0.1 * recv_buff[m];//保护区高度报警
    m=m+1;

/******************将数据存到数据库**********************/
    QSqlTableModel *saveKDipData = new QSqlTableModel();
    saveKDipData->setTable("KdData");//打开数据库表
    saveKDipData->setFilter("id = 0");
    saveKDipData->select();
//用setData的方法依赖于数据库字段的顺序
    if (saveKDipData->rowCount() == 0) {            //表中不存在数据,则新建
        int row = 0 ;
        saveKDipData->insertRows(row,1);
        saveKDipData->setData(saveKDipData->index(row,0),0);//id = 0
        saveKDipData->setData(saveKDipData->index(row,1),weightK_r);
        saveKDipData->setData(saveKDipData->index(row,2),weightD_r);
        saveKDipData->setData(saveKDipData->index(row,3),spanK_r);
        saveKDipData->setData(saveKDipData->index(row,4),spanD_r);
        saveKDipData->setData(saveKDipData->index(row,5),heightK_r);
        saveKDipData->setData(saveKDipData->index(row,6),heightD_r);
        saveKDipData->setData(saveKDipData->index(row,7),serverIP_r);
        saveKDipData->setData(saveKDipData->index(row,8),serverPort_r);
        saveKDipData->setData(saveKDipData->index(row,9),ZigbeeChannel_r);
        saveKDipData->setData(saveKDipData->index(row,10),ZigbeeID_r);
        saveKDipData->setData(saveKDipData->index(row,11),pitchHeight_r);
        saveKDipData->setData(saveKDipData->index(row,12),initHeight_r);
        saveKDipData->setData(saveKDipData->index(row,13),pitchNum_r);
        saveKDipData->setData(saveKDipData->index(row,14),proHeightEW_r);
        saveKDipData->setData(saveKDipData->index(row,15),proHeightW_r);
        saveKDipData->setData(saveKDipData->index(row,16),kd_ip_version_r);

        saveKDipData->submitAll();
        if (FALSE == saveKDipData->submitAll())
        {
            qDebug()<<"KD值和端口信息在数据库添加失败";
            return storeSuccess ;
        }
    }
    else if (saveKDipData->rowCount() == 1) //如果有一条数据则更新表数据
    {
        QSqlRecord record = saveKDipData->record(0);
        record.setValue("weightK",weightK_r);
        record.setValue("weightD",weightD_r);
        record.setValue("spanK",spanK_r);
        record.setValue("spanD",spanD_r);
        record.setValue("heightK",heightK_r);
        record.setValue("heightD",heightD_r);
        record.setValue("serverIP",serverIP_r);
        record.setValue("serverPort",serverPort_r);
        record.setValue("ZigbeeChannel",ZigbeeChannel_r);
        record.setValue("ZigbeeID",ZigbeeID_r);
        record.setValue("pitchHeight",pitchHeight_r);
        record.setValue("initHeight",initHeight_r);
        record.setValue("pitchNum",pitchNum_r);
        record.setValue("proHeightEW",proHeightEW_r);
        record.setValue("proHeightW",proHeightW_r);
        record.setValue("craneKDVersion",kd_ip_version_r);
        saveKDipData->setRecord(0,record);//在第0行保存上面刚设置好的记录

        if (FALSE == saveKDipData->submitAll())
        {
            qDebug()<<"KD值和端口信息在数据库中保存失败";
            return storeSuccess;
        }
        else
            storeSuccess = true;

    }
    sql_mutex.unlock();
    return storeSuccess;

}

/***************************************/
/***********保护区信息解析**********/
/*参数：recv_buff[]为已经去除帧头帧尾，去转义，并且CRC校验后的数据段
             proZone_index为保护区序号
*/
/***************************************/
bool EncSpl::SplProZone(quint8 *recv_buff,quint8 proZone_index)
{
    sql_mutex.lock();
    qDebug()<<"proZone_index = "<<proZone_index;
    //return true;
    /*******存数据库前的中间数据变量*******/
    quint16 proZoneVersion_r;//保护区信息版本
    quint8 proZoneNum_r;//保护区个数
    quint8 proZoneType_r;//保护区类型
    quint8 proZoneIndex_r;//保护区序号
    quint8 proZoneEleNum_r;//保护区元素个数
    QString proZoneName_r;//保护区名称
    quint8 proZoneID;//保护区ID
    quint8 proBuildType_r;//保护区建筑类型
    double proZoneHeight_r;//保护区高度

    quint8 elementType_r;          //元素类型（0x00点，0x01圆弧）
    double pointX_r;                  //X坐标(（2Bytes）-3276.8-3276.8)
    double pointY_r;                  //Y坐标(（2Bytes）-3276.8-3276.8)
    double radius_r;                 //圆半径(（2Bytes）0-6553.5)
    double startAngle_r;              //起始角度(（2Bytes）0-360)
    double endAngle_r;                //终止角度(（2Bytes）0-360)

    quint32 ProZoneDataKeyID;//将保护区数据与元素数据联系起来的关键字，<元素表>中的字段
                                            //proZoneID对应的是<保护区表>的字段id
    bool storeSuccess = false;
    bool proZone_data_isClear = false;
    bool clearProSuc = false;
    bool clearEleSuc = false;
/************从字节数组中分离字段储存到中间变量中**********************/
    quint32 m=0;
    proZoneVersion_r = (recv_buff[0]<<8) + recv_buff[1];//信息版本，范围0～65535
    m = m + 2;
    proZoneNum_r = recv_buff[m];//保护区个数
    m = m + 1;

    proZoneType_r = (recv_buff[m] >> 7);//保护区类型
    proZoneIndex_r = (recv_buff[m] & 0x7F) >> 4;//保护区序号
//    if(proZoneIndex_r < 1)//如果保护区序号为0，则说明数据传送错误
//    {
//        ///qDebug()<<"ProZone doesn't exist index 0!!";
//        return false;
//    }
    proZoneEleNum_r =  (recv_buff[m] & 0x0F) ;//保护区元素个数
    m = m+1;
    quint8 proZoneName[16];
    Array_copy(recv_buff,m,proZoneName,0,16);
    proZoneName_r = Array_to_String(proZoneName,16);//转换为字符串
    m = m+16;
    proZoneID = recv_buff[m];//保护区ID
    m = m+1;
    proBuildType_r = recv_buff[m];//保护区建筑类型
    m = m+1;

    quint8 temp[2];
    Array_copy(recv_buff,m,temp,0,2);
    proZoneHeight_r = 0.1*Hex_to_Double(temp);//保护区高度
    m = m+2;//m=24

/******************将数据存到数据库**********************/
    QSqlTableModel *proZoneData = new QSqlTableModel;

    proZoneData->setTable("ProZoneData");  // 打开数据库中保护区的表
    proZoneData->select();
    QSqlTableModel *element = new QSqlTableModel;
    element->setTable("ElementData");
    element->select();
    if (proZone_index <=1 )
        {
            if(proZoneData->rowCount() == 0)//数据表为空，则不用清
            {
                proZone_data_isClear = true;
            }
            else if ( proZoneData->rowCount() > 0 )//数据表不为空，清除数据库
            {
                proZoneData->setTable("ProZoneData");  // 打开数据库中保护区的表
                proZoneData->select();
                clearProSuc = proZoneData->removeRows(0,proZoneData->rowCount());
                qDebug()<<"~~~~~~~~~~clearProSuc = "<<clearProSuc;

                element->setTable("ElementData");// 打开数据库中元素信息的表
                element->select();
                clearEleSuc = element->removeRows(0,element->rowCount());
                qDebug()<<"~~~~~~~~~~clearEleSuc = "<<clearEleSuc;
            }
        }
        /***清空数据库后则执行插入工作***/
        //如果表中没数据，并且上位机没有下配保护区信息，那么上位机会发出全0的信息帧，则proZone_index=0；如果保护区表和元素信息表中都没数据，直接返回true
        if( (proZone_index == 0) && (proZoneData->rowCount()==0) && (element->rowCount()==0)  )
        {
            return true;
        }
        else if (proZone_index >= 1)
        {
            /**如果下发的是保护1区信息，首先先要保证数据库表中没有数据，再插；
                 或者下发的是proZone_index>1的数据，则可以直接插****/
            //if( (proZone_index == 1) && ( (proZone_data_isClear == true)|| ( (clearProSuc==true)&&(clearEleSuc==true) )  ) )
            if(    ( (proZone_index == 1) && (proZoneData->rowCount()==0) && (element->rowCount()==0) )
                   || (proZone_index > 1)&&(proZoneData->rowCount() == proZone_index - 1)  )
            {
                proZoneData->setTable("ProZoneData");  // 打开数据库中保护区的表
                proZoneData->select();
                int row = 0 ;
                proZoneData->insertRows(row,1);

                ///qDebug()<<"After inserting: proZoneData->rowCount()="<<proZoneData->rowCount();
                proZoneData->setData(proZoneData->index(row, 0), proZone_index-1);
                proZoneData->setData(proZoneData->index(row,1), proZoneVersion_r);
                proZoneData->setData(proZoneData->index(row, 2), proZoneName_r);//
                proZoneData->setData(proZoneData->index(row, 3), proZoneID);
                proZoneData->setData(proZoneData->index(row, 4), proZoneType_r);
                proZoneData->setData(proZoneData->index(row, 5), proZoneIndex_r);
                proZoneData->setData(proZoneData->index(row, 6), proZoneEleNum_r);
                proZoneData->setData(proZoneData->index(row, 7), proBuildType_r);
                proZoneData->setData(proZoneData->index(row, 8), proZoneHeight_r);
                proZoneData->setData(proZoneData->index(row, 9), proZoneNum_r);

                if (false == proZoneData->submitAll())
                {
                ///qDebug()<<"#1#ProZone_"<<proZone_index<<" information failed to stored in database";
                return storeSuccess ;
                }
            }
            //保护区表的字段id为联系元素表的关键字段
            ProZoneDataKeyID = proZoneData->record(proZone_index-1).value("id").toInt();
            element->setTable("ElementData");// 打开数据库中元素信息的表
            element->select();
            //如果下发保护1区信息，并且此时元素信息表中没有数据
            if( (proZone_index==1)&&(element->rowCount()==0))
            {
                //元素信息的字段解析
                for(quint32 j = 0; j < proZoneEleNum_r; j++)
                {
                    elementType_r = recv_buff[m];//元素类型
                    m = m+1;
                    Array_copy(recv_buff,m,temp,0,2);
                    pointX_r= 0.1*Hex_to_Double(temp);//X
                    m = m+2;
                    Array_copy(recv_buff,m,temp,0,2);
                    pointY_r= 0.1*Hex_to_Double(temp);//Y
                    m = m+2;
                    if(elementType_r == 0x01)
                    {
                        Array_copy(recv_buff,m,temp,0,2);
                        radius_r= 0.1*Hex_to_Double(temp);//R
                        m = m+2;
                        Array_copy(recv_buff,m,temp,0,2);
                        startAngle_r= 0.1*Hex_to_Double(temp);//起始角度
                        m = m+2;
                        Array_copy(recv_buff,m,temp,0,2);
                        endAngle_r= 0.1*Hex_to_Double(temp);//终止角度
                        m = m+2;
                    }
                    element->setTable("ElementData");  // 打开数据库中相干塔吊的表
                    element->select();
                    int row = 0 ;
                    element->insertRows(row,1);
                    ///qDebug()<<"#1#current element table rowCount ="<<element->rowCount();
                    element->setData(proZoneData->index(row, 0), element->rowCount() - 1);
                    element->setData(proZoneData->index(row, 1), ProZoneDataKeyID);
                    element->setData(proZoneData->index(row, 2), elementType_r);// 元素类型
                    element->setData(proZoneData->index(row, 3), pointX_r);//X
                    element->setData(proZoneData->index(row, 4), pointY_r);//Y
                    if(elementType_r == 0x00)
                    {
                        element->setData(proZoneData->index(row, 5), 0);//R
                        element->setData(proZoneData->index(row, 6), 0);//起始角度
                        element->setData(proZoneData->index(row, 7), 0);//终止角度

                    }
                    else if(elementType_r == 0x01)
                    {
                        element->setData(proZoneData->index(row, 5), radius_r);//R
                        element->setData(proZoneData->index(row, 6), startAngle_r);//起始角度
                        element->setData(proZoneData->index(row, 7), endAngle_r);//终止角度
                    }

                    //提交
                    //submitElement = element->submitAll();
                    if (false == element->submitAll())
                    {
                        ///qDebug()<<"element_"<<proZone_index<<"information failed to stored in database";
                        return storeSuccess;
                    }

                }
                storeSuccess = true;
            }
            else if( (proZone_index > 1)&&(element->rowCount() > 0))
            {
                element->setTable("ElementData");  // 打开数据库中相干塔吊的表
                element->select();
                //通过所有保护区的元素个数总数是否等于 元素表 的数据行数 来再次判断所有信息是否全部存取
                    quint32 elementNum = 0;
                    for (quint32 i=0; i < proZoneNum_r;i++)
                    {
                        elementNum += proZoneData->record(i).value("proZoneEleNum").toInt();
                    }

                    ///qDebug()<<"elementNum = "<<elementNum;
                    ///qDebug()<<"element->rowCount() = "<<element->rowCount();
                    if (elementNum == element->rowCount())
                    {

                        return true;
                    }
                for(quint32 j = 0; j < proZoneEleNum_r; j++)
                {
                    elementType_r = recv_buff[m];//元素类型
                    m = m+1;
                    Array_copy(recv_buff,m,temp,0,2);
                    pointX_r= 0.1*Hex_to_Double(temp);//X
                    m = m+2;
                    Array_copy(recv_buff,m,temp,0,2);
                    pointY_r= 0.1*Hex_to_Double(temp);//Y
                    m = m+2;
                    if(elementType_r == 0x01)
                    {
                        Array_copy(recv_buff,m,temp,0,2);
                        radius_r= 0.1*Hex_to_Double(temp);//R
                        m = m+2;
                        Array_copy(recv_buff,m,temp,0,2);
                        startAngle_r= 0.1*Hex_to_Double(temp);//起始角度
                        m = m+2;
                        Array_copy(recv_buff,m,temp,0,2);
                        endAngle_r= 0.1*Hex_to_Double(temp);//终止角度
                        m = m+2;
                    }
                    element->setTable("ElementData");  // 打开数据库中相干塔吊的表
                    element->select();
                    int row = 0 ;
                    element->insertRows(row,1);
                    ///qDebug()<<"#2#current element table rowCount ="<<element->rowCount();
                    element->setData(proZoneData->index(row, 0), element->rowCount() - 1);
                    element->setData(proZoneData->index(row, 1), ProZoneDataKeyID);
                    element->setData(proZoneData->index(row, 2), elementType_r);// 元素类型
                    element->setData(proZoneData->index(row, 3), pointX_r);//X
                    element->setData(proZoneData->index(row, 4), pointY_r);//Y
                    if(elementType_r == 0x00)
                    {
                        element->setData(proZoneData->index(row, 5), 0);//R
                        element->setData(proZoneData->index(row, 6), 0);//起始角度
                        element->setData(proZoneData->index(row, 7), 0);//终止角度

                    }
                    else if(elementType_r == 0x01)
                    {
                        element->setData(proZoneData->index(row, 5), radius_r);//R
                        element->setData(proZoneData->index(row, 6), startAngle_r);//起始角度
                        element->setData(proZoneData->index(row, 7), endAngle_r);//终止角度
                    }

                    if (false == element->submitAll())
                    {
                        ///qDebug()<<"element_"<<proZone_index<<"information failed to stored in database";
                        return storeSuccess;
                    }

                }
                storeSuccess = true;
            }
        }
        sql_mutex.unlock();
return storeSuccess;
}

/*****封装报警信息******/
quint32 EncSpl::EncAlarmData(char *alarm_message , char alarm_type, char *detail , char *center_time){
    char event_type[1] = {0x01};
    char type[1] = {alarm_type};
    quint8 detail_len;
    quint32 addr = 0;
    char angle_char[2];
    char span_char[2];
    char height_char[2];
    char weight_char[2];
    char moment_char[1];

    char windspeed_char[2];
    char tileangleX_char[1];
    char tileangleY_char[1];

    switch (alarm_type)
    {
    case 0x01:
        detail_len = 2;//干涉报警
        break;
    case 0x04:
        detail_len = 4;//限位报警
        break;
    default:
        detail_len = 1;//其他报警
        break;
    }
    Double_To_Array(angle_char , 10*globalData->craneList.at(0).angle);
    Double_To_Array(span_char , 10*globalData->craneList.at(0).span);
    Double_To_Array(height_char , 10*globalData->craneList.at(0).craneHeight);
    Double_To_Array(weight_char , 100*globalData->craneList.at(0).wight);
    moment_char[0] = globalData->craneList.at(0).moment;

    Double_To_Array(windspeed_char , 10*globalData->craneList.at(0).windspeed);
    Double_To_Array(tileangleX_char , 10*globalData->craneList.at(0).tiltangleX);
    Double_To_Array(tileangleY_char , 10*globalData->craneList.at(0).tiltangleY);


    addr = globalData->split_joint_hex_data(alarm_message , addr , center_time ,4);
    addr = globalData->split_joint_hex_data(alarm_message , addr , event_type ,1);
    addr = globalData->split_joint_hex_data(alarm_message , addr , type ,1);
    addr = globalData->split_joint_hex_data(alarm_message , addr , detail ,detail_len);
    addr = globalData->split_joint_hex_data2(alarm_message , addr , angle_char ,2);
    addr = globalData->split_joint_hex_data(alarm_message , addr , span_char ,2);
    addr = globalData->split_joint_hex_data2(alarm_message , addr , height_char ,2);
    addr = globalData->split_joint_hex_data(alarm_message , addr , weight_char ,2);
    addr = globalData->split_joint_hex_data(alarm_message , addr , moment_char ,1);
    addr = globalData->split_joint_hex_data(alarm_message , addr , windspeed_char ,2);
    addr = globalData->split_joint_hex_data(alarm_message , addr , tileangleX_char ,1);
    addr = globalData->split_joint_hex_data(alarm_message , addr , tileangleY_char ,1);
    return addr;
}
/*********违章信息封装*************/
quint32 EncSpl::EncIllegalData(char *illegal_message, char illegal_type, char *detail, char *center_time){
    char event_type[1] = {0x02};
    char type[1] = {illegal_type};
    quint32 addr = 0;
    char angle_char[2];
    char span_char[2];
    char height_char[2];
    char weight_char[2];
    char moment_char[1];

    Double_To_Array(angle_char , 10*globalData->craneList.at(0).angle);
    Double_To_Array(span_char , 10*globalData->craneList.at(0).span);
    Double_To_Array(height_char , 10*globalData->craneList.at(0).craneHeight);
    Double_To_Array(weight_char , 100*globalData->craneList.at(0).wight);
    moment_char[0] = globalData->craneList.at(0).moment;

    addr = globalData->split_joint_hex_data(illegal_message , addr , center_time ,4);
    addr = globalData->split_joint_hex_data(illegal_message , addr , event_type ,1);
    addr = globalData->split_joint_hex_data(illegal_message , addr , type ,1);
    addr = globalData->split_joint_hex_data(illegal_message , addr , detail ,1);
    addr = globalData->split_joint_hex_data2(illegal_message , addr , angle_char ,2);
    addr = globalData->split_joint_hex_data(illegal_message , addr , span_char ,2);
    addr = globalData->split_joint_hex_data2(illegal_message , addr , height_char ,2);
    addr = globalData->split_joint_hex_data(illegal_message , addr , weight_char ,2);
    addr = globalData->split_joint_hex_data(illegal_message , addr , moment_char ,1);
    return addr;
}

/******故障信息封装*************/
quint32 EncSpl::EncFaultData(char *fault_message, char fault_type, char *detail, char *center_time){
    char event_type[1] = {0x03};
    char type[1] = {fault_type};
    quint32 addr = 0;
    char angle_char[2];
    char span_char[2];
    char height_char[2];
    char weight_char[2];
    char moment_char[1];
    char windspeed_char[2];
    char tileangleX_char[1];
    char tileangleY_char[1];

    Double_To_Array(angle_char , 10*globalData->craneList.at(0).angle);
    Double_To_Array(span_char , 10*globalData->craneList.at(0).span);
    Double_To_Array(height_char , 10*globalData->craneList.at(0).craneHeight);
    Double_To_Array(weight_char , 100*globalData->craneList.at(0).wight);
    moment_char[0] = globalData->craneList.at(0).moment;

    Double_To_Array(windspeed_char , 10*globalData->craneList.at(0).windspeed);
    Double_To_Array(tileangleX_char , 10*globalData->craneList.at(0).tiltangleX);
    Double_To_Array(tileangleY_char , 10*globalData->craneList.at(0).tiltangleY);

    addr = globalData->split_joint_hex_data(fault_message , addr , center_time ,4);
    addr = globalData->split_joint_hex_data(fault_message , addr , event_type ,1);
    addr = globalData->split_joint_hex_data(fault_message , addr , type ,1);
    addr = globalData->split_joint_hex_data(fault_message , addr ,detail ,1);
    addr = globalData->split_joint_hex_data2(fault_message , addr , angle_char ,2);
    addr = globalData->split_joint_hex_data(fault_message , addr , span_char ,2);
    addr = globalData->split_joint_hex_data2(fault_message , addr , height_char ,2);
    addr = globalData->split_joint_hex_data(fault_message , addr , weight_char ,2);
    addr = globalData->split_joint_hex_data(fault_message , addr , moment_char ,1);
    addr = globalData->split_joint_hex_data(fault_message , addr , windspeed_char ,2);
    addr = globalData->split_joint_hex_data(fault_message , addr , tileangleX_char ,1);
    addr = globalData->split_joint_hex_data(fault_message , addr , tileangleY_char ,1);
    return addr;
}

/***************************************/
/***********塔群信息解析**********/
/*参数：recv_buff[]为已经去除帧头帧尾，去转义，并且CRC校验后的数据段
             group_index为保护区序号
*/
/***************************************/
bool EncSpl::SplCraneGroup(quint8 *recv_buff,quint8 group_index)
{
    sql_mutex.lock();
    bool storeSuccess = false;

    /*******存数据库前的中间数据变量*******/
    quint16 craneGroupVersion_r;//1.信息版本
    //QString craneName;            //塔吊名称（16字节)
                                                 //2.塔群序号group_index
    quint8 groupID_r;                    //3.塔群ID
    quint32 relatedCraneNum_r;      //4.相干塔吊个数
    quint8 craneID_r;                    //5.塔机ID
    QString craneMAC_r;                      //6.塔机IMEI

    double coordinateX_r;             //7.X坐标(-3276.8-3276.8)
    double coordinateY_r;             //8.Y坐标(-3276.8-3276.8)
    double forearmLen_r;             //9.前臂长(0-6553.5)
    double backarmLen_r;            //10.后臂长(0-6553.5)
    double craneHeight_r;          //11.塔臂高度
    double craneTopHeight_r;      // 12.塔顶高度(0-25.6)
    /************从字节数组中分离字段储存到中间变量中**********************/
    quint32 m=0;
    craneGroupVersion_r = (recv_buff[0]<<8) + recv_buff[1];//1.信息版本，范围0～65535
    m = m + 2;
    m = m +1;// 2.塔群序号group_index,已经作为参数被传进来了
    groupID_r = recv_buff[m];//3.塔群ID
    m = m +1;
    relatedCraneNum_r = recv_buff[m];//4.相干塔吊个数
    m = m +1;
    //qDebug()<<"relatedCraneNum_"<<group_index<<relatedCraneNum_r;
    craneID_r = recv_buff[m];//5.塔机ID
    m = m +1;
    quint8 craneIMEI[16];
    Array_copy(recv_buff,m,craneIMEI,0,16);
    craneMAC_r = Array_to_String(craneIMEI,16);//6.塔机IMEI转换为字符串
    m = m + 16;
    //qDebug()<<"craneMAC_"<<group_index<<craneMAC_r;
    quint8 temp[2];
    Array_copy(recv_buff,m,temp,0,2);
    coordinateX_r = 0.1*Hex_to_Double(temp);//7.X坐标
    m = m+2;
    //qDebug()<<"coordinateX_"<<group_index<<coordinateX_r;
    Array_copy(recv_buff,m,temp,0,2);
    coordinateY_r = 0.1*Hex_to_Double(temp);//8.Y坐标
    m = m+2;
    //qDebug()<<"coordinateY_"<<group_index<<coordinateY_r;
    Array_copy(recv_buff,m,temp,0,2);
    forearmLen_r = 0.1*Hex_to_Double(temp);//9.前臂长
    m = m+2;
    qDebug()<<"forearmLen_"<<group_index<<":"<<forearmLen_r;
    Array_copy(recv_buff,m,temp,0,2);
    backarmLen_r = 0.1*Hex_to_Double(temp);//10.后臂长
    m = m+2;
    qDebug()<<"backarmLen_"<<group_index<<":"<<backarmLen_r;
    Array_copy(recv_buff,m,temp,0,2);
    craneHeight_r = 0.1*Hex_to_Double(temp);//11.塔臂高度
    m = m+2;
    Array_copy(recv_buff,m,temp,0,2);
    craneTopHeight_r = 0.1*Hex_to_Double(temp);//12.塔顶高度
    m = m+2;
    /******************将数据存到数据库**********************/
//    QSqlTableModel *BasicData = new QSqlTableModel;
//    BasicData->setTable("BasicData");
//    BasicData->setFilter("id = 1");
//    BasicData->select();
//    bool clearBasicSuc = BasicData->removeRows(0,BasicData);
//    qDebug()<<"!!!!!!!!clearBasicSuc = "<<clearBasicSuc;



    QSqlTableModel *groupData = new QSqlTableModel;
    groupData->setTable("CranesData");  // 打开数据库中相干塔吊的表
    groupData->select();

//    qDebug()<<"rowCount = "<<groupData->rowCount();
//    //qDebug()<<"relatedCraneNum = "<<relatedCraneNum_r;
//    //groupData->clear();//先清除之前的所有数据
//    bool clearSuc = groupData->removeRows(0,groupData->rowCount());
//    qDebug()<<"11111111~~~~~~~~~~clearSuc = "<<clearSuc;
//    groupData->submitAll();

    if(  (groupData->rowCount() == 0)  ||  ((groupData->rowCount() > 0)&&(group_index==1))  )
    {            //表中不存在数据,则新建
                 //如果塔群1数据到来，且表中存在之前的数据则先将之前的数据清除
        groupData->setTable("CranesData");  // 打开数据库中相干塔吊的表
        //groupData->setEditStrategy(QSqlTableModel::OnFieldChange);//表的编辑策略：不进行数据缓存，直接写入数据库
        groupData->select();
        bool clearSuc = groupData->removeRows(0,groupData->rowCount());
        qDebug()<<"~~~~~~~~~~clearSuc = "<<clearSuc;
        bool clearSuccess = groupData->submitAll();//一定要全部

        //groupData->select();
        qDebug()<<"After clearing: groupData->rowCount()="<<groupData->rowCount();


        if ( (clearSuccess== true) && (groupData->rowCount() ==0) )
        {
            groupData->setTable("CranesData");  // 打开数据库中相干塔吊的表
            groupData->select();
            int row = 0 ;
            groupData->insertRows(row,1);

            qDebug()<<"After inserting: groupData->rowCount()="<<groupData->rowCount();
            groupData->setData(groupData->index(row, 0), 0);
            groupData->setData(groupData->index(row,1), 1);
            groupData->setData(groupData->index(row, 2), craneMAC_r);//
            groupData->setData(groupData->index(row, 3), craneID_r);//保存塔机字段
            groupData->setData(groupData->index(row, 4), groupID_r);
            groupData->setData(groupData->index(row, 6), coordinateX_r);
            groupData->setData(groupData->index(row, 7), coordinateY_r);
            groupData->setData(groupData->index(row, 8), craneHeight_r);
            groupData->setData(groupData->index(row, 9), craneTopHeight_r);
            groupData->setData(groupData->index(row, 10), forearmLen_r);
            groupData->setData(groupData->index(row, 11), backarmLen_r);
            groupData->setData(groupData->index(row, 17), relatedCraneNum_r);
            if (FALSE == groupData->submitAll())
            {
                qDebug()<<"#1#Crane Group_"<<group_index<<" information failed to stored in database";
                return storeSuccess ;
            }
            else
                storeSuccess = true;
        }

        //groupData->submitAll();

    }
    /*如果塔群2,3,4....数据到来,则插入。并且可能出现数据库存是没问题，
    但是下位机给上位机上发的回应帧上位机没收到，上位机继续发送这帧数据，
    数据库就会出现多条同样的数据，应避免这种情况*/
    else if(  (groupData->rowCount() > 0) && (group_index > 1)  )
    {

        groupData->setTable("CranesData");  // 打开数据库中相干塔吊的表
        groupData->select();
        quint32 latestID = groupData->record(group_index-2).value("id").toInt();
        quint32 difference = groupData->rowCount() - latestID;
        if(difference == 1)//只有当当前表中数据行数-id=1时，才继续插入，否则表示已经插入成功，返回true
        {
            int row = 0;
            groupData->insertRows(row,1);
            groupData->setData(groupData->index(row, 0), group_index-1);
            groupData->setData(groupData->index(row, 1), group_index);
            groupData->setData(groupData->index(row, 2), craneMAC_r);//
            groupData->setData(groupData->index(row, 3), craneID_r);//保存塔机字段
            groupData->setData(groupData->index(row, 4), groupID_r);
            groupData->setData(groupData->index(row, 6), coordinateX_r);
            groupData->setData(groupData->index(row, 7), coordinateY_r);
            groupData->setData(groupData->index(row, 8), craneHeight_r);
            groupData->setData(groupData->index(row, 9), craneTopHeight_r);
            groupData->setData(groupData->index(row, 10), forearmLen_r);
            groupData->setData(groupData->index(row, 11), backarmLen_r);
            groupData->setData(groupData->index(row, 17), relatedCraneNum_r);
            //groupData->submitAll();

            if (FALSE == groupData->submitAll())
            {
                qDebug()<<"#2#Crane Group_"<<group_index<<" information failed to stored in database";
                sql_mutex.unlock();
                return storeSuccess;
            }
            else
                storeSuccess = true;
        }
        else if (difference > 1){
                sql_mutex.unlock();
                return true;
            }
    }
//    QSqlRecord record = groupData->record(group_index-1);//将数据存到表的第(group_index-1)行，即保护区1的信息存到第0行
//    //groupDataKeyID = groupData->record(group_index-1).value("id").toInt();
//    //record.setValue("proZoneVersion",proZoneVersion_r);// 保存信息版本
//    record.setValue("groupID",groupID_r);//3.塔群ID
//    record.setValue("relatedCraneNum",relatedCraneNum_r);//4.相干塔吊个数
//    record.setValue("craneID",craneID_r);//5.塔机ID
//    record.setValue("craneMAC",craneMAC_r);//6.塔机IMEI
//    record.setValue("coordinateX",coordinateX_r);//7.X坐标
//    record.setValue("coordinateY",coordinateY_r);//8.Y坐标
//    record.setValue("forearmLen",forearmLen_r);//9.前臂长
//    record.setValue("backarmLen",backarmLen_r);//10.后臂长
//    record.setValue("craneHeight",craneHeight_r);//11.塔臂高度
//    record.setValue("craneTopHeight",craneTopHeight_r);//12.塔顶高度
//    groupData->setRecord(group_index-1,record);//group_index-1行保存上面刚设置好的记录

//    //如果表中的数据条数小于相干塔吊个数，则需要在后面插入数据
//    if (rowCount < group_index)
//    {
//        groupData->setTable("CranesData");  // 打开数据库中相干塔吊的表
//        groupData->select();
//        groupData->insertRows(0,1);//在第rowCount+1行插入一行
//        //groupData->setData(groupData->index(group_index-1, 0), group_index);
//        groupData->setData(groupData->index(0, 2), craneMAC_r);//
//        groupData->setData(groupData->index(0, 3), craneID_r);//保存塔机字段
//        groupData->setData(groupData->index(0, 4), groupID_r);
//        groupData->setData(groupData->index(0, 6), coordinateX_r);
//        groupData->setData(groupData->index(0, 7), coordinateY_r);
//        groupData->setData(groupData->index(0, 8), craneHeight_r);
//        groupData->setData(groupData->index(0, 9), craneTopHeight_r);
//        groupData->setData(groupData->index(0, 10), forearmLen_r);
//        groupData->setData(groupData->index(0, 11), backarmLen_r);
//        groupData->setData(groupData->index(0, 17), relatedCraneNum_r);
//        //groupData->submitAll();
//       // groupData->setData(groupData->index(group_index-1, 12), craneID_r);
//        if (FALSE == groupData->submitAll())
//        {
//            qDebug()<<"#2#Crane Group_"<<group_index<<" information failed to stored in database";
//            return storeSuccess;
//        }
//    }
//    //如果表中的数据条数大于相干塔吊个数，则需要将后面的数据删除
//    else if(rowCount > relatedCraneNum_r)
//    {
//        groupData->setTable("CranesData");  // 打开数据库中相干塔吊的表
//        groupData->select();
//        qDebug()<<"kakakakakakakakkakakakakakakakakakakakakaka";
//        //删去从relatedCraneNum_r - 1开始的（rowCount - relatedCraneNum_r）行

//        for(quint32 i = rowCount; i > relatedCraneNum_r; i--)
//        {
//            groupData->removeRows(i - 1, 1);
//        }
//        qDebug()<<"gagagagagagagaggagagagagagagagagagagagagaga";
//        if (FALSE == groupData->submitAll())
//        {
//            qDebug()<<"#3#Crane Group_"<<group_index<<" information failed to stored in database";
//            return storeSuccess;
//        }

//    }

    storeSuccess = true;




/***************调试输出***********************/
//    double coordinateX;
//    coordinateX= groupData->record(group_index-1).value("coordinateX").toDouble();
//    qDebug()<<"coordinateX = "<<coordinateX;
//    double coordinateY = groupData->record(group_index-1).value("coordinateY").toDouble();
//    qDebug()<<"coordinateY = "<<coordinateY;
    double forearmLen = groupData->record(group_index-1).value("forearmLen").toDouble();
    qDebug()<<"forearmLen = "<<forearmLen;
    double backarmLen = groupData->record(group_index-1).value("backarmLen").toDouble();
    qDebug()<<"backarmLen = "<<backarmLen;
/**************************************/
    sql_mutex.unlock();
    return storeSuccess;

}

/***************************************/
/***********去转义**************/
/***************************************/
int EncSpl::delete_fe(char *receive_data, quint32 len)
{
   // int len = len1
    for(quint32 i = 0;i<= len-1;++i)
    {
        if(i<len-1)
        {
            if (receive_data[i] == 0xfe)
            {
                if(receive_data[i+1] == 0xfe)
                {
                    for(quint32 j = i ; j<=len-1;j++)
                    {
                        receive_data[j] = receive_data[j+1];
                    }
                }
                len = len -1;
             }
         }

    }
    return len;
}

///***************************************/
///*******加转义,**********************/
///*参数：info[]包括CRC校验，包括帧头，所以在for循环的循环变量初始值为2，要排除掉帧头
//             send_data[]是在info[]的基础上加来转义，加帧尾的数组，也就是待发送的数据数组
//             len为不包括帧头，包括【协议版本/厂商代码】，【帧类型/信息段长】，【设备代码】
//                 【信息段】，【CRC】的长度
//   返回：len+重复的0xFE个数
//*/
///***************************************/
//quint32 EncSpl::add_fe(quint8 *info, quint8 *send_data, quint32 len)
//{
//    int repeat_num = 0;
//    quint32 addr = 0;
//    //int addr=0;
//    for(quint32 i = 2; i<len;i++)
//        {
//            if(info[i] == 0xFE)//如果遇到0xFE则在其后再增加一个0xFE
//            {
//                repeat_num++;
//                send_data[i+ repeat_num - 1] = info[i];
//                send_data[i+ repeat_num] = 0xFE;
//            }
//            else
//            {
//                send_data[i+ repeat_num] = info[i];
//            }
//        }
//    globalData->split_joint_hex_data(send_data , addr ,  frame_header  , 2);//加帧头
//    //Array_copy(frame_header,0,send_data,0,2);//加帧头
//    addr =globalData->split_joint_hex_data(send_data , len+repeat_num ,  frame_end  , 2);//加帧尾
//    //Array_copy(frame_end,0,send_data,len+2+repeat_num,2);//加帧尾
//    /***测试：打印加0xFE的个数****/
////    qDebug()<<"**add_fe()***repeat_num="<<repeat_num;
////    qDebug()<<"len+repeat_num="<<len+repeat_num;
//    return addr;//返回len+重复的0xFE个数
//}
///********************************************************/
///*********最后的发送格式，向上位机发送各信息，加帧头帧尾，校验**********************/
///*参数：protocol_version为协议版本
//             manufactory_id厂商代码
//             imei为imei号，string类型
//             frame_type帧类型
//             data_len信息段长度
//             info[]为以Enc开头的将各字段打包为16进制字节数组的函数拼接的数组，即信息段数组
//             send_data[]为最后拼接完成，发送出去的数组
//    返回：send_data[]的长度
//*/
///********************************************************/
//quint32 EncSpl::add_frame_CRC_FE(quint8 protocol_version, quint8 manufactory_id, QString imei, quint8* info,
//                     quint32 data_len, quint8* send_data, quint8 frame_type)
//{
//    /**********for test*************/
////    quint32 orig_datalen;
////    //orig_datalen=( sizeof(info) ) / ( sizeof(info[0]) );
////    orig_datalen=( sizeof(info[0]) ) ;
////    qDebug()<<"*****orig_datalen = "<<orig_datalen;
////    for(quint32 i = 0;i < 40; i++)
////    {
////        qDebug()<<hex<<info[i];
////    }
//    quint32 addr = 0;
//    bool imei_flag = false;
//    quint8 return_info[250]={0};
//    //Array_copy(frame_header,0,return_info,0,2);
//    addr = globalData->split_joint_hex_data(return_info , addr ,  frame_header  , 2);//拼接帧头
//    return_info[2]= ((protocol_version<<4)+ manufactory_id) & 0xFF;//将协议版本和厂商代码合成一个字节
//    return_info[3]=(  (frame_type<<3)+data_len/256  ) & 0xFF;
//    return_info[4]= data_len & 0xFF;
//    addr = addr +3;
//    quint8 str_imei[16];
//    imei = globalData->basicData.craneIMEI;//从全局变量获得塔吊的IMEI号
////    /**for test***/
////    qDebug()<<"****imei:"<<globalData->basicData.craneIMEI;
////    qDebug()<<"****imei length"<<globalData->basicData.craneIMEI.size();
//    imei_flag=imei_to_byteArray(imei,str_imei);//将imei号转换为16字节数组
//    addr = globalData->split_joint_hex_data(return_info , addr ,  str_imei  , 16);//将imei字节数组拼接
//    addr = globalData->split_joint_hex_data(return_info , addr ,  info  , data_len);//将信息段数组拼接
//    /****CRC校验，不包括帧头，所以return_info要加2，CRC的低字节在前，高字节在后****/
//    quint8 CRC[2];
//    globalData->dataCRC16((char*)return_info, 21+data_len, CRC);//CRC校验要包括帧头，不包括帧尾
//    /***测试：打印CRC****/
////    qDebug()<<"CRC[0]="<<hex<<CRC[0];
////    qDebug()<<"CRC[1]="<<hex<<CRC[1];
//    addr = globalData->split_joint_hex_data(return_info ,addr ,CRC ,2);//将CRC数组拼接
//    addr = globalData->split_joint_hex_data(return_info ,addr ,frame_end  ,2);//将帧尾拼接

//    quint32 return_len;
//    //return_len包括帧头帧尾的数据段长度，第一个2为帧头，19为3+16，2为CRC数组的长度
//    return_len=add_fe(return_info,send_data,2+19+data_len+2);
//    /***************测试：发送出去的数组****************/
////    qDebug()<<"*****test return_info[]******";
////        for(quint32 i = 0;i < 65; i++)
////        {
////            qDebug()<<i<<":"<<hex<<return_info[i];
////        }
//    /***************测试：发送出去的数组****************/
////    qDebug()<<"*****test send_data[]******";
////        for(quint32 i = 0;i < return_len; i++)
////        {
////            qDebug()<<i<<":"<<hex<<send_data[i];
////        }
////    qDebug()<<"return_len"<<return_len;
//    /**************************************************/
//    return return_len;//最后拼接完成，发送出去的数组的长度
//}

//quint32 EncSpl::add_frame_CRC_FE(quint8 protocol_version, quint8 manufactory_id, QString imei, quint8* info,
//                     quint32 data_len, quint8* send_data, quint8 frame_type, quint32 info_index )
//{
//    /**********for test*************/
////    quint32 orig_datalen;
////    //orig_datalen=( sizeof(info) ) / ( sizeof(info[0]) );
////    orig_datalen=( sizeof(info[0]) ) ;
////    qDebug()<<"*****orig_datalen = "<<orig_datalen;
////    for(quint32 i = 0;i < 40; i++)
////    {
////        qDebug()<<hex<<info[i];
////    }
//    quint32 addr = 0;
//    bool imei_flag = false;
//    quint8 return_info[250]={0};
//    //Array_copy(frame_header,0,return_info,0,2);
//    addr = globalData->split_joint_hex_data(return_info , addr ,  frame_header  , 2);//拼接帧头
//    return_info[2]= ((protocol_version<<4)+ manufactory_id) & 0xFF;//将协议版本和厂商代码合成一个字节
//    return_info[3]=(  (frame_type<<3)+data_len/256  ) & 0xFF;
//    return_info[4]= data_len & 0xFF;
//    addr = addr +3;
//    quint8 str_imei[16];
//    imei = globalData->basicData.craneIMEI;//从全局变量获得塔吊的IMEI号
////    /**for test***/
////    qDebug()<<"****imei:"<<globalData->basicData.craneIMEI;
////    qDebug()<<"****imei length"<<globalData->basicData.craneIMEI.size();
//    imei_flag=imei_to_byteArray(imei,str_imei);//将imei号转换为16字节数组
//    addr = globalData->split_joint_hex_data(return_info , addr ,  str_imei  , 16);//将imei字节数组拼接
//    addr = globalData->split_joint_hex_data(return_info , addr ,  info  , data_len);//将信息段数组拼接
//    quint8 CRC[2];
//    qDebug()<<"info_index ="<<info_index;
//    if (info_index > 0)//如果信息段数据是保护区或塔群数据，则涉及到发送多条数据,在数据段后应该再加信息段序号
//    {
//        quint8 info_index[1];
//        addr = globalData->split_joint_hex_data(return_info , addr ,  info_index  , 1);

//        /****CRC校验，不包括帧头，所以return_info要加2，CRC的低字节在前，高字节在后****/

//        globalData->dataCRC16(return_info, 22+data_len, CRC);//CRC校验要包括帧头，不包括帧尾

//    }
//    /****CRC校验，不包括帧头，所以return_info要加2，CRC的低字节在前，高字节在后****/

//    globalData->dataCRC16(return_info, 21+data_len, CRC);//CRC校验要包括帧头，不包括帧尾
//    /***测试：打印CRC****/
////    qDebug()<<"CRC[0]="<<hex<<CRC[0];
////    qDebug()<<"CRC[1]="<<hex<<CRC[1];
//    addr = globalData->split_joint_hex_data(return_info ,addr ,CRC ,2);//将CRC数组拼接
//    addr = globalData->split_joint_hex_data(return_info ,addr ,frame_end  ,2);//将帧尾拼接

//    quint32 return_len;
//    if (info_index > 0)
//    {
//        //return_len包括帧头帧尾的数据段长度，第一个2为帧头，19为3+16，2为CRC数组的长度
//        return_len=add_fe(return_info,send_data,2+19+data_len+1+2);
//    }
//    //return_len包括帧头帧尾的数据段长度，第一个2为帧头，19为3+16，2为CRC数组的长度
//    return_len=add_fe(return_info,send_data,2+19+data_len+2);
//    /***************测试：发送出去的数组****************/
////    qDebug()<<"*****test return_info[]******";
////        for(quint32 i = 0;i < 65; i++)
////        {
////            qDebug()<<i<<":"<<hex<<return_info[i];
////        }
//    /***************测试：发送出去的数组****************/
////    qDebug()<<"*****test send_data[]******";
////        for(quint32 i = 0;i < return_len; i++)
////        {
////            qDebug()<<i<<":"<<hex<<send_data[i];
////        }
////    qDebug()<<"return_len"<<return_len;
//    /**************************************************/
//    return return_len;//最后拼接完成，发送出去的数组的长度
//}

///********************************************************/
///*********imei号码转换为十六进制**********************/
///*参数：crane_imei字符串形式的imei号
//            str_imei字节数组形式的imei
//*/
///********************************************************/
//bool EncSpl::imei_to_byteArray(QString crane_imei, quint8 *str_imei)
//{
//    QByteArray imei_Qbyte;
//    quint32 len_imei ;
//    imei_Qbyte = crane_imei.toAscii();
//    len_imei= crane_imei.size();
//    //qDebug()<<"len_imei"<<len_imei;
//    if(crane_imei.length() != 15)
//    {
//        qDebug()<<"Something is wrong with the length of imei number!";
//        return false;
//    }

//    for (quint32 i = 0;i<15;i++)
//    {
//        str_imei[i] = imei_Qbyte.data()[i];
//        //qDebug()<<"str_imei["<<i<<"]"<<hex<<str_imei[i];//测试打印本机imei字段
//    }
//    str_imei[15] = 0x00;// 最后一位补0
//    return true;

//}

