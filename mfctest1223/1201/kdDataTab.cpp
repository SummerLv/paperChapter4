#include "kdDataTab.h"

KDdataTab::KDdataTab(QWidget *parent)
{
    //db = new SqlDB;                                     //打开已链接的数据库

    seclectNum = -1;
    interfaceInit();

    //20140901 此处注释出问题？
    qDebug()<<"kddatatab 1";
   zigbeeScreenCfg= new ZigbeeScreenCfg;
   // ZigbeeReceive *zigbeeReceive =new ZigbeeReceive;
    qDebug()<<"kddatatab 2";

    connect(saveButton,SIGNAL(clicked()),this,SLOT(saveButtonClicked()) );
    connect(setButtons,SIGNAL(buttonClicked(int)),this,SLOT(setButtonsSelect(int)) );
    connect(quitButton,SIGNAL(clicked()),parent,SLOT(windowClose()) );
    //connect(this,SIGNAL(IDChannelChange()),zigbeeReceive,SLOT( zigbeeIDChannelCfg() )  );

}
KDdataTab::~KDdataTab()
{

}

/*界面初始化函数*/
void KDdataTab::interfaceInit()
{
    globalData = GlobalData::getGlobalData();
    saveButton = new QPushButton(globalData->tc->toUnicode ("保存"));
    quitButton = new QPushButton( globalData->tc->toUnicode ("退出") );

    QString name[6] = { globalData->tc->toUnicode("校正吊重") , globalData->tc->toUnicode("校正变幅"), globalData->tc->toUnicode("校正高度"), globalData->tc->toUnicode("设置Zigbee"),globalData->tc->toUnicode("设置网络"),globalData->tc->toUnicode("校正转角") };
    setButtons = new QButtonGroup;
    for (quint8 i = 0 ; i < 7 ;i++) {                                           //添加设置及计算KD按键
        QPushButton *newButton = new QPushButton(name[i]);
        setButtons->addButton(newButton,i);
    }
    /*左侧第一列布局*/
    QGridLayout *H1Layout = new QGridLayout;
    //--------------------------添加Label-------------------------------------------------------
    QString H1Name[5] = { globalData->tc->toUnicode("吊重K值"),globalData->tc->toUnicode("变幅K值"),globalData->tc->toUnicode("高度K值"),globalData->tc->toUnicode("Zigbee信道"),globalData->tc->toUnicode("联网地址") };
    QLabel *H1Label[5];
    for (quint8 i = 0 ; i < 5 ; i++) {
        H1Label[i] = new QLabel(H1Name[i]);
        H1Layout->addWidget(H1Label[i],i,0,1,1,Qt::AlignLeft);
    }
    //-----------------------添加LineEdit-------------------------------------------------------
    weightK = new QLineEdit;
    weightK->setAlignment(Qt::AlignRight);
    spanK = new QLineEdit;
    spanK->setAlignment(Qt::AlignRight);
    heightK = new QLineEdit;
    heightK->setAlignment(Qt::AlignRight);
    zigbeeChannel = new QLineEdit;
    zigbeeChannel->setAlignment(Qt::AlignRight);
    serverIP = new QLineEdit;
    serverIP->setAlignment(Qt::AlignRight);
    H1Layout->addWidget(weightK,0,1,1,1,Qt::AlignLeft);
    H1Layout->addWidget(spanK,1,1,1,1,Qt::AlignLeft);
    H1Layout->addWidget(heightK,2,1,1,1,Qt::AlignLeft);
    H1Layout->addWidget(zigbeeChannel,3,1,1,1,Qt::AlignLeft);
    H1Layout->addWidget(serverIP,4,1,1,1,Qt::AlignLeft);
    /*左侧第二列布局*/
    QGridLayout *H2Layout = new QGridLayout;
    //--------------------------添加Label------------------------------------------------------
    QString H2Name[5] = { globalData->tc->toUnicode("吊重D值"),globalData->tc->toUnicode("变幅D值"),globalData->tc->toUnicode("高度D值"),tr("ZigbeeID"),globalData->tc->toUnicode("联网端口") };
    QLabel *H2Label[5];
    for (quint8 i = 0 ; i < 5 ; i++) {
        H2Label[i] = new QLabel(H2Name[i]);
        H2Layout->addWidget(H2Label[i],i,0,1,1,Qt::AlignLeft);
    }
    //-----------------------添加LineEdit------------------------------------------------------
    weightD = new QLineEdit;
    weightD->setAlignment(Qt::AlignRight);
    spanD = new QLineEdit;
    spanD->setAlignment(Qt::AlignRight);
    heightD = new QLineEdit;
    heightD->setAlignment(Qt::AlignRight);
    zigbeeID = new QLineEdit;
    zigbeeID->setAlignment(Qt::AlignRight);
    serverPort = new QLineEdit;
    serverPort->setAlignment(Qt::AlignRight);
    H2Layout->addWidget(weightD,0,1,1,1,Qt::AlignLeft);
    H2Layout->addWidget(spanD,1,1,1,1,Qt::AlignLeft);
    H2Layout->addWidget(heightD,2,1,1,1,Qt::AlignLeft);
    H2Layout->addWidget(zigbeeID,3,1,1,1,Qt::AlignLeft);
    H2Layout->addWidget(serverPort,4,1,1,1,Qt::AlignLeft);
/*左侧第三列布局*/
    QVBoxLayout *H3Layout = new QVBoxLayout;
    for (quint8 i = 0 ; i < 5 ; i++) {
        setButtons->button(i)->setFixedWidth(100);
        H3Layout->addWidget(setButtons->button(i));
    }
   QHBoxLayout *editLayout = new QHBoxLayout;
   editLayout->addLayout(H1Layout);
   editLayout->addSpacing(50);
   editLayout->addLayout(H2Layout);
   editLayout->addSpacing(100);
   editLayout->addLayout(H3Layout);

/*底部布局*/
    QHBoxLayout *bottomLayout = new QHBoxLayout;
    QLabel *angleLabel = new QLabel(globalData->tc->toUnicode("转角校正") );
    angle = new QLineEdit;
    angle->setAlignment(Qt::AlignRight);
    bottomLayout->addWidget(angleLabel);
    bottomLayout->addWidget(angle);
    bottomLayout->addWidget(setButtons->button(5));
    bottomLayout->addStretch(1);
    bottomLayout->addWidget(saveButton);
    bottomLayout->addWidget(quitButton);

/*主布局*/
    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addLayout(editLayout,0,0,1,1);
    mainLayout->addLayout(bottomLayout,5,0,1,1);
  //  mainLayout->addLayout(bottomLayout,0,0,1,1,Qt::AlignCenter);
    setLayout(mainLayout);
//    setLayout(editLayout);
//    setLayout(bottomLayout);

    //数据库数据显示
   // weightK->setText("22");
   // qDebug() <<"start";

    QSqlTableModel *readkdData = new QSqlTableModel;
    readkdData->setTable("KdData");
    readkdData->setFilter("id = 0");
    readkdData->select();
//    QSqlQueryModel *readkdData = new QSqlQueryModel;     //操作数据库
//    readkdData->setQuery("SELECT * From KdData WHERE id = 0");
 //   qDebug()<< "weightK"<<readkdData->record(0).value("weightK").toString();
    if (readkdData->rowCount() == 1) {
        //weightK->setText("1121");
       weightK->setText(readkdData->record(0).value("weightK").toString());
        weightD->setText(readkdData->record(0).value("weightD").toString());
        spanK->setText(readkdData->record(0).value("spanK").toString());
        spanD->setText(readkdData->record(0).value("spanD").toString());
        heightK->setText(readkdData->record(0).value("heightK").toString());
        heightD->setText(readkdData->record(0).value("heightD").toString());
        serverIP->setText(readkdData->record(0).value("serverIP").toString());
        serverPort->setText(readkdData->record(0).value("serverPort").toString());
        zigbeeID->setText(readkdData->record(0).value("ZigbeeID").toString());
        zigbeeChannel->setText(readkdData->record(0).value("ZigbeeChannel").toString());
    }
}
/*  quint16 craneKDVersion;      //信息版本
  double weightK;                     //吊重K值
  double weightD;                      //吊重K值
  double spanK;                            //变幅K值
  double spanD;                            //变幅D值
  double heightK;                          //高度K值
  double heightD;                              //高度D值
  QString serverIP;                            //IP
  QString serverPort;                          //端口
  QString ZigbeeChannel;                      //通信信道
  QString ZigbeeID;                                   //网络ID*/
void KDdataTab::saveButtonClicked()
{
    bool ok;
    QSqlTableModel *saveKDData = new QSqlTableModel;
    saveKDData->setTable("KdData");
    saveKDData->setFilter("id = 0");
    saveKDData->select();
    if (saveKDData->rowCount() == 1 ) {                     //更新数据库中KD值
        QSqlRecord record = saveKDData->record(0);
        record.setValue("weightK",weightK->text().toDouble(&ok));
        record.setValue("weightD",weightD->text().toDouble(&ok));
        record.setValue("spanK",spanK->text().toDouble(&ok));
        record.setValue("spanD",spanD->text().toDouble(&ok));
        record.setValue("heightK",heightK->text().toDouble(&ok));
        record.setValue("heightD",heightD->text().toDouble(&ok));
        record.setValue("serverIP",serverIP->text());
        record.setValue("serverPort",serverPort->text());
        record.setValue("ZigbeeChannel",zigbeeChannel->text());
        record.setValue("ZigbeeID",zigbeeID->text());
        saveKDData->setRecord(0,record);
        if (FALSE == saveKDData->submitAll()) {
            QMessageBox::warning(this,globalData->tc->toUnicode ("修正值信息"),globalData->tc->toUnicode ("修正值信息保存失败"),QMessageBox::Yes);
            return;
        }
    }else if(saveKDData->rowCount() == 0) {
        saveKDData->insertRows(0,1);
        saveKDData->setData(saveKDData->index(0,0),0);
        saveKDData->setData(saveKDData->index(0,1),weightK->text().toDouble(&ok));
        saveKDData->setData(saveKDData->index(0,2),weightD->text().toDouble(&ok));
        saveKDData->setData(saveKDData->index(0,3),spanK->text().toDouble(&ok));
        saveKDData->setData(saveKDData->index(0,4),spanD->text().toDouble(&ok));
        saveKDData->setData(saveKDData->index(0,5),heightK->text().toDouble(&ok));
        saveKDData->setData(saveKDData->index(0,6),heightD->text().toDouble(&ok));
        saveKDData->setData(saveKDData->index(0,7),serverIP->text());
        saveKDData->setData(saveKDData->index(0,8),serverPort->text());
        saveKDData->setData(saveKDData->index(0,9),zigbeeChannel->text());
        saveKDData->setData(saveKDData->index(0,10),zigbeeID->text());
        if (FALSE == saveKDData->submitAll()) {
            QMessageBox::warning(this,globalData->tc->toUnicode ("修正值信息"),globalData->tc->toUnicode ("修正值信息保存失败"),QMessageBox::Yes);
            return;
        }
    }
}

/*
void KDdataTab::valueRefresh(double kData,double dData)
{
    switch(seclectNum) {
    case 0:                                                         //吊重KD值更新
        weightK->setText(QString::number(kData));
        weightD->setText(QString::number(dData));
        break;
    case 1:                                                         //变幅KD值更新
        spanK->setText(QString::number(kData));
        spanD->setText(QString::number(dData));
        break;
    case 2:                                                         //高度KD值更新
        heightK->setText(QString::number(kData));
        heightD->setText(QString::number(dData));
        break;
    default:
        break;
    }
}
*/

void KDdataTab::setButtonsSelect(int i)
{
    seclectNum = i;                                 //标定选择项
    //  选中的为吊重，变幅，高度调整
    if ( CORRECT_WEIGHT == i
         || CORRECT_SPAN == i
         || CORRENT_HEIGHT == i) {

    }
    // 选中的是ZIGBEE设置
//    if (SET_ZIGBEE == i) {
//    }
    /****** 20140901****************************************/
    // 选中的是ZIGBEE设置
    //  *******打开 kdData_table >> 存入数据 >> 打开zigbee 配置***20140828****
    if (SET_ZIGBEE == i)
    {
        QSqlTableModel *saveKDData = new QSqlTableModel;
        saveKDData->setTable("KdData");
        saveKDData->setFilter("id = 0");
        saveKDData->select();
        if (saveKDData->rowCount() == 1 )
        {                     //更新数据库中***ZIGBEE*****值
           qDebug()<< "update the sql rowcount == 1 ";
            QSqlRecord record = saveKDData->record(0);
            record.setValue("ZigbeeChannel",zigbeeChannel->text());
            record.setValue("ZigbeeID",zigbeeID->text());
            saveKDData->setRecord(0,record);
//            if (FALSE == saveKDData->submitAll())
//            {
//                QMessageBox::warning(this,globalData->tc->toUnicode ("修正值信息"),globalData->tc->toUnicode ("修正值信息保存失败"),QMessageBox::Yes);
//                return;
//            }
        }
        else if(saveKDData->rowCount() == 0)
        {
            qDebug()<< "complete  the sql rowcount == 0 ";
            saveKDData->insertRows(0,1);
            saveKDData->setData(saveKDData->index(0,0),0);
            saveKDData->setData(saveKDData->index(0,9),zigbeeChannel->text());
            saveKDData->setData(saveKDData->index(0,10),zigbeeID->text());
//            if (FALSE == saveKDData->submitAll())
//            {
//                QMessageBox::warning(this,globalData->tc->toUnicode ("修正值信息"),globalData->tc->toUnicode ("修正值信息保存失败"),QMessageBox::Yes);
//                return;
//            }
        }
        if (FALSE == saveKDData->submitAll())
        {
            QMessageBox::warning(this,globalData->tc->toUnicode ("修正值信息"),globalData->tc->toUnicode ("修正值信息保存失败"),QMessageBox::Yes);
            qDebug()<< "saveKDData->submitAll() ** failed ***";
            return;
        }
        else
        {
            qDebug()<< "saveKDData->submitAll() ** successed ***";
            // Signal---Slot  mechanism*****   2 | 1
            //emit  IDChannelChange();
            globalData->mStop = true;
            zigbeeScreenCfg->cfgNetIDChannel();
        }
    }
     /****************************************************************/
    // 选中的是网络设置
    if (SET_SERVER_NET  == i) {
        QSqlTableModel *saveKDData = new QSqlTableModel;
        saveKDData->setTable("KdData");
        saveKDData->setFilter("id = 0");
        saveKDData->select();
        if (saveKDData->rowCount() == 1 ) {                     //更新数据库中KD值
            QSqlRecord record = saveKDData->record(0);
            record.setValue("serverIP",serverIP->text());
            record.setValue("serverPort",serverPort->text());
            saveKDData->setRecord(0,record);
            if (FALSE == saveKDData->submitAll()) {
                QMessageBox::warning(this,globalData->tc->toUnicode ("修正值信息"),globalData->tc->toUnicode ("修正值信息保存失败"),QMessageBox::Yes);
                return;
            }
        }else if(saveKDData->rowCount() == 0) {
            saveKDData->insertRows(0,1);
            saveKDData->setData(saveKDData->index(0,0),0);
            saveKDData->setData(saveKDData->index(0,7),serverIP->text());
            saveKDData->setData(saveKDData->index(0,8),serverPort->text());
            if (FALSE == saveKDData->submitAll()) {
                QMessageBox::warning(this,globalData->tc->toUnicode ("修正值信息"),globalData->tc->toUnicode ("修正值信息保存失败"),QMessageBox::Yes);
                return;
            }
        }
    }
    // 选中的是转角校正
    if (SET_ANGLE == i) {

    }

}


/****************************************************************
 ****************************************************************
 ****************************************************************/
/*kd值计算界面*/
/*
CorrectDialog::CorrectDialog(QWidget *parent)
{
    dialogInit();
    connect(getValueButton,SIGNAL(clicked()),this,SLOT(getValueButtonClicked()));
    connect(addValueButton,SIGNAL(clicked()),this,SLOT(addValueButtonClicked()));
    connect(delValueButton,SIGNAL(clicked()),this,SLOT(delValueButtonClicked()));
    connect(calculateButton,SIGNAL(clicked()),this,SLOT(calculateButtonClicked()));
    connect(saveValueButton,SIGNAL(clicked()),this,SLOT(saveValueButtonClicked()));
    connect(cancelButton,SIGNAL(clicked()),this,SLOT(cancelButtonClicked()));
    connect(this,SIGNAL(dataChange(double,double)),parent,SLOT(valueRefresh(double,double)));

}

void CorrectDialog::dialogInit()
{
    QWidget::setFixedSize(800,600);

    getValueButton = new QPushButton(tr("获取AD值"));
    cancelButton = new QPushButton(tr("取消"));
    calculateButton = new QPushButton(tr ("计算"));
    saveValueButton = new QPushButton(tr ("保存"));
    addValueButton = new QPushButton(tr ("添加"));
    delValueButton = new QPushButton(tr ("删除"));

    actualValue = new QLineEdit(tr ("0"));
    actualValue->setAlignment(Qt::AlignRight);
    ADValue = new QLineEdit(tr ("0"));
    ADValue->setAlignment(Qt::AlignRight);
    KValue = new QLineEdit (tr ("0"));
    KValue->setAlignment(Qt::AlignRight);
    DValue = new QLineEdit(tr("0"));
    DValue->setAlignment(Qt::AlignRight);

    QStringList labels;
    labels << tr("实际值") << tr("AD值");
    valueTab = new QTableWidget;                //显示框
    valueTab->setColumnCount(2);
   // valueTab->setRowCount(1);
   // valueTab->setCurrentCell(0,0);
    valueTab->verticalHeader()->setVisible(false);  //隐藏列表头
    valueTab->setSelectionBehavior(QAbstractItemView::SelectRows);         //设置选择为整行选中
    valueTab->setSelectionMode(QAbstractItemView::SingleSelection);     //设置选择模式位选择单行
    valueTab->setEditTriggers(QAbstractItemView::NoEditTriggers);
    valueTab->setHorizontalHeaderLabels(labels);
    valueTab->horizontalHeader()->setStretchLastSection(true);
    valueTab->setContentsMargins(20,20,20,20);

    QGridLayout *editLayout = new QGridLayout;
    QLabel *label1 = new QLabel(tr ("实际值"));
    QLabel *label2 = new QLabel(tr ("AD值"));
    QLabel *label3 = new QLabel(tr ("K值"));
    QLabel *label4 = new QLabel(tr ("D值"));

    editLayout->addWidget(label1,0,0,1,1,Qt::AlignLeft);
    editLayout->addWidget(label2,0,3,1,1,Qt::AlignLeft);
    editLayout->addWidget(label3,7,0,1,1,Qt::AlignLeft);
    editLayout->addWidget(label4,7,3,1,1,Qt::AlignLeft);

    editLayout->addWidget(actualValue,0,1,1,1,Qt::AlignLeft);
    editLayout->addWidget(ADValue,0,4,1,1,Qt::AlignLeft);
    editLayout->addWidget(KValue,7,1,1,1,Qt::AlignLeft);
    editLayout->addWidget(DValue,7,4,1,1,Qt::AlignLeft);

    editLayout->addWidget(valueTab,1,0,6,6);

    editLayout->addWidget(getValueButton,0,7,1,1,Qt::AlignCenter);
    editLayout->addWidget(addValueButton,1,7,1,1,Qt::AlignCenter);
    editLayout->addWidget(delValueButton,2,7,1,1,Qt::AlignCenter);
    editLayout->addWidget(calculateButton,5,7,1,1,Qt::AlignCenter);
    editLayout->addWidget(saveValueButton,6,7,1,1,Qt::AlignCenter);
    editLayout->addWidget(cancelButton,7,7,1,1,Qt::AlignCenter);
    setLayout(editLayout);
}

void CorrectDialog::getValueButtonClicked()         //获取    AD槽函数
{

}
void CorrectDialog::addValueButtonClicked()         //添加组值 函数
{
    valueTab->insertRow(valueTab->rowCount());                                 //插入一行
    qDebug()<<valueTab->rowCount();
    valueTab->setItem(valueTab->rowCount()-1,0,new QTableWidgetItem(actualValue->text()));
    valueTab->setItem(valueTab->rowCount()-1,1,new QTableWidgetItem(ADValue->text()));
}
void CorrectDialog::delValueButtonClicked()             //删除组值函数
{
    valueTab->removeRow(valueTab->currentRow());

}
void CorrectDialog::calculateButtonClicked()                //计算KD值槽函数
{

}
void CorrectDialog::saveValueButtonClicked()                //保存KD值
{
    bool ok;
    emit dataChange(KValue->text().toDouble(&ok),DValue->text().toDouble(&ok));
    CorrectDialog::close();

}
void CorrectDialog::cancelButtonClicked()                       //取消操作
{
    CorrectDialog::close();

}
*/
