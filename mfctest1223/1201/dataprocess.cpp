#include "dataprocess.h"
#define BUFFER_SIZE 200
//  didn't  use
//#define DATA_TRANSFER_COM 1
#define DATA_LEN 8
//#define TABLE_DATA_LEN 50

#define SAFE_GAP_LOW  8
#define SAFE_GAP_WARN 6
#define SAFE_GAP_ALARM 3

#define SAFE_COLOR (Qt::black)          //安全颜色
#define WARN_COLOR (Qt::magenta)      //预警颜色
#define ALARM_COLOR (Qt::red)               //报警颜色




#define Pzone_alarm_color (Qt::red)
#define Pzone_safe_color (Qt::black)
#define Pzone_warning_color (Qt::magenta)

quint8 e_moment_limit_time = 0;//力矩限位判断计时
quint8 moment_limit_time = 0;//力矩限位判断计时
//quint32 table_span[TABLE_DATA_LEN] = {0};
//quint32 table_weight[TABLE_DATA_LEN] = {0};
const int TABLE_DATA_LEN = 50;

DataProcess::DataProcess(QObject *parent) :
    QThread(parent)
{
    globalData = GlobalData::getGlobalData();
    encSplData = new EncSpl();
    tableLen = 0;

    readTableFlag = false;
    readMomentData();

    connect(this,SIGNAL(paintSignal()),parent,SLOT(timer1UpDate()),Qt::AutoConnection);//定时器1做图像更新

}
/* 本机塔臂比对方高（情况一）判断*/
void DataProcess::localHigherJudge(quint32 i)
{
    double Ax,Ay;                   //本机后端点坐标
    double Bx,By;                   //本机前端点坐标
    double Cx,Cy;                   //相邻机后端点坐标
    double Dx,Dy;                   //相邻机前端点坐标

    qint32 angle0;
    double distanceBodyToArm;           //本机塔身到对方塔臂的距离
    double distanceCarToArm;            //本机小车到对方塔臂的最短距离

    Ax = globalData->craneList.at(0).backarmPointX;
    Ay = globalData->craneList.at(0).backarmPointY;             //中心塔吊后端坐标
    Bx = globalData->craneList.at(0).forearmPointX;
    By = globalData->craneList.at(0).forearmPointY;                 //中心塔吊前段坐标

    Cx = globalData->craneList.at(i).backarmPointX;
    Cy = globalData->craneList.at(i).backarmPointY;                                      //相邻机后端点坐标
    Dx = globalData->craneList.at(i).forearmPointX;
    Dy = globalData->craneList.at(i).forearmPointY;                                     //相邻机前端点坐标

    double hookX,hookY,hookZ;
    double shortest;

    //吊钩所处三维坐标


    angle0 = globalData->craneList.at(0).angle*TVECT;
    quint8 condition;
    if (globalData->craneList.at(0).hookHeight > globalData->craneList.at(i).topEnvelope )
        condition = 0;             //本机吊钩高于对方塔臂包络
    else
        condition = 1;              //本机吊钩不高于对方塔臂包络

    switch(condition)
    {
    case 0:                                          //本机吊钩高于对方塔臂包络
        //-----------------------低碰撞判断----------------------------------------
        if((globalData->craneList.at(0).hookHeight - globalData->craneList.at(i).topEnvelope) <= SAFE_GAP_LOW) {  //高度差低于安全值时候进一步判断是否危险
            double distanceHook,Cz,Dz;
            hookX = globalData->craneList.at(0).coordinateX + globalData->craneList.at(0).span*cos_tab[angle0];
            hookY = globalData->craneList.at(0).coordinateY + globalData->craneList.at(0).span*sin_tab[angle0];
            hookZ = globalData->craneList.at(0).hookHeight;
            Cz = Dz = globalData->craneList.at(i).topEnvelope;
            //计算吊钩到对方塔臂的最短距离distanceHook
            distanceHook = distancePointToLine(Cx,Cy,Cz,Dx,Dy,Dz,hookX,hookY,hookZ);

            if(distanceHook < SAFE_GAP_LOW*0.80)  {              //小于报警距离
                globalData->craneList[0].lowCollision = 2;
                globalData->craneList[i].paintColor = ALARM_COLOR;
                break;
            }else if (distanceHook < SAFE_GAP_LOW) {                //小于预警距离，大于报警距离
                globalData->craneList[0].lowCollision = 1;
                globalData->craneList[i].paintColor = WARN_COLOR;
               break;
            }else{                                                                      //安全
                globalData->craneList[0].lowCollision = 0;
                globalData->craneList[i].paintColor = SAFE_COLOR;
                break;
            }
        }
//--------------------------对方干涉本机塔身判断-----------------------------------
        distanceBodyToArm = distancePointToLine(Cx,Cy,0,Dx,Dy,0,globalData->craneList.at(0).coordinateX,globalData->craneList.at(0).coordinateY,0);
        if (distanceBodyToArm > SAFE_GAP_WARN) {    //大于预警值
            globalData->craneList[0].bodyCollision = 0 ;
            globalData->craneList[i].paintColor = SAFE_COLOR;

        }else if (distanceBodyToArm < SAFE_GAP_ALARM) {     //小于报警值
            globalData->craneList[0].bodyCollision = 2 ;
            globalData->craneList[i].paintColor = ALARM_COLOR;

        }else{
            globalData->craneList[0].bodyCollision = 1 ;             //预警值与报警值之间
            globalData->craneList[i].paintColor = WARN_COLOR;

        }
        break;
    case 1:                                     //本机吊钩不高于对方塔臂包络

        hookX = globalData->craneList.at(0).coordinateX + globalData->craneList.at(0).span*cos_tab[angle0];
        hookY = globalData->craneList.at(0).coordinateY + globalData->craneList.at(0).span*sin_tab[angle0];
        shortest = minDistanceCalculate(Ax,Ay,Bx,By,Cx,Cy,Dx,Dy);  //计算是否交叉

//--------------------------------------------------------塔臂交叉----------------------------
        if (shortest <= 0.00001) {
            //---------同侧--------------------------------（小车与塔身在同侧）,远碰撞
            if (pointOnRightOrLeft(Cx,Cy,Dx,Dy,hookX,hookY) *
                    pointOnRightOrLeft(Cx,Cy,Dx,Dy,globalData->craneList.at(0).coordinateX,globalData->craneList.at(0).coordinateY) > 0) {

                distanceCarToArm = distancePointToLine(Cx,Cy,0,Dx,Dy,0,hookX,hookY,0);      //本机小车到对方塔臂的最短距离
                if (distanceCarToArm > SAFE_GAP_WARN) {                   //大于预警值，安全
                    globalData->craneList[0].farCollision = 0;
                    globalData->craneList[0].nearCollision = 0 ;
                    globalData->craneList[0].bodyCollision = 0;
                    globalData->craneList[i].paintColor = SAFE_COLOR;
                }else if (distanceCarToArm < SAFE_GAP_ALARM) {        //小于报警距离，报警
                    globalData->craneList[0].farCollision = 2;
                    globalData->craneList[0].nearCollision = 0;
                    globalData->craneList[0].bodyCollision = 0;
                    globalData->craneList[i].paintColor = ALARM_COLOR;
                }else{                                                                         //预警状态，预警
                    globalData->craneList[0].farCollision = 2;
                    globalData->craneList[0].nearCollision = 0;
                    globalData->craneList[0].bodyCollision = 0;
                    globalData->craneList[i].paintColor = WARN_COLOR;
                }
           //------------不同侧------------------------（小车与塔身在直线不同侧）
            }else{
                distanceCarToArm = distancePointToLine(Cx,Cy,0,Dx,Dy,0,hookX,hookY,0);
                distanceBodyToArm = distancePointToLine(Cx,Cy,0,Dx,Dy,0,globalData->craneList.at(0).coordinateX,globalData->craneList.at(0).coordinateY,0);
                //小车到CD的距离比塔身到CD的距离近，近碰撞
                if (distanceCarToArm < distanceBodyToArm) {
                    if (distanceCarToArm > SAFE_GAP_WARN) {             //大于预警值，安全
                        globalData->craneList[0].farCollision = 0;
                        globalData->craneList[0].nearCollision = 0 ;
                        globalData->craneList[0].bodyCollision = 0;
                        globalData->craneList[i].paintColor = SAFE_COLOR;
                    }else if (distanceCarToArm < SAFE_GAP_ALARM) {          //小于报警距离，报警
                        globalData->craneList[0].farCollision = 0;
                        globalData->craneList[0].nearCollision = 2 ;
                        globalData->craneList[0].bodyCollision = 0;
                        globalData->craneList[i].paintColor = ALARM_COLOR;
                    }else{
                        globalData->craneList[0].farCollision = 0;                  //预警状态，预警
                        globalData->craneList[0].nearCollision = 1 ;
                        globalData->craneList[0].bodyCollision = 0;
                        globalData->craneList[i].paintColor = WARN_COLOR;
                    }

                }else{
                 //小车到CD的距离比塔身到CD的距离远，塔身碰撞
                    if (distanceBodyToArm > SAFE_GAP_WARN) {             //大于预警值，安全
                        globalData->craneList[0].farCollision = 0;
                        globalData->craneList[0].nearCollision = 0 ;
                        globalData->craneList[0].bodyCollision = 0;
                        globalData->craneList[i].paintColor = SAFE_COLOR;
                    }else if (distanceBodyToArm < SAFE_GAP_ALARM) {          //小于报警距离，报警
                        globalData->craneList[0].farCollision = 0;
                        globalData->craneList[0].nearCollision = 0 ;
                        globalData->craneList[0].bodyCollision = 2;
                        globalData->craneList[i].paintColor = ALARM_COLOR;
                    }else{
                        globalData->craneList[0].farCollision = 0;                  //预警状态，预警
                        globalData->craneList[0].nearCollision = 0 ;
                        globalData->craneList[0].bodyCollision = 1;
                        globalData->craneList[i].paintColor = WARN_COLOR;
                    }

                }

            }
//-----------------------------塔臂不交叉----------------------------
        }else{
            distanceCarToArm = distancePointToLine(Cx,Cy,0,Dx,Dy,0,hookX,hookY,0);
            distanceBodyToArm = distancePointToLine(Cx,Cy,0,Dx,Dy,0,globalData->craneList.at(0).coordinateX,globalData->craneList.at(0).coordinateY,0);
            //小车到CD的距离比塔身到CD距离短,远碰撞
            if (distanceCarToArm < distanceBodyToArm) {
                if (distanceCarToArm > SAFE_GAP_WARN) {             //大于预警值，安全
                    globalData->craneList[0].farCollision = 0;
                    globalData->craneList[0].nearCollision = 0 ;
                    globalData->craneList[0].bodyCollision = 0;
                    globalData->craneList[i].paintColor = SAFE_COLOR;
                }else if (distanceCarToArm < SAFE_GAP_ALARM) {          //小于报警距离，报警
                    globalData->craneList[0].farCollision = 2;
                    globalData->craneList[0].nearCollision = 0 ;
                    globalData->craneList[0].bodyCollision = 0;
                    globalData->craneList[i].paintColor = ALARM_COLOR;
                }else{
                    globalData->craneList[0].farCollision = 1;                  //预警状态，预警
                    globalData->craneList[0].nearCollision = 0 ;
                    globalData->craneList[0].bodyCollision = 0;
                    globalData->craneList[i].paintColor = WARN_COLOR;
                }
            //小车到CD的距离比塔身到CD距离长,塔身碰撞
            }else{
                if (distanceBodyToArm > SAFE_GAP_WARN) {             //大于预警值，安全
                    globalData->craneList[0].farCollision = 0;
                    globalData->craneList[0].nearCollision = 0 ;
                    globalData->craneList[0].bodyCollision = 0;
                    globalData->craneList[i].paintColor = SAFE_COLOR;
                }else if (distanceBodyToArm < SAFE_GAP_ALARM) {          //小于报警距离，报警
                    globalData->craneList[0].farCollision = 0;
                    globalData->craneList[0].nearCollision = 0 ;
                    globalData->craneList[0].bodyCollision = 2;
                    globalData->craneList[i].paintColor = ALARM_COLOR;
                }else{
                    globalData->craneList[0].farCollision = 0;                  //预警状态，预警
                    globalData->craneList[0].nearCollision = 0 ;
                    globalData->craneList[0].bodyCollision = 1;
                    globalData->craneList[i].paintColor = WARN_COLOR;
                }
            }
        }
        break;
    default:
        break;
    }
}
/*本机塔臂比对方低*/
void DataProcess::neighborHigherJudge(quint32 i)
{
    double Ax,Ay;
    double Bx,By;
    double CTx,CTy;
    long temp;
    double tmpDistanceX;
    double tmpDistanceY;
    qint32 angleX;
    quint8 condition;
    double distanceArmToBody;           //本机塔臂到对方塔身的最短距离
    double distanceArmToCar;            //本机塔臂到对方小车的最短距离
    double tmp1 = 0,tmp2 = 0,tmp3 = 0;                      //计算两点距离临时值
    quint8 endNum = 0;                                              //判断点到直线距离最近点的标志位
    double side;                                //用于判断某点在直线的左侧还是右侧
    Ax = globalData->craneList.at(0).backarmPointX;
    Ay = globalData->craneList.at(0).backarmPointY;             //中心塔臂后端坐标
    Bx = globalData->craneList.at(0).forearmPointX;
    By = globalData->craneList.at(0).forearmPointY;             //中心塔臂前端坐标

    CTx = globalData->craneList[i].coordinateX;
    CTy = globalData->craneList[i].coordinateY;

    //清除报警标志，进行下一次判断
    globalData->craneList[0].leftCollision = 0;
    globalData->craneList[0].rightCollision = 0;
    globalData->craneList[0].tailCollision = 0;
    globalData->craneList[i].paintColor = SAFE_COLOR;

    if (globalData->craneList[i].hookHeight > globalData->craneList.at(0).topEnvelope)
        condition = 0;                                                      //对方吊钩高于本机塔臂包络顶边
    else
        condition = 1;                                                      //对方吊钩不高于本机塔臂包络顶边

    switch (condition) {
    case 0:                     //对方吊钩高于本机塔臂包络顶边，此时只有可能本机吊杆与对方塔身干涉
        distanceArmToBody = distancePointToLine(Ax,Ay,0,Bx,By,0,CTx,CTy,0);
        //判断线外点到A距离最短还是到B距离最短
        tmp1 = sqrt((pow((Bx - CTx),2) + pow((By - CTy),2)));
        tmp2 = sqrt((pow((Ax - CTx),2) + pow((Ay - CTy),2)));
        tmp3 = sqrt((pow((Ax - Bx),2) + pow((Ay - By),2)));
        if (tmp1*tmp1 >= tmp2*tmp2 + tmp3*tmp3)
            endNum = 1;                 //表明AB线外的点到B（末点）的距离最短
        if (tmp2*tmp2 >= tmp1*tmp1+tmp3*tmp3)
            endNum = 2;                 //表明AB线外的点到A（起点）的距离最短

        if (distanceArmToBody > SAFE_GAP_WARN) {      //大于预警值
            globalData->craneList[0].leftCollision = 0;
            globalData->craneList[0].rightCollision = 0;
            globalData->craneList[0].tailCollision = 0;
            globalData->craneList[i].paintColor = SAFE_COLOR;

        }else if(distanceArmToBody < SAFE_GAP_ALARM) {          //小于报警值
            if(1 == endNum) {         //A点（塔臂尾）到C最近
                globalData->craneList[0].tailCollision = 2;
            }else{
                side = pointOnRightOrLeft(Ax,Ay,Bx,By,CTx,CTy);         //判断C（对方圆心）在AB的左右
                temp = (int)side*10;
                side = temp/10;
                if (side < 0){                                                //C在AB右侧
                    globalData->craneList[0].rightCollision = 2;
                    globalData->craneList[0].leftCollision = 0;
                }else{                                                           //C在AB左侧
                    globalData->craneList[0].rightCollision = 0;
                    globalData->craneList[0].leftCollision = 2;
                }
                globalData->craneList[i].paintColor = ALARM_COLOR;
            }

        }else if(distanceArmToBody < SAFE_GAP_WARN) {               //小于预警值
            if (endNum == 1) {                                                         //A(塔臂尾)到C最近
                globalData->craneList[0].tailCollision = 1;
            }else{
                side = pointOnRightOrLeft(Ax,Ay,Bx,By,CTx,CTy);
                temp = (int)side*10;
                side = temp/10;
                if(side <0){
                    globalData->craneList[0].rightCollision = 1;
                    globalData->craneList[0].leftCollision = 0;
                }else{
                    globalData->craneList[0].rightCollision = 0;
                    globalData->craneList[0].leftCollision = 1;
                }
            }
            globalData->craneList[i].paintColor = WARN_COLOR;
        }
        break;
    case 1:             //对方吊钩不高于本机塔臂包络顶边
        distanceArmToBody = distancePointToLine(Ax,Ay,0,Bx,By,0,CTx,CTy,0);//本机塔臂AB到对方塔身的最短距离
        //本机塔臂AB到对方小车的距离
        angleX = globalData->craneList.at(i).angle * TVECT;
        tmpDistanceX = CTx+globalData->craneList[i].span*cos_tab[angleX];
        tmpDistanceY = CTy+globalData->craneList[i].span*sin_tab[angleX];
        distanceArmToCar = distancePointToLine(Ax,Ay,0,Bx,By,0,tmpDistanceX,tmpDistanceY,0);
        //-----------------------到对方钢绳的距离小于到塔身的距离-----------------------------------------
        if (distanceArmToCar < distanceArmToBody) {
            endNum = 0;
            tmp1 = sqrt((pow((Bx - tmpDistanceX),2) + pow((By - tmpDistanceY),2)));//判断线外点到A距离最短还是到B距离最短
            tmp2 = sqrt((pow((Ax - tmpDistanceX),2) + pow((Ay - tmpDistanceY),2)));
            tmp3 = sqrt((pow((Ax - Bx),2) + pow((Ay - By),2)));
            if (tmp1*tmp1 >= tmp2*tmp2 + tmp3*tmp3)
                endNum = 1;                 //表明AB线外的点到B（末点）的距离最短
            if (tmp2*tmp2 >= tmp1*tmp1+tmp3*tmp3)
                endNum = 2;                 //表明AB线外的点到A（起点）的距离最短
            side = pointOnRightOrLeft(Ax,Ay,Bx,By,tmpDistanceX,tmpDistanceY);
            temp = (int)side*10;
            side = temp/10;
            if (distanceArmToCar > SAFE_GAP_WARN) {                     //大于预警值
                globalData->craneList[0].leftCollision = 0;
                globalData->craneList[0].rightCollision = 0;
                globalData->craneList[i].paintColor = SAFE_COLOR;
            }else if(distanceArmToCar < SAFE_GAP_ALARM) {           //小于报警值
                if(endNum == 1){                //A（塔臂尾）到C最近
                    globalData->craneList[0].tailCollision = 2;
                }else{
                    if (side < 0){
                        globalData->craneList[0].rightCollision = 2;
                        globalData->craneList[0].leftCollision = 0;
                    }else{
                        globalData->craneList[0].rightCollision = 0;
                        globalData->craneList[0].leftCollision = 2;
                    }
                }
                globalData->craneList[i].paintColor = ALARM_COLOR;
            }else if(distanceArmToCar < SAFE_GAP_WARN) {            //小于预警值
                if (endNum == 1) {       //A(塔臂尾)到C距离最近
                    globalData->craneList[0].tailCollision = 1;
                }else{
                    if (side < 0) {
                        globalData->craneList[0].rightCollision = 1;
                        globalData->craneList[0].leftCollision = 0;
                    }else{
                        globalData->craneList[0].rightCollision = 0;
                        globalData->craneList[0].leftCollision = 1;
                    }
                }
                globalData->craneList[i].paintColor = WARN_COLOR;
            }
        //------------------到对方塔身的距离小于到钢绳的距离-------------------------------------
        }else{
            endNum = 0;
            distanceArmToBody = distancePointToLine(Ax,Ay,0,Bx,By,0,CTx,CTy,0);//本机塔臂AB到对方塔身的最短距离
            tmp1 = sqrt((pow((Bx - CTx),2) + pow((By - CTy),2)));//判断线外点到A距离最短还是到B距离最短
            tmp2 = sqrt((pow((Ax - CTx),2) + pow((Ay - CTy),2)));
            tmp3 = sqrt((pow((Ax - Bx),2) + pow((Ay - By),2)));
            if (tmp1*tmp1 >= tmp2*tmp2 + tmp3*tmp3)
                endNum = 1;                 //表明AB线外的点到B（末点）的距离最短
            if (tmp2*tmp2 >= tmp1*tmp1+tmp3*tmp3)
                endNum = 2;                 //表明AB线外的点到A（起点）的距离最短
            side = pointOnRightOrLeft(Ax,Ay,Bx,By,CTx,CTy);
            temp = (int)side*10;
            side = temp/10;
            if (distanceArmToBody > SAFE_GAP_WARN) {                //大于预警值（安全）
                globalData->craneList[0].leftCollision = 0;
                globalData->craneList[0].rightCollision = 0;
                globalData->craneList[i].paintColor = SAFE_COLOR;

            }else if(distanceArmToBody < SAFE_GAP_ALARM){           //小于报警值
                if (endNum == 1) {
                    globalData->craneList[0].tailCollision = 2;
                }else{
                    if (side < 0) {
                        globalData->craneList[0].rightCollision = 2;
                        globalData->craneList[0].leftCollision = 0;
                    }else{
                        globalData->craneList[0].rightCollision = 0;
                        globalData->craneList[0].leftCollision = 2;
                    }
                }
                globalData->craneList[i].paintColor = ALARM_COLOR;

            }else if (distanceArmToBody < SAFE_GAP_WARN)  {              //小于预警值
                if (endNum == 1) {
                    globalData->craneList[0].tailCollision = 1;
                }else{
                    if (side < 0) {
                        globalData->craneList[0].rightCollision = 1;
                        globalData->craneList[0].leftCollision = 0;
                    }else{
                        globalData->craneList[0].rightCollision = 0;
                        globalData->craneList[0].leftCollision = 1;
                    }
                }
                globalData->craneList[i].paintColor = ALARM_COLOR;
            }
        }
        break;
    default:
        break;
    }
}



/*两塔臂包络不能错开（等高）*/
void DataProcess::equalHightJudge(quint32 i)
{
    double Ax,Ay;           //本机前端点坐标
    double Bx,By;           //本机尾端点坐标
    double Cx,Cy;           //邻机前臂端点坐标
    double Dx,Dy;             //邻机后臂端点坐标
    double shortest;
    double side;
    long temp;
    quint8 endNum = 0;
    double foreEnd = 200000,backEnd = 200001;   //定义两个特殊值（运算中不会出现的结果）

    Ax = globalData->craneList.at(0).backarmPointX;
    Ay = globalData->craneList.at(0).backarmPointY;
    Bx = globalData->craneList.at(0).forearmPointX;
    By = globalData->craneList.at(0).forearmPointY;


    Cx = globalData->craneList.at(i).backarmPointX;
    Cy = globalData->craneList.at(i).backarmPointY;
    Dx = globalData->craneList.at(i).forearmPointX;
    Dy = globalData->craneList.at(i).forearmPointY;

    //计算线段AB，CD的最短距离
    double r,s;
    double Px,Py,Qx,Qy;
    double Da_cd,Db_cd,Dd_ab,Dc_ab;
    r = ((Ay-Cy)*(Dx-Cx) - (Ax-Cx)*(Dy-Cy))*1.0/((Bx-Ax)*(Dy-Cy) - (By-Ay)*(Dx-Cx));
    s = ((Ay-Cy)*(Bx-Ax) - (Ax-Cx)*(By-Ay))*1.0/((Bx-Ax)*(Dy-Cy) - (By-Ay)*(Dx-Cx));
    if ((( 0<r || r == 0)&&(r<1 || r ==1)) &&
            ((0<s || s == 0)&&(s < 1 || s == 1))) {
        Px = Ax + r*(Bx - Ax);
        Py = Ay + r*(By - Ay);
        Qx = Cx + s*(Dx - Cx);
        Qy = Cy + s*(Dy - Cy);
        shortest = sqrt(pow((Px-Qx),2) + pow((Py - Qy),2));
    }else{
        Da_cd = distancePointToLine(Cx,Cy,0,Dx,Dy,0,Ax,Ay,0);   //A到CD的最短距离
        Db_cd = distancePointToLine(Cx,Cy,0,Dx,Dy,0,Bx,By,0);   //B到CD的最短距离
        Dc_ab = distancePointToLine(Ax,Ay,0,Bx,By,0,Cx,Cy,0);   //C到AB的最短距离
        Dd_ab = distancePointToLine(Ax,Ay,0,Bx,By,0,Dx,Dy,0);   //D到AB的最短距离
        shortest = Da_cd<Db_cd?Da_cd:Db_cd;
        shortest = shortest<Dc_ab?shortest:Dc_ab;
        shortest = shortest<Dd_ab?shortest:Dd_ab;
        if (shortest == Da_cd) endNum = 1;
        else if (shortest == Db_cd) endNum = 2;
        else if (shortest == Dc_ab) endNum = 3;
        else endNum = 4;
    }

    if (shortest > SAFE_GAP_WARN) {
        globalData->craneList[0].leftCollision = 0;
        globalData->craneList[0].rightCollision = 0;
        globalData->craneList[0].tailCollision = 0;
        globalData->craneList[0].farCollision = 0;
        globalData->craneList[i].paintColor = SAFE_COLOR;
    }else if(shortest < 0.0001){

        if(globalData->craneList.at(0).craneHeight > globalData->craneList[i].craneHeight)//本机塔臂比对方高
            localHigherJudge(i);
        else if (globalData->craneList.at(0).craneHeight < globalData->craneList[i].craneHeight)//本机塔臂比对方低
            neighborHigherJudge(i);
        else{                                                                                                       //塔臂等高
            side = pointOnRightOrLeft(Ax,Ay,Bx,By,Dx,Dy);
            temp = (int)side * 10;
            side = temp/10;
            if(side < 0)  {
                globalData->craneList[0].leftCollision = 2;
            }else{
                globalData->craneList[0].rightCollision = 2;
            }
            globalData->craneList[i].paintColor = ALARM_COLOR;

        }
 //---------------------------------------------------------------------------------------------------------------------
    }else if (shortest < SAFE_GAP_ALARM) {
        if(endNum == 4 || endNum == 3) {                //到线段最近的C/D端点到AB
            if(endNum == 4)
                side = pointOnRightOrLeft(Ax,Ay,Bx,By,Dx,Dy);           //判断D点（对方臂前端）在本机摆臂的左右
            else
                side = pointOnRightOrLeft(Ax,Ay,Bx,By,Cx,Cy);           //判断C点（对方臂后端）在本机摆臂的左右
            temp = (int)side*10;
            side = temp/10;
        }else if(endNum == 2 || endNum == 1) {
            if(endNum ==2)
                side = foreEnd;                                                     //远碰撞，foreEnd正常叉乘不可能出现foreEnd，用来代表远碰撞
            else
                side = backEnd;
        }
    if(side == foreEnd)
        globalData->craneList[0].farCollision = 2;
    else if(side == backEnd)
        globalData->craneList[0].tailCollision = 2;
    else if(side < 0)
        globalData->craneList[0].rightCollision = 2;
    else
        globalData->craneList[0].leftCollision = 2;

    globalData->craneList[i].paintColor = ALARM_COLOR;

    }else if(shortest < SAFE_GAP_WARN) {
        if(endNum == 4 || endNum == 3) {
            if(endNum == 4)
                side = pointOnRightOrLeft(Ax,Ay,Bx,By,Dx,Dy);
            else
                side = pointOnRightOrLeft(Ax,Ay,Bx,By,Cx,Cy);
            temp = (int)side * 10;
            side = temp/10;
        }else if(endNum == 2 || endNum == 1) {
            if(endNum == 2)
                side = foreEnd;
            else
                side = backEnd;
        }

        if(side == foreEnd)
            globalData->craneList[0].farCollision = 1;
        else if(side == backEnd)
            globalData->craneList[0].tailCollision = 1;
        else if(side < 0)
             globalData->craneList[0].rightCollision = 1 ;
        else
             globalData->craneList[0].leftCollision = 1;
        globalData->craneList[i].paintColor = WARN_COLOR;
    }
}


void DataProcess::run()
{
    quint32 angle;

   //qDebug()<<overload_judge(10.0,25.5);


    while(1){                   //20141118  adjustmemt oringe ->  1

   // globalData->craneList[0].angle = globalData->craneList[0].angle +1;      //测试多线程
//      if( globalData->craneList[0].angle >360) {
//         globalData->craneList[0].angle = globalData->craneList[0].angle - 360;
//    }
//    if (globalData->craneList[0].angle < 0 ) {
//        globalData->craneList[0].angle = globalData->craneList[0].angle+360;
//    }

    /****判断变幅是否大于前臂长********/
    if(globalData->craneList[0].span > globalData->craneList[0].forearmLen)
        globalData->craneList[0].span = globalData->craneList[0].forearmLen;
    /*
    globalData->craneList[0].hookHeight = 1;
   globalData->craneList[1].angle = 200;
   // globalData->craneList[0].angle = 80;
    //qDebug()<<(globalData->craneList[0].angle);
*/
    //globalData->craneList[0].paintColor = ALARM_COLOR;
    /*相干塔机防碰撞计算*/
    //计算防碰撞信息两个端点，即每个塔吊前臂和后壁的端点坐标
    //quint32 angle;
    for (quint8 i = 0 ; i < globalData->craneList.count() ; i++) {
        angle = (qint32)(globalData->craneList.at(i).angle*TVECT);
        globalData->craneList[i].backarmPointX = globalData->craneList.at(i).coordinateX -
                globalData->craneList.at(i).backarmLen * cos_tab[angle];
        globalData->craneList[i].backarmPointY = globalData->craneList.at(i).coordinateY -
                globalData->craneList.at(i).backarmLen * sin_tab[angle];

        globalData->craneList[i].forearmPointX = globalData->craneList.at(i).coordinateX +
                globalData->craneList.at(i).forearmLen * cos_tab[angle];
        globalData->craneList[i].forearmPointY = globalData->craneList.at(i).coordinateY +
                globalData->craneList.at(i).forearmLen * sin_tab[angle];
    }

    for (quint8 i = 1 ; i <globalData->craneList.count() ; i++) {
        if (globalData->craneList.at(0).bottomEnvelope > globalData->craneList.at(i).topEnvelope) {
            //------------------------------- 本机塔臂比对方高（情况一）--------------------------------------------
            localHigherJudge(i);

        }else if (globalData->craneList.at(0).topEnvelope < globalData->craneList.at(i).bottomEnvelope) {
            //------------------------------- 本机塔臂比对方底（情况二）--------------------------------------------
            neighborHigherJudge(i);

        }else{
            //------------------------------- 两臂包络品面不能错开（情况三）--------------------------------------
            //equalHightJudge(globalData->craneList[i]);
            equalHightJudge(i);
        }
        if(globalData->craneList.at(i).paintColor == ALARM_COLOR)
            globalData->interfere_crane_num = globalData->craneList.at(i).craneID;
    }
    globalData->ultimate_warn_flag = 0;
    globalData->left_forbid_warn_flag = 0;
    globalData->right_forbid_warn_flag = 0;
    globalData->near_forbid_warn_flag =0;
    globalData->far_forbid_warn_flag =0;
    globalData->left_barrier_warn_flag=0;
    globalData->right_barrier_warn_flag=0;
    globalData->near_barrier_warn_flag=0;
    globalData->far_barrier_warn_flag=0;
    globalData->low_barrier_warn_flag=0;


    protect_area_handle(globalData->craneList[0].angle);

    globalData->ultimate_warn_flag = globalData->ultimate_warn_flag>globalData->craneList.at(0).leftCollision? globalData->ultimate_warn_flag:globalData->craneList.at(0).leftCollision;
    globalData->ultimate_warn_flag = globalData->ultimate_warn_flag>globalData->craneList.at(0).rightCollision? globalData->ultimate_warn_flag:globalData->craneList.at(0).rightCollision;

    globalData->ultimate_warn_flag = globalData->ultimate_warn_flag>globalData->craneList.at(0).farCollision? globalData->ultimate_warn_flag:globalData->craneList.at(0).farCollision;
    globalData->ultimate_warn_flag = globalData->ultimate_warn_flag>globalData->craneList.at(0).nearCollision? globalData->ultimate_warn_flag:globalData->craneList.at(0).nearCollision;
    globalData->ultimate_warn_flag = globalData->ultimate_warn_flag>globalData->craneList.at(0).lowCollision? globalData->ultimate_warn_flag:globalData->craneList.at(0).lowCollision;

    globalData->ultimate_warn_flag = globalData->ultimate_warn_flag>globalData->craneList.at(0).tailCollision? globalData->ultimate_warn_flag:globalData->craneList.at(0).tailCollision;
    globalData->ultimate_warn_flag = globalData->ultimate_warn_flag>globalData->craneList.at(0).bodyCollision? globalData->ultimate_warn_flag:globalData->craneList.at(0).bodyCollision;

    globalData->final_warn_2 = 0;
    globalData->final_warn_1 = limit_warn_judge();
    globalData->final_warn_2 = globalData->final_warn_1>globalData->final_warn_2?globalData->final_warn_1:globalData->final_warn_2;
    globalData->final_warn_1 = moment_warn_judge(90 , 110);
    globalData->final_warn_2 = globalData->final_warn_1>globalData->final_warn_2?globalData->final_warn_1:globalData->final_warn_2;
    globalData->final_warn_1 = wind_warn_judge(10.8);
    globalData->final_warn_2 = globalData->final_warn_1>globalData->final_warn_2?globalData->final_warn_1:globalData->final_warn_2;
    globalData->final_warn_1 = overload_judge( globalData->craneList[0].wight, globalData->craneList[0].span);
    globalData->final_warn_2 = globalData->final_warn_1>globalData->final_warn_2?globalData->final_warn_1:globalData->final_warn_2;
    globalData->ultimate_warn_flag = globalData->final_warn_2>globalData->ultimate_warn_flag?globalData->final_warn_2:globalData->ultimate_warn_flag;
    emit paintSignal();

    msleep(200);

    }  
     // close ( globalData->ZigbeeFd );
}
/****限位报警总判断****************/
quint8 DataProcess::limit_warn_judge()
{
    quint8 warn_1 = 0,warn_2 = 0;
    warn_1 = right_left_limit_warn_judge(globalData->limitData.leftLimit,globalData->limitData.rightLimit);
    warn_2 = warn_1>warn_2?warn_1:warn_2;
    warn_1 = near_far_limit_warn_judge(globalData->limitData.nearLmit,globalData->limitData.farLimit,2);
    warn_2 = warn_1>warn_2?warn_1:warn_2;
    warn_1 = high_limit_warn_judge(globalData->limitData.highLimit,3);
    warn_2 = warn_1>warn_2?warn_1:warn_2;
    warn_1 = low_limit_warn_judge(5,1);
    warn_2 = warn_1>warn_2?warn_1:warn_2;
    if(warn_2 == 2)
        globalData->craneList[0].paintColor = ALARM_COLOR;
    else if (warn_2 == 1)
        globalData->craneList[0].paintColor =  WARN_COLOR;
    return warn_2;
}

/********左右限位报警判断,不确定***************/
quint8 DataProcess::right_left_limit_warn_judge(float left_warn_val, float right_warn_val)
{
    quint8 final_warn = 0;
    globalData->craneList[0].left_limit_warn_flag = 0;
    globalData->craneList[0].right_limit_warn_flag = 0;

    if(left_warn_val >= 0)
    {
        if (globalData->craneList.at(0).angle >= left_warn_val*0.9)
        {
        }
    }
    return final_warn;
}
/********远近限位报警判断************/
quint8 DataProcess::near_far_limit_warn_judge(float near_w_val, float far_w_val, float buffer)
{
    quint8 final_warn = 0;
    float near_ew_val = near_w_val + buffer;
    float far_ew_val = far_w_val + buffer;

    if(globalData->craneList.at(0).span > (globalData->craneList.at(0).forearmLen - far_w_val))
    {
        globalData->craneList[0].far_limit_warn_flag = 2;
    }
    else if(globalData->craneList.at(0).span > (globalData->craneList.at(0).forearmLen-far_ew_val))
    {
        globalData->craneList[0].far_limit_warn_flag = 1;
    }
    else if(globalData->craneList.at(0).span < near_ew_val)
    {
        globalData->craneList[0].near_limit_warn_flag = 1;
    }
    else
    {
        globalData->craneList[0].far_limit_warn_flag = 0;
        globalData->craneList[0].near_limit_warn_flag = 0;
    }
    final_warn = globalData->craneList[0].far_limit_warn_flag>globalData->craneList[0].near_limit_warn_flag?globalData->craneList[0].far_limit_warn_flag:globalData->craneList[0].near_limit_warn_flag;
    return final_warn;
}
/***************高低限位报警判断****************/
quint8 DataProcess::high_limit_warn_judge(float warn_val, float buffer)
{
    float ew_val = warn_val+buffer;
    if(globalData->craneList.at(0).craneHeight>globalData->craneList.at(0).craneHeight - warn_val)
    {
        globalData->craneList[0].high_limit_warn_flag = 2;
    }
    else if (globalData->craneList.at(0).craneHeight>globalData->craneList.at(0).craneHeight - ew_val)
    {
        globalData->craneList[0].high_limit_warn_flag = 1;
    }
    else
        globalData->craneList[0].high_limit_warn_flag = 0;

    return globalData->craneList[0].high_limit_warn_flag;
}
/***************高低限位报警判断****************/
quint8 DataProcess::low_limit_warn_judge(float warn_val, float buffer)
{
    float ew_val = warn_val+buffer;
    if(globalData->craneList.at(0).craneHeight<warn_val)
    {
        globalData->craneList[0].low_limit_warn_flag = 2;
    }
    else if (globalData->craneList.at(0).craneHeight<ew_val)
    {
        globalData->craneList[0].low_limit_warn_flag = 1;
    }
    else
        globalData->craneList[0].low_limit_warn_flag = 0;

    return globalData->craneList[0].low_limit_warn_flag;
}
/***********吊重是否超载判断****************/
quint8 DataProcess::overload_judge(double  weight, double span)
{

    if(readTableFlag == false)
   {
        qDebug()<<"read MomentData  failed";
        return  -1;
   }
//qDebug()<<"tableLen   : "<<tableLen;


int index = 0;
index = binSearch(spanArray,tableLen,span);            //获取实时数据的接口
//qDebug()<<"the index of the realTimeSpan is :\t"<<index;

double realtimeWeight = weight;                            //实时数据接口
if( realtimeWeight < table_weight_double[index] *0.9 )
    globalData->craneList[0].overweight_limit_warn_flag = 0;
else if(realtimeWeight > table_weight_double[index] )
    globalData->craneList[0].overweight_limit_warn_flag = 2;
else
    globalData->craneList[0].overweight_limit_warn_flag = 1;
//----------------------------------------------------
globalData->craneList[0].moment = realtimeWeight/table_weight_double[index] * 100;
if(globalData->craneList[0].moment > 100)
    {
        globalData->craneList[0].moment = 100;
    }
//-----------------------------------------------------
return globalData->craneList[0].overweight_limit_warn_flag;
}

/***********力矩报警判断********************/
quint8 DataProcess::moment_warn_judge(float e_warn_val, float warn_val)
{
    if(globalData->craneList.at(0).moment < e_warn_val)
    {
        moment_limit_time = 0;
        globalData->craneList[0].moment_limit_warn_flag = 0;
    }
    else if ((globalData->craneList.at(0).moment > warn_val) &&
             (moment_limit_time++ > 200))
    {
        moment_limit_time = 0;
        globalData->craneList[0].moment_limit_warn_flag = 2;
    }
    else if ((globalData->craneList.at(0).moment > e_warn_val) &&
             (e_moment_limit_time++ > 200))
    {
        e_moment_limit_time = 0;
        globalData->craneList[0].moment_limit_warn_flag = 1;//预警
    }
    return globalData->craneList[0].moment_limit_warn_flag;
}
/**********风速报警判断****************/
quint8 DataProcess::wind_warn_judge(float warn_val)
{
    if(globalData->craneList.at(0).windspeed < warn_val*0.9)
        globalData->craneList[0].wind_limit_warn_flag =0;
    else if(globalData->craneList.at(0).windspeed > warn_val)
        globalData->craneList[0].wind_limit_warn_flag =2;
    else if (globalData->craneList.at(0).windspeed > warn_val*0.9)
        globalData->craneList[0].wind_limit_warn_flag =1;
    return globalData->craneList[0].wind_limit_warn_flag;
}

void DataProcess::protect_area_handle(float angle)
{
    unsigned char left_barrier_flg[6]={0,0,0,0,0,0};     //左障碍报警标志
    unsigned char right_barrier_flg[6]={0,0,0,0,0,0};    //右障碍报警标志
    unsigned char far_barrier_flg[6]={0,0,0,0,0,0};      //远障碍碰撞报警标志
    unsigned char near_barrier_flg[6]={0,0,0,0,0,0};     //近障碍碰撞报警标志
    unsigned char low_barrier_flg[6]={0,0,0,0,0,0};      //低障碍碰

    unsigned char left_forbid_flg[6]={0,0,0,0,0,0};      //左禁行标志
    unsigned char right_forbid_flg[6]={0,0,0,0,0,0};    //右禁行标志
    unsigned char far_forbid_flg[6]={0,0,0,0,0,0};      //远禁行标志
    unsigned char near_forbid_flg[6]={0,0,0,0,0,0};     //近禁行标志

    unsigned char safe_distance=3;                         //吊钩和塔吊距保护区实物的可视为安全的距离
    unsigned char result=0;
    unsigned char i=0;
    int angleN = (int)angle;
    float arc_span[4]={0};
    float span_dis;
    float alarm_buff = 3;
    float warn_buff = 2;
    float low_alarm_distance = 2;
    float low_warning_distance = 3;
    span_dis = globalData->craneList.at(0).span;
    //-------------保护区显示及报警判断-------------------
    for( i=0;i<globalData->proZoneDataList.count();i++)
    {
        globalData->proZoneDataList[i].currentColor=Pzone_safe_color; //初始默认为安全，待下一步检验
        char Pzone_type = 0;
        float startAngle = 0;
        float endAngle = 0;
        Pzone_type = globalData->proZoneDataList.at(i).proZoneType;
        startAngle = globalData->proZoneDataList.at(i).proZoneStartAngle;
        endAngle = globalData->proZoneDataList.at(i).proZoneEndAngle;
//----------判断当前角度是否处于警报角度区域内，预警报警带宽度均为5.0度----------
        char b_angle=suit(angle,startAngle,endAngle,5.0,5.0);
//----------读取这个角度范围内保护区的详细信息进一步判断--------------
        if( b_angle) {
              float float_data[7];
              for(int j=0;j<7;j++) {
                  float_data[j]=globalData->proZoneDataList.at(i).point_perangle[angleN][j];
              }
 //-float_data[0]为当前角度内的保护区个数---
//------------------------------------------------------------------------------------------------- -----------------------------//
//--------------------------------------------------------禁行区----------------------------------------------------------------//
//------------------------------------------------------------------------------------------------------------------------------//
              if( Pzone_type==0) {//禁行区
                  if(b_angle==3) {      //进入保护区角度内
//-----------------------实际中应当不会出现(撞上保护区继续深入)-----------------------
                      if((globalData->craneList.at(0).craneHeight-globalData->proZoneDataList.at(i).proZoneHeight)<safe_distance) {
                          quint8 result=trigger_left_right_warn_flag(Pzone_type,angle,startAngle,endAngle);
                          switch(result)
                          {
                          case 1:  left_forbid_flg[i]=2;   break;      //左禁行标志
                          case 2:  right_forbid_flg[i]=2;  break;      //右禁行标志
                          case 3:  left_barrier_flg[i]=2;  break;      //左障碍报警标志
                          case 4:  right_barrier_flg[i]=2; break;      //右障碍报警标志
                          }
                          globalData->proZoneDataList[i].currentColor = Pzone_alarm_color;                                 //吊钩低于保护区高度且进入保护区，报警
                          globalData->warn_area_num=i;
                      }
                            //-----------------------实际中应当不会出现(撞上保护区继续深入)-----------------------
                      else {
                             //-----报警幅度内--------------------
                          arc_span[0]=float_data[1]-alarm_buff;
                          arc_span[1]=float_data[2]+alarm_buff;
                          char in_pzone2=0;
                          if(float_data[0]>1) {    //保护区当前角度不连续分区大于1个
                              arc_span[2]=float_data[3]-alarm_buff;
                              arc_span[3]=float_data[4]+alarm_buff;
                              if(span_dis>= arc_span[2]&&span_dis<=arc_span[3])
                                  in_pzone2=1;
                          }
                          if((span_dis>=arc_span[0])&&(span_dis<=arc_span[1])||(in_pzone2)) {  //保护区内当前变幅下小车处于扇形区域内的报警位置判断
                              if(0==in_pzone2) //在第一个分区
                                   result = trigger_far_near_warn_flag(Pzone_type,span_dis,arc_span[0],arc_span[1]);
                              else            //在第二个分区
                                   result = trigger_far_near_warn_flag(Pzone_type,span_dis,arc_span[2],arc_span[3]);
                              switch(result)
                              {
                              case 1:  near_forbid_flg[i]=2;   break;      //近禁行标志
                              case 2:  far_forbid_flg[i]=2;  break;        //远禁行标志
                              case 3:  near_barrier_flg[i]=2;  break;      //左障碍报警标志
                              case 4:  far_barrier_flg[i]=2; break;      //右障碍报警标志
                              }
                              globalData->proZoneDataList[i].currentColor=Pzone_alarm_color;                                 //吊钩低于保护区高度且进入保护区，报警
                              globalData->warn_area_num=i;
                          }
                          else {
                              //-----预警幅度内--------------------
                              arc_span[0]=float_data[1]-warn_buff-alarm_buff;
                              arc_span[1]=float_data[2]+warn_buff+alarm_buff;
                              char in_pzone2=0;
                              if(float_data[0]>1) {     //分区个数大于1个
                                  arc_span[2]=float_data[3]-warn_buff-alarm_buff;
                                  arc_span[3]=float_data[4]+warn_buff+alarm_buff;
                                  if(span_dis>= arc_span[2]&&span_dis<=arc_span[3])
                                      in_pzone2=1;
                              }
                              if((span_dis>=arc_span[0])&&(span_dis<=arc_span[1])||in_pzone2) {   //保护区内当前变幅下小车处于扇形区域内的报警位置判断
                                  if(0==in_pzone2) //在第一个分区
                                      result= trigger_far_near_warn_flag(Pzone_type,span_dis,arc_span[0],arc_span[1]);
                                  else                   //在第二个分区
                                      result= trigger_far_near_warn_flag(Pzone_type,span_dis,arc_span[2],arc_span[3]);
                                  switch(result)
                                  {
                                  case 1:  near_forbid_flg[i]=1;   break;      //近禁行标志
                                  case 2:  far_forbid_flg[i]=1;    break;        //远禁行标志
                                  case 3:  near_barrier_flg[i]=1;  break;      //左障碍报警标志
                                  case 4: far_barrier_flg[i]=1;    break;      //右障碍报警标志
                                  }
                                  globalData->proZoneDataList[i].currentColor=Pzone_warning_color;                                 //吊钩低于保护区高度且进入保护区，报警
                                  globalData->warn_area_num=i;
                              }
                              else{
                                  near_forbid_flg[i]=0;
                                  far_forbid_flg[i]=0;
                                  near_barrier_flg[i]=0;
                                  far_barrier_flg[i]=0;
                                  globalData->proZoneDataList[i].currentColor=Pzone_safe_color;                                 //吊钩低于保护区高度且进入保护区，报警
                              }
                          }
                      }
                  }
                  else if(b_angle==2) { //进入报警角度内
                      if(((globalData->craneList.at(0).craneHeight-globalData->proZoneDataList.at(i).proZoneHeight)>safe_distance)) {
 //-----------------------如果本机塔高比保护区高度大于safe_distance m以上-----------------------------------------
//如果高于则判断载重与否
                          if( globalData->craneList.at(0).wight>0) { //如果载重则警，不载重可通过
                              quint8 result=trigger_left_right_warn_flag(Pzone_type, angle,startAngle,endAngle);
                              switch(result)
                              {
                              case 1:  left_forbid_flg[i]=2;   break;      //左禁行标志
                              case 2:  right_forbid_flg[i]=2;  break;      //右禁行标志
                              case 3:  left_barrier_flg[i]=2;  break;      //左障碍报警标志
                              case 4:  right_barrier_flg[i]=2; break;      //右障碍报警标志
                              }
                              globalData->proZoneDataList[i].currentColor=Pzone_alarm_color;                                 //吊钩低于保护区高度且进入保护区，报警
                              globalData->warn_area_num=i;
                          }
                          else {            //不载重
                          arc_span[0]=globalData->proZoneDataList.at(i).min_span;  //保护区的最小幅度
                          arc_span[1]=globalData->proZoneDataList.at(i).max_span;  //保护区的最大幅度
                          if((span_dis>=arc_span[0])&&(span_dis<=arc_span[1])||(span_dis>= arc_span[2])&&(span_dis<=arc_span[3])) {  //当前变幅下吊钩处于保护区的报警角度内且在报警幅度内

                              {
                                  quint8 result=trigger_left_right_warn_flag(Pzone_type, angle,startAngle,endAngle);
                                  switch(result)
                                  {
                                  case 1:  left_forbid_flg[i]=2;   break;      //左禁行标志
                                  case 2:  right_forbid_flg[i]=2;  break;      //右禁行标志
                                  case 3:  left_barrier_flg[i]=2;  break;      //左障碍报警标志
                                  case 4:  right_barrier_flg[i]=2; break;      //右障碍报警标志
                                  }
                                  globalData->proZoneDataList[i].currentColor=Pzone_alarm_color;                                 //吊钩低于保护区高度且进入保护区，报警
                                  globalData->warn_area_num=i;
                              }
                          }
                          else {
                              left_forbid_flg[i]=0;   //左禁行标志
                              right_forbid_flg[i]=0;  //右禁行标志
                              far_forbid_flg[i]=0;    //远禁行标志
                              near_forbid_flg[i]=0;   //近禁行标志
                              globalData->proZoneDataList[i].currentColor=Pzone_safe_color;                                 //吊钩低于保护区高度且进入保护区，报警
                          }
                          }
                      }
                            //-----------------------如果本机塔高比保护区高度不大于safe_distance以上-----------------------------------------
                      else {
                          quint8 result=trigger_left_right_warn_flag(Pzone_type, angle,startAngle,endAngle);
                          switch(result)
                          {
                          case 1:  left_forbid_flg[i]=2;   break;      //左禁行标志
                          case 2:  right_forbid_flg[i]=2;  break;      //右禁行标志
                          case 3:  left_barrier_flg[i]=2;  break;      //左障碍报警标志
                          case 4:  right_barrier_flg[i]=2; break;      //右障碍报警标志
                          }
                          globalData->proZoneDataList[i].currentColor=Pzone_alarm_color;                                 //吊钩低于保护区高度且进入保护区，报警
                          globalData->warn_area_num=i;
                      }
                  }
                  else if(b_angle==1) { //进入预警角度内
                      if(((globalData->craneList.at(0).craneHeight-globalData->proZoneDataList.at(i).proZoneHeight)>safe_distance)) {
                                  //-----------------------如果本机塔高比保护区高度大于safe_distance以上-----------------------------------------
                                  //如果高于则判断载重与否
                          if( globalData->craneList.at(0).wight>0) {  //如果载重则警，不载重可通过
                              quint8 result=trigger_left_right_warn_flag(Pzone_type, angle,startAngle,endAngle);
                              switch(result)
                              {
                              case 1:  left_forbid_flg[i]=1;   break;      //左禁行标志
                              case 2:  right_forbid_flg[i]=1;  break;      //右禁行标志
                              case 3:  left_barrier_flg[i]=1;  break;      //左障碍报警标志
                              case 4:  right_barrier_flg[i]=1; break;      //右障碍报警标志
                              }
                              globalData->proZoneDataList[i].currentColor=Pzone_warning_color;                                 //吊钩低于保护区高度且进入保护区，预警
                              globalData->warn_area_num=i;
                          }
                          else {                //不载重
                              arc_span[0]=globalData->proZoneDataList.at(i).min_span;  //保护区的最小幅度
                              arc_span[1]=globalData->proZoneDataList.at(i).max_span;  //保护区的最大幅度
                              if((span_dis>=arc_span[0])&&(span_dis<=arc_span[1])||(span_dis>= arc_span[2])&&(span_dis<=arc_span[3])){   //当前变幅下吊钩处于保护预警角度内
                                  {
                                      quint8 result=trigger_left_right_warn_flag(Pzone_type, angle,startAngle,endAngle);
                                      switch(result)
                                      {
                                      case 1:  left_forbid_flg[i]=1;   break;      //左禁行标志
                                      case 2:  right_forbid_flg[i]=1;  break;      //右禁行标志
                                      case 3:  left_barrier_flg[i]=1;  break;      //左障碍报警标志
                                      case 4:  right_barrier_flg[i]=1; break;      //右障碍报警标志
                                      }
                                      globalData->proZoneDataList[i].currentColor=Pzone_warning_color;                                 //吊钩低于保护区高度且进入保护区，报警
                                      globalData->warn_area_num=i;
                                  }
                              }
                              else {
                                  left_forbid_flg[i]=0;   //左禁行标志
                                  right_forbid_flg[i]=0;  //右禁行标志
                                  far_forbid_flg[i]=0;    //远禁行标志
                                  near_forbid_flg[i]=0;   //近禁行标志
                                  globalData->proZoneDataList[i].currentColor=Pzone_safe_color;                                 //吊钩低于保护区高度且进入保护区，报警
                              }
                          }
                      }
                            //-----------------------如果本机塔高比保护区高度不大于safe_distance以上-----------------------------------------
                      else{
                          quint8 result=trigger_left_right_warn_flag(Pzone_type, angle,startAngle,endAngle);
                          switch(result)
                          {
                          case 1:  left_forbid_flg[i]=1;   break;      //左禁行标志
                          case 2:  right_forbid_flg[i]=1;  break;      //右禁行标志
                          case 3:  left_barrier_flg[i]=1;  break;      //左障碍报警标志
                          case 4:  right_barrier_flg[i]=1; break;      //右障碍报警标志
                          }
                          globalData->proZoneDataList[i].currentColor=Pzone_warning_color;                                 //吊钩低于保护区高度且进入保护区，报警
                          globalData->warn_area_num=i;
                      }
                  }
              }
//------------------------------------------------------------------------------------------------------------------------------//
//--------------------------------------------------------障碍物----------------------------------------------------------------//
//------------------------------------------------------------------------------------------------------------------------------//
              else  { //if( Pzone_type==1)//障碍物
                  if(b_angle==3) {      //进入保护区角度内
                      //-----------------------实际中应当不会出现(撞上保护区继续深入)↓-----------------------
                      if((globalData->craneList.at(0).craneHeight-globalData->proZoneDataList.at(i).proZoneHeight)<safe_distance) {
                          quint8 result=trigger_left_right_warn_flag(Pzone_type, angle,startAngle,endAngle);
                          switch(result)
                          {
                          case 1:  left_forbid_flg[i]=2;   break;      //左禁行标志
                          case 2:  right_forbid_flg[i]=2;  break;      //右禁行标志
                          case 3:  left_barrier_flg[i]=2;  break;      //左障碍报警标志
                          case 4:  right_barrier_flg[i]=2; break;      //右障碍报警标志
                          }
                          globalData->proZoneDataList[i].currentColor=Pzone_alarm_color;                                 //吊钩低于保护区高度且进入保护区，报警
                          globalData->warn_area_num=i;
                      }
                            //-----------------------实际中应当不会出现(撞上保护区继续深入)↑-----------------------
                      else {
                          if((globalData->craneList.at(0).hookHeight-globalData->proZoneDataList.at(i).proZoneHeight)<safe_distance)
                          {
                                        //-----报警幅度内--------------------
                              arc_span[0]=float_data[1];
                              arc_span[1]=float_data[2];
                              char in_pzone2=0;
                              if(float_data[0]>1) {//保护区个数大于1个
                                  arc_span[2]=float_data[3];
                                  arc_span[3]=float_data[4];
                                  if(span_dis>= arc_span[2]&&span_dis<=arc_span[3])
                                      in_pzone2=1;
                              }
                              if((span_dis>=arc_span[0])&&(span_dis<=arc_span[1])||in_pzone2)  {  //保护区内当前变幅下小车处于扇形区域内的报警位置判断
                                 //-----吊钩到保护区顶部距离小于报警距离，则报警，否则小于预警距离则预警
                                  if(globalData->craneList.at(0).hookHeight-globalData->proZoneDataList.at(i).proZoneHeight<low_alarm_distance) {
                                      low_barrier_flg[i]=2;
                                      globalData->proZoneDataList[i].currentColor=Pzone_alarm_color;
                                  }else  if(globalData->craneList.at(0).hookHeight-globalData->proZoneDataList.at(i).proZoneHeight<low_warning_distance) {
                                      low_barrier_flg[i]=1;
                                      globalData->proZoneDataList[i].currentColor=Pzone_warning_color;
                                  }
                                  else {
                                      left_barrier_flg[i]=0;   //左障碍报警标志
                                      right_barrier_flg[i]=0;  //右障碍报警标志
                                      far_barrier_flg[i]=0;    //远障碍碰撞报警标志
                                      near_barrier_flg[i]=0;   //近障碍碰撞报警标志
                                      low_barrier_flg[i]=0;    //低障碍碰撞报警标志
                                      globalData->proZoneDataList[i].currentColor=Pzone_safe_color;                                 //吊钩低于保护区高度且进入保护区，报警
                                  }
                              }
                              else {
                                              //-----报警幅度内--------------------
                                  arc_span[0]=float_data[1]-alarm_buff;
                                  arc_span[1]=float_data[2]+alarm_buff;
                                  char in_pzone2=0;
                                  if(float_data[0]>1) { //保护区个数大于1个
                                      arc_span[2]=float_data[3]-alarm_buff;
                                      arc_span[3]=float_data[4]+alarm_buff;
                                      if(span_dis>= arc_span[2]&&span_dis<=arc_span[3])
                                          in_pzone2=1;
                                  }
                                  if((span_dis>=arc_span[0])&&(span_dis<=arc_span[1])||in_pzone2) {  //保护区内当前变幅下小车处于扇形区域内的报警位置判断
                                      if(0==in_pzone2) //在第一个保护区
                                          result= trigger_far_near_warn_flag(Pzone_type,span_dis,arc_span[0],arc_span[1]);
                                      else
                                          result= trigger_far_near_warn_flag(Pzone_type,span_dis,arc_span[2],arc_span[3]);
                                      switch(result)
                                      {
                                      case 1:  near_forbid_flg[i]=2;   break;      //近禁行标志
                                      case 2:  far_forbid_flg[i]=2;    break;      //远禁行标志
                                      case 3:  near_barrier_flg[i]=2;  break;      //左障碍报警标志
                                      case 4:  far_barrier_flg[i]=2;   break;      //右障碍报警标志
                                      }
                                      globalData->proZoneDataList[i].currentColor=Pzone_alarm_color;                                 //吊钩低于保护区高度且进入保护区，报警
                                      globalData->warn_area_num=i;
                                  }
                                  else{
                                                    //-----预警幅度内--------------------
                                      arc_span[0]=float_data[1]-warn_buff-alarm_buff;
                                      arc_span[1]=float_data[2]+warn_buff+alarm_buff;
                                      char in_pzone2=0;
                                      if(float_data[0]>1) {//保护区个数大于1个
                                          arc_span[2]=float_data[3]-warn_buff-alarm_buff;
                                          arc_span[3]=float_data[4]+warn_buff+alarm_buff;
                                          if(span_dis>= arc_span[2]&&span_dis<=arc_span[3])
                                              in_pzone2=1;
                                      }
                                      if((span_dis>=arc_span[0])&&(span_dis<=arc_span[1])||in_pzone2) {   //保护区内当前变幅下小车处于扇形区域内的报警位置判断
                                          if(0==in_pzone2) //在第一个保护区
                                              result= trigger_far_near_warn_flag(Pzone_type,span_dis,arc_span[0],arc_span[1]);
                                          else
                                              result= trigger_far_near_warn_flag(Pzone_type,span_dis,arc_span[2],arc_span[3]);
                                          switch(result)
                                          {
                                          case 1:  near_forbid_flg[i]=1;   break;      //近禁行标志
                                          case 2:  far_forbid_flg[i]=1;    break;       //远禁行标志
                                          case 3:  near_barrier_flg[i]=1;  break;      //左障碍报警标志
                                          case 4:  far_barrier_flg[i]=1;   break;      //右障碍报警标志
                                          }
                                          globalData->proZoneDataList[i].currentColor=Pzone_warning_color;                                 //吊钩低于保护区高度且进入保护区，报警
                                          globalData->warn_area_num=i;
                                      }
                                      else{
                                          left_barrier_flg[i]=0;   //左障碍报警标志
                                          right_barrier_flg[i]=0;  //右障碍报警标志
                                          far_barrier_flg[i]=0;    //远障碍碰撞报警标志
                                          near_barrier_flg[i]=0;   //近障碍碰撞报警标志
                                          low_barrier_flg[i]=0;    //低障碍碰撞报警标志
                                          globalData->proZoneDataList[i].currentColor=Pzone_safe_color;                                 //吊钩低于保护区高度且进入保护区，报警
                                      }
                                  }
                              }
                          }
                          else{
                              left_barrier_flg[i]=0;   //左障碍报警标志
                              right_barrier_flg[i]=0;  //右障碍报警标志
                              far_barrier_flg[i]=0;    //远障碍碰撞报警标志
                              near_barrier_flg[i]=0;   //近障碍碰撞报警标志
                              low_barrier_flg[i]=0;    //低障碍碰撞报警标志
                              globalData->proZoneDataList[i].currentColor=Pzone_safe_color;                                 //吊钩低于保护区高度且进入保护区，报警
                          }
                      }
                  }
                  else if(b_angle==2)  {  //进入报警角度内
                      if((globalData->craneList.at(0).craneHeight-globalData->proZoneDataList.at(i).proZoneHeight>safe_distance)) {
        //-----------------------如果本机塔高比保护区高度大于safe_distance以上-----------------------------------------
                          arc_span[0]=globalData->proZoneDataList.at(i).min_span;  //保护区的最小幅度
                          arc_span[1]=globalData->proZoneDataList.at(i).max_span;  //保护区的最大幅度
                          if((span_dis>=arc_span[0])&&(span_dis<=arc_span[1])||(span_dis>= arc_span[2])&&(span_dis<=arc_span[3])) {  //保护区内当前变幅下吊钩处于扇形区域内的报警位置判断
                              if(globalData->craneList.at(0).hookHeight<globalData->proZoneDataList.at(i).proZoneHeight){
                                  quint8 result=trigger_left_right_warn_flag(Pzone_type, angle,startAngle,endAngle);
                                  switch(result)
                                  {
                                  case 1:  left_forbid_flg[i]=2;   break;      //左禁行标志
                                  case 2:  right_forbid_flg[i]=2;  break;      //右禁行标志
                                  case 3:  left_barrier_flg[i]=2;  break;      //左障碍报警标志
                                  case 4:  right_barrier_flg[i]=2; break;      //右障碍报警标志
                                  }
                                  globalData->proZoneDataList[i].currentColor=Pzone_alarm_color;                                 //吊钩低于保护区高度且进入保护区，报警
                                  globalData->warn_area_num=i;
                              }
                              else //吊钩高锅保护区
                              {
                                  left_barrier_flg[i]=0;   //左障碍报警标志
                                  right_barrier_flg[i]=0;  //右障碍报警标志
                                  far_barrier_flg[i]=0;    //远障碍碰撞报警标志
                                  near_barrier_flg[i]=0;   //近障碍碰撞报警标志
                                  low_barrier_flg[i]=0;    //低障碍碰撞报警标志
                                  globalData->proZoneDataList[i].currentColor=Pzone_safe_color;                                 //吊钩低于保护区高度且进入保护区，报警
                              }
                          }
                          else
                          {
                              left_barrier_flg[i]=0;   //左障碍报警标志
                              right_barrier_flg[i]=0;  //右障碍报警标志
                              far_barrier_flg[i]=0;    //远障碍碰撞报警标志
                              near_barrier_flg[i]=0;   //近障碍碰撞报警标志
                              low_barrier_flg[i]=0;    //低障碍碰撞报警标志
                              globalData->proZoneDataList[i].currentColor=Pzone_safe_color;                                 //吊钩低于保护区高度且进入保护区，报警
                          }
                      }
                            //-----------------------如果本机塔高比保护区高度不大于safe_distance以上-----------------------------------------
                      else
                      {
                          quint8 result=trigger_left_right_warn_flag(Pzone_type, angle,startAngle,endAngle);
                          switch(result)
                          {
                          case 1:  left_forbid_flg[i]=2;   break;      //左禁行标志
                          case 2:  right_forbid_flg[i]=2;  break;      //右禁行标志
                          case 3:  left_barrier_flg[i]=2;  break;      //左障碍报警标志
                          case 4:  right_barrier_flg[i]=2; break;      //右障碍报警标志
                          }
                          globalData->proZoneDataList[i].currentColor=Pzone_alarm_color;                                 //吊钩低于保护区高度且进入保护区，报警
                          globalData->warn_area_num=i;
                      }
                  }
                  else if(b_angle==1)  //进入预警角度内
                  {
                      if((globalData->craneList.at(0).craneHeight-globalData->proZoneDataList.at(i).proZoneHeight>safe_distance))
                                  //-----------------------如果本机塔高比保护区高度大于5m以上-----------------------------------------
                      {
                          arc_span[0]=globalData->proZoneDataList.at(i).min_span;  //保护区的最小幅度
                          arc_span[1]=globalData->proZoneDataList.at(i).max_span;  //保护区的最大幅度
                          if((span_dis>=arc_span[0])&&(span_dis<=arc_span[1])||(span_dis>= arc_span[2])&&(span_dis<=arc_span[3]))   //保护区内当前变幅下吊钩处于扇形区域内的报警位置判断
                          {
                              if((globalData->craneList.at(0).hookHeight<globalData->proZoneDataList.at(i).proZoneHeight))
                              {
                                  quint8 result=trigger_left_right_warn_flag(Pzone_type, angle,startAngle,endAngle);
                                  switch(result)
                                  {
                                  case 1:  left_forbid_flg[i]=1;   break;      //左禁行标志
                                  case 2:  right_forbid_flg[i]=1;  break;      //右禁行标志
                                  case 3:  left_barrier_flg[i]=1;  break;      //左障碍报警标志
                                  case 4:  right_barrier_flg[i]=1; break;      //右障碍报警标志
                                  }
                                  globalData->proZoneDataList[i].currentColor=Pzone_warning_color;                  //塔臂低于保护区高度且进入预警区，预警
                                  globalData->warn_area_num=i;
                              }
                              else //吊钩高锅保护区
                              {
                                  left_barrier_flg[i]=0;   //左障碍报警标志
                                  right_barrier_flg[i]=0;  //右障碍报警标志
                                  far_barrier_flg[i]=0;    //远障碍碰撞报警标志
                                  near_barrier_flg[i]=0;   //近障碍碰撞报警标志
                                  low_barrier_flg[i]=0;    //低障碍碰撞报警标志
                                  globalData->proZoneDataList[i].currentColor=Pzone_safe_color;                                 //吊钩低于保护区高度且进入保护区，报警
                              }
                          }
                          else
                          {
                              left_barrier_flg[i]=0;   //左障碍报警标志
                              right_barrier_flg[i]=0;  //右障碍报警标志
                              far_barrier_flg[i]=0;    //远障碍碰撞报警标志
                              near_barrier_flg[i]=0;   //近障碍碰撞报警标志
                              low_barrier_flg[i]=0;    //低障碍碰撞报警标志
                              globalData->proZoneDataList[i].currentColor=Pzone_safe_color;                                 //吊钩低于保护区高度且进入保护区，报警
                          }
                      }
                      else
                      {
                          quint8 result=trigger_left_right_warn_flag(Pzone_type, angle,startAngle,endAngle);
                          switch(result)
                          {
                          case 1:  left_forbid_flg[i]=1;   break;      //左禁行标志
                          case 2:  right_forbid_flg[i]=1;  break;      //右禁行标志
                          case 3:  left_barrier_flg[i]=1;  break;      //左障碍报警标志
                          case 4:  right_barrier_flg[i]=1; break;      //右障碍报警标志
                          }
                          globalData->proZoneDataList[i].currentColor=Pzone_warning_color;                  //塔臂低于保护区高度且进入预警区，报警
                          globalData->warn_area_num=i;
                      }
                  }
              }
        }
        else
        {
            if( Pzone_type==0)//禁行区
            {
                left_forbid_flg[i]=0;   //左禁行标志
                right_forbid_flg[i]=0;  //右禁行标志
                far_forbid_flg[i]=0;    //远禁行标志
                near_forbid_flg[i]=0;   //近禁行标志
                globalData->proZoneDataList[i].currentColor=Pzone_safe_color;                                 //吊钩低于保护区高度且进入保护区，报警
            }
            else
            {
                left_barrier_flg[i]=0;   //左障碍报警标志
                right_barrier_flg[i]=0;  //右障碍报警标志
                far_barrier_flg[i]=0;    //远障碍碰撞报警标志
                near_barrier_flg[i]=0;   //近障碍碰撞报警标志
                low_barrier_flg[i]=0;    //低障碍碰撞报警标志
                globalData->proZoneDataList[i].currentColor=Pzone_safe_color;                                 //吊钩低于保护区高度且进入保护区，报警
            }
        }
        globalData->proZoneDataList[i].far_barrier = far_barrier_flg[i];
        globalData->proZoneDataList[i].near_barrier = near_barrier_flg[i];
        globalData->proZoneDataList[i].left_barrier = left_barrier_flg[i];
        globalData->proZoneDataList[i].right_barrier = right_barrier_flg[i];
        globalData->proZoneDataList[i].low_barrier = low_barrier_flg[i];
        globalData->proZoneDataList[i].left_forbid = left_forbid_flg[i];
        globalData->proZoneDataList[i].right_forbid = right_forbid_flg[i];
        globalData->proZoneDataList[i].far_forbid = far_forbid_flg[i];
        globalData->proZoneDataList[i].near_forbid = near_forbid_flg[i];
        //baojing yong biaozhiwei zhiwei
        globalData->far_barrier_warn_flag = far_barrier_flg[i] > globalData->far_barrier_warn_flag ? far_barrier_flg[i]:globalData->far_barrier_warn_flag;
        globalData->near_barrier_warn_flag = near_barrier_flg[i] >globalData->near_barrier_warn_flag? near_barrier_flg[i]:globalData->near_barrier_warn_flag;
        globalData->left_barrier_warn_flag = left_barrier_flg[i]>globalData->left_barrier_warn_flag?  left_barrier_flg[i]:globalData->left_barrier_warn_flag;
        globalData->right_barrier_warn_flag = right_barrier_flg[i]>globalData->right_barrier_warn_flag? right_barrier_flg[i]:globalData->right_barrier_warn_flag;
        globalData->low_barrier_warn_flag = low_barrier_flg[i]>globalData->low_barrier_warn_flag?  low_barrier_flg[i]:globalData->low_barrier_warn_flag;

        globalData->left_forbid_warn_flag = left_forbid_flg[i]>globalData->left_forbid_warn_flag? left_forbid_flg[i]:globalData->left_forbid_warn_flag;
        globalData->right_forbid_warn_flag = right_forbid_flg[i]>globalData->right_forbid_warn_flag? right_forbid_flg[i]:globalData->right_forbid_warn_flag;
        globalData->far_forbid_warn_flag = far_forbid_flg[i]>globalData->far_forbid_warn_flag? far_forbid_flg[i]:globalData->far_forbid_warn_flag;
        globalData->near_forbid_warn_flag = near_forbid_flg[i]>globalData->near_forbid_warn_flag? near_forbid_flg[i]:globalData->near_forbid_warn_flag;
    }

    globalData->ultimate_warn_flag = globalData->ultimate_warn_flag>globalData->left_forbid_warn_flag? globalData->ultimate_warn_flag:globalData->left_forbid_warn_flag;
    globalData->ultimate_warn_flag = globalData->ultimate_warn_flag>globalData->right_forbid_warn_flag? globalData->ultimate_warn_flag:globalData->right_forbid_warn_flag;
    globalData->ultimate_warn_flag = globalData->ultimate_warn_flag>globalData->near_forbid_warn_flag? globalData->ultimate_warn_flag:globalData->near_forbid_warn_flag;
    globalData->ultimate_warn_flag = globalData->ultimate_warn_flag>globalData->far_forbid_warn_flag? globalData->ultimate_warn_flag:globalData->far_forbid_warn_flag;
    globalData->ultimate_warn_flag = globalData->ultimate_warn_flag>globalData->left_barrier_warn_flag? globalData->ultimate_warn_flag:globalData->left_barrier_warn_flag;
    globalData->ultimate_warn_flag = globalData->ultimate_warn_flag>globalData->right_barrier_warn_flag? globalData->ultimate_warn_flag:globalData->right_barrier_warn_flag;
    globalData->ultimate_warn_flag = globalData->ultimate_warn_flag>globalData->near_barrier_warn_flag? globalData->ultimate_warn_flag:globalData->near_barrier_warn_flag;
    globalData->ultimate_warn_flag = globalData->ultimate_warn_flag>globalData->far_barrier_warn_flag? globalData->ultimate_warn_flag:globalData->far_barrier_warn_flag;
    globalData->ultimate_warn_flag = globalData->ultimate_warn_flag>globalData->low_barrier_warn_flag? globalData->ultimate_warn_flag:globalData->low_barrier_warn_flag;
}
/*
*********************************************************************************************************
** 函数名称  ：trigger_left_right_warn_flag()
** 函数功能 ：触发左右禁行报警标志位，与禁行/障碍区报警判断结合,判断报警时是左禁行报警还是右禁行报警
angle：当前角度 stangle：禁行/障碍物扇形起始角度
endangle：禁行/障碍物扇形终止角度   warn_class：1预警，2报警  Pzone_type:保护区类型
** 说明     ：进入报警区后，改程序判断当前塔臂是在报警区内的左半边还是右半边侧，在左则报右禁行警报,向右则深入
**          ： 禁行区，向左退出禁行区，反之亦然
返回： 返回：1、左禁行警报 2、右禁行警报 3、左障碍警报 4、右障碍警报
*********************************************************************************************************
*/

quint8 DataProcess::trigger_left_right_warn_flag(quint8 Pzone_type,float angle,float stangle,float endangle)
{

      float temp_s;
      float temp_e;
      quint8 warn_type=0;
      if(endangle>stangle)       //0°线不在扇形内
      {

            if(angle>endangle||angle<stangle)    //进入扇形区之前
            {
                  if(angle<stangle)
                  {
                        temp_e=fabs(angle+360-endangle);  //结束角半边的角宽度
                        temp_s=fabs(angle-stangle);       //其实角半边的角宽度
                  }
                  else
                  {
                        temp_e=fabs(angle-endangle);      //结束角半边的角宽度
                        temp_s=fabs(stangle+360-angle);   //其实角半边的角宽度
                  }
            }
            else                                          //进入扇形区之后
            {


                  temp_e=fabs(angle-endangle);           //结束角半边的角宽度
                  temp_s=fabs(angle-stangle);            //其实角半边的角宽度

            }
      }
      else                                               //0°线在扇形内,区域被分开
      {

            if(angle>endangle&&angle<stangle)            //进入扇形区之前
            {
                  temp_s=fabs(angle-stangle);
                  temp_e=fabs(angle-endangle);
            }
            else                                         //进入扇形区之后
            {

                  temp_s=fabs(360-stangle+angle);
                  temp_e=fabs(endangle-angle);
            }

      }


      if(Pzone_type==0)                                   //报警类型是禁行报警
      {

            if(temp_e>temp_s)                             //左边比右边的宽，则为左
            {
                  // left_forbid_warn_flage=warn_class;              //左禁行标志
                  warn_type= 1;
            }
            else
            {
                  // right_forbid_warn_flage=warn_class;             //右禁行标志
                  warn_type= 2;
            }


      }
      else     //Pzone_type==1                           //报警类型是障碍物报警
      {

            if(temp_e>temp_s)                               //左边比右边的宽，则为左
            {
                  // left_barrier_warn_flage=warn_class;         //左障碍碰撞报警标志
                  warn_type= 3;
            }
            else
            {
                  //right_barrier_warn_flage=warn_class;        //右障碍碰撞报警标志
                  warn_type= 4;
            }
      }
      return warn_type;
}
/*
*********************************************************************************************************
** 函数名称  trigger_far_near_warn_flag()
** 函数功能 ：触发远近禁行报警标志位，与禁行/障碍区报警判断结合,判断报警时是远禁行报警还是近禁行报警
** 说明     ：进入报警区后，改程序判断当前吊钩是在报警区内的（扇形）内半部分还是外半部分，在内半部分则报
远禁行警报,提示向右则深入禁行区，向内退出禁行区，反之亦然
**    ： 返回：1、近禁行警报 2、远禁行警报 3、近障碍警报 4、远障碍警报
*********************************************************************************************************
*/

quint8 DataProcess::trigger_far_near_warn_flag(quint8 prtc_type,float span,float w_span_s,float w_span_e)
{
      float temp_s;
      float temp_e;
      quint8 warn_type=0;
      temp_s=fabs(span-w_span_s);
      temp_e=fabs(span-w_span_e);

      if(prtc_type==0)                                  //报警类型是禁行报警
      {

            if(span>w_span_s&&span<w_span_e)
            {

                  if(temp_s>temp_e)
                  {
                        // near_forbid_warn_flage=warn_class;   //近禁行标志
                        warn_type=1;
                  }
                  else
                  {
                        //far_forbid_warn_flage=warn_class;    //远禁行标志
                        warn_type= 2;
                  }
            }

      }
      else     //prtc_type==1                           //报警类型是障碍物报警
      {

            if(span>w_span_s&&span<w_span_e)
            {

                  if(temp_s>temp_e)
                  {

                        // near_barrier_warn_flage=warn_class;   //近障碍碰撞报警标志
                        warn_type= 3;
                  }
                  else
                  {
                        //far_barrier_warn_flage=warn_class;   //远障碍碰撞报警标志
                        warn_type= 4;
                  }
            }
      }

      return warn_type;
}

/*
*********************************************************************************************************
** 函数名称  suit()
** 函数功能 ：判断一个角度是否在指定的起点终点范围之内(报警区域内），如果在则返回1，不在则返回0
** 说明     ：入口参数：angle 待判段角度  结构体 protect_data protect,  pre_warn_v ：缓冲角宽度
** 返回值   ： 0：没有进入区域内，1：进入保护区外的预警角度区  2：进入保护区外的报警角度区 3、进入保护区角度内
*********************************************************************************************************
*/
int DataProcess::suit(float angle,float start_angle,float end_angle,float yujing_band,float baojing_band)       //判断一个角度是否在指定的起点终点范围之内
{
     // float start_angle=protect.stangle;
   //   float end_angle=protect.endangle;
      float angle1,angle2;                                  //预警区域起始终止角度
      if(start_angle<=end_angle)
      {
            angle1=start_angle;
            angle2=end_angle;
            //------进入保护区角度内------
            if((angle1)>=0&&(angle2)<=360)
            {
                  if(angle<=angle2&&angle>=angle1)
                  {
                        return 3;                                      //返回为真；
                  }
            }
            else if(angle1>=0&&angle2>360)
            {
                  if((angle<=360&&angle>=angle1)||(angle<=(angle2-360)&&angle>=0))
                  {
                        return 3;
                  }
            }
            else if(angle1<0&&angle2<=360)
            {
                  if((angle<=360&&angle>=(angle1+360))||(angle<=angle2&&angle>=0))
                  {
                        return 3;
                  }
            }

            angle1=start_angle-baojing_band;
            angle2=end_angle+baojing_band;


            //------若没进入保护区角度内，进入保护区外的报警角度内------
            if((angle1)>=0&&(angle2)<=360)
            {
                  if(angle<=angle2&&angle>=angle1)
                  {
                        return 2;                                      //返回为真；
                  }
            }
            else if(angle1>=0&&angle2>360)
            {
                  if((angle<=360&&angle>=angle1)||(angle<=(angle2-360)&&angle>=0))
                  {
                        return 2;
                  }
            }
            else if(angle1<0&&angle2<=360)
            {
                  if((angle<=360&&angle>=(angle1+360))||(angle<=angle2&&angle>=0))
                  {
                        return 2;
                  }
            }


            //------若没进入报警角度内，进入保护区外的预警角度内------

            angle1=start_angle-yujing_band-baojing_band;
            angle2=end_angle+yujing_band+baojing_band;

            if((angle1)>=0&&(angle2)<=360)
            {
                  if(angle<=angle2&&angle>=angle1)
                  {
                        return 1;                                      //返回为真；
                  }
            }
            else if(angle1>=0&&angle2>360)
            {
                  if((angle<=360&&angle>=angle1)||(angle<=(angle2-360)&&angle>=0))
                  {
                        return 1;
                  }
            }
            else if(angle1<0&&angle2<=360)
            {
                  if((angle<=360&&angle>=(angle1+360))||(angle<=angle2&&angle>=0))
                  {
                        return 1;
                  }
            }
      }
      else
      {
            //------进入保护区角度内------
            angle1=start_angle;
            angle2=end_angle;
            if((angle<=360&&angle>=angle1)||(angle<=angle2&&angle>=0))
            {
                  return 3;
            }

            //------若没进入保护区角度内，进入保护区外的报警角度内------
            angle1=start_angle-baojing_band;
            angle2=end_angle+baojing_band;
            if((angle<=360&&angle>=angle1)||(angle<=angle2&&angle>=0))
            {
                  return 2;
            }
            //------若没进入报警角度内，进入保护区外的预警角度内------
            angle1=start_angle-yujing_band-baojing_band;
            angle2=end_angle+yujing_band+baojing_band;
            if((angle<=360&&angle>=angle1)||(angle<=angle2&&angle>=0))
            {
                  return 1;
            }
      }
      return 0;
}
/*
*********************************************************************************************************
** 函数名称  suit_baojin()
** 函数功能 ：判断一个角度是否在指定的起点终点范围之内(报警区域内），如果在则返回1，不在则返回0
** 说明     ：
**          ：
*********************************************************************************************************
*/
char DataProcess::suit_baojin(float angle,float stangle,float endangl)
{

      if (stangle>endangl)
      {
            if ((angle>=stangle)&&(angle<=360)||(angle>=0)&&(angle<=endangl))
            {
                  return 1;
            }
            else return 0;
      }
      else
      {
            if ((angle>=stangle)&&angle<=endangl)
            {
                  return 1;
            }
            else if (angle==0&&endangl==360)
            {
                  return 1;
            }
            else
            {
                  return 0;
            }
      }
}

/*
*********************************************************************************************************
** 函数名称  max_min_protect_area()
** 函数功能 ：计算保护区相对塔吊x，y最大最小的值
** 说明     ：
**          ：max_min[0]表示x最小值， max_min[1]表示x最大值， max_min[2]表示y最小值， max_min[3]表示y最大值，
*********************************************************************************************************
*/
void DataProcess::max_min_protect_area(char num,char *data ,int len,float*max_min)   //换保护区
{

      int i=0;

      int point[15][2];             //定义点位置坐标

      float x;
      float y;
      float r;
      float stangle;
      float endangle;
      int m=0;                      //定义有多少个点

      char temp2=data[i*200+3];
      temp2=temp2&0x0F;
      int element_num=temp2;

      for(int j=0;j<element_num;j++)
      {

            if(data[i*200+j*11+24]==0)
            {
                  if(j==0)
                  {
                        char temp1,temp2;
                        int temp3;
                        temp1=data[i*200+j*11+25];
                        temp2=data[i*200+j*11+26];
                        temp3=(temp1<<8)+temp2;
                        point[m][0]=(int)((temp3)/10);   //点X坐标


                        temp1=data[i*200+j*11+27];
                        temp2=data[i*200+j*11+28];
                        temp3=(temp1<<8)+temp2;
                        point[m][1]=(int)((temp3)/10);    //点Y坐标
                        m++;
                  }
                  else
                  {
                        char temp1,temp2;
                        int temp3;

                        temp1=data[i*200+j*11+25];
                        temp2=data[i*200+j*11+26];

                        temp3=(temp1<<8)+temp2;
                        point[m][0]=(int)((temp3)/10);   //点X坐标


                        temp1=data[i*200+j*11+27];
                        temp2=data[i*200+j*11+28];
                        temp3=(temp1<<8)+temp2;
                        point[m][1]=(int)((temp3)/10);    //点Y坐标
                        m++;
                  }
            }
            else if(data[i*200+j*11+24]==1)
            {
                  char temp1,temp2;
                  int temp3;
                  temp1=data[i*200+j*11+25];
                  temp2=data[i*200+j*11+26];

                  temp3=(temp1<<8)+temp2;
                  x=(temp3)/10;



                  temp1=data[i*200+j*11+27];
                  temp2=data[i*200+j*11+28];

                  temp3=(temp1<<8)+temp2;
                  y=(temp3)/10;


                  temp1=data[i*200+j*11+29];
                  temp2=data[i*200+j*11+30];

                  r=((temp1<<8)+temp2)/10;     //保护区圆弧半径r

                  temp1=data[i*200+j*11+31];
                  temp2=data[i*200+j*11+32];

                  stangle=((temp1<<8)+temp2)/10;     //保护区圆弧起点角度

                  temp1=data[i*200+j*11+33];
                  temp2=data[i*200+j*11+34];

                  endangle=((temp1<<8)+temp2)/10;     //保护区圆弧起点角度


                  int temp_angle;                           //定义一个临时角度
                  temp_angle=(int)(stangle*TVECT);

                  point[m][0]=(int)(x+r*cos_tab[temp_angle]);   //圆弧起点位置
                  point[m][1]=(int)(y+r*sin_tab[temp_angle]);

                  m++;

                  temp_angle=(int)(endangle*TVECT);

                  point[m][0]=(int)(x+r*cos_tab[temp_angle]);   //圆弧终点位置
                  point[m][1]=(int)(y+r*sin_tab[temp_angle]);
                  m++;
            }
      }
      if(m<1)
      {
            return;
      }
      max_min[0]=max_min[1]=point[0][0];
      max_min[2]=max_min[3]=point[0][1];
      for(int i=0;i<m;i++)
      {
            if(point[i][0]<max_min[0])  //如果x坐标小于最小x坐标
            {
                  max_min[0]=point[i][0];
            }
            if(point[i][0]>max_min[1])//如果x坐标大于最大x坐标
            {
                  max_min[1]=point[i][0];
            }

            if(point[i][1]<max_min[2])
            {
                  max_min[2]=point[i][1];
            }
            if(point[i][1]>max_min[3])
            {
                  max_min[3]=point[i][1];
            }
      }
}



//20141117   吊重超载判断
//20141118
/*
 *@入口参数：spanArray   weightArray
 *@出口参数：成功则返回true,否则 false
 *
 *@  功能  ： 从力矩表中读取变幅/吊重数据
**/
void  DataProcess::readMomentData()
{
        file = new QFile();
        switch( globalData->basicData.craneRate )
        {
            case 2:   //查看数据库表，配置过来的数据为0 ，测试时暂且使用0先读出数组
                    if(file->exists("momenttable2.txt"))
                        {
                            file->setFileName("momenttable2.txt");
                            //qDebug()<<"assign file to momenttable2";
                        }
                    else
                        {
                            qDebug()<<"@ read momentDATA  file not exits";
                            return;
                        }
                    break;

            case 4:
                    if(file->exists("momenttable4.txt"))
                        {
                            file->setFileName("momenttable4.txt");
                            //qDebug()<<"assign file to momenttable4";
                        }
                    else
                        {
                            qDebug()<<"@ read momentDATA  file not exits";
                            return ;
                        }
                        break;

            default:
                qDebug()<<"******  globalData->basicData.craneRate***NO***match***";

        }
        file->open(QIODevice::ReadOnly);
        quint8  temArrayLen = 100;
        char  temArray[temArrayLen] ;
        memset(temArray,0,temArrayLen);
        int  readlen = 0;
        if( ( readlen = file->read(temArray,temArrayLen)) == -1 )
        {
                qDebug()<<"read moment X .txt failed";
                return ;
        }
        else
        {
            tableLen = (readlen-1)/4;
//                qDebug()<<"read length  :        "<<readlen;
//                for(int i=0; i<readlen; ++i)
//                    qDebug()<<"      "<<i<<"        "<<(int)temArray[i];
        }

        if(tableLen > TABLE_DATA_LEN)
            tableLen = TABLE_DATA_LEN;
        spanArray = new quint32 [tableLen ];
        quint32 *weightArray = new quint32 [tableLen ];
        table_weight_double = new double[tableLen];
        //double weightArrayD[temArrayLen] ;
        for(int j=1,m=0; j<readlen; j+=4,m++)
        {
             spanArray[m]   = ( ( temArray[j]<<8 ) + temArray[j+1] ) ;
             weightArray[m] = ( ( temArray[j+2]<<8 ) + temArray[j+3] )  ;
             //qDebug()<<"    "<<m<<"     "<<spanArray[m]<<"      "<<weightArray[m];
        }


       // double table_weight_double[tableLen] ;
        memset(table_weight_double,0,tableLen);
        for( int i=0; i<tableLen; ++i  )
        {
            spanArray[i] = spanArray[i] / 10;
            table_weight_double[i] = weightArray[i]/1000.0;
           // qDebug()<<"\t "<<i<<"\t"<<table_span[i]<<"\t"<<table_weight_double[i];
        }

       readTableFlag = true;

}


/*
* @入口参数：spanArray[] , spanLen, key
* @出口参数：index of real_time span(double) in the spanArray
*
* @返回值 ：
*/
    int  DataProcess::binSearch(quint32 spanArray [] ,int arrayLen, double key)
    {
         int left = 0,
              right = arrayLen -1,
              mid = 0;
         while( (right - left) > 1  )
         {
               mid = ( left + right )/2;

               if(key == spanArray[mid])
                   return mid;
               else if( key<spanArray[mid])
                   right = mid ;
               else
                   left = mid ;
         }
         return right;//向下取整，保证阈值是安全吊重
    }















