#include "dynamicgetusbcom.h"

void UsbComTool::GetUsbCom()
{
   QString command("ls /dev | grep \"ttyUSB\" > ls.txt");
   char *S;
   S = command.toAscii().data();
   system(S);
   QFile infile("ls.txt");
   if(!infile.open(QIODevice::ReadOnly | QIODevice::Text))
       return;
   QVector<QString> temp_vec;
   QString temp_string;

   QTextStream in(&infile);
   while(!in.atEnd()){
       //qDebug()<<"run here";
       temp_string = in.readLine();
       temp_vec.push_back(temp_string);
   }
   infile.close();

   temp_string = "/dev/";
   QFile outfile("ls.txt");
   if(!outfile.open(QIODevice::WriteOnly | QIODevice::Text))
       return;
   for(QVector<QString>::iterator iter = temp_vec.begin();iter != temp_vec.end();++iter){
       *iter = temp_string+*iter+"\n";
       std::string temp_std_str = iter->toStdString();
       outfile.write(temp_std_str.c_str());
   }
   outfile.close();
}

const char **UsbComTool::GetComNameFromFile(QVector<QString>::size_type &size){
   QFile infile("ls.txt");
   QVector<QString> temp_vec;
   QString temp_string;
   if(!infile.open(QIODevice::ReadOnly | QIODevice::Text))
       return NULL;
   // std::string s;
  // s.r

   QTextStream in(&infile);
   QVector<QString>::size_type i = 0;
   while(!in.atEnd()){
        temp_string = in.readLine();
       temp_vec.push_back(temp_string);
   }
   infile.close();

   char **temp_ptr = new  char*[temp_vec.size()];
   for(int i = 0;i<temp_vec.size();++i){
       temp_ptr[i] = new char[temp_vec.at(i).size()+2];
   //qDebug()<<"tempvec size:"<<temp_vec.at(i).size();
   }
   size = temp_vec.size();
   for(QVector<QString>::iterator iter = temp_vec.begin();iter != temp_vec.end();++i,++iter){
     //  qDebug()<<*iter;
       std::string temp_std_str =  iter->toStdString() ;
     // qDebug()<<"tempvec size:"<<temp_std_str.size();
      const char *tem_str = temp_std_str.c_str();
       //memcpy(temp_ptr[i] , tem_str, temp_std_str.size());
      strcpy(temp_ptr[i] ,tem_str);
    //   temp_ptr[i] = tem_str;
   }
   const char **cons_temp_ptr = (const char **)temp_ptr;
   return cons_temp_ptr;
}

int UsbComTool::dynamicOpenPort(const char ** &dev,int comPort , int size){
         int fd;

         if ((comPort < 0) )
                 return -1;
         if ( comPort >= size)
              return -1;

    /*打开串口*/
        fd = open(dev[comPort] , O_RDWR | O_NOCTTY | O_NDELAY); //读写方式打开，该终端不会成为调用open的那个进程的控制终端
       // qDebug()<<"dev2 [comPort]"<<dev2 [comPort];
    if (fd < 0) {
        qDebug() << "open serial port error";
        return -1;
    }

    /*恢复串口为阻塞状态*/
    if (fcntl( fd , F_SETFL , 0) < 0)
        qDebug()<<"fcntl F_SETFL";

    /*测试是否为终端设备*/
    if (isatty(STDIN_FILENO) == 0)
        qDebug()<<"standard input is not a terminal device";

    return fd;
}
