#include "dbsql.h"

static dbSql* head = nullptr;

dbSql::dbSql()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(DB_FILE_NAME);
}

dbSql::~dbSql()
{
    if(head) {
        delete head;
        head = nullptr;
    }
}

dbSql* dbSql::getDbHead() {
    if(!head) {
        head = new dbSql;
    }
    return head;
}

bool dbSql::isTabExist(dbBase &it) {
    QMutexLocker locker(&dbLock);
    bool state = true;
    bool isExist = false;
    dbTransaction dbTrans(db, false, state);

    QSqlQuery query(db);
    query.setForwardOnly(true);
    query.exec(it.isTabExist());
    while(query.next()) {
        if(query.value(0).toInt() > 0) {
            isExist = true;
            break;
        }
    }

    query.finish();
    query.clear();
    return isExist;
}

bool dbSql::createTab(dbBase &it) {
    if(isTabExist(it)) {
        qDebug("[%s:%d] createTab failed! table is exist", __func__, __LINE__);
        return false;
    }
    QMutexLocker locker(&dbLock);
    bool state = true;
    dbTransaction dbTrans(db, false, state);

    QSqlQuery query(db);
    query.exec(it.createTab());
    if(query.lastError().isValid()) {
        qDebug() << "dbSql::createTab err:" <<query.lastError();
        state = false;
    }
    query.finish();
    return state;

}

int dbSql::del(dbBase &it) {
    QMutexLocker locker(&dbLock);
    bool state = true;
    dbTransaction dbTrans(db, false, state);
    QSqlQuery query(db);
    query.exec(it.del());
    if(query.lastError().isValid()) {
        qDebug() << "dbSql::del err:" << query.lastError();
    }
    int delNum = query.numRowsAffected();
    query.finish();
    return delNum;
}

int dbSql::insertBind(const QList<dbBase *> &list) {
    QMutexLocker locker(&dbLock);
    bool state = true;
    dbTransaction dbTrans(db, true, state);
    int cnt = 0;
    QSqlQuery query(db);
    QList<QVariant> variList;
    QString sql;

    for(auto it : list){
        variList = it->insertBind();
        if(variList.first().canConvert<QString>()) {
            sql = variList.first().value<QString>();
            variList.pop_front();
        }

        query.prepare(sql);
        /*for(QList<QVariant>::const_iterator elem = variList.begin();
            elem != variList.end(); ++elem) {
            query.addBindValue(*elem);
        }*/
        for (auto elem : variList) {
           query.addBindValue(elem);
         }
        query.exec();
        if(query.lastError().isValid()) {
            qDebug() << "dbSql::insertBind err:" << query.lastError();
            cnt = -1;
            break;
        }
        ++cnt;
    }
    return cnt;
}

QList<QVariant> dbSql::find(dbBase &it) {
    QMutexLocker locker(&dbLock);
    bool state = true;
    dbTransaction dbTrans(db, false, state);
    QSqlQuery query(db);

    QList<QVariant> infos;

    query.setForwardOnly(true);
    query.exec(it.find());
    while(query.next()) {
        QList<QVariant> info;
        QSqlRecord reco = query.record();

        for(int i=0; i<reco.count(); ++i) {
            info.append(query.value(i));
        }
        infos.push_back(it.pkgInfo(info));
       // info.clear();
    }
    query.finish();
    return infos;
}
