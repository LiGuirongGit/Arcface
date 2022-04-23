#include "faceinfo.h"

faceInfo::faceInfo(persionInfo &p) {

    memcpy(curItem.feature, p.feature, p.featureLen);
    strcpy(curItem.ID, p.ID);
    strcpy(curItem.name, p.name);
    curItem.featureLen = p.featureLen;

}

faceInfo &faceInfo::condit(QString key, QString val) {
    if(!_condit.isEmpty()) {
        _condit.append(" AND ");
    }
    _condit.append(QString("%1='%2'").arg(key).arg(val));
    return *this;
}


QString faceInfo::isTabExist() {
    QString sql = QString("SELECT COUNT(*) FROM SQLITE_MASTER WHERE TYPE='table' AND NAME='%1';")
            .arg(FACEINFO_TAB_NAME);
    return sql;
}

QString faceInfo::createTab() {
    QString sql = QString("CREATE TABLE %1("
                  "ID TEXT(64) UNIQUE,"
                  "name TEXT(64),"
                  "feature BLOB NOT NULL);").arg(FACEINFO_TAB_NAME);
    return sql;
}

QList<QVariant> faceInfo::insertBind() {
    QList<QVariant> list;
    QString sql = QString("INSERT INTO %1 (ID, name, feature) "
                          "VALUES(?, ?, ?)").arg(FACEINFO_TAB_NAME);
    list.append(QVariant(sql));
    list.append(QVariant(curItem.ID));
    list.append(QVariant(curItem.name));
    list.append(QVariant::fromValue(QByteArray(curItem.feature, curItem.featureLen)));
    return list;
}

QString faceInfo::del() {
    QString sql = QString("DELETE FROM %1")
            .arg(FACEINFO_TAB_NAME);
    if(!_condit.isEmpty()) {
        sql.append(" WHERE " + _condit);
    }
    return sql;
}

QString faceInfo::find() {
    QString sql = QString("SELECT "
                          "ID, name, feature "
                          "FROM %1")
            .arg(FACEINFO_TAB_NAME);
    if(!_condit.isEmpty()) {
        sql.append(" WHERE " + _condit);
    }
    return sql;
}

/*
函数作用：将从数据库中得到的数据解析，并重新打包成persionInfo结构体
说明：为什么要重新打包成一个结构体？
因为刚从数据库中得到的数据是零零散散的，不知道哪一条数据对应哪个人脸的，
更不知道具体是什么类型的，
所以，在这里先将这些数据解析成具体的数据类型，
再打包成一个结构体，方便之后能够区分是哪一个人脸的数据
*/
QVariant faceInfo::pkgInfo(QList<QVariant> &info) {
    persionInfo item;
    QByteArray arr = info[0].toByteArray();
    memset(item.ID, 0, sizeof(item.ID));
    strncpy(item.ID, arr.data(), sizeof(item.ID));

    arr = info[1].toByteArray();
    memset(item.name, 0, sizeof(item.name));
    strncpy(item.name, arr.data(), sizeof(item.name));

    arr = info[2].toByteArray();
    memset(item.feature, 0, sizeof(item.feature));
    memcpy(item.feature, arr.data(), arr.length());

    item.featureLen = arr.length();

    /*for(int i=0; i<item.featureLen; ++i)
        qDebug() << "db feature="<<(unsigned char )item.feature[i];*/

    return QVariant::fromValue(item);
}
