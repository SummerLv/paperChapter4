#include "temtab.h"

temtab::temtab(QWidget *parent)
{
    InterfaceInit();
}
temtab::~temtab(){

}

void temtab::InterfaceInit(){
    globaldata = GlobalData::getGlobalData();
    //总布局
  //  QHBoxLayout *mainlayout = new QHBoxLayout;
    QVBoxLayout *rightlayout = new QVBoxLayout;
  //  mainlayout->addLayout(leftlayout);
   // mainlayout->addLayout(rightlayout);
    //上布局
    vol_ad = new QTableWidget;
    QHBoxLayout *toplayout = new QHBoxLayout;
    QHBoxLayout *bottomlayout = new QHBoxLayout;
    QGridLayout *tem_gridlayout = new QGridLayout;
    QGridLayout *vol_gridlayout = new QGridLayout;
    read_ad = new QPushButton(globaldata->tc->toUnicode("读取AD"));
    temprature = new QLineEdit;
    voltage = new QLineEdit;

    QLabel *vollabel = new QLabel(globaldata->tc->toUnicode("电压值"));
    QLabel *vol_v = new QLabel(globaldata->tc->toUnicode("V"));
    vol_gridlayout->addWidget(vollabel,0,0,1,1);
    vol_gridlayout->addWidget(voltage,0,1,2,1);
    vol_gridlayout->addWidget(vol_v,0,3,1,1);

    QLabel *temlabel = new QLabel(globaldata->tc->toUnicode("温度"));
    QLabel *tem_unit = new QLabel(globaldata->tc->toUnicode("度"));
    tem_gridlayout->addWidget(temlabel,0,0,1,1);
    tem_gridlayout->addWidget(temprature,0,1,2,1);
    tem_gridlayout->addWidget(tem_unit,0,3,1,1);

    toplayout->addLayout(vol_gridlayout);
    toplayout->setSpacing(10);
    toplayout->addWidget(read_ad);
    toplayout->setSpacing(10);
    toplayout->addLayout(tem_gridlayout);

        //下部分布局
    vol_ad->resize(500,360);
    vol_ad->setRowCount(10);
    vol_ad->setColumnCount(2);
    vol_ad->setItem(0,0,new QTableWidgetItem(globaldata->tc->toUnicode("电压值")));
    vol_ad->setItem(0,1,new QTableWidgetItem(globaldata->tc->toUnicode("AD值")));


    fitting = new QPushButton(globaldata->tc->toUnicode("拟合"));
    save = new QPushButton(globaldata->tc->toUnicode ("保存"));
    delete_line = new QPushButton( globaldata->tc->toUnicode ("删除") );

    rightlayout->addWidget(fitting);
    rightlayout->addWidget(save);
    rightlayout->addWidget(delete_line);

    bottomlayout->addWidget(vol_ad);
    bottomlayout->addLayout(rightlayout);

    QVBoxLayout *mainlayout = new QVBoxLayout;
    mainlayout->addLayout(toplayout);
    mainlayout->addLayout(bottomlayout);
    setLayout(mainlayout);

}
