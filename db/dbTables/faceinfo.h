#ifndef FACEINFO_H
#define FACEINFO_H
#include "../dbBase/dbbase.h"

#define FACEINFO_TAB_NAME "faceInfo"

class faceInfo : public dbBase
{
public:
    explicit faceInfo(persionInfo &);
    faceInfo() {}
    virtual ~faceInfo(){}

    faceInfo &condit(QString key, QString val);

    QList<QVariant> insertBind() override;
    QString del() override;
    QString isTabExist() override;
    QString createTab() override;
    QString find() override;
    QVariant pkgInfo(QList<QVariant> &info) override;
private:
    persionInfo curItem;
    QString _condit;
};

#endif // FACEINFO_H
