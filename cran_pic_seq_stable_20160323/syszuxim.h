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

#ifndef SYSZUXIM_H
#define SYSZUXIM_H
#include <QWSInputMethod>
#include <QLineEdit>

class SyszuxPinyin;
class SyszuxIM:public QWSInputMethod
{
Q_OBJECT
public:
        SyszuxIM();
        ~SyszuxIM();
        void updateHandler(int);

public slots:
        void confirmString(QString gemfield);
        void preeditString(QString newString);
        void hideInput();
private:
        SyszuxPinyin* syszuxpinyin;
        QString inputString;                //输入的预处理字符
        qint32 widgetMoveY;


};
#endif
