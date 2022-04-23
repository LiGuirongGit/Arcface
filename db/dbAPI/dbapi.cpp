#include "dbapi.h"
#include "../dbSql/dbsql.h"
#include "../dbTables/faceinfo.h"

dbSql *dbHead = nullptr;

void dbInit() {
    dbHead = dbSql::getDbHead();
}

void deDbInit() {
    if(dbHead) {
        delete dbHead;
        dbHead = nullptr;
    }
}

/*
 * 函数作用：判断是否存在一张表，如果不存在就创建这张表
 * 返回值：0，创建成功；-1，创建失败(该表已存在)
*/
int createFaceInfoTab() {
    faceInfo item;
    return dbHead->createTab(item) == 1 ? 0 : -1;
}

/*
 * 函数作用：添加一个人脸信息
 * 返回值：0，添加失败；其它，返回添加成功的数量
*/
int addFaceInfo(persionInfo *p) {
    QList<dbBase *> list;
    persionInfo info;

    memset(&info, 0, sizeof(persionInfo));

    memcpy(info.feature, p->feature, p->featureLen);
    strcpy(info.ID, p->ID);
    strcpy(info.name, p->name);
    info.featureLen = p->featureLen;


    faceInfo *it = new faceInfo(info);
    list.append(it);

    int cnt = dbHead->insertBind(list);

    for(auto elem : list) {
        delete elem;
        elem = nullptr;
    }

    return cnt;
}

/*
 * 函数作用：获取数据库中所有人脸信息
 * 返回值：0，获取成功
*/
int getFacesInfo(QVector<persionInfo> &ps) {
    faceInfo it;
    QList<QVariant> list = dbHead->find(it);
    if(list.size() == 0) {
        qDebug("[%s:%d] find num == 0!", __func__, __LINE__);
        return 0;
    }
    for(QList<QVariant>::const_iterator elem = list.constBegin();
        elem != list.constEnd(); ++elem) {
        if(elem->canConvert<persionInfo>()) {
            persionInfo t = elem->value<persionInfo>();
            ps.append(t);
        }
    }
    return 0;
}

/*
 * 函数作用：删除人脸信息
 * 参数：key，列名；val，该列的值
 * 返回值：返回删除成功的数量
*/
int delFaceInfo(QString key, QString val) {
    faceInfo it;
    it.condit(key, val);
    return dbHead->del(it);
}
