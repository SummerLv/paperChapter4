#include "proZoneDataTab.h"
ProZoneDataTab::ProZoneDataTab(QWidget *parent)
{
    globalData = GlobalData::getGlobalData();
    //db = new SqlDB;                                     //打开已链接的数据库
    interfaceInit();
    /*按键信号与槽*/
    connect( quitProZone , SIGNAL( clicked( ) ) , parent , SLOT(windowClose( ) ) );
    connect(ProButtons,SIGNAL(buttonClicked(int)) , this , SLOT(proZoneSelect(int)));
    connect(saveProZone,SIGNAL( clicked( ) ) , this , SLOT (saveProZoneClick ()));//保存保护区
    connect(delProZone,SIGNAL( clicked( ) ) , this , SLOT (delProZoneClick ()));//删除保护区
    connect(saveElement,SIGNAL( clicked( ) ) , this , SLOT (saveElementClick ()));//保存当前元素
    connect(delElement , SIGNAL ( clicked() ) , this , SLOT (delElementClick())); //删除元素
    /*COMBOBOX信号与槽*/
    connect(proZoneType , SIGNAL ( currentIndexChanged(int) ) , this , SLOT (proZoneTypeSelect(int))); //保护区类型选择，使高度变成可编辑
    connect(elementType , SIGNAL ( currentIndexChanged(int) ) , this , SLOT (elementTypeSelect(int))); //元素类型选择，改变圆心，半径变成可编辑
    connect(elementNum , SIGNAL ( currentIndexChanged(int) ) , this , SLOT (elementNumSelect(int)));//元素个数选择，改变元素显示区元素编号个数
    connect(elementN , SIGNAL ( currentIndexChanged(int) ) , this , SLOT (elementNSelect(int))); //元素选择，改变每个元素显示信息
}

ProZoneDataTab::~ProZoneDataTab()
{

}


void ProZoneDataTab::interfaceInit()                //保护区主界面
{
    currentProZoneSelect = 0;                           //初始化所选值
    QPushButton *proButton1 = new QPushButton(globalData->tc->toUnicode("保护区1号"),this);
    QPushButton *proButton2 = new QPushButton(globalData->tc->toUnicode("保护区2号"),this);
    QPushButton *proButton3 = new QPushButton(globalData->tc->toUnicode("保护区3号"),this);
    QPushButton *proButton4 = new QPushButton(globalData->tc->toUnicode("保护区4号"),this);
    QPushButton *proButton5 = new QPushButton(globalData->tc->toUnicode("保护区5号"),this);
    QPushButton *proButton6 = new QPushButton(globalData->tc->toUnicode("保护区6号"),this);

    addProZone = new QPushButton(globalData->tc->toUnicode ("新增保护区") , this);
    saveProZone = new QPushButton ( globalData->tc->toUnicode ("保存") , this );
    delProZone = new QPushButton ( globalData->tc->toUnicode ("删除") ,  this );
    quitProZone = new QPushButton ( globalData->tc->toUnicode ( "退出" ) , this );
    saveElement = new QPushButton ( globalData->tc->toUnicode ("保存元素") , this);
    delElement = new QPushButton ( globalData->tc->toUnicode ("删除元素"),this );
//-----------------------------底部按键布局-----------------------------------------------------
    QHBoxLayout *bottomButtons = new QHBoxLayout;
   // bottomButtons->addWidget(addProZone);
    bottomButtons->addWidget(saveProZone);
    bottomButtons->addWidget(delProZone);
    bottomButtons->addWidget(quitProZone);
    bottomButtons->setContentsMargins(100,10,10,10);
//-----------------------------左边按键布局-------------------------------------------------------
    ProButtons = new QButtonGroup;
    ProButtons->addButton(addProZone,0);            //增加保护区按键
    ProButtons->addButton(proButton1,1);
    ProButtons->addButton(proButton2,2);
    ProButtons->addButton(proButton3,3);
    ProButtons->addButton(proButton4,4);
    ProButtons->addButton(proButton5,5);
    ProButtons->addButton(proButton6,6);

    QSqlTableModel *data = new QSqlTableModel;
    data->setTable("ProZoneData");
    data->select();
    for (quint8 i =( data->rowCount()+1); i <7 ; i++) {     //不需要的按键隐藏
        ProButtons->button(i)->setHidden(true);
    }

    QVBoxLayout *VBoxLayout1 = new QVBoxLayout;      //左边第一列垂直分布
    VBoxLayout1->addWidget(proButton1);
    VBoxLayout1->addWidget(proButton2);
    VBoxLayout1->addWidget(proButton3);
    VBoxLayout1->addWidget(proButton4);
    VBoxLayout1->addWidget(proButton5);
    VBoxLayout1->addWidget(proButton6);
    VBoxLayout1->addStretch(1);
    VBoxLayout1->addWidget(addProZone);
//------------------------------中间编辑区布局--------------------------------------

    name = new QLineEdit (globalData->tc->toUnicode ("新建保护区"));
    name->setAlignment(Qt::AlignRight);

    ID = new QLineEdit  (tr ("0"));
    QValidator *validator1 = new QIntValidator (0,64,this);
    ID->setAlignment(Qt::AlignRight);
    ID->setValidator(validator1);

    height = new QLineEdit(tr ("0"));
    QValidator *validator2 = new QDoubleValidator (0,6553.5,1,this);
    height->setValidator(validator2);
    height->setEnabled(false);
    height->setAlignment(Qt::AlignRight);

    //-----元素编辑框---------------------
    elementX = new QLineEdit;
    QValidator *validator3 = new QDoubleValidator (-3276.8,3276.8,1,this);
    elementX->setValidator(validator3);
    elementX->setAlignment(Qt::AlignRight);

    elementY = new QLineEdit;
    QValidator *validator4 = new QDoubleValidator (-3276.8,3276.8,1,this);
    elementY->setValidator(validator4);
    elementY->setAlignment(Qt::AlignRight);

    elementBaginAngle = new QLineEdit;
    QValidator *validator5 = new QDoubleValidator (0,359.9,1,this);
    elementBaginAngle->setValidator(validator5);
    elementBaginAngle->setAlignment(Qt::AlignRight);

    elementEndAngle = new QLineEdit;
    QValidator *validator6 = new QDoubleValidator (0,359.9,1,this);
    elementEndAngle->setValidator(validator6);
    elementEndAngle->setAlignment(Qt::AlignRight);

    elementR = new QLineEdit;
    QValidator *validator7 = new QDoubleValidator (0,6553.5,1,this);
    elementR->setValidator(validator7);
    elementR->setAlignment(Qt::AlignRight);


    buildType = new QComboBox;
    proZoneType = new QComboBox;
    proZoneType->addItem(globalData->tc->toUnicode("0禁行区"));
    proZoneType->addItem(globalData->tc->toUnicode ("1障碍物"));
    elementN = new QComboBox;
    elementType = new QComboBox;
    elementType->addItem(globalData->tc->toUnicode ("0点元素"));
    elementType->addItem( globalData->tc->toUnicode ("1圆弧元素"));
    elementNum = new QComboBox;
    elementNum->addItem(tr ("3"));
    elementNum->addItem(tr ("4"));
    elementNum->addItem(tr ("5"));
    elementNum->addItem(tr ("6"));
    elementNum->addItem(tr ("7"));
    elementNum->addItem(tr ("8"));
    elementNum->addItem(tr ("9"));
    elementNum->addItem(tr ("10"));
   // QGridLayout *midEditLayout = new QGridLayout;
   // QString labelName[] = {tr("名称") , tr( "ID") , tr( "建筑类别") , tr( "类型") , tr("高度") ,tr( "元素个数")};
    QString buildTypeName[]= {globalData->tc->toUnicode("0其他") , globalData->tc->toUnicode( "1医院") , globalData->tc->toUnicode( "2学校") , globalData->tc->toUnicode( "3广场") ,
                                            globalData->tc->toUnicode("4道路") ,globalData->tc->toUnicode( "5居民区") ,globalData->tc->toUnicode( "6办公区") ,globalData->tc->toUnicode( "7高压线")};
    QString elementNumber[] = {globalData->tc->toUnicode("元素1") , globalData->tc->toUnicode( "元素2") , globalData->tc->toUnicode( "元素3") , globalData->tc->toUnicode( "元素4") ,globalData->tc->toUnicode ("元素5"),globalData->tc->toUnicode ("元素6"),
                                              globalData->tc->toUnicode("元素7") ,globalData->tc->toUnicode( "元素8") ,globalData->tc->toUnicode( "元素9") ,globalData->tc->toUnicode( "元素10")};
    //---------------添加建筑类型下拉标签----------------------------
         for (quint8 i = 0 ; i < 8 ; i++) {
             buildType->addItem(buildTypeName[i]);
         }
   //----------------添加元素编号------------------------------------
         for (quint8 i = 0 ; i < 10 ; i++) {
             elementN->addItem(elementNumber[i]);
         }

//-------------------名称和类型网格布局--------------------------
    QGridLayout *gridLayout1 = new QGridLayout;
    QLabel *label0 = new QLabel(globalData->tc->toUnicode ("名称"));
    gridLayout1->addWidget(label0,0,0,1,1,Qt::AlignRight);
    gridLayout1->addWidget(name,0,1,1,1,Qt::AlignCenter);
    QLabel *label1 = new QLabel(globalData->tc->toUnicode ("类型"));
    gridLayout1->addWidget(label1,1,0,1,1,Qt::AlignRight);
    gridLayout1->addWidget(proZoneType,1,1,1,1,Qt::AlignCenter);
//--------------------ID和高度网格布局--------------------------------
    QGridLayout *gridLayout2 = new QGridLayout;
    QLabel *label2 = new QLabel (tr ("ID"));
    gridLayout2->addWidget(label2 , 0,0,1,1,Qt::AlignRight);
    gridLayout2->addWidget(ID,0,1,1,1,Qt::AlignCenter);
    QLabel *label3 = new QLabel ( globalData->tc->toUnicode ("高度"));
    gridLayout2->addWidget(label3,1,0,1,1,Qt::AlignRight);
    gridLayout2->addWidget(height,1,1,1,1,Qt::AlignCenter);
    QLabel *label4 = new QLabel ( globalData->tc->toUnicode ("米"));
    gridLayout2->addWidget(label4,1,2,1,1,Qt::AlignLeft);
//---------------------建筑类别和元素个数网格布局---------------
    QGridLayout *gridLayout3 = new QGridLayout;
    QLabel *label5 = new QLabel (globalData->tc->toUnicode ("建筑类别"));
    gridLayout3->addWidget(label5,0,0,1,1,Qt::AlignRight);
    gridLayout3->addWidget( buildType,0,1,1,1,Qt::AlignCenter);
    QLabel *label6 = new QLabel (globalData->tc->toUnicode ("元素个数"));
    gridLayout3->addWidget(label6 , 1,0,1,1,Qt::AlignRight);
    gridLayout3->addWidget(elementNum,1,1,1,1,Qt::AlignCenter);

    //---------------------编辑框整体布局------------------------------
        QHBoxLayout *midEditLayout = new QHBoxLayout;
        midEditLayout->addLayout(gridLayout1);
        midEditLayout->addSpacing(20);
        midEditLayout->addLayout(gridLayout2);
        midEditLayout->addSpacing(20);
        midEditLayout->addLayout(gridLayout3);

//-------------------元素信息信息group-------------------------------------------
    //2014.03.03更新：不使用group
    //QGroupBox *elementGroup = new QGroupBox(tr ("元素信息"));
   // QGridLayout *elementGrid = new QGridLayout;
    QGridLayout *elementV1 = new QGridLayout;
    QLabel *label7 = new QLabel( globalData->tc->toUnicode ("元素编号"));
    elementV1->addWidget(label7,0,0,1,1,Qt::AlignLeft);
    elementV1->addWidget(elementN,0,1,1,2,Qt::AlignLeft);

    QLabel *label8 = new QLabel( globalData->tc->toUnicode ("元素类型"));
    elementV1->addWidget(label8,0,2,1,1,Qt::AlignLeft);
    elementV1->addWidget(elementType,0,3,1,2,Qt::AlignLeft);
 //-----------------------------------------------------------------------------------------------------------------
   QGridLayout *elementV2 = new QGridLayout;
    QLabel *label9 = new QLabel ( globalData->tc->toUnicode ( "点坐标X"));
    elementV2->addWidget(label9,0,0,1,1,Qt::AlignLeft);
    elementV2->addWidget(elementX,0,1,1,1,Qt::AlignLeft);
    QLabel *label10 = new QLabel (globalData->tc->toUnicode ("米"));
    elementV2->addWidget(label10,0,2,1,1,Qt::AlignLeft);

    QLabel *label11 = new QLabel ( globalData->tc->toUnicode ( "点坐标Y"));
    elementV2->addWidget(label11,0,3,1,1,Qt::AlignLeft);
    elementV2->addWidget(elementY,0,4,1,1,Qt::AlignLeft);
    QLabel *label12 = new QLabel (globalData->tc->toUnicode ("米"));
    elementV2->addWidget(label12,0,5,1,1,Qt::AlignLeft);
//-----------------------------------------------------------------------------------------------------------------
    QGridLayout *elementV3 = new QGridLayout;
    QLabel *label13 = new QLabel (globalData->tc->toUnicode ("起始角度"));
    elementV3->addWidget(label13,0,0,1,1,Qt::AlignLeft);
    elementV3->addWidget(elementBaginAngle,0,1,1,1,Qt::AlignLeft);
    QLabel *label14 = new QLabel (globalData->tc->toUnicode ("度"));
    elementV3->addWidget(label14,0,2,1,1,Qt::AlignLeft);

    QLabel *label15 = new QLabel (globalData->tc->toUnicode ("半径"));
    elementV3->addWidget(label15,0,3,1,1,Qt::AlignLeft);
    elementV3->addWidget(elementR,0,4,1,1,Qt::AlignLeft);
    QLabel *label16 = new QLabel ( globalData->tc->toUnicode ("米"));
    elementV3->addWidget(label16,0,5,1,1,Qt::AlignLeft);

//-----------------------------------------------------------------------------------------------------------------
    QGridLayout *elementV4 = new QGridLayout;
    QLabel *label17 = new QLabel (globalData->tc->toUnicode ("终止角度"));
    elementV4->addWidget(label17,0,0,1,1,Qt::AlignLeft);
    elementV4->addWidget(elementEndAngle,0,1,1,1,Qt::AlignLeft);
    QLabel *label18 = new QLabel (globalData->tc->toUnicode ("度"));
    elementV4->addWidget(label18,0,2,1,1,Qt::AlignLeft);
    QHBoxLayout *saveele = new QHBoxLayout;
    saveele->addWidget(saveElement);
    saveele->setSpacing(40);
    saveele->addWidget(delElement);
    elementV4->addLayout(saveele,0,3,1,1,Qt::AlignLeft);
    //画图空白区

//--------------------元素框布局---------------------------------------------------------
    QVBoxLayout * elementH = new QVBoxLayout;
   // elementH->addSpacing(80);
    elementH->addLayout(elementV1);
    elementH->setSpacing(10);
    elementH->addLayout(elementV2);
    elementH->setSpacing(10);
    elementH->addLayout(elementV3);
    elementH->setSpacing(10);
    elementH->addLayout(elementV4);
   // elementGroup->setLayout(elementH);
   // elementGroup->setFixedSize(400,200);

    QGroupBox *paintZone = new QGroupBox(globalData->tc->toUnicode ("保护区预览"));//预览区
    //elementH->addLayout(paintZone);
   // paintZone->setFixedSize(300,200);
     QHBoxLayout *elementLayout = new QHBoxLayout;
      elementLayout->addLayout(elementH);
      //elementLayout->addLayout(paintZone);
    elementLayout->addWidget(paintZone,3,0);

//----------------------元素框与上层编辑框合并 ，上下结构-------------------------------
    QGridLayout *midVLayout = new QGridLayout;
    midVLayout->addLayout(midEditLayout,0,0,1,1);
    midVLayout->addLayout(elementLayout,1,0,3,1);


//----------------------中层整体水平布局,左右结构------------------------------------------------
    QHBoxLayout *midHLayout = new QHBoxLayout;
    midHLayout->addLayout(VBoxLayout1);
    midHLayout->addLayout(midVLayout);


//-------------------------------------主布局-------------------------------------------
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(midHLayout);
    mainLayout->addLayout(bottomButtons);
    setLayout(mainLayout);



}

void ProZoneDataTab::editInterfaceRefresh()         //编辑区刷新
{

    delProZone->setEnabled(false);                          //删除保护区按键无效
    name->setText( tr ("新建保护区"));
    ID->setText(tr ("0"));
    height->setText(tr ("0"));
    height->setEnabled(false);
    elementNum->setCurrentIndex(0);
    buildType->setCurrentIndex(0);
    proZoneType->setCurrentIndex(0);
    elementN->setMaxCount(10);
    elementN->setCurrentIndex(0);
    elementType->setCurrentIndex(0);
    elementX->setText(tr ("0"));
    elementY->setText(tr ("0"));
    elementBaginAngle->setText(tr ("0"));
    elementBaginAngle->setEnabled(false);
    elementEndAngle->setText(tr ("0"));
    elementEndAngle->setEnabled(false);
    elementR->setText(tr ("0"));
    elementR->setEnabled(false);


}


void ProZoneDataTab::proZoneSelect(int i)
{
    bool ok;
    currentProZoneSelect = i;               //保存当前所选保护区编号
    ElementData elementData;
    elementDataList.clear();
    if (0 == i)                             //新增保护区被按下
    {
        /*新增保护区,清空显示区域*/
        editInterfaceRefresh();
        elementDataList.clear();
        for (quint8 k= 0 ; k < 3 ; k++) {
            elementData.elementType = 0;
            elementData.pointX = 0 ;
            elementData.pointY = 0 ;
            elementData.startAngle = 0;
            elementData.endAngle =0;
            elementData.radius = 0;
            elementDataList.append(elementData);
        }


    } else {                                                                                           //保护区i被选中
        /*查看并修改保护区*/
        delProZone->setEnabled(true);                          //删除保护区按键无效
        //数据库版本，固定信息显示
        QSqlTableModel *proZoneData = new QSqlTableModel;
        proZoneData->setTable("ProZoneData");                                   // 打开保护区的表
        proZoneData->select();
        if (proZoneData->rowCount() > 0) {
            ProZoneDataKeyID = proZoneData->record(i-1).value("id").toInt();
            name->setText(proZoneData->record(i-1).value("proZoneName").toString());
            ID->setText(proZoneData->record(i-1).value("proZoneID").toString());
            if (1 == proZoneData->record(i-1).value("proZoneType").toInt(&ok)) {              ////如果是障碍物，才显示高度
                height->setEnabled(true);
                height->setText(proZoneData->record(i-1).value("proZoneHeight").toString());
            } else if (0 == proZoneData->record(i-1).value("proZoneType").toInt(&ok)) {           //如果是禁行区，不可编辑
                height->setEnabled(false);
            }
            buildType->setCurrentIndex(proZoneData->record(i-1).value("proBuildType").toInt(&ok));
            proZoneType->setCurrentIndex(proZoneData->record(i-1).value("proZoneType").toInt(&ok));
            elementNum->setCurrentIndex(proZoneData->record(i-1).value("proZoneEleNum").toInt(&ok));
        }

        //数据库版本,元素显示
        QSqlTableModel *element = new QSqlTableModel;
        element->setTable("ElementData");
        element->setFilter(QString("proZoneID = %1").arg(ProZoneDataKeyID));
        element->select();
        qDebug()<<element->rowCount();
        for (int m = 0 ; m < element->rowCount() ; m++) {                           //取出相应保护区中的元素信息
            elementData.elementType = element->record(m).value("elementType").toInt(&ok);
            elementData.pointX = element->record(m).value("pointX").toDouble(&ok) ;
            elementData.pointY = element->record(m).value("pointY").toDouble(&ok) ;
            elementData.startAngle = element->record(m).value("startAngle").toInt(&ok);
            elementData.endAngle =element->record(m).value("endAngle").toInt(&ok);
            elementData.radius = element->record(m).value("radius").toDouble(&ok);
            elementDataList.append(elementData);
        }
        //---------------------元素区显示,显示第一个元素信息-------------------------------
        elementN->setCurrentIndex(0);
        elementType->setCurrentIndex(elementDataList[0].elementType);
        elementX->setText(QString::number(elementDataList[0].pointX));
        elementY->setText(QString::number(elementDataList[0].pointY));
        if (elementType->currentIndex() == 1) {
            elementBaginAngle->setText(QString::number(elementDataList[0].startAngle));
            elementEndAngle->setText(QString::number(elementDataList[0].endAngle));
            elementR->setText(QString::number(elementDataList[0].radius));
        } else {                //点元素不可编辑
            elementBaginAngle->setEnabled(false);
            elementEndAngle->setEnabled(false);
            elementR->setEnabled(false);
        }
    }
}


void ProZoneDataTab::proZoneTypeSelect(int i)                      //保护区类型选择，使高度变成可编辑
{
    if (0 == i )             //选择了禁行区
        height->setEnabled(false);
    else if (1 == i)        //选择了障碍物
        height->setEnabled(true);

}

void ProZoneDataTab::elementNumSelect(int i)                      //元素个数选择，改变元素显示区元素编号个数
{
    ElementData elementData;                            //元素信息
    quint8 tmp;
    tmp =  elementDataList.count();         //当前存有的保护区元素个数
    if ( (i + 3) > tmp )                //所选个数大于当前个数,在LIST中添加多出来的元素个数
    {
        for ( quint8 j =0 ; j< (i+3-tmp) ; j++) {
        elementData.elementType = 0;
        elementData.pointX = 0;
        elementData.pointY = 0;
        elementData.startAngle = 0;
        elementData.endAngle = 0;
        elementData.radius = 0;
        elementDataList.append(elementData);
        }
    }else{                          //所选个数小于当前存有的个数，删掉LIST中多余的个数
        for (quint8 j = 0 ; j < (tmp-(i+3)) ; j++) {
            elementDataList.removeLast();
        }

    }

}

void ProZoneDataTab::elementTypeSelect(int i)                   //元素类型选择，改变圆心，半径变成可编辑
{
    switch (i)
    {
    case 0:                                                             //选择点元素
        elementBaginAngle->setEnabled(false);
        elementEndAngle->setEnabled(false);
        elementR->setEnabled(false);
        break;
    case 1:                                                            //选择圆弧元素
        elementBaginAngle->setEnabled(true);
        elementEndAngle->setEnabled(true);
        elementR->setEnabled(true);
        break;
    default:
        break;
    }
}

void ProZoneDataTab::elementNSelect(int i)                          //元素选择，改变每个元素显示信息
{
    if ( i >= (elementNum->currentIndex() + 3)){
        QMessageBox::warning(this,globalData->tc->toUnicode ("选择错误"),globalData->tc->toUnicode ("所选元素编号大于元素个数"),QMessageBox::Yes);
        elementN->setCurrentIndex(0);
        return;
    }
                                         //如果是已存保护区,找出已经存在的保护区的数据做显示
        if ( i >= elementDataList.count()) {                 //如果所选元素编号大于实际元素个数
            elementType->setCurrentIndex(0);           //初始设置为点元素
            elementX->setText(tr ("0"));
            elementY->setText(tr ("0"));
            elementBaginAngle->setEnabled(false);
            elementEndAngle->setEnabled(false);
            elementR->setEnabled(false);
        }else{
        elementType->setCurrentIndex((elementDataList.at(i).elementType));
        elementX->setText(QString::number(elementDataList.at(i).pointX));
        elementY->setText(QString::number(elementDataList.at(i).pointY));
        if (elementType->currentIndex() == 0) {                 //元素是点
            elementBaginAngle->setText(tr ("0"));
            elementBaginAngle->setEnabled(false);
            elementEndAngle->setText(tr ("0"));
            elementEndAngle->setEnabled(false);
            elementR->setText(tr ("0"));
            elementR->setEnabled(false);
        } else{                                                                //元素是圆弧
            elementBaginAngle->setText(QString::number(elementDataList.at(i).startAngle));
            elementEndAngle->setText(QString::number(elementDataList.at(i).endAngle));
            elementR->setText(QString::number(elementDataList.at(i).radius));
        }

    }

}

void ProZoneDataTab::saveElementClick()                              //暂存所选元素信息
{
    ElementData elementData;                            //元素信息
    bool ok;
    if (elementN->currentIndex() > (elementNum->currentIndex()+3))          //如果保存的元素比当前总个数还大，直接返回
    {
        QMessageBox::warning(this,"选择错误","所选元素编号大于元素总数",QMessageBox::Yes);
        return;
    }


    elementData.elementType = elementType->currentIndex();
    elementData.pointX = elementX->text().toDouble(&ok);
    elementData.pointY = elementY->text().toDouble(&ok);
    elementData.startAngle = elementBaginAngle->text().toDouble(&ok);
    elementData.endAngle = elementEndAngle->text().toDouble(&ok);
    elementData.radius = elementR->text().toDouble(&ok);
   // qDebug()<< elementData.pointX<<elementData.pointY<<elementData.startAngle<< elementData.endAngle<<elementData.radius;
    elementDataList.replace(elementN->currentIndex(),elementData);//替换掉当前的元素数据


}
void ProZoneDataTab::delElementClick()                              //删除所选元素信息
{
    if (elementDataList.count()<4) {         //元素不得低于3个
        QMessageBox::warning(this,globalData->tc->toUnicode ("删除错误"),globalData->tc->toUnicode ("元素不得低于3个"),QMessageBox::Yes);
        return;
    }

    elementDataList.removeAt(elementN->currentIndex());         //删除当前元素信息,并清空显示
    elementType->setCurrentIndex(elementDataList.at(0).elementType);
    elementX->setText(QString::number(elementDataList.at(0).pointX));
    elementY->setText(QString::number(elementDataList.at(0).pointY));
    if (elementType->currentIndex() == 0) {

    elementBaginAngle->setEnabled(false);
    elementEndAngle->setEnabled(false);
    elementR->setEnabled(false);
    }else{
    elementBaginAngle->setText(QString::number(elementDataList.at(0).startAngle));
    elementEndAngle->setText(QString::number(elementDataList.at(0).endAngle));
    elementR->setText(QString::number(elementDataList.at(0).radius));
    }
    //元素个数-1
    elementNum->setCurrentIndex(elementNum->currentIndex() -1);
    elementN->setCurrentIndex(0);


}


void ProZoneDataTab::saveProZoneClick()                         //保存编辑好的保护区信息
{
    bool ok;
    //--------------------数据库版本----------------------------------
  /* 数据库中保护区格式
   * 0 id integer primary key ,
      1 proZoneVersion int,    //信息版本
      2 proZoneName varchar(16),"      //保护区名称（16字节）
      3 proZoneID int,         //保护区ID（1Bytes）
      4 proZoneType int,             //保护区类型（1bit  [7]）
      5 proZoneOrder int,            //保护区序号（3bits [6:4]）
      6 proZoneEleNum int,                 //保护区元素个数（4bits [3:0]）三种合成一个Byte
      7 proBuildType int,           //保护区建筑类型（1Bytes）
      8 proZoneHeight double                    //保护区高度（2Bytes）*/
    /*数据库中元素信息格式
    id integer primary key
    proZoneID int
    quint8 elementType;          //元素类型（0x00点，0x01圆弧）
    double pointX;                  //X坐标(（2Bytes）-3276.8-3276.8)
    double pointY;                  //Y坐标(（2Bytes）-3276.8-3276.8)
    double radius;                 //圆半径(（2Bytes）0-6553.5)
    double startAngle;              //起始角度(（2Bytes）0-360)
    double endAngle;                //终止角度(（2Bytes）0-360)*/
    quint32 keyID = 0;                                  //保护区ID键值
    QSqlTableModel *saveProZoneData = new QSqlTableModel;
    QSqlTableModel *element = new QSqlTableModel;
    QSqlQueryModel *findID = new QSqlQueryModel;
    //quint8 elementRow;                  //元素信息添加行
    quint8 proZoneRow = 0;                  //保护区信息添加行
    saveProZoneData->setTable("ProZoneData");


    element->setTable("ElementData");
    if ( 0 == currentProZoneSelect ) {                          //新增加的保护区

        saveProZoneData->select();
        for (qint32 k = 0 ; k < saveProZoneData->rowCount() ; k++) {
            if (saveProZoneData->record(k).value("proZoneID").toInt() == ID->text().toShort(&ok,10)) {
                QMessageBox::warning(this , globalData->tc->toUnicode ("保存保护区") , globalData->tc->toUnicode ("保存保护区失败：ID重复"),QMessageBox::Yes);
                return;
            }
        }
        proZoneRow = 0;
        saveProZoneData->insertRows(proZoneRow,1);
        saveProZoneData->setData(saveProZoneData->index(proZoneRow,1),2);                           //保护区信息版本，
        saveProZoneData->setData(saveProZoneData->index(proZoneRow,2),name->text());
        saveProZoneData->setData(saveProZoneData->index(proZoneRow,3),ID->text().toShort(&ok,10));
        saveProZoneData->setData(saveProZoneData->index(proZoneRow,4),proZoneType->currentIndex());
        saveProZoneData->setData(saveProZoneData->index(proZoneRow,5),0);                           //保留字段
        saveProZoneData->setData(saveProZoneData->index(proZoneRow,6),elementNum->currentIndex());
        saveProZoneData->setData(saveProZoneData->index(proZoneRow,7),buildType->currentIndex());
        saveProZoneData->setData(saveProZoneData->index(proZoneRow,8),height->text().toDouble(&ok));
        if (FALSE == saveProZoneData->submitAll() ) {
            QMessageBox::warning(this , globalData->tc->toUnicode ("增加保护区") , globalData->tc->toUnicode ("增加保护区失败"),QMessageBox::Yes);
            return;
        }

        findID->setQuery(QString("SELECT id FROM ProZoneData WHERE proZoneID = %1").arg(ID->text().toShort(&ok,10)));
        keyID = findID->record(0).value(0).toInt();
        /*重新布局按键*/
        saveProZoneData->select();
        if (saveProZoneData->rowCount() > 0) {
        for (quint8 i =(saveProZoneData->rowCount()+1); i <7 ; i++) {     //不需要的按键隐藏
            ProButtons->button(i)->setHidden(true);
        }
        for (quint8 i = 1 ; i < saveProZoneData->rowCount()+1 ; i ++ ){
            ProButtons->button(i)->setHidden(false);
        }
        }
    } else {
        saveProZoneData->select();
       // qDebug()<<"saveProZoneData->rowCount()"<<saveProZoneData->rowCount();
        if (saveProZoneData->rowCount() > 0 ) {
            QSqlRecord record = saveProZoneData->record(currentProZoneSelect-1);          //当前选中的保护区
            record.setValue("proZoneName",name->text());
            record.setValue("proZoneID",ID->text().toShort(&ok,10));
            record.setValue("proZoneType",proZoneType->currentIndex());
            record.setValue("proZoneOrder",0);
            record.setValue("proZoneEleNum",elementNum->currentIndex());
            record.setValue("proBuildType",buildType->currentIndex());
            record.setValue("proZoneHeight",height->text().toDouble(&ok));
            saveProZoneData->setRecord(currentProZoneSelect-1,record);
            if (FALSE == saveProZoneData->submitAll() ) {
                QMessageBox::warning(this , globalData->tc->toUnicode ("修改保护区") , globalData->tc->toUnicode ("修改保护区失败"),QMessageBox::Yes);
                return;
            }
            element->setFilter(QString("proZoneID = %1").arg(ProZoneDataKeyID));
            element->select();
            if (element->rowCount() > 0) {
                element->removeRows(0,element->rowCount());
                element->submitAll();
            }
            keyID = ProZoneDataKeyID;
        }


    }

    /*更新元素信息表*/
    qDebug()<<elementDataList.count();
    for (quint8 i = 0 ; i < elementDataList.count() ; i++ ) {
        element->insertRows(i,1);
        element->setData(element->index(i,1),keyID);
        element->setData(element->index(i,2),elementDataList.at(i).elementType);
        element->setData(element->index(i,3),elementDataList.at(i).pointX);
        element->setData(element->index(i,4),elementDataList.at(i).pointY);
        element->setData(element->index(i,5),elementDataList.at(i).radius);
        element->setData(element->index(i,6),elementDataList.at(i).startAngle);
        element->setData(element->index(i,7),elementDataList.at(i).endAngle);
        if (FALSE == element->submitAll() ) {
            QMessageBox::warning(this , globalData->tc->toUnicode ("增加保护区元素") , globalData->tc->toUnicode ("增加保护区元素失败"),QMessageBox::Yes);
            return;
        }
    }
    editInterfaceRefresh();
    //清空临时list
    elementDataList.clear();
}


void ProZoneDataTab::delProZoneClick()                                //删除保护区
{

    //---------------------数据库版本-----------------------------
    /*删除保护区表中数据*/
   // bool ok;
    quint32 keyID;                      //保护区键值ID
    QSqlTableModel *deleteData = new QSqlTableModel;
    deleteData->setTable("ProZoneData");
    deleteData->select();
    if (deleteData->rowCount() > 0 ) {
        keyID = deleteData->record(currentProZoneSelect-1).value(0).toInt();
        deleteData->removeRows(currentProZoneSelect-1,1);
        if (deleteData->submitAll()) {
        for (quint8 i = deleteData->rowCount()+1 ; i <7 ; i++)      //不需要的按键隐藏
            ProButtons->button(i)->setHidden(true);
        }
    }else{
            QMessageBox::warning(this , globalData->tc->toUnicode ("删除保护区") , globalData->tc->toUnicode ("删除保护区失败"),QMessageBox::Yes);
            return;
        }
    /*删除元素信息表中的数据*/
    QSqlTableModel *elementDelete = new QSqlTableModel;
    elementDelete->setTable("ElementData");
    elementDelete->setFilter(QString("proZoneID = %1").arg(keyID));
    elementDelete->select();
    qDebug()<<"elementDelete->rowCount()"<<elementDelete->rowCount();
    if (elementDelete->rowCount() > 0 ) {
        elementDelete->removeRows(0,elementDelete->rowCount());
        elementDelete->submitAll();
    }else{
        QMessageBox::warning(this , globalData->tc->toUnicode ("删除保护区") , globalData->tc->toUnicode ("删除保护区失败"),QMessageBox::Yes);
        return;
    }
   // ProButtons->button(proZoneNum)->setHidden(false);
   // ProButtons->button(proZoneNum)->setEnabled(false);                  //新建保护区不可选
    editInterfaceRefresh();
    elementDataList.clear();
    currentProZoneSelect = 0;
    ElementData elementData;                            //元素信息
    for (quint8 k= 0 ; k < 3 ; k++) {
        elementData.elementType = 0;
        elementData.pointX = 0 ;
        elementData.pointY = 0 ;
        elementData.startAngle = 0;
        elementData.endAngle =0;
        elementData.radius = 0;
        elementDataList.append(elementData);
    }
}
