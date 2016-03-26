#ifndef CAMTAB_H
#define CAMTAB_H
#include "data.h"
#include "dialog.h"
#include "camtest.h"
#include "gprs.h"
#include "record.h"
#include <QDir>
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
    uchar *imgBuf;
    QImage *frame;
private:
    QPushButton *ChangeTab;
    QPushButton *TakePhoto;
    QPushButton *DeletePhoto;
    QPushButton *SendPhoto;
    QPushButton *RecordVideo;
    QPushButton *RecordDisplay;
    QPushButton *RecordStop;
    QPushButton *mfctest;//测试mfc功能
    QLabel *DisLabel;
    bool WhetherRecord;
    void interfaceInit();

private slots:
    void SendPhotoAvailable();
public slots:
    int DisplayVideo();
    int TakePhotoClick();
    int DeletePhotoClick();
    int RecordClick();
    //@1225
    int RecordClickDisplay();
    int timerUpdate();

    int RecordStopClick();
    void mfcconvertClick();
};

#endif // CAMTAB_H
