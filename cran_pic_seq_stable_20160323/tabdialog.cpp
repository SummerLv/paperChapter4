#include "tabdialog.h"

TabDialog::TabDialog(QWidget *parent) :
    QDialog(parent)
{
    globalData = GlobalData::getGlobalData();
    TabDialog::setWindowFlags(Qt::FramelessWindowHint); //  去掉标题栏
    QWidget::setFixedSize(SCREEN_W,SCREEN_H);
    QLabel *TitleLabel = new QLabel(globalData->tc->toUnicode("起重式塔吊参数设置"));
    QFont font;
    font.setPointSize(24);
    font.setFamily(("wenquanyi"));
    font.setBold(true);
    TitleLabel->setFont(font);
    TitleLabel->setFrameStyle(QFrame::NoFrame | QFrame::NoFrame);
    TitleLabel->setAlignment(Qt::AlignTop | Qt::AlignCenter);

    QTabWidget *tabWidget = new QTabWidget(this);
   // tabWidget->addTab(new KDdataTab(this) , tr ("参数设置信息"));
    tabWidget->addTab(new BasicDataTab(this) , globalData->tc->toUnicode ("基本信息"));
    tabWidget->addTab(new ProZoneDataTab(this) , globalData->tc->toUnicode ("保护区信息") );
    tabWidget->addTab(new LimitDataTab(this) , globalData->tc->toUnicode ("限位值信息"));
    //tabWidget->addTab(new KDdataTab(this) , tr ("参数设置信息"));
    tabWidget->addTab(new InterfereCraneTab(this) , globalData->tc->toUnicode ("相干塔吊信息"));
    tabWidget->addTab(new temtab(this) , globalData->tc->toUnicode("温度补偿信息"));
    tabWidget->addTab(new KDdataTab(this) , globalData->tc->toUnicode ("参数设置信息"));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    //mainLayout->setSizeConstraint(QLayout :: SetNoConstraint);//不强站位置，可以将窗口压倒高度为0
    mainLayout->addWidget(TitleLabel);
    mainLayout->addWidget(tabWidget);
    setLayout(mainLayout);
}

TabDialog::~TabDialog()
{

}

void TabDialog::windowClose()      //点击退出关闭窗口的槽函数
{
   GprsConnect *gprs = GprsConnect::Instance();
   gprs->exit();
    system("./Kill_and_Restart.sh");
    //close();
}
