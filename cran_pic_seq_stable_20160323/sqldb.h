#ifndef SQLDB_H
#define SQLDB_H
/*数据库类*/
#include <QObject>
#include <QSqlDatabase>
#include <QSqlRecord>
#include <QSqlQuery>
//#include <QtSql/QSQLiteDriver>
#include <QSqlError>
#include <QtGui>
class SqlDB : public QObject
{
    Q_OBJECT
public:
    explicit SqlDB(QObject *parent = 0);
    void connectDB();                                    //创建链接函数
    void closeDB();                                          //关闭链接函数
    
signals:
    
public slots:
private:
    QSqlDatabase db;        //建立数据库的文件描述符
    
};

#endif // SQLDB_H
