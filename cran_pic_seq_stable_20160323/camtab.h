#ifndef CAMTAB_H
#define CAMTAB_H
#include "data.h"
#include "dialog.h"
#include "camtest.h"
#include "gprs.h"
#include "record.h"
#include <QDir>
class CamThread;
class CamTab:public QDialog
{
    Q_OBJECT
public:
    GlobalData *globalData;
    TFrameBuffer *FrameBuffer;
    TVideo *Video;
    record *uvideo;
    MFC *mfc;
    QTimer *timer;
    CamTab(QWidget *parent = 0);
    ~CamTab();
private:
    QPushButton *ChangeTab;
    QPushButton *TakePhoto;
    QPushButton *DeletePhoto;
    QPushButton *SendPhoto;
    QPushButton *RecordVideo;
    QPushButton *RecordStop;
    QPushButton *mfctest;//测试mfc功能
    QLabel *DisLabel;
    CamThread *camThread;
    bool WhetherRecord;
    void interfaceInit();
private slots:
    void SendPhotoAvailable();
public slots:
    int DisplayVideo();
    void DisplayUVideo();
    int TakePhotoClick();
    int DeletePhotoClick();
    int RecordClick();
    int RecordStopClick();
    void mfcconvertClick();
};

class CamThread:public QThread
{
    Q_OBJECT
public:
    CamThread(QWidget *parent);
signals:
    void camFlush();
private:
    void run();
};

#endif // CAMTAB_H
