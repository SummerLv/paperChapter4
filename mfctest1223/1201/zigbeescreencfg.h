#ifndef ZIGBEESCREENCFG_H
#define ZIGBEESCREENCFG_H

#include  "sqldb.h"
#include  "zigbeecfg.h"
#include  <QSqlTableModel>
class ZigbeeCfg ;

class ZigbeeScreenCfg
{
public:
    ZigbeeScreenCfg();
    ~ZigbeeScreenCfg();

 void   getNetIDChannel(void);
 void  cfgNetIDChannel(void);

 public:
     //SqlDB *sqldb;
     QByteArray netId;
     QByteArray channel;

     ZigbeeCfg   *zigbeeCfg;
};

#endif // ZIGBEESCREENCFG_H

