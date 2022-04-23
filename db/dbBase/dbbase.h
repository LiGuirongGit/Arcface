#ifndef DBBASE_H
#define DBBASE_H

#include <QObject>
#include <QVariant>
#include <QSqlRecord>
#include <QDebug>
#include "../dbAPI/dbapi.h"


class dbBase
{
public:
    dbBase();
    virtual ~dbBase();
    virtual QString del();
    virtual QString isTabExist();
    virtual QString createTab();
    virtual QList<QVariant> insertBind(){return QList<QVariant>();}
    virtual QString find();
    virtual QVariant pkgInfo(QList<QVariant> &info);

};

#endif // DBBASE_H
