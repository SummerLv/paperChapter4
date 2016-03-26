#include "interferecranetab.h"

InterfereCraneTab::InterfereCraneTab(QWidget *parent)
{
    globalData = GlobalData::getGlobalData();
    //db = new SqlDB;                                         //打开数据库
    interfaceInit();
    //currentCrane = 0;
    connect(quitButton , SIGNAL(clicked()) , parent , SLOT(windowClose( ) ) );
    connect(craneGroup , SIGNAL(buttonClicked(int)) , this , SLOT (craneSelect(int)) );
    connect(saveButton , SIGNAL (clicked()) , this , SLOT (saveButtonClicked()));
    connect(delCraneButton , SIGNAL (clicked()) , this , SLOT (delButtonClicked()) );
}
InterfereCraneTab::~InterfereCraneTab()
{

}

void InterfereCraneTab::interfaceInit()
{
    currentCrane = 8;

    QPushButton *button0 = new QPushButton(globalData->tc->toUnicode ("干涉塔吊1号") , this);
    QPushButton *button1 = new QPushButton(globalData->tc->toUnicode ("干涉塔吊2号") , this);
    QPushButton *button2 = new QPushButton(globalData->tc->toUnicode ("干涉塔吊3号") , this);
    QPushButton *button3 = new QPushButton(globalData->tc->toUnicode ("干涉塔吊4号") , this);
    QPushButton *button4 = new QPushButton(globalData->tc->toUnicode ("干涉塔吊5号") , this);
    QPushButton *button5 = new QPushButton(globalData->tc->toUnicode ("干涉塔吊6号") , this);
    QPushButton *button6 = new QPushButton(globalData->tc->toUnicode ("干涉塔吊7号") , this);
    QPushButton *button7 = new QPushButton(globalData->tc->toUnicode ("干涉塔吊8号") , this);

    addCraneButton = new QPushButton(globalData->tc->toUnicode ("新增") , this);
    delCraneButton = new QPushButton(globalData->tc->toUnicode ("删除") , this);
    delCraneButton->setEnabled(false);      //初始删除无效
    saveButton = new QPushButton(globalData->tc->toUnicode ("保存") , this);
    saveButton->setEnabled(false);
    quitButton = new QPushButton(globalData->tc->toUnicode ("退出") , this);

    craneGroup = new QButtonGroup;
    //craneGroup->addButton(addCraneButton,0);
    craneGroup->addButton(button0,0);
    craneGroup->addButton(button1,1);
    craneGroup->addButton(button2,2);
    craneGroup->addButton(button3,3);
    craneGroup->addButton(button4,4);
    craneGroup->addButton(button5,5);
    craneGroup->addButton(button6,6);
    craneGroup->addButton(button7,7);
    craneGroup->addButton(addCraneButton,8);
    //craneGroup->addButton(button8,8);
    /*
    for (quint8 i = 0 ; i < 9 ; i++) {
        craneGroup->button(i)->setFixedSize(80,40);
    }*/
    QSqlQueryModel readData;
    readData.setQuery("SELECT *FROM CranesData");
   // qDebug()<<readData.rowCount();
    for (quint8 i =( readData.rowCount()); i <8 ; i++) {     //不需要的按键隐藏
        craneGroup->button(i)->setHidden(true);
    }
    //craneGroup->button(0)->setHidden(false);                //新增的按键总是显示
    /*布局左边按键，从上而下，最下边是增加按钮*/
    QVBoxLayout *buttonsLayout = new QVBoxLayout;
    for (quint8 i = 0 ; i < 8 ; i++) {
        buttonsLayout->addWidget(craneGroup->button(i));
    }
    buttonsLayout->addStretch(1);
    buttonsLayout->addWidget(craneGroup->button(8));

    /*编辑区布局*/
    QLabel *label[3][3];
    QString labelName[3][3] = { {globalData->tc->toUnicode ("塔吊名称"),globalData->tc->toUnicode ("塔群ID"),globalData->tc->toUnicode ("塔机ID")},
                                {globalData->tc->toUnicode ("塔机坐标X"),globalData->tc->toUnicode ("塔机坐标Y"),globalData->tc->toUnicode ("塔顶高度")},
                                {globalData->tc->toUnicode ( "前臂长度"),globalData->tc->toUnicode ("后臂长度"),globalData->tc->toUnicode("塔臂高度")}};
    //创建单位Label
    QLabel *unit[2][3];
    QString units = globalData->tc->toUnicode ("米");

    /******************编辑区第一列********************/
    QGridLayout *VLayout1 = new QGridLayout;
    //添加Label
    for(quint8 i = 0 ;i < 3 ; i++) {
        label[i][0] = new QLabel(labelName[i][0]);
        VLayout1->addWidget(label[i][0],i,0,1,1,Qt::AlignLeft);
    }
    //添加编辑框
    craneName = new QLineEdit(globalData->tc->toUnicode ("新建干涉塔吊"),this);
    craneName->setAlignment(Qt::AlignRight);
    VLayout1->addWidget(craneName,0,1,1,2,Qt::AlignLeft);
    craneX = new QLineEdit(globalData->tc->toUnicode ("0"),this);
    craneX->setAlignment(Qt::AlignRight);
    VLayout1->addWidget(craneX,1,1,1,1,Qt::AlignLeft);
    foreArmLen = new QLineEdit(globalData->tc->toUnicode ("0"),this);
    foreArmLen->setAlignment(Qt::AlignRight);
    VLayout1->addWidget(foreArmLen,2,1,1,1,Qt::AlignLeft);
    //添加单位
    for (quint8 i = 0 ; i < 2 ; i++) {
        unit[i][0] = new QLabel(units);
        VLayout1->addWidget(unit[i][0],i+1,2,1,1,Qt::AlignLeft);
    }
    /****************编辑区第二列*********************/
    QGridLayout *VLayout2 = new QGridLayout;
    //添加Label
    for(quint8 i = 0 ;i < 3 ; i++) {
        label[i][1] = new QLabel(labelName[i][1]);
        VLayout2->addWidget(label[i][1],i,0,1,1,Qt::AlignLeft);
    }
    //添加编辑框
    craneGroupID = new QLineEdit(tr ("0"),this);
    craneGroupID->setAlignment(Qt::AlignRight);
    VLayout2->addWidget(craneGroupID,0,1,1,2,Qt::AlignLeft);
    craneY = new QLineEdit(tr ("0"),this);
    craneY->setAlignment(Qt::AlignRight);
    VLayout2->addWidget(craneY,1,1,1,1,Qt::AlignLeft);
    backArmLen = new QLineEdit(tr ("0"),this);
    backArmLen->setAlignment(Qt::AlignRight);
    VLayout2->addWidget(backArmLen,2,1,1,1,Qt::AlignLeft);
    //添加单位
    for (quint8 i = 0 ; i < 2 ; i++) {
        unit[i][1] = new QLabel(units);
        VLayout2->addWidget(unit[i][1],i+1,2,1,1,Qt::AlignLeft);
    }
    /****************编辑区第三列********************/
    QGridLayout *VLayout3 = new QGridLayout;
    //添加Label
    for(quint8 i = 0 ;i < 3 ; i++) {
        label[i][2] = new QLabel(labelName[i][2]);
        VLayout3->addWidget(label[i][2],i,0,1,1,Qt::AlignLeft);
    }
    //添加编辑框
    craneID = new QLineEdit(tr ("0"),this);
    craneID->setAlignment(Qt::AlignRight);
    VLayout3->addWidget(craneID,0,1,1,2,Qt::AlignLeft);
    craneTopHeight = new QLineEdit(tr ("0"),this);
    craneTopHeight->setAlignment(Qt::AlignRight);
    VLayout3->addWidget(craneTopHeight,1,1,1,1,Qt::AlignLeft);
    craneHeight = new QLineEdit(tr ("0"),this);
    craneHeight->setAlignment(Qt::AlignRight);
    VLayout3->addWidget(craneHeight,2,1,1,1,Qt::AlignLeft);
    //添加单位
    for (quint8 i = 0 ; i < 2 ; i++) {
        unit[i][2] = new QLabel(units);
        VLayout3->addWidget(unit[i][2],i+1,2,1,1,Qt::AlignLeft);
    }

    /***************编辑区布局***************/
    QHBoxLayout *editHLayout = new QHBoxLayout;
    editHLayout->addLayout(VLayout1);
    editHLayout->addSpacing(40);
    editHLayout->addLayout(VLayout2);
    editHLayout->addSpacing(40);
    editHLayout->addLayout(VLayout3);

    /****************底部布局**************/
    QHBoxLayout *bottomLayout = new QHBoxLayout;
    QLabel *label1 = new QLabel(globalData->tc->toUnicode ("IMEI(MAC地址)"),this);
    IMEI = new QLineEdit();
    IMEI->setFixedWidth(200);
    IMEI->setAlignment(Qt::AlignRight);
    bottomLayout->addWidget(label1,1,Qt::AlignLeft);
    bottomLayout->addWidget(IMEI,2,Qt::AlignRight);
    bottomLayout->addWidget(delCraneButton,1,Qt::AlignCenter);
    bottomLayout->addWidget(saveButton,1,Qt::AlignCenter);
    bottomLayout->addWidget(quitButton,1,Qt::AlignCenter);
    /**********编辑区与底部布局合并**************/
    QGridLayout *rightLayout = new QGridLayout;
    rightLayout->addLayout(editHLayout,0,0,5,1);
   // rightLayout->addStretch(1);
    rightLayout->addLayout(bottomLayout,5,0,1,1);
    /*********主布局，左右结构****************/
    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addLayout(buttonsLayout);
    mainLayout->addSpacing(10);
    mainLayout->addStretch(1);
    mainLayout->addLayout(rightLayout);
    setLayout(mainLayout);
}

void InterfereCraneTab::craneSelect(int i)
{
    saveButton->setEnabled(true);
    QSqlTableModel *cranesData = new QSqlTableModel;
    cranesData->setTable("CranesData");
    cranesData->select();
    /*如果选中的是新增相干塔吊*/
    if (8 == i) {
        if (cranesData->rowCount() > 8) {
            QMessageBox::warning(this,globalData->tc->toUnicode ("新增相干塔吊错误"),globalData->tc->toUnicode ("相干塔吊个数超过8台"),QMessageBox::Yes);
            return;
        }
        currentCrane = 8;
        editRefresh();      //初始化显示区
        delCraneButton->setEnabled(false);  //删除按键置无效
    } else if (i < 8 ) {
/*如果选中的是对应的相干塔吊,从数据库中取出数据显示*/
        currentCrane = i;
        delCraneButton->setEnabled(true);       //删除按键有效
        if (cranesData->rowCount() >0 ) {
        craneName->setText(cranesData->record(currentCrane).value("craneName").toString());
        craneGroupID->setText(cranesData->record(currentCrane).value("groupID").toString());
        craneID->setText(cranesData->record(currentCrane).value("craneID").toString());
        craneX->setText(cranesData->record(currentCrane).value("coordinateX").toString());
        craneY->setText(cranesData->record(currentCrane).value("coordinateY").toString());
        craneTopHeight->setText(cranesData->record(currentCrane).value("craneTopHeight").toString());
        foreArmLen->setText(cranesData->record(currentCrane).value("forearmLen").toString());
        backArmLen->setText(cranesData->record(currentCrane).value("backarmLen").toString());
        craneHeight->setText(cranesData->record(currentCrane).value("craneHeight").toString());
        IMEI->setText(cranesData->record(currentCrane).value("craneMAC").toString());
        }
    }

}


void InterfereCraneTab::saveButtonClicked()
{
    if ( 8 == currentCrane ) {                                     //代表新增的相干塔吊,添加数据库中的信息
        QSqlTableModel *saveData = new QSqlTableModel;
        saveData->setTable("CranesData");
        saveData->select();
        delCraneButton->setEnabled(false);
        qint16 row = 0;
        saveData->insertRows(row,1);
        saveData->setData(saveData->index(row,1),craneName->text());
        saveData->setData(saveData->index(row,2),IMEI->text());
        saveData->setData(saveData->index(row,3),craneID->text());
        saveData->setData(saveData->index(row,4),craneGroupID->text());
        saveData->setData(saveData->index(row,5),QString::number(0));           //厂商
        saveData->setData(saveData->index(row,6),craneX->text());
        saveData->setData(saveData->index(row,7),craneY->text());
        saveData->setData(saveData->index(row,8),craneHeight->text());
        saveData->setData(saveData->index(row,9),craneTopHeight->text());
        saveData->setData(saveData->index(row,10),foreArmLen->text());
        saveData->setData(saveData->index(row,11),backArmLen->text());
        if (FALSE == saveData->submitAll()) {
            QMessageBox::warning(this,globalData->tc->toUnicode ("新增保护区"),globalData->tc->toUnicode ("新增保护区信息失败"),QMessageBox::Yes);
            return;
        }
        //  重新布局按键
        for (quint8 i = 0 ; i < saveData->rowCount() ; i++)
            craneGroup->button(i)->setHidden(false);
        } else {                                                                // 代表修改的是已存的相干塔吊,更新数据库中的数据信息
        QSqlTableModel *changeData = new QSqlTableModel;
        changeData->setTable("CranesData");
        //changeData->setFilter(QString("dataID = %1").arg(currentCrane));
        changeData->select();
        if (changeData->rowCount() == 1) {
            QSqlRecord record = changeData->record(currentCrane);
            record.setValue("craneName",craneName->text());
            record.setValue("groupID",craneGroupID->text());
            record.setValue("craneID",craneID->text());
            record.setValue("coordinateX",craneX->text());
            record.setValue("coordinateY",craneY->text());
            record.setValue("craneTopHeight",craneTopHeight->text());
            record.setValue("forearmLen",foreArmLen->text());
            record.setValue("backarmLen",backArmLen->text());
            record.setValue("craneHeight",craneHeight->text());
            record.setValue("craneMAC",IMEI->text());
            changeData->setRecord(0,record);
            if (FALSE == changeData->submitAll()) {
                QMessageBox::warning(this,globalData->tc->toUnicode ("修改保护区"),globalData->tc->toUnicode ("保护区信息修改失败"),QMessageBox::Yes);
                return;
            }
            //  重新布局按键
            for (quint8 i = 1 ; i < changeData->rowCount() ; i++)
                craneGroup->button(i)->setHidden(false);

        }
    }
}

/*删除当前相干塔吊*/
void InterfereCraneTab::delButtonClicked()
{

    QSqlTableModel *deleteData = new QSqlTableModel;
    deleteData->setTable("CranesData");
    deleteData->select();
    if (deleteData->rowCount() > 0) {
        deleteData->removeRows(currentCrane,1);
            qDebug()<< deleteData->rowCount();
        for (quint8 i =( deleteData->rowCount() ); i <8 ; i++)     //不需要的按键隐藏
            craneGroup->button(i)->setHidden(true);
    }else{
        QMessageBox::warning(this,globalData->tc->toUnicode ("删除保护区"),globalData->tc->toUnicode ("保护区信息删除失败"),QMessageBox::Yes);
        return;
}
    editRefresh();
    saveButton->setEnabled(false);
    //画图信息,重新计算缩放比例
    //globalData->calculateScale();
    delCraneButton->setEnabled(false);

}

/*编辑区清空显示，用于删除或新建时调用*/
void InterfereCraneTab::editRefresh()
{
    craneName->setText(globalData->tc->toUnicode ("新建干涉塔吊"));
    craneGroupID->setText(tr ("0"));
    craneID->setText(tr ("0"));
    craneX->setText(tr ("0"));
    craneY->setText(tr ("0"));
    craneTopHeight->setText(tr ("0"));
    foreArmLen->setText(tr ("0"));
    backArmLen->setText(tr ("0"));
    craneHeight->setText(tr ("0"));
    IMEI->setText(tr ("0"));
}
