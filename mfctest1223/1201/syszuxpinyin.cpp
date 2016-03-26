/***************************************************************************
**
**  <SYSZUXpinyin 1.0 , a chinese input method based on Qt for Embedded linux>
**  Copyright (C) <2010> <Gemfield> <gemfield@civilnet.cn>
**
**  This program is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License version 3 as published
**  by the Free Software Foundation.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
**  If you have questions regarding the use of this file, please contact
**  Gemfield at gemfield@civilnet.cn or post your questions at
**  http://civilnet.cn/syszux/bbs
**
****************************************************************************/

#include <QtGui>
#include "syszuxpinyin.h"
#include "syszuxim.h"
#include "basicDataDialog.h"
QString syszux_lower_letter[52]={"1","2","3","4","5","6","7","8","9","0","-","=","backspace","q","w","e","r","t","y","u","i",
                            "o","p","[","]","\\","a","s","d","f","g","h","j","k","l",";","\'","enter","z","x","c","v",
                            "b","n","m",",",".","/","shift"," "};
QString syszux_upper_letter[52]={"!","@","#","$","%","^","&&","*","(",")","_","+","backspace","Q","W","E","R","T","Y","U","I",
                            "O","P","{","}","|","A","S","D","F","G","H","J","K","L",":","\"","enter","Z","X","C","V",
                            "B","N","M","<",">","?","SHIFT"," "};
SyszuxPinyin::SyszuxPinyin(QWSInputMethod* im) :QDialog(0,Qt::Tool | Qt::FramelessWindowHint),button_group(new QButtonGroup(this)),input_method(0),lower_upper(0),page_count(0)
{
    int ret=0;// | Qt::WindowStaysOnBottomHint
    setupUi(this);
    resize(800,239);
    move (0,480-239);
   // windowMoveUp();                         //判断窗体是否需要移动
    initGb();
    pinyin_file.setFileName(":/syszux/syszuxpinyin");

    if( !pinyin_file.open(QIODevice::ReadOnly) )
        QMessageBox::warning(0,tr("syszuxpinyin"),tr("can't load"));
    else
        //qDebug() << "open pinyin_file";
    regExp.setCaseSensitivity(Qt::CaseSensitive);
    regExp.setPattern(QString("([a-z]+)"));   //匹配大小写字母

    while(!pinyin_file.atEnd())
    {
//        qDebug()<<"keyboard";
        QByteArray data = pinyin_file.readLine();
        ret = regExp.indexIn(QString(data.data()),0,QRegExp::CaretAtZero);
        pinyin_map.insert(regExp.cap(1),data.left(ret));
    }
    connect(this,SIGNAL(sendPinyin(QString)),im,SLOT(preeditString(QString)));  //界面发送与输入法给焦点窗口发送相连接
    connect(this,SIGNAL(sendString(QString)),im,SLOT(confirmString(QString)));  //不通过预显示，直接显示
    connect(this ,SIGNAL (quitInput () ) , im , SLOT (hideInput()) );                    //隐藏界面
}
SyszuxPinyin::~SyszuxPinyin()
{
}
/*
void  SyszuxPinyin::windowMoveUp()
{

     QWidget *currentWidget = new QWidget;
     QWidget *parentWidget = new QWidget;
     currentWidget = QApplication::focusWidget();
     parentWidget = currentWidget->parentWidget();
     QPoint globalPoint (currentWidget->mapToGlobal(QPoint(0,0)));               //  获取控件在窗体中的坐标
     QPoint parentGlobalPoint (parentWidget->mapToGlobal(QPoint(0,0)));
     if ( globalPoint.ry() > 239)  {                                            //输入法遮挡了当前要输入的窗口部件
         widgetMoveY = globalPoint.ry() - 239 + parentGlobalPoint.ry();
        // qDebug() << "enter move";//+ currentWidget->height()
         parentWidget->move(0,-widgetMoveY);
     } else {
         widgetMoveY = 0;                                                   //初始化0
     }

}
*/
/*
void SyszuxPinyin::windowMoveDown()
{
    QWidget *currentWidget= new QWidget;
    currentWidget = QApplication::activeWindow();
      if ( 0 != widgetMoveY) {
          qDebug() << "enter move" << widgetMoveY;
          currentWidget->move(0,widgetMoveY);
           widgetMoveY = 0;
      }
}

*/

void SyszuxPinyin::initGb()
{
    QPushButton *pushButton=new QPushButton(this);
    pushButton->hide();
    pushButton=pushButton_hanzi_left;
    for(int i=1;i<63;i++)
    {
        button_vector.push_back(pushButton);
        button_group->addButton(pushButton,i);
        pushButton=qobject_cast<QPushButton *>(pushButton->nextInFocusChain());
    }
    connect(button_group,SIGNAL(buttonClicked(int)),SLOT(buttonClickResponse(int)));
}
void SyszuxPinyin::buttonClickResponse(int gemfield)
{
    if(gemfield==1)
    {
        selectHanziPre();
        return;
    }
    else if(gemfield==10)
    {
        selectHanziNext();
        return;
    }
    else if(gemfield<10)
    {
        hanzi = (button_vector.at(gemfield-1)->text());
        pinyinString.clear();
        clearString();
        emit sendString(hanzi);
        return;
    }
    else if(gemfield==23)
    {
        deleteString();
        return;
    }
    else if(gemfield==59)
    {
         changeLowerUpper();
         return;
    }

    else if(gemfield>10 && gemfield<=60 )
    {
        if(input_method == 0) {                         //ENG输入
        if(lower_upper) {
          //  event=new QKeyEvent(QEvent::KeyPress, 0, Qt::NoModifier,syszux_upper_letter[gemfield-11]);
            emit sendString(syszux_upper_letter[gemfield-11]);
        } else {
           // event=new QKeyEvent(QEvent::KeyPress, 0, Qt::NoModifier,syszux_lower_letter[gemfield-11]);
            emit sendString(syszux_lower_letter[gemfield-11]);
        }
        } else {                                                //中文输入
           // event=new QKeyEvent(QEvent::KeyPress, 0, Qt::NoModifier,syszux_lower_letter[gemfield-11]);
            pinyinString += syszux_lower_letter[gemfield-11];
            //lineEdit_pinyin->setFocus();
            //QApplication::sendEvent(focusWidget(),event);
          //  matching(lineEdit_pinyin->text());
            matching(pinyinString);
            emit sendPinyin(pinyinString);      //发送到欲显示窗口
        }
    }
    else if(gemfield==61)
    {
        changeInputMethod();
        return;
    }
    else if(gemfield==62)             //确认建
    {
        emit quitInput ();               //发送隐藏输入法信号
        return;
    }    
    else if(gemfield>62)
    {
        switch(gemfield)
        {
        case 63:
            QWSServer::sendKeyEvent(0x01000012 , Qt::Key_Left , Qt::NoModifier , true ,false);
            //event=new QKeyEvent(QEvent::KeyPress, Qt::Key_Left, Qt::NoModifier);
            break;
        case 64:
            QWSServer::sendKeyEvent(0x01000015 , Qt::Key_Down , Qt::NoModifier , true ,false);
            //event=new QKeyEvent(QEvent::KeyPress, Qt::Key_Down, Qt::NoModifier);
            break;
        case 65:
            QWSServer::sendKeyEvent(0x01000014 , Qt::Key_Right , Qt::NoModifier , true ,false);
            //event=new QKeyEvent(QEvent::KeyPress, Qt::Key_Right, Qt::NoModifier);
            break;
        case 66:
           // event=new QKeyEvent(QEvent::KeyPress, Qt::Key_Up, Qt::NoModifier);
            QWSServer::sendKeyEvent(0x01000013 , Qt::Key_Up , Qt::NoModifier , true ,false);
            break;
        }
    }

}
void SyszuxPinyin::matching(QString gemfield)
{
    pinyin_list = pinyin_map.values(gemfield);
    changePage(0);
    page_count=0;
}

void SyszuxPinyin::changePage(int index)
{
    int count = pinyin_list.size();
    int i=index*8,j=0;
    while(++j != 9 )
        button_vector.at(j)->setText(tr(pinyin_list.value(count-(++i)).toAscii()));
    if(index==0)
        pushButton_hanzi_left->setEnabled(false);
    else
        pushButton_hanzi_left->setEnabled(true);
    if(pinyin_list.size()>(index*8+8))
        pushButton_hanzi_right->setEnabled(true);
    else
        pushButton_hanzi_right->setEnabled(false);
}

void SyszuxPinyin::selectHanziPre()
{
    changePage(--page_count);
}

void SyszuxPinyin::selectHanziNext()
{
    changePage(++page_count);
}
void SyszuxPinyin::clearString()
{
    int i=0;
    while(++i!=9)
        button_vector.at(i)->setText("");
}
void SyszuxPinyin::changeInputMethod()
{
    if(pushButton_shift->text()=="SHIFT")
        return;
    pinyinString.clear();
   if(pushButton_is_hanzi->text()=="En")
       input_method=1,pushButton_is_hanzi->setText("CH");
   else
       input_method=0,pushButton_is_hanzi->setText("En");
}
void SyszuxPinyin::changeLowerUpper()
{
    if(pushButton_shift->text()=="shift")
    {
        lower_upper=1,pushButton_shift->setText("SHIFT");
        input_method=0,pushButton_is_hanzi->setText("En");
        int i=9;
        while(++i!=59)
            button_vector.at(i)->setText(syszux_upper_letter[i-10]);
    }
    else
    {
        lower_upper=0,pushButton_shift->setText("shift");
        int i=9;
        while(++i!=59)
            button_vector.at(i)->setText(syszux_lower_letter[i-10]);
    }
}
void SyszuxPinyin::deleteString()
{
    event=new QKeyEvent(QEvent::KeyPress, Qt::Key_Backspace, Qt::NoModifier);
    if(input_method)
    {
        if ( ! pinyinString.isEmpty()) {
            pinyinString.resize(pinyinString.length() - 1);
            matching(pinyinString);
            emit sendPinyin(pinyinString);      //发送到欲显示窗口
        }else{
            QWSServer::sendKeyEvent(0x01000003 , Qt::Key_Backspace , Qt::NoModifier , true ,false);
        }
    }
    else
    {

        QWSServer::sendKeyEvent(0x01000003 , Qt::Key_Backspace , Qt::NoModifier , true ,false);

    }
}



