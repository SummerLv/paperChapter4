#include"dataEncapsulateSpliteAPI.h"

//GlobalData *data = GlobalData::getGlobalData();
int addDataToFrame(quint8 *send_data)
{
    /***********************************
    //                   拼接数据
    //塔群识别码（6bits）+塔机识别码（6bits）+厂商（3bits) +
       回转（12bits）+转速（7bits）+变幅（11bits）+起升（13bits）
    */
    /**/
    GlobalData *data = GlobalData::getGlobalData();
    const quint16 frame_header[2] = {0xFE, 0xFB};    //帧头
    const quint16 frame_end[2] = {0xFE, 0xFA};        //帧尾

    quint8 checkCRC[2];
    //quint8 send_data[30] = {0};                       //最后从串口发送的数据
    quint8 wirelessM[15] = {0};                    //拼接的字节数组

    quint8 groupID = 0x01;                          //塔群ID
    quint8 craneID = 0x02;                          //塔机ID
    quint8 firmID = 0x01;                            //厂商号

    quint16 angle;                                       //回转
    quint16 rotateSpeed;                              //转速
    quint16 span;                                        //变幅
    qint16 height;                                      //起升

    /*测试用数据*/
    double angle_DOUBLE = 120.0;               //测试用数据，回转
    double rotateSpeed_DOUBLE = 0.0;         //测试用数据，转速
    double span_DOUBLE = 20.2;                 //测试用数据，变幅
    double height_DOUBLE = -49.9;               //测试用数据，起升
    /*将double型的回转，转速，变幅，起升4个变量强制转换为无符号16位整型*/
    angle = (quint16)(angle_DOUBLE/0.08789);
    rotateSpeed = (quint16)(rotateSpeed_DOUBLE*10);
    span = (quint16)(span_DOUBLE*10);
    height = (qint16)(height_DOUBLE*10);          //先将double型的height_DOUBLE转换为无符号的
    qDebug()<<"height"<<height;
    if (height < 0)                                              //正数直接用，负数则做补码运算
    {
        height = abs(height);
        qDebug()<<"height"<<height;
        height = ((~height) & 0xFFF);                      //再做按位取反运算，并且屏蔽掉13位以上的每一bit
        height = (height | 0x1000) + 1;                   //置符号位，并+1得到补码
    }


    /*按照协议 塔机终端间的通信协议 开始拼接*/
   // memcpy(wirelessM, frame_header, 2);          //帧头
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
    data->dataCRC16((char*)wirelessM + 2, 8, checkCRC);
    wirelessM[10] = checkCRC[0];                         //CRC的低字节
    wirelessM[11] = checkCRC[1];                         //CRC的高字节
    //qDebug()<< "checkCRC"<<hex<<checkCRC[0]<<checkCRC[1];
   // memcpy(wirelessM + 12, frame_end, 2);       //加帧尾
 //////////////////////////////////////////////////////////////////////////////////////////
/****按照协议要求 在从帧头到CRC校验码共12个字节长的信息段中 如果有 FE的情况 则插入一个FE****/
//////////////////////////////////////////////////////////////////////////////////////////
    quint8 repeat_num = 0;                                  //0xFE重复的次数
    quint8 info_size = 12;                                    //信息总长度，包括帧头和CRC

    send_data[0] = wirelessM[0];                         //添加帧头
    send_data[1] = wirelessM[1];


    for (quint8 i = 2; i < info_size; i++)
    {
        if(wirelessM[i] == 0xFE)
        {
            repeat_num++;
            send_data[i + repeat_num-1 ] = 0xFE;
        }
        send_data[ i + repeat_num ] = wirelessM[i];
    }
    send_data[ info_size + repeat_num] = frame_end[0];
    send_data[ info_size + repeat_num + 1] = frame_end[1];

    int send_data_len = info_size + repeat_num +2;



    //wirelessM[12] = frame_end[0];
    //wirelessM[13] = frame_end[1];
//    qDebug()<<"wirelessM[]"<<hex<<wirelessM[0]<<wirelessM[1]<<wirelessM[2]<<wirelessM[3]
//                                                <<wirelessM[4]<<wirelessM[5]<<wirelessM[6]<<wirelessM[7]
//                                                <<wirelessM[8]<<wirelessM[9]<<wirelessM[10]<<wirelessM[11]
//                                                <<wirelessM[12]<<wirelessM[13];
     qDebug()<<"send_data[]";
    for(int i=0; i< send_data_len; i++)
    {
        qDebug()<<hex<<send_data[i];
    }
    qDebug()<<"send_data_len"<<send_data_len;
    return send_data_len;
}



void splitDataframe(quint8 *recv_buff)
{
    quint8 recvLen = 14;                                     //塔机间通讯接受到的14个字节
    /******************从接受的数据帧分拆得到的中间整型变量********************/
    quint8 groupID_r;                                     //塔群ID
    quint8 craneID_r;                                     //塔机ID
    quint8 firmID_r;                                       //厂商号

    quint16 angle_r;                                       //回转
    quint16 rotateSpeed_r;                              //转速
    quint16 span_r;                                        //变幅
    qint16 height_r;                                      //起升
    /******************将后面4个整型变量转换为double型变量********************/
    double angle_DOUBLE_r;                                       //回转
    double rotateSpeed_DOUBLE_r;                              //转速
    double span_DOUBLE_r;                                        //变幅
    double height_DOUBLE_r;                                      //起升
    /**************************************************************/
    quint8 CRCdataLen= recvLen - 6;                   //校验帧头到CRC出现之前的数据段，不算帧头帧尾及CRC
    //测试用数据，即为上面测试数据正确打包后的发送
    //quint8 recv_buff[14] = {0XFE, 0XFB, 0X81, 0X10, 0X58, 0X02, 0X28, 0XE6, 0XCD, 0X02, 0XD1, 0XB8, 0XFE, 0XFA};
    quint8 recvCRC[2] = {0};
    GlobalData *data = GlobalData::getGlobalData();
    data->dataCRC16((char*)recv_buff + 2, 8, recvCRC); //对数据段进行CRC校验并得到CRC校验码
    //判断CRC校验码是否正确，正确才进行下一步的数据段解析，并得到各个数据信息
    if ( (recv_buff[CRCdataLen+2] == recvCRC[0]) && (recv_buff[CRCdataLen+3] == recvCRC[1]))
    {
        groupID_r = recv_buff[2] & 0x3F;
        craneID_r = ( (recv_buff[2] >> 6) & 0x03 ) + ( (recv_buff[3] << 2) & 0x3C );
        firmID_r = (recv_buff[3] >> 4) & 0x07;

        angle_r = (( (quint16)recv_buff[3] >> 7 ) & 0x0001 ) + (( (quint16)recv_buff[4] << 1 ) & 0x01FE )
                   + (( (quint16)recv_buff[5] << 9 ) & 0x0E00 );
        rotateSpeed_r = (( (quint16)recv_buff[5] >> 3 ) & 0x001F ) + (( (quint16)recv_buff[6] << 5 ) & 0x0060 );
        span_r = (( (quint16)recv_buff[6] >> 2 ) & 0x003F ) + (( (quint16)recv_buff[7] << 6 ) & 0x07C0 );
        height_r = (( (quint16)recv_buff[7] >> 5 ) & 0x0007 ) + (( (quint16)recv_buff[8] << 3 ) & 0x07F8 )
                    + (( (quint16)recv_buff[9] << 11 ) & 0x1800 );
        if( (height_r & 0x1000) == 0x1000 )                                          //最高位为1，即为负数
        {
            height_r =~ (height_r - 1);                                                    //补码逆运算
            height_r = (height_r & 0xFFF);                                               //屏蔽掉13位以上符号位及无效位
            height_r = 0 - height_r;                                                        //取反，正数变成负数
        }

        angle_DOUBLE_r = (double)angle_r * 0.08789;
        rotateSpeed_DOUBLE_r = (double)rotateSpeed_r / 10;
        span_DOUBLE_r = (double)span_r / 10;
        height_DOUBLE_r = (double)height_r / 10;

        qDebug()<< "groupID"<<groupID_r;
        qDebug()<< "craneID"<<craneID_r;
        qDebug()<< "firmID"<<firmID_r;
        qDebug()<< "angle"<<angle_DOUBLE_r;
        qDebug()<< "rotateSpeed"<<rotateSpeed_DOUBLE_r;
        qDebug()<< "span"<<span_DOUBLE_r;
        qDebug()<< "height"<<height_DOUBLE_r;
    }
}

//int find_fe_fb()
