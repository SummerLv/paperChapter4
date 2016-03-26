#include "basicDataDialog.h"
#include <QPalette>
#include <QDebug>
 #include <QValidator>
/*基本信息配置界面及操作*/
BasicDataTab::BasicDataTab(QWidget *parent)
{
    globalData = GlobalData::getGlobalData();
     db = new SqlDB;                                     //打开已链接的数据库

    interfaceInit();                                        //界面初始化
    connect( saveButton , SIGNAL ( clicked() ) , this , SLOT(saveButtonClicked() ) );
    connect( cancelButton , SIGNAL( clicked( ) ) , parent , SLOT(windowClose( ) ) );
}

BasicDataTab::~BasicDataTab()
{

}

void BasicDataTab::interfaceInit()
{
    globalData = GlobalData::getGlobalData();
    QLabel *versionLable = new QLabel ( globalData->tc->toUnicode ("信息版本") );
    QLabel *firmLable = new QLabel ( globalData->tc->toUnicode ("厂商代号") );
    QLabel *craneTypeLable = new QLabel ( globalData->tc->toUnicode ("塔吊类型") );
    QLabel *craneNameLable = new QLabel ( globalData->tc->toUnicode ("塔吊名称") );

    QLabel *craneGIDLable = new QLabel  ( globalData->tc->toUnicode ("塔群编号") );
    QLabel *craneIDLable = new QLabel  ( globalData->tc->toUnicode ("塔机编号") );
    QLabel *craneRateLable = new QLabel  ( globalData->tc->toUnicode ("吊绳倍率") );
    QLabel *craneTopLable = new QLabel  ( globalData->tc->toUnicode ("塔顶高度") );

    QLabel *craneXLable = new QLabel  ( globalData->tc->toUnicode ("塔机X坐标") );
    QLabel *craneYLable = new QLabel  ( globalData->tc->toUnicode ("塔机Y坐标") );
    QLabel *foreArmLable = new QLabel  ( globalData->tc->toUnicode ("前臂长") );
    QLabel *backArmLable = new QLabel  ( globalData->tc->toUnicode ("后臂长") );

    QLabel *sectionNumLable = new QLabel  ( globalData->tc->toUnicode ("塔身节数") );
    QLabel *preHeightLable = new QLabel  ( globalData->tc->toUnicode ("塔节高度") );
    QLabel *initHeightLable = new QLabel  ( globalData->tc->toUnicode ("首节高度") );
    QLabel *craneHeightLable = new QLabel  ( globalData->tc->toUnicode ("塔身高度") );

    QLabel *IMEILable = new QLabel (globalData->tc->toUnicode ("IMEI"));

    QLabel *unitLable1 = new QLabel (globalData->tc->toUnicode ("米"));
    QLabel *unitLable2 = new QLabel (globalData->tc->toUnicode ("米"));
    QLabel *unitLable3 = new QLabel (globalData->tc->toUnicode ("米"));
    QLabel *unitLable4 = new QLabel (globalData->tc->toUnicode ("米"));
    QLabel *unitLable5 = new QLabel (globalData->tc->toUnicode ("米"));
    QLabel *unitLable6 = new QLabel (globalData->tc->toUnicode ("米"));
    QLabel *unitLable7 = new QLabel (globalData->tc->toUnicode ("米"));
    QLabel *unitLable8 = new QLabel (globalData->tc->toUnicode ("米"));

    version = new QLineEdit;
    version->setAlignment(Qt::AlignRight); //设置右对齐
    QValidator *validator = new QIntValidator (0,65535,this);       //设置显示范围
    version->setValidator(validator);

    firmID = new QLineEdit;
    firmID->setAlignment(Qt::AlignRight); //设置右对齐

    craneType = new QComboBox();
    craneType->addItem(globalData->tc->toUnicode("固定塔吊"));
    craneType->addItem(globalData->tc->toUnicode("行走塔吊"));

    craneName = new QLineEdit;
    craneName->setAlignment(Qt::AlignRight); //设置右对齐
    groupID = new QLineEdit;
    groupID->setAlignment(Qt::AlignRight); //设置右对齐
    craneID = new QLineEdit;
    craneID->setAlignment(Qt::AlignRight); //设置右对齐
    craneTopheight = new QLineEdit;
    craneTopheight->setAlignment(Qt::AlignRight); //设置右对齐

    CraneRate = new QComboBox();
    CraneRate->addItem(globalData->tc->toUnicode("2倍吊绳"));
    CraneRate->addItem(globalData->tc->toUnicode("4倍吊绳"));

    craneX = new QLineEdit;
    craneX->setAlignment(Qt::AlignRight); //设置右对齐
    craneY = new QLineEdit;
    craneY->setAlignment(Qt::AlignRight); //设置右对齐
    foreArmLen = new QLineEdit;
    foreArmLen->setAlignment(Qt::AlignRight); //设置右对齐
    backArmLen = new QLineEdit;
    backArmLen->setAlignment(Qt::AlignRight); //设置右对齐
    craneHeight = new QLineEdit;
    craneHeight->setAlignment(Qt::AlignRight); //设置右对齐
    sectionNum = new QLineEdit;
    sectionNum->setAlignment(Qt::AlignRight); //设置右对齐
    preHeight = new QLineEdit;
    preHeight->setAlignment(Qt::AlignRight); //设置右对齐
    firstSectionH = new QLineEdit;
    firstSectionH->setAlignment(Qt::AlignRight); //设置右对齐
    IMEI = new QLineEdit;
    IMEI->setAlignment(Qt::AlignRight); //设置右对齐


     QGridLayout *v1Layout = new QGridLayout;
     v1Layout->addWidget(versionLable,0,0,1,1,Qt::AlignCenter);//添加label
     v1Layout->addWidget(firmLable,1,0,1,1,Qt::AlignCenter);
     v1Layout->addWidget(craneTypeLable,2,0,1,1,Qt::AlignCenter);
     v1Layout->addWidget(craneNameLable,3,0,1,1,Qt::AlignCenter);
     v1Layout->addWidget(version,0,1,1,1,Qt::AlignCenter);//添加编辑框
     v1Layout->addWidget(firmID,1,1,1,1,Qt::AlignCenter);
     v1Layout->addWidget(craneType,2,1,1,1,Qt::AlignCenter);
     v1Layout->addWidget(craneName,3,1,1,1,Qt::AlignCenter);

     QGridLayout *v2Layout = new QGridLayout;
     v2Layout->addWidget(craneGIDLable,0,0,1,1,Qt::AlignCenter);//添加label
     v2Layout->addWidget(craneIDLable,1,0,1,1,Qt::AlignCenter);
     v2Layout->addWidget(craneRateLable,2,0,1,1,Qt::AlignCenter);
     v2Layout->addWidget(craneTopLable,3,0,1,1,Qt::AlignCenter);
     v2Layout->addWidget(groupID,0,1,1,1,Qt::AlignCenter);//添加编辑框
     v2Layout->addWidget(craneID,1,1,1,1,Qt::AlignCenter);
     v2Layout->addWidget(CraneRate,2,1,1,1,Qt::AlignCenter);
     v2Layout->addWidget(craneTopheight,3,1,1,1,Qt::AlignCenter);
     v2Layout->addWidget(unitLable1,3,3,1,1,Qt::AlignLeft);//添加单位

     QGridLayout *v3Layout = new QGridLayout;
     v3Layout->addWidget(craneXLable,0,0,1,1,Qt::AlignCenter);//添加label
     v3Layout->addWidget(craneYLable,1,0,1,1,Qt::AlignCenter);
     v3Layout->addWidget(foreArmLable,2,0,1,1,Qt::AlignCenter);
     v3Layout->addWidget(backArmLable,3,0,1,1,Qt::AlignCenter);
     v3Layout->addWidget(craneX,0,1,1,1,Qt::AlignCenter);//添加编辑框
     v3Layout->addWidget(craneY,1,1,1,1,Qt::AlignCenter);
     v3Layout->addWidget(foreArmLen,2,1,1,1,Qt::AlignCenter);
     v3Layout->addWidget(backArmLen,3,1,1,1,Qt::AlignCenter);
     v3Layout->addWidget(unitLable2,0,2,1,1,Qt::AlignLeft);//添加单位
     v3Layout->addWidget(unitLable3,1,2,1,1,Qt::AlignLeft);
     v3Layout->addWidget(unitLable4,2,2,1,1,Qt::AlignLeft);
     v3Layout->addWidget(unitLable5,3,2,1,1,Qt::AlignLeft);

     QGridLayout *v4Layout = new QGridLayout;
     v4Layout->addWidget(sectionNumLable,0,0,1,1,Qt::AlignCenter);//添加label
     v4Layout->addWidget(preHeightLable,1,0,1,1,Qt::AlignCenter);
     v4Layout->addWidget(initHeightLable,2,0,1,1,Qt::AlignCenter);
     v4Layout->addWidget(craneHeightLable,3,0,1,1,Qt::AlignCenter);
     v4Layout->addWidget(sectionNum,0,1,1,1,Qt::AlignCenter);//添加编辑框
     v4Layout->addWidget(preHeight,1,1,1,1,Qt::AlignCenter);
     v4Layout->addWidget(firstSectionH,2,1,1,1,Qt::AlignCenter);
     v4Layout->addWidget(craneHeight,3,1,1,1,Qt::AlignCenter);
     v4Layout->addWidget(unitLable6,1,2,1,1,Qt::AlignLeft);//添加单位
     v4Layout->addWidget(unitLable7,2,2,1,1,Qt::AlignLeft);
     v4Layout->addWidget(unitLable8,3,2,1,1,Qt::AlignLeft);

     QHBoxLayout *HBoxLayout = new QHBoxLayout;
     HBoxLayout->setSizeConstraint(QLayout :: SetNoConstraint);//不强站位置，可以将窗口压倒高度为0
     HBoxLayout->addLayout(v1Layout);
     HBoxLayout->addSpacing(20);
     HBoxLayout->addLayout(v2Layout);
     HBoxLayout->addSpacing(20);
     HBoxLayout->addLayout(v3Layout);
     HBoxLayout->addSpacing(20);
     HBoxLayout->addLayout(v4Layout);
     //setLayout(HBoxLayout);
     //HBoxLayout->setSizeConstraint();

     saveButton = new QPushButton(globalData->tc->toUnicode("保存") , this);
     cancelButton = new QPushButton(globalData->tc->toUnicode("退出") , this);
     QHBoxLayout *bootmLayout = new QHBoxLayout;
     bootmLayout->addWidget(IMEILable);
     bootmLayout->addWidget(IMEI);
     bootmLayout->addSpacing(200);
     bootmLayout->addWidget(saveButton);
     bootmLayout->addWidget(cancelButton);

     QGridLayout *mainLayout = new QGridLayout;
     mainLayout->addLayout(HBoxLayout,0,0);
    // mainLayout->addSpacing(10);
     mainLayout->addLayout(bootmLayout,1,0);
     this->adjustSize();
     setLayout(mainLayout);

     /*填入数据*/
     QSqlQueryModel *basicDataRead = new QSqlQueryModel(db);     //操作数据库
    basicDataRead->setQuery("SELECT * FROM BasicData where id=0");
    if(basicDataRead->rowCount() == 0)              //数据库中没有数据
        return;
    version->setText( basicDataRead->record(0).value("basicVersion").toString() );   //信息版本输入框
    firmID->setText( basicDataRead->record(0).value("firmID").toString() );     //厂商ID
    craneName->setText( basicDataRead->record(0).value("craneName").toString() );     //塔吊名称
    groupID->setText( basicDataRead->record(0).value("groupID").toString() );     //群ID
    craneID->setText( basicDataRead->record(0).value("craneID").toString() );     //塔ID
    craneTopheight->setText( basicDataRead->record(0).value("craneTopheight").toString() );     //塔顶高度
    craneX->setText( basicDataRead->record(0).value("coordinateX").toString() );     //X坐标
    craneY->setText( basicDataRead->record(0).value("coordinateY").toString() );     //Y坐标
    foreArmLen->setText( basicDataRead->record(0).value("forearmLen").toString() );  //前臂长
    backArmLen->setText( basicDataRead->record(0).value("backarmLen").toString() );  //后臂长
    craneHeight->setText( basicDataRead->record(0).value("craneHeight").toString() );  //塔高
    sectionNum->setText( basicDataRead->record(0).value("craneSectionNum").toString() );  //塔吊节数
    preHeight->setText( basicDataRead->record(0).value("preSectionHeight").toString() );  //节高
    firstSectionH->setText( basicDataRead->record(0).value("firstSectionHeight").toString() );  //首节高
    IMEI->setText( basicDataRead->record(0).value("craneIMEI").toString() );  //首节高
}

void BasicDataTab::saveButtonClicked()              //保存按键槽函数
{
    QSqlTableModel *saveData = new QSqlTableModel(db);
    saveData->setTable("BasicData");
    saveData->setFilter("id = 0");
    saveData->select();
    if (saveData->rowCount() == 1) {                 //数据库中没有数据
        QSqlRecord record = saveData->record(0);
        record.setValue("basicVersion",version->text());
        record.setValue("firmID",firmID->text());
        record.setValue("craneName",craneName->text());
        record.setValue("groupID",groupID->text());
        record.setValue("craneID",craneID->text());
        record.setValue("craneTopheight",craneTopheight->text());
        record.setValue("coordinateX",craneX->text());
        record.setValue("coordinateY",craneY->text());
        record.setValue("forearmLen",foreArmLen->text());
        record.setValue("backarmLen",backArmLen->text());
        record.setValue("craneHeight",craneHeight->text());
        record.setValue("craneSectionNum",sectionNum->text());
        record.setValue("preSectionHeight",preHeight->text());
        record.setValue("firstSectionHeight",firstSectionH->text());
        record.setValue("craneIMEI",IMEI->text());

        saveData->setRecord(0,record);
        if (FALSE == saveData->submitAll()) {
            QMessageBox::warning(this,globalData->tc->toUnicode ("修改基本信息"),globalData->tc->toUnicode ("基本信息修改失败"),QMessageBox::Yes);
        }


    }


}

