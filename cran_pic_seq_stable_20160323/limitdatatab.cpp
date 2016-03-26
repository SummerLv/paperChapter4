#include "limitdatatab.h"
LimitDataTab::LimitDataTab(QWidget *parent)
{
    globalData = GlobalData::getGlobalData();
    //db = new SqlDB;                                     //打开已链接的数据库
    interfaceInit();
    connect (quitButton,SIGNAL(clicked()) , parent , SLOT(windowClose()));
    connect(saveButton,SIGNAL(clicked()),this,SLOT(saveButtonClicked()));
}

LimitDataTab::~LimitDataTab()
{

}

void LimitDataTab::interfaceInit()          //界面初始化函数
{
    QLabel *label1 = new QLabel (globalData->tc->toUnicode ("信息版本"));
    QLabel *label2 = new QLabel (globalData->tc->toUnicode ("起重限位"));
    QLabel *label3 = new QLabel (globalData->tc->toUnicode ("高度限位"));
    QLabel *label4 = new QLabel (globalData->tc->toUnicode ("力矩限位"));
    QLabel *label5 = new QLabel (globalData->tc->toUnicode ("左转限位"));
    QLabel *label6 = new QLabel (globalData->tc->toUnicode ("右转限位"));
    QLabel *label7 = new QLabel (globalData->tc->toUnicode ("远行限位"));
    QLabel *label8 = new QLabel (globalData->tc->toUnicode ("近行限位"));
    //test
    //QLabel *label9 = new QLabel (tr("测试栏"));

    version = new QLineEdit;
    version->setAlignment(Qt::AlignRight);
    leftLimit = new QLineEdit;
    leftLimit->setAlignment(Qt::AlignRight);
    rightLimit = new QLineEdit;
    rightLimit->setAlignment(Qt::AlignRight);
    farLimit = new QLineEdit;
    farLimit->setAlignment(Qt::AlignRight);
    closeLimit = new QLineEdit;
    closeLimit->setAlignment(Qt::AlignRight);
    heightLimit = new QLineEdit;
    heightLimit->setAlignment(Qt::AlignRight);
    weightLimit = new QLineEdit;
    weightLimit->setAlignment(Qt::AlignRight);
    momentLimit = new QLineEdit;
    momentLimit->setAlignment(Qt::AlignRight);
   // limit_test = new QLineEdit;

    QLabel *unit1 = new QLabel (globalData->tc->toUnicode ("吨"));
    QLabel *unit2 = new QLabel (globalData->tc->toUnicode ("米"));
    QLabel *unit3 = new QLabel (globalData->tc->toUnicode ("吨*米"));
    QLabel *unit4 = new QLabel (globalData->tc->toUnicode ("度"));
    QLabel *unit5 = new QLabel (globalData->tc->toUnicode ("度"));
    QLabel *unit6 = new QLabel (globalData->tc->toUnicode ("米"));
    QLabel *unit7 = new QLabel (globalData->tc->toUnicode ("米"));

    QGridLayout *editLayout = new QGridLayout;
    /*信息版本，吊重限位布局*/
    editLayout->addWidget(label1,0,0,1,1,Qt::AlignRight);
    editLayout->addWidget(version,0,1,1,1,Qt::AlignLeft);
    editLayout->addWidget(label2,0,3,1,1,Qt::AlignRight);
    editLayout->addWidget(weightLimit,0,4,1,1,Qt::AlignLeft);
    editLayout->addWidget(unit1,0,5,1,1,Qt::AlignLeft);
    /*高限位，力矩限位布局*/
    editLayout->addWidget(label3,1,0,1,1,Qt::AlignRight);
    editLayout->addWidget(heightLimit,1,1,1,1,Qt::AlignLeft);
    editLayout->addWidget(unit2,1,2,1,1,Qt::AlignLeft);
    editLayout->addWidget(label4,1,3,1,1,Qt::AlignRight);
    editLayout->addWidget(momentLimit,1,4,1,1,Qt::AlignLeft);
    editLayout->addWidget(unit3,1,5,1,1,Qt::AlignLeft);
    /*左限位右限位布局*/
    editLayout->addWidget(label5,2,0,1,1,Qt::AlignRight);
    editLayout->addWidget(leftLimit,2,1,1,1,Qt::AlignLeft);
    editLayout->addWidget(unit4,2,2,1,1,Qt::AlignLeft);
    editLayout->addWidget(label6,2,3,1,1,Qt::AlignRight);
    editLayout->addWidget(rightLimit,2,4,1,1,Qt::AlignLeft);
    editLayout->addWidget(unit5,2,5,1,1,Qt::AlignLeft);
    /*远限位，进限位布局*/
    editLayout->addWidget(label7,3,0,1,1,Qt::AlignRight);
    editLayout->addWidget(farLimit,3,1,1,1,Qt::AlignLeft);
    editLayout->addWidget(unit6,3,2,1,1,Qt::AlignLeft);
    editLayout->addWidget(label8,3,3,1,1,Qt::AlignRight);
    editLayout->addWidget(closeLimit,3,4,1,1,Qt::AlignLeft);
    editLayout->addWidget(unit7,3,5,1,1,Qt::AlignLeft);
    /*传感器使能布局*/
    QGridLayout *sensorLayout = new QGridLayout;
    QString sensorName[8] = { globalData->tc->toUnicode ("回转"),globalData->tc->toUnicode ("幅度"),globalData->tc->toUnicode ("高度"),globalData->tc->toUnicode ("称重"),
                                            globalData->tc->toUnicode ("行走"),globalData->tc->toUnicode ("风速"),globalData->tc->toUnicode ("塔身倾角"),globalData->tc->toUnicode ("塔臂倾角")};
    for (quint8 i = 0 ; i <8 ; i++) {                                                           //创建控件
        sensor[i] = new QCheckBox( sensorName[i], this);
    }
    for (quint8 i = 0 ; i < 4 ; i ++) {                                                          //布局
        sensorLayout->addWidget(sensor[i],0,i,1,1,Qt::AlignLeft);
        sensorLayout->addWidget(sensor[4+i],1,i,1,1,Qt::AlignLeft);
    }
    //sensorLayout->addWidget(limit_test , 0,4,1,1,Qt::AlignLeft);

    QGroupBox *sensorEnable = new QGroupBox (globalData->tc->toUnicode ("传感器使能") , this);
    sensorEnable->setLayout(sensorLayout);
    /*保存退出按键布局*/
    saveButton = new QPushButton(globalData->tc->toUnicode ("保存"),this);
    quitButton = new QPushButton (globalData->tc->toUnicode ("退出"),this);
    QVBoxLayout *buttonLayout = new QVBoxLayout;
    buttonLayout->addWidget(saveButton);
    buttonLayout->addWidget(quitButton);

    /*底层布局*/
    QHBoxLayout *bottomLayout = new QHBoxLayout;
    bottomLayout->addWidget(sensorEnable,5,Qt::AlignCenter);
    bottomLayout->addLayout(buttonLayout,1);
    /*整体布局*/
    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addLayout(editLayout,0,0,4,1);
    mainLayout->addLayout(bottomLayout,4,0,1,1);
    setLayout(mainLayout);

    QSqlTableModel *readData = new QSqlTableModel;
    bool ok;
    readData->setTable("LimitData");
    readData->setFilter("id = 0");
    readData->select();
    if (readData->rowCount() == 1 ) {               // 显示数据
        version->setText(readData->record(0).value("limitVersion").toString());
        leftLimit->setText(readData->record(0).value("leftLimit").toString());
        rightLimit->setText(readData->record(0).value("rightLimit").toString());
        farLimit->setText(readData->record(0).value("farLimit").toString());
        closeLimit->setText(readData->record(0).value("nearLmit").toString());
        heightLimit->setText(readData->record(0).value("highLimit").toString());
        weightLimit->setText(readData->record(0).value("weightLimit").toString());
        momentLimit->setText(readData->record(0).value("momentLimit").toString());
        for (quint8 i = 0 ; i < 8 ; i++ ) {
            if (  readData->record(0).value("enableBit").toInt(&ok) & (1<<i))
                sensor[i]->setChecked(true);
        }
    }
}

void LimitDataTab::saveButtonClicked()  //保存按钮槽函数,获取数值，并跟新全局变量
{
    bool ok;
    qint8 sensorData = 0;
    for (quint8 i=0 ; i < 8 ; i++) {            //去传感器使能数据
    if (sensor[i]->isChecked()) {
        sensorData |= (1<<i);
    } else {
        sensorData &= ~(1<<i);
    }
    }
    //数据库版本

    QSqlTableModel *saveData = new QSqlTableModel;
    saveData->setTable("LimitData");
    saveData->setFilter("id = 0");
    saveData->select();
    if (saveData->rowCount() == 0) {            //表中不存在数据
        int row = 0 ;
        saveData->insertRows(row,1);
        saveData->setData(saveData->index(row,0),0);
        saveData->setData(saveData->index(row,1),version->text().toInt(&ok,10));
        saveData->setData(saveData->index(row,2),leftLimit->text().toDouble(&ok));
        saveData->setData(saveData->index(row,3),rightLimit->text().toDouble(&ok));
        saveData->setData(saveData->index(row,4),farLimit->text().toDouble(&ok));
        saveData->setData(saveData->index(row,5),closeLimit->text().toDouble(&ok));
        saveData->setData(saveData->index(row,6),heightLimit->text().toDouble(&ok));
        saveData->setData(saveData->index(row,7),weightLimit->text().toDouble(&ok));
        saveData->setData(saveData->index(row,8),momentLimit->text().toDouble(&ok));

        saveData->setData(saveData->index(row,9),sensorData);
        if (FALSE == saveData->submitAll()) {
            QMessageBox::warning(this,globalData->tc->toUnicode ("限位信息"),globalData->tc->toUnicode ("限位信息保存失败"),QMessageBox::Yes);
            return;
        }
    }else if (saveData->rowCount() == 1) {              //更新表数据
        QSqlRecord record = saveData->record(0);
        record.setValue("limitVersion",version->text().toInt(&ok,10));
        record.setValue("leftLimit",leftLimit->text().toDouble(&ok));
        record.setValue("rightLimit",rightLimit->text().toDouble(&ok));
        record.setValue("farLimit",farLimit->text().toDouble(&ok));
        record.setValue("nearLmit",closeLimit->text().toDouble(&ok));
        record.setValue("highLimit",heightLimit->text().toDouble(&ok));
        record.setValue("weightLimit",weightLimit->text().toDouble(&ok));
        record.setValue("momentLimit",momentLimit->text().toDouble(&ok));
        record.setValue("enableBit",sensorData);
        saveData->setRecord(0,record);
        if (FALSE == saveData->submitAll()) {
            QMessageBox::warning(this,globalData->tc->toUnicode ("限位信息"),globalData->tc->toUnicode ("修改限位信息失败"),QMessageBox::Yes);
            return;
        }
    }
}

