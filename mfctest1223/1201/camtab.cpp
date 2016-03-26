#include "camtab.h"

#define image_width  640  //图片显示宽度
#define image_height 480  //图片显示高度
#define image_Format      QImage::Format_RGB888  //图片显示格式

#include "sys/time.h"


CamTab::CamTab(QWidget *parent) :
    QDialog(parent)
{
    globalData = GlobalData::getGlobalData();
    Dialog *w = Dialog::getDiaglogData();
   GprsConnect *gprs = GprsConnect::Instance();
   //@1225 图片帧
    imgBuf = (unsigned char *)malloc(image_width * image_height * 3 * sizeof(char));
    frame = new QImage(imgBuf,image_width,image_height,image_Format);

    interfaceInit();
    connect(ChangeTab , SIGNAL(clicked()),w , SLOT(ChangeCamButtonClicked()));
    connect(timer , SIGNAL(timeout()) , this , SLOT(DisplayVideo()));
    connect(TakePhoto , SIGNAL(clicked()) , this , SLOT(TakePhotoClick()));
    connect(DeletePhoto , SIGNAL(clicked()) , this ,SLOT(DeletePhotoClick()));
    connect(RecordVideo , SIGNAL(clicked()) , this , SLOT(RecordClick()));
    connect(RecordDisplay , SIGNAL(clicked()) , this , SLOT(RecordClickDisplay()));

    connect(SendPhoto , SIGNAL(clicked()) , gprs , SLOT(SendPhoto()));
  //  connect(RecordStop , SIGNAL(clicked()) , this , SLOT(RecordStopClick()));
    connect(mfctest , SIGNAL(clicked()) , this ,SLOT(mfcconvertClick()) );
  //  connect(gprs->tcppicsender , SIGNAL(connected()) , this , SLOT(SendPhotoAvailable()));
}

CamTab::~CamTab(){
    timer->stop();

    delete Video;
    Video = NULL;

    delete uvideo;
    uvideo = NULL;

    delete mfc;
    mfc = NULL;

}

void CamTab::interfaceInit(){
    QWidget::setFixedSize(SCREEN_W,SCREEN_H);
    CamTab::setWindowFlags(Qt::FramelessWindowHint); //  去掉标题栏
    QLabel *CamName = new QLabel(globalData->tc->toUnicode("实时视频图像"));
    DisLabel = new QLabel;
    timer = new QTimer(this);
    ChangeTab  = new QPushButton(globalData->tc->toUnicode("界面切换"));
    TakePhoto = new QPushButton(globalData->tc->toUnicode("拍照"));
    DeletePhoto = new QPushButton(globalData->tc->toUnicode("删除"));
    SendPhoto = new QPushButton(globalData->tc->toUnicode("发送"));
    RecordVideo = new QPushButton(globalData->tc->toUnicode("录制"));
    RecordDisplay = new QPushButton(globalData->tc->toUnicode("录制显示"));
   // RecordStop = new QPushButton(globalData->tc->toUnicode("停止"));
    mfctest = new QPushButton(globalData->tc->toUnicode("MFC测试"));
    //RecordStop->hide();
    WhetherRecord = true;
    SendPhoto->setEnabled(globalData->sendphotoavailable);
   // FrameBuffer = new TFrameBuffer;
    //@1225
    //Video = new TVideo;
    Video = NULL;
    uvideo = NULL;
    //uvideo = new record;
  //  uvideo->start();
    mfc = NULL;
    //mfc = new MFC;
    QGridLayout *mainlayout = new QGridLayout;
    mainlayout->addWidget(CamName ,0,0,1,4);
    mainlayout->addWidget(ChangeTab,0,4,1,1);
    mainlayout->addWidget(TakePhoto,1,4,1,1);
    mainlayout->addWidget(SendPhoto,2,4,1,1);
    mainlayout->addWidget(DeletePhoto,3,4,1,1);
    mainlayout->addWidget(DisLabel,1,0,6,4);
    mainlayout->addWidget(RecordVideo,4,4,1,1);
    //mainlayout->addWidget(RecordStop,4,4,1,1);
    mainlayout->addWidget(mfctest , 5,4,1,1);
    mainlayout->addWidget(RecordDisplay,6,4,1,1);
    setLayout(mainlayout);
//    if(Video->IsValid())
//    {
//        timer->start(25);
//    }

}

int CamTab::DisplayVideo(){
    try {

            if (Video->WaitPic()) //设备可读即返回
            {
                if (Video->FetchPicture())
                {
                   // qDebug()<<"run here";
                    QImage image((const uchar*)Video->Addr , Video->Width , Video->Height ,QImage::Format_RGB888);
                    DisLabel->setPixmap(QPixmap::fromImage(image,Qt::AutoColor));
                    //FrameBuffer.DrawRect(Video);
                }
            }
        }
 catch (TError &e) {
        e.Output();
        return 1;
    }
}

int CamTab::TakePhotoClick(){
    try {

            if (Video->WaitPic()) {

                if (Video->FetchPicture()) {
                   // qDebug()<<"run here";
                   QImage image((const uchar*)Video->Addr , Video->Width , Video->Height ,QImage::Format_RGB32);
                    image.save("123.png");
                    QDir qdir;
                    if(qdir.exists("/sdcard") == true){
                        image.save("/sdcard/123.png");
                    }
                    else{
                        qDebug()<<"SDcard does not exist";
                    }
                }
            }
        }
 catch (TError &e)
    {
        e.Output();
        return -1;
    }
    return 1;
}

int CamTab::DeletePhotoClick(){
   bool ret = QFile::remove("123.png");
   if(ret == false)
   {
       qDebug()<<"Delete failed";
       return 0;
   }
   return 1;
}

int CamTab::RecordClick()
{

    if(WhetherRecord == true)
    {
            if(uvideo == NULL)
            {
                uvideo = new record;//call the constuctor function
            }
            else
            {
                delete uvideo;
                uvideo = NULL;
                qDebug()<<"CamTab::RecordClick()    ----   uvideo is not NULL";
            }

            bool openFIleFlag = uvideo->OpenFile();//打开需要写入的设备 fp_nv12

            if(openFIleFlag == true)
            {
                uvideo->start();                            //Class   record 是一个线程，视频采集线程
            }

            RecordVideo->setText(globalData->tc->toUnicode("停止"));
            WhetherRecord = false;

            return 1;
           // RecordVideo->hide();
           //  RecordStop->show();
    }

    else
    {
        qDebug()<<"WhetherRecord == false  in the      recordClick()";
        if ( uvideo != NULL)
        {
            uvideo->stopThread = true;
            uvideo->StopRecord();
            uvideo->wait();
            if(uvideo->isFinished())
            {
                delete uvideo;
                uvideo = NULL;
            }
        }
        //RecordStop->hide();
        RecordVideo->setText(globalData->tc->toUnicode("录制"));
        WhetherRecord = true;
        return 0;
    }

}


int CamTab::RecordClickDisplay()
{
    uvideo = new record;
    QTimer  *timerRecord = new QTimer;
     connect(timerRecord,SIGNAL(timeout()),this,SLOT(timerUpdate()));
     timerRecord->start(5);

}

int CamTab::timerUpdate()
{
   //qDebug()<<"timeupdate";
    uchar * pImgBuf;
    unsigned int len;
    int camReturn;
    //抓取一张图片
    camReturn = uvideo->get_frame((void **)&pImgBuf,&len);
    if(-1 == camReturn)
    {
        qDebug()<<"uvideo->get_frame  failed";
    }
    uvideo->convert_yuv_to_rgb_buffer(pImgBuf,imgBuf,image_width,image_height);
    frame->loadFromData((uchar *)imgBuf,image_width * image_height * 3 * sizeof(char));
    DisLabel->setPixmap(QPixmap::fromImage(*frame));//display
    camReturn = uvideo->unget_frame();

    return 0;
}


int CamTab::RecordStopClick(){

}

void CamTab::SendPhotoAvailable(){
  //  qDebug()<<"run here";
    SendPhoto->setEnabled(true);
}

void CamTab::mfcconvertClick(){

  //---------1224
    struct timeval tpstart,tpend;
    float  timeuse;
    gettimeofday(&tpstart,NULL);
  //---------1224

            //  capture
            record *pRecord = new record();
            if ( mfc == NULL){
                mfc = new MFC;
            }
            else{
                qDebug()<<"in  the function ::  mfcconvertClick()";
            }
            int captureNum = 0;
            for(; captureNum < 500; captureNum++){
                pRecord->StartRecord();
                mfc->Encode();
                if( (captureNum % 100) == 0){
                     qDebug()<<"captureNum \t"<<captureNum;
                }

            }

     //---------1224
            gettimeofday(&tpend,NULL);
            timeuse = 1000000 * (tpend.tv_sec-tpstart.tv_sec) + tpend.tv_usec - tpstart.tv_usec;
            timeuse/=1000000;
            printf("timeuse total Used Time( Second ):%f\n",timeuse);
     //---------1224

            mfc->closeMFC();

            return ;
}






