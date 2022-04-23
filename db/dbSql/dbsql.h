#ifndef DBSQL_H
#define DBSQL_H

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QMutexLocker>
#include "../dbBase/dbbase.h"

#define DB_FILE_NAME "../../db/faceInfo.db"

class dbSql
{
public:
    dbSql();
    virtual ~dbSql();
    static dbSql* getDbHead();
    bool isTabExist(dbBase &it);
    bool createTab(dbBase &it);
    int del(dbBase &it);
    int insertBind(const QList<dbBase *> &list);
    QList<QVariant> find(dbBase &it);
private:
    QSqlDatabase db;
    QMutex dbLock;
    class dbTransaction{
    public:
        explicit dbTransaction(const QSqlDatabase &db, bool trans, const bool state):
            transDb(const_cast<QSqlDatabase*>(&db)),
            isTrans(trans),
            transState(const_cast<bool*>(&state)){
            if(!transDb->isOpen()) {
                transDb->open();
            }
            if(isTrans) {
                transDb->transaction();
            }
        }
        ~dbTransaction(){
            if(isTrans) {
                if(*transState) {
                    transDb->commit();
                } else {
                    transDb->rollback();
                }
            }
            if(transDb->isOpen()) {
                transDb->close();
            }
        }
    private:
        QSqlDatabase *transDb;
        bool isTrans;
        bool *transState;
    };
};

#endif // DBSQL_H
