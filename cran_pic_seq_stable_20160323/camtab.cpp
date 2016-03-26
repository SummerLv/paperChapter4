#include "camtab.h"
CamTab::CamTab(QWidget *parent) :
    QDialog(parent)
{
    globalData = GlobalData::getGlobalData();
    Dialog *w = Dialog::getDiaglogData();
 //  GprsConnect *gprs = GprsConnect::Instance();
    interfaceInit();
    connect(ChangeTab , SIGNAL(clicked()),w , SLOT(ChangeCamButtonClicked()));
   // connect(timer , SIGNAL(timeout()) , this , SLOT(DisplayUVideo()));
   // connect(TakePhoto , SIGNAL(clicked()) , this , SLOT(TakePhotoClick()));
    //connect(DeletePhoto , SIGNAL(clicked()) , this ,SLOT(DeletePhotoClick()));
   // connect(RecordVideo , SIGNAL(clicked()) , this , SLOT(RecordClick()));
  //  connect(SendPhoto , SIGNAL(clicked()) , gprs , SLOT(SendPhoto()));
  //  connect(RecordStop , SIGNAL(clicked()) , this , SLOT(RecordStopClick()));
    connect(uvideo , SIGNAL(getFrame()),this , SLOT(DisplayUVideo()));
    connect(mfctest , SIGNAL(clicked()) , this ,SLOT(mfcconvertClick()) );
  //  connect(gprs->tcppicsender , SIGNAL(connected()) , this , SLOT(SendPhotoAvailable()));
}

CamTab::~CamTab(){
    timer->stop();
    delete Video;
   // delete uvideo;
    Video = NULL;
   // uvideo = NULL;
  //  uvideo->stopThread = true;

 //   uvideo->exit();
  //  delete uvideo;
//    Video->~TVideo();
//        qDebug()<<"run here1";
//    FrameBuffer->~TFrameBuffer();
//        qDebug()<<"run here2";

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
   // RecordStop = new QPushButton(globalData->tc->toUnicode("停止"));
    mfctest = new QPushButton(globalData->tc->toUnicode("MFC测试"));
    //RecordStop->hide();
    WhetherRecord = true;
    SendPhoto->setEnabled(globalData->sendphotoavailable);
   // FrameBuffer = new TFrameBuffer;
  //  Video = new TVideo;
    uvideo = NULL;
    uvideo = new record;   // 调用构造函数的时候进行了初始化的操作
    uvideo->start();   // 这里继承了 qthread 类这里就会执行 run函数的内容
    qDebug()<<"the record class is new here , construct function will call here";
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
    setLayout(mainlayout);
 //   if(Video->IsValid()){
 //   timer->start(25);
 //   }
}

CamThread::CamThread(QWidget *parent)
    :QThread(parent)
{
    //connect(this,SIGNAL(camFlush()),parent,PaintUpdate());
}

void CamThread::run()
{
    emit camFlush();
    QThread::exec();
}

int CamTab::DisplayVideo(){
    try {

            if (Video->WaitPic()) {

                if (Video->FetchPicture()) {
                   // qDebug()<<"run here";
                    QImage image((const uchar*)Video->Addr , Video->Width , Video->Height ,QImage::Format_RGB32);
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

void CamTab::DisplayUVideo(){
    if( (uvideo->picData.size() != 0)){
      //  qDebug()<<"display video";
        QByteArray byte(uvideo->picData.mid(4));
     //   qDebug()<<"jpg length"<<byte.size();
        QImage image = QImage::fromData(byte , "JPG");
        DisLabel->setPixmap(QPixmap::fromImage(image));
        DisLabel->show();
        uvideo->picData.clear();
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
 catch (TError &e) {
        e.Output();
        return -1;
    }
    return 1;
}

int CamTab::DeletePhotoClick(){
   bool ret = QFile::remove("123.png");
   if(ret == false){
       qDebug()<<"Delete failed";
       return 0;
   }
   return 1;
}

int CamTab::RecordClick(){
//    if (WhetherRecord == true){
//        WhetherRecord = false;
//    QString command = tr("./nv12 &");
//    char *S;
//    S = command.toAscii().data();
//    system(S);

//    RecordVideo->setEnabled(false);
//    sleep(30);
//    RecordVideo->setEnabled(true);
//    WhetherRecord = false;
//    }


    if(WhetherRecord == true){
    if(uvideo == NULL)
        uvideo = new record;
    else{
        delete uvideo;
        uvideo = NULL;
    }

   bool openFIleFlag = uvideo->OpenFile();

    if(openFIleFlag == true)
        uvideo->start();
    RecordVideo->setText(globalData->tc->toUnicode("停止"));
        WhetherRecord = false;
        return 1;
   // RecordVideo->hide();
 //   RecordStop->show();
    }

    else{
        qDebug()<<"run here";
        if ( uvideo != NULL){
            uvideo->stopThread = true;
            uvideo->StopRecord();
            uvideo->wait();
            if(uvideo->isFinished()){
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

int CamTab::RecordStopClick(){

}

void CamTab::SendPhotoAvailable(){
  //  qDebug()<<"run here";
    SendPhoto->setEnabled(true);
}

void CamTab::mfcconvertClick(){
    if ( mfc == NULL){
        qDebug()<<"run here";
    mfc = new MFC;
 //   char *test;
    mfc->Encode();
   // qDebug()<<"run here";
    mfc->closeMFC();
    }
    /*
    if( mfc != NULL){
        delete mfc;
        mfc = NULL;
    }
    */
}
