#include "dialog.h"
//#include "zigbeecfg.h"


Dialog *Dialog::m_DialogData = NULL;
Dialog::Dialog(QWidget *parent) :
    QDialog(parent)
{

    /*界面初始化*/
    globalData = GlobalData::getGlobalData();//初始化数据，防止多线程危险（暂时方法）


   // QFont font;
    interfaceInit();
    //paintInit();
     /*定义定时器,定时刷新显示*/
    //QTimer *timer1 = new QTimer(this);

    //00002
//    DataProcess *dataProcess = new DataProcess(this);           //数据采集与处理线程
//    dataProcess->start();

    //00003     @1119  test  the module of  dataprocess_overWeightJudge()
//    GprsConnect *gprs_start = GprsConnect::Instance();
//    gprs_start->start();

    //00004
//    VoicePlay *voicePlay = new VoicePlay(this);           //语音播放处理线程
//    voicePlay->start();

    //00005                                                                                     // 数据接收
//    ZigbeeReceive *zigbeeReceive = new ZigbeeReceive();
//    zigbeeReceive->start();

    //00001
//    DataGather *datagather = new DataGather();
//    datagather->start();


//    connect(setButton , SIGNAL( clicked() ) , this , SLOT(setButtonClicked()));
        connect(ChangeTab , SIGNAL( clicked() ),this,SLOT(ChangeTabButtonClicked()));


//    //00006
//    SerialSendData *serialSendData = new SerialSendData();//串口发送线程实例化
//    serialSendData->start();

//    connect(zigbeeReceive,SIGNAL(reqest_Info(quint8,quint32)),serialSendData,SLOT(sendInfo_afterRequest(quint8,quint32)));
//    connect(zigbeeReceive,SIGNAL(transimit_Info(quint8,quint32,quint32)),serialSendData,SLOT(storeInfo_afterRecv(quint8,quint32,quint32)));
//   connect(zigbeeReceive,SIGNAL(resetDevice()),serialSendData,SLOT(ResetDevice()));

   //00007              @1128      指纹线程   待测试
//   Finger_Gather  *fingergather = new Finger_Gather();
//   fingergather->start();


}

/*绘图事件函数*/

void Dialog::paintEvent(QPaintEvent *event)
{
    QDialog::paintEvent(event);

    //绘制保护区
    QPainter proPainter(this);
    QBrush proZoneBrush;
    proZoneBrush.setStyle(Qt::Dense4Pattern);
    proPainter.setRenderHint(QPainter::Antialiasing , true);                 //使绘制时边缘平滑
    proPainter.setPen(QPen((QBrush)(Qt::black),2,Qt::DotLine,Qt::RoundCap));
    for (quint8 i = 0 ; i < globalData->proZoneDataList.count() ; i++)
    {
        proZoneBrush.setColor(globalData->proZoneDataList.at(i).currentColor);
        //proZoneBrush.setColor(Qt::red);
        proPainter.setBrush(proZoneBrush);
        proPainter.drawPath(globalData->proZoneDataList.at(i).drawPath);
    }
    //绘制相干塔吊

    QPainter cranePainter(this);
    cranePainter.setRenderHint(QPainter::Antialiasing , true);                 //使绘制时边缘平滑

    for (quint8 j = 0 ; j < globalData->craneList.count() ; j++)
    {
        cranePainter.setPen(QPen((QBrush)(globalData->craneList[j].paintColor),2,Qt::DotLine,Qt::RoundCap));
        //cranePainter.setPen(QPen((QBrush)(Qt::red),2,Qt::DotLine,Qt::RoundCap));
        cranePainter.drawPath(globalData->craneList.at(j).drawPath);

           //绘制半径塔臂,先做坐标旋转，画完恢复坐标系
        cranePainter.save();   //保存坐标系现在状态
        cranePainter.setPen(QPen((QBrush)(globalData->craneList[j].paintColor),10,Qt::SolidLine,Qt::RoundCap));
      // cranePainter.setPen(QPen((QBrush)(Qt::red),10,Qt::SolidLine,Qt::RoundCap));
        cranePainter.drawPoint(globalData->craneList[j].drawCenter);
        cranePainter.translate(globalData->craneList[j].drawCenter.rx(),
                              globalData->craneList[j].drawCenter.ry());            //平移原点到画圆圆心
        cranePainter.rotate(-(globalData->craneList[j].angle));                        //坐标顺时针旋转（角度取负值，规定逆时针为正）
        cranePainter.setPen(QPen((QBrush)(globalData->craneList[j].paintColor),2,Qt::SolidLine,Qt::RoundCap));
        cranePainter.drawLine(-(globalData->craneList[j].drawBackLen),0,globalData->craneList[j].drawForeLen,0);            //画半径
        cranePainter.setPen(QPen((QBrush)(globalData->craneList[j].paintColor),10,Qt::SolidLine,Qt::RoundCap));
        cranePainter.drawPoint(globalData->craneList[j].span * globalData->scaleValue,0);                  //绘制变幅点

        cranePainter.restore();                                                                //恢复坐标系状态
    }


    //未下发配置数据时，默认显示数据 FF
    if (globalData->craneList.count() > 0)
        {
               bool dataConfiged(false);//参数是否被配置
               if(dataConfiged)
               {
                   //const QString  *defaultValue("FFFF");
                   const int defaultValue(0);

                   LCDNumberAngle->display(defaultValue);                   //实时显示角度值
                   LCDNumberWeight->display(defaultValue);                  //实时显示吊重
                   LCDNumberSpan->display(defaultValue);                    //实时显示变幅值
                   LCDNumberHeight->display(defaultValue);         //实时显示吊钩高度 defalut  50
                   LCDNumberWind->display(defaultValue);            //风速
                   LCDNumberX->display(defaultValue);                   //倾角
                   LCDNumberY->display(defaultValue);                   //倾角
               }
               else
               {
                   LCDNumberAngle->display(globalData->craneList[0].angle);  //实时显示角度值
                   LCDNumberAngle->setNumDigits(4);
                   LCDNumberWeight->display(globalData->craneList[0].wight);  //实时显示吊重
                   LCDNumberWeight->setNumDigits(3);
                   LCDNumberSpan->display(globalData->craneList[0].span);   //实时显示变幅值
                   LCDNumberSpan->setNumDigits(3);
                   LCDNumberHeight->display(globalData->craneList[0].hookHeight);//高度
                   LCDNumberHeight->setNumDigits(3);
                   LCDNumberWind->display(globalData->craneList[0].windspeed);//风速
                   LCDNumberWind->setNumDigits(2);
                   LCDNumberX->display(globalData->craneList[0].tiltangleX);//倾角
                   LCDNumberX->setNumDigits(3);
                   LCDNumberY->display(globalData->craneList[0].tiltangleY);//倾角
                   LCDNumberY->setNumDigits(3);
                   LCDNumberMoment->display(globalData->craneList[0].moment);//力矩
                   LCDNumberMoment->setNumDigits(2);
                }


        }

    if(globalData->status_success == 1)
        itemsLabelLinkdis->setText(globalData->tc->toUnicode("已连接"));
}

/*定时器1槽函数*/
void Dialog::timer1UpDate()
{

   //momentBar->setValue(80);

   Dialog::update();
   //globalData->craneList[0].angle = globalData->craneList[0].angle -10;      //测试多线程

 }

/*定时器2槽函数,做数据处理更新*/
void Dialog::timer2UpDate()
{


 }

/*设置按键槽函数*/
void Dialog::setButtonClicked()
{
    qDebug()<< "run in the setbuttonclicked()__SLOT";
    TabDialog *tab = new TabDialog;
   // system("killall -9 madplay");
    tab->show();
    tab->exec();
}

void Dialog::ChangeTabButtonClicked(){
    camTab = new CamTab;
    camTab->show();
//    camTab->DisplayVideo();
   // camTab->exec();
}

void Dialog::ChangeCamButtonClicked(){
   // camTab->~CamTab();
    //camTab->timer->stop();
   // camTab->Video->CloseDevice();
    //camTab->uvideo->CloseDevice();
    bool stopJudge = false;
    int count = 300;
    while( (count-- > 0) && (camTab != NULL) ){
        if(stopJudge == false){
      //  camTab->uvideo->stopThread = true;
      //  camTab->uvideo->exit();
       // if(camTab->uvideo->isFinished() == true){
            delete camTab;
            camTab = NULL;
            stopJudge = true;
            break;
   //     }
        }
    }
    if(stopJudge == true)
        qDebug()<<"Close Success!";
    else
        qDebug()<<"Close Failed!";
    //camTab->close();
    //delete camTab->FrameBuffer;
    //delete camTab->Video;
    //delete camTab;
}

/*指纹按键槽函数*/
void Dialog::fingerButtonClicked()
{
}

void Dialog::interfaceInit(void)
{
    QWidget::setFixedSize(SCREEN_W,SCREEN_H);
    Dialog::setWindowFlags(Qt::FramelessWindowHint); //  去掉标题栏
    /*第一层的Title*/
    //QLabel *TitleLabel = new QLabel(tr("起重式塔吊监控系统"));
    QLabel *TitleLabel = new QLabel(globalData->tc->toUnicode("起重式塔吊监控系统"));
    ChangeTab = new QPushButton(globalData->tc->toUnicode("视频切换"));
    camTab = NULL;
    QFont font;
    font.setPointSize(24);
    font.setFamily(("wenquanyi"));
    font.setBold(true);
    TitleLabel->setFont(font);
    TitleLabel->setFrameStyle(QFrame::NoFrame | QFrame::NoFrame);
    TitleLabel->setAlignment(Qt::AlignTop | Qt::AlignCenter);//

    /*第二层左边buttons 组*/
    //QButtonGroup *buttonGroup = new QButtonGroup;

    QLabel *itemsLabelChannel = new QLabel(globalData->tc->toUnicode("无线信道"));
    QLabel *itemsLabelID = new QLabel(globalData->tc->toUnicode("无线ID"));
    QLabel *itemsLabelAddr = new QLabel(globalData->tc->toUnicode("节点地址"));
    QLabel *itemsLabelLink = new QLabel(globalData->tc->toUnicode("入网状态"));
    LCDNumberChannel = new QLCDNumber();
    LCDNumberChannel->setSegmentStyle (QLCDNumber::Flat);
  //  LCDNumberChannel->setStyleSheet("QLCDNumber{color:white;background:black}");
    LCDNumberChannel->setDigitCount(4);
    if(!globalData->craneKD.ZigbeeChannel.isEmpty())
             LCDNumberChannel->display(globalData->craneKD.ZigbeeChannel);


    LCDNumberID = new QLCDNumber();
    LCDNumberID->setSegmentStyle (QLCDNumber::Flat);
  //  LCDNumberID->setStyleSheet("QLCDNumber{color:white;background:black}");
    LCDNumberID->setDigitCount(4);
    if(!globalData->craneKD.ZigbeeID.isEmpty())
             LCDNumberID->display(globalData->craneKD.ZigbeeID);

    LCDNumberAddr = new QLCDNumber();
    LCDNumberAddr->setSegmentStyle (QLCDNumber::Flat);
 //   LCDNumberAddr->setStyleSheet("QLCDNumber{color:white;background:black}");
    LCDNumberAddr->setDigitCount(4);

      itemsLabelLinkdis =new QLabel(globalData->tc->toUnicode("未连接"));
     // itemsLabelLinkdis->text();
  //  LCDNumberLink = new QLCDNumber();
  //  LCDNumberLink->setSegmentStyle (QLCDNumber::Flat);
//    LCDNumberLink->setStyleSheet("QLCDNumber{color:white;background:black}");
  //  LCDNumberLink->setDigitCount(4);
   // LCDNumberLink->display("OFF");


    //第一层布局
    QGridLayout *linkLayout = new QGridLayout();
    linkLayout->addWidget(itemsLabelAddr,0,0,1,1,Qt::AlignCenter);
    linkLayout->addWidget(LCDNumberAddr,0,1,1,1,Qt::AlignCenter);

    linkLayout->addWidget(itemsLabelChannel,1,0,1,1,Qt::AlignCenter);
    linkLayout->addWidget(LCDNumberChannel,1,1,1,1,Qt::AlignCenter);

    linkLayout->addWidget(itemsLabelID,2,0,1,1,Qt::AlignCenter);
    linkLayout->addWidget(LCDNumberID,2,1,1,1,Qt::AlignCenter);

    linkLayout->addWidget(itemsLabelLink,3,0,1,1,Qt::AlignCenter);
    linkLayout->addWidget(itemsLabelLinkdis,3,1,1,1,Qt::AlignCenter);
   // linkLayout->addWidget(LCDNumberLink,3,1,1,1,Qt::AlignCenter);
    QGroupBox *linkGroup = new QGroupBox(globalData->tc->toUnicode("联网参数"));
    linkGroup->setLayout(linkLayout);


    //第二层布局左边
   // QButtonGroup *buttongroup = new QButtonGroup();
    setButton = new QPushButton(globalData->tc->toUnicode("设置"));

    fingerButton = new QPushButton(globalData->tc->toUnicode("指纹"));
  //  buttongroup->addButton(setButton);
  //  buttongroup->addButton(fingerButton);

    QGridLayout *linkGridLayout = new QGridLayout();
    linkGridLayout->addWidget(linkGroup,0,0,4,2);
    linkGridLayout->addWidget (setButton,4,0,1,1);
    linkGridLayout->addWidget (fingerButton,4,1,1,1);
   // setLayout(linkGridLayout);




   /*第二层的右边数据显示*/
    //数据显示布局
    QLabel *itemsLabelAngle = new QLabel(globalData->tc->toUnicode("回转"));
    QLabel *itemsLabelWeight = new QLabel(globalData->tc->toUnicode("吊重"));
    QLabel *itemsLabelMoment = new QLabel(globalData->tc->toUnicode("力矩"));
    QLabel *itemsLabelSpan = new QLabel(globalData->tc->toUnicode("变幅"));
    QLabel *itemsLabelHeight = new QLabel(globalData->tc->toUnicode("高度"));
    QLabel *itemsLabelX = new QLabel(globalData->tc->toUnicode("X倾角"));
    QLabel *itemsLabelY = new QLabel(globalData->tc->toUnicode("Y倾角"));
    QLabel *itemsLabelWind = new QLabel(globalData->tc->toUnicode("风速"));

    QLabel *itemsLabelAngleU = new QLabel(globalData->tc->toUnicode("度"));
    QLabel *itemsLabelWeightU = new QLabel(globalData->tc->toUnicode("吨"));
    QLabel *itemsLabelSpanU = new QLabel(globalData->tc->toUnicode("米"));
    QLabel *itemsLabelHeightU = new QLabel(globalData->tc->toUnicode("米"));
    QLabel *itemsLabelXU = new QLabel(globalData->tc->toUnicode("度"));
    QLabel *itemsLabelYU = new QLabel(globalData->tc->toUnicode("度"));
    QLabel *itemsLabelWindU = new QLabel(globalData->tc->toUnicode("级"));
    QLabel *itemsLabelMometU = new QLabel(globalData->tc->toUnicode("%"));

//    momentBar = new QProgressBar();
//    momentBar->setRange(0,100);      //设置表示范围
    //momentBar->setStyleSheet("QProgressBar::chunk{\background:blue;margin:1.0px;height:3px;}QProgressBar{border:6px solid red;background:black}");

    LCDNumberAngle = new QLCDNumber();
    LCDNumberAngle->setSegmentStyle (QLCDNumber::Flat);
    LCDNumberAngle->setDigitCount(6);
    //LCDNumberAngle->setNumDigits(4);

    LCDNumberWeight = new QLCDNumber();
    LCDNumberWeight->setSegmentStyle (QLCDNumber::Flat);
    LCDNumberWeight->setDigitCount(6);
    //LCDNumberWeight->setNumDigits(3);

    //@1127   momnet   LCDNumberMoment
    LCDNumberMoment = new QLCDNumber();
    LCDNumberMoment->setSegmentStyle (QLCDNumber::Flat);
    LCDNumberMoment->setDigitCount(6);
    //LCDNumberMoment->setNumDigits(2);

    LCDNumberSpan = new QLCDNumber();
    LCDNumberSpan->setSegmentStyle (QLCDNumber::Flat);
    LCDNumberSpan->setDigitCount(6);
    //LCDNumberSpan->setNumDigits(3);

    LCDNumberHeight = new QLCDNumber();
    LCDNumberHeight->setSegmentStyle (QLCDNumber::Flat);
    LCDNumberHeight->setDigitCount(6);
    //LCDNumberHeight->setNumDigits(3);

    LCDNumberX = new QLCDNumber();
    LCDNumberX->setSegmentStyle (QLCDNumber::Flat);
    LCDNumberX->setDigitCount(6);
    //LCDNumberX->setNumDigits(2);

    LCDNumberY = new QLCDNumber();
    LCDNumberY->setSegmentStyle (QLCDNumber::Flat);
    LCDNumberY->setDigitCount(6);
    //LCDNumberY->setNumDigits(2);

    LCDNumberWind = new QLCDNumber();
    LCDNumberWind->setSegmentStyle (QLCDNumber::Flat);
    LCDNumberWind->setDigitCount(6);
    //LCDNumberWind->setNumDigits(2);


    QGridLayout *DataLayout = new QGridLayout();
    DataLayout->addWidget(itemsLabelAngle,0,0,1,1,Qt::AlignCenter);
    DataLayout->addWidget(LCDNumberAngle,0,1,1,1,Qt::AlignCenter);
    DataLayout->addWidget(itemsLabelAngleU,0,2,1,1,Qt::AlignCenter);

    DataLayout->addWidget(itemsLabelWeight,1,0,1,1,Qt::AlignCenter);
    DataLayout->addWidget(LCDNumberWeight,1,1,1,1,Qt::AlignCenter);
    DataLayout->addWidget(itemsLabelWeightU,1,2,1,1,Qt::AlignCenter);

    DataLayout->addWidget(itemsLabelMoment,2,0,1,1,Qt::AlignCenter);
    DataLayout->addWidget(LCDNumberMoment,2,1,1,1,Qt::AlignCenter);
    DataLayout->addWidget(itemsLabelMometU,2,2,1,1,Qt::AlignCenter);

    DataLayout->addWidget(itemsLabelSpan,3,0,1,1,Qt::AlignCenter);
    DataLayout->addWidget(LCDNumberSpan,3,1,1,1,Qt::AlignCenter);
    DataLayout->addWidget(itemsLabelSpanU,3,2,1,1,Qt::AlignCenter);

    DataLayout->addWidget(itemsLabelHeight,4,0,1,1,Qt::AlignCenter);
    DataLayout->addWidget(LCDNumberHeight,4,1,1,1,Qt::AlignCenter);
    DataLayout->addWidget(itemsLabelHeightU,4,2,1,1,Qt::AlignCenter);

    DataLayout->addWidget(itemsLabelX,5,0,1,1,Qt::AlignCenter);
    DataLayout->addWidget(LCDNumberX,5,1,1,1,Qt::AlignCenter);
    DataLayout->addWidget(itemsLabelXU,5,2,1,1,Qt::AlignCenter);

    DataLayout->addWidget(itemsLabelY,6,0,1,1,Qt::AlignCenter);
    DataLayout->addWidget(LCDNumberY,6,1,1,1,Qt::AlignCenter);
    DataLayout->addWidget(itemsLabelYU,6,2,1,1,Qt::AlignCenter);

    DataLayout->addWidget(itemsLabelWind,7,0,1,1,Qt::AlignCenter);
    DataLayout->addWidget(LCDNumberWind,7,1,1,1,Qt::AlignCenter);
    DataLayout->addWidget(itemsLabelWindU,7,2,1,1,Qt::AlignCenter);
    //DataLayout->setHorizontalSpacing(10);
    //DataLayout->setVerticalSpacing(10);
    DataLayout->setContentsMargins(10,10,10,10);                        //设置布局四边的空隙
    QGroupBox *GroupBox = new QGroupBox(globalData->tc->toUnicode("运行参数"));
    GroupBox->setLayout(DataLayout);


/*中层画图布局，由左往右，buttons + Spacing + GroupBox*/
    QHBoxLayout *PaintLayout = new QHBoxLayout;
    //PaintLayout->addWidget(buttonGroup);
    //PaintLayout->addStretch(1);
    PaintLayout->addLayout(linkGridLayout);
    PaintLayout->addStretch(1);
    PaintLayout->addSpacing(PAINT_W);
    PaintLayout->addStretch(1);
    PaintLayout->addWidget(GroupBox);

/*全界面布局，由上往下*/
    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(TitleLabel,0,0,1,4);
    mainLayout->addWidget(ChangeTab,0,4,1,1);
    mainLayout->addLayout(PaintLayout,1,0,6,5);
    setLayout(mainLayout);
}



