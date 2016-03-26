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

#include "syszuxim.h"
#include "syszuxpinyin.h"
#include "QDebug"
SyszuxIM::SyszuxIM()
{
    //qDebug()<<"keyboard";
        syszuxpinyin = new SyszuxPinyin(this);
        syszuxpinyin->resize(800,239);
        //qDebug()<<"keyboard";
}
SyszuxIM::~SyszuxIM()
{
    syszuxpinyin->deleteLater();
}


void SyszuxIM::preeditString(QString newString)   //发送预处理字符
{
    /*
    if (newString == "Backspace") {
     if (inputString.length() == 0 ) {                                  //如果没有预处理字符,删除已有数据
    QWSServer::sendKeyEvent(0x01000003 , Qt::Key_Backspace , Qt::NoModifier , true ,false);
    } else {                                                                //删除预处理中的字符
         inputString.resize(inputString.length() - 1);
         this->sendPreeditString(inputString , 0);
        }
    } else {                                                                //显示预处理字符
        inputString += newString;
        sendPreeditString(inputString ,0);
    }*/

    sendPreeditString(newString ,0);
}

void SyszuxIM::confirmString(QString gemfield)    //发送已确认的字符
{
     sendCommitString(gemfield);
}

void SyszuxIM::hideInput()                              //隐藏输入法槽函数
{
    QWidget *currentWidget = new QWidget;
    QWidget *parentWidget = new QWidget;
    currentWidget = QApplication::focusWidget();
    parentWidget = currentWidget->parentWidget();
     if ( 0 != widgetMoveY) {
          parentWidget->move(0,parentWidget->pos().ry() + widgetMoveY);
          widgetMoveY = 0;
      }
      currentWidget->clearFocus();

}




void SyszuxIM::updateHandler(int type)
{
        switch(type)
        {
        case QWSInputMethod::FocusIn:
        {         
            QWidget *currentWidget = new QWidget;
            QWidget *parentWidget = new QWidget;
            currentWidget = QApplication::focusWidget();
            parentWidget = currentWidget->parentWidget();
            QPoint globalPoint (currentWidget->mapToGlobal(QPoint(0,0)));             //  获取控件在窗体中的坐标
            QPoint parentGlobalPoint (parentWidget->mapToGlobal(QPoint(0,0)));
            if ( globalPoint.ry() > 239)                     //输入法遮挡了当前要输入的窗口部件
            {
                widgetMoveY = globalPoint.ry() - 239 + parentGlobalPoint.ry();

                parentWidget->move(0,-widgetMoveY);
               // qDebug() << "move"<<(-widgetMoveY);
            }

            syszuxpinyin->show();
        }
            break;

        case QWSInputMethod::FocusOut:
        {

             syszuxpinyin->hide();
        }
            break;
        default:
            break;
        }

}


