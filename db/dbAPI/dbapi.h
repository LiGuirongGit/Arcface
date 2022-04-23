#ifndef DBAPI_H
#define DBAPI_H
#include <QObject>


struct persionInfo {
    char ID[64];
    char name[64];
    char feature[2048];
    unsigned int featureLen;
};

Q_DECLARE_METATYPE(persionInfo)

void dbInit();
void deDbInit();
int createFaceInfoTab();
int addFaceInfo(persionInfo *p);
int getFacesInfo(QVector<persionInfo> &ps);
int delFaceInfo(QString key, QString val);

#endif // DBAPI_H
