#include "dialog.h"
#include <QApplication>
#include "syszuxim.h"
#include "syszuxpinyin.h"
#include "voiceplay.h"
#include "dataprocess.h"
#include "gprs.h"
#include"data.h"
#include <QSqlTableModel>
#include "sqldb.h"

#include "zigbeecfg.h"

//#include "cv.h"
//#include "list"

//using namespace cv;
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
//   // QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
   // QTextCodec::setCodecForCStrings(QTextCodec::codecForName("Latin1"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("Unicode"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("GB2312"));
//    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GB2312"));
//    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("GB2312"));

//    QFont font;
//    font.setPointSize(12);
//    font.setFamily(("wenquanyi"));
//    font.setBold(true);
//    a.setFont(font);

    SqlDB *db = new SqlDB();   //建立
    db->connectDB();            //连接


    //QWSServer::setCursorVisible(FALSE);   //隐藏鼠标
    /*打开输入法*/
    QWSInputMethod *im = new SyszuxIM;
    QWSServer::setCurrentInputMethod(im);
    im->updateHandler(QWSInputMethod::FocusOut);
    //qDebug()<<"keyboard";
    Dialog *w = Dialog::getDiaglogData();             //------单件模式--只能生成一个对象
    w->show();

//    ZigbeeCfg *zigbeeCfg = new  ZigbeeCfg();
//    zigbeeCfg->zigbeeCFG_ID_CHANNEL();

    //DataProcess dataProcess;                        //数据采集与处理线程
        //dataProcess.start();
//    VoicePlay voicePlay;                                  //语音播放处理线程
//    voicePlay.start();
//    GlobalData *globalData = GlobalData::getGlobalData();
//    GprsConnect gprs_start;
//    gprs_start.start();

//    if (globalData->Connect_Success == 1)
//    {
//    Udp_Socket_Sender data_sender;
//    data_sender.start();
//    }
    return a.exec();
}
