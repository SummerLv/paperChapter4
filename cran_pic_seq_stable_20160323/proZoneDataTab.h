 #ifndef PROZONEDATATAB_H
#define PROZONEDATATAB_H

#include <QDialog>
#include "data.h"
#include <QtGui>
#include <QValidator>
#include <QDoubleValidator>
#include <QSqlTableModel>
#include "sqldb.h"

class ProZoneDataTab : public QDialog
{
    Q_OBJECT

public:
    ProZoneDataTab(QWidget *parent);
    ~ProZoneDataTab();
private:
    void interfaceInit();    //保护区界面初始化
    void editInterfaceRefresh();      //编辑区恢复出世界面
private:
    QPushButton *addProZone;
    QPushButton *saveProZone;
    QPushButton *delProZone;
    QPushButton *quitProZone;
    QPushButton *saveElement;
    QPushButton *delElement;
    QButtonGroup *ProButtons;


    QLineEdit *name;
    QLineEdit *ID;
    QLineEdit *height;
    QComboBox *elementNum;

    QLineEdit *elementX;
    QLineEdit *elementY;
    QLineEdit *elementBaginAngle;
    QLineEdit *elementEndAngle;
    QLineEdit *elementR;

    QComboBox *buildType;
    QComboBox *proZoneType;
    QComboBox *elementN;
    QComboBox *elementType;

    GlobalData *globalData;
    quint8 proZoneNum;                                  //保护区个数
    quint8 currentProZoneSelect;                        //当前被选中的正在编辑的保护区


    QList<ElementData>  elementDataList;            //增加的元素信息
   // SqlDB *db;                      //数据库类
    quint32 ProZoneDataKeyID;                           //保护区数据keyID，位了与元素信息相对应


 public slots:
    void proZoneSelect(int i);                           //保护区选中槽函数
    void proZoneTypeSelect(int i);                      //保护区类型选择，使高度变成可编辑
    void elementNumSelect(int i);                      //元素个数选择，改变元素显示区元素编号个数
    void elementTypeSelect(int i);                      //元素类型选择，改变圆心，半径变成可编辑
    void elementNSelect(int i);                           //元素选择，改变每个元素显示信息
    void saveElementClick();                                //暂存所选元素信息
    void delElementClick();                                 //删除元素
    void saveProZoneClick();                                //保存编辑好的保护区信息
    void delProZoneClick();                                   //删除保护区
signals:

};

#endif // PROZONEDATATAB_H
