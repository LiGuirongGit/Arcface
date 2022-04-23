#ifndef WORKPTHREAD_H
#define WORKPTHREAD_H

#include <QThread>
#include <QDebug>
#include <QQueue>
#include <QObject>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include "opencv2/highgui.hpp"

#include "ASF/inc/merror.h"
#include "ASF/inc/asvloffscreen.h"
#include "ASF/inc/arcsoft_face_sdk.h"
#include "ASF/inc/amcomdef.h"

#include "db/dbAPI/dbapi.h"

#define NSCALE 16
#define FACENUM 5
#define MANUAL_REGISTER_IMAGE_PATH "../../image/manual/"
#define AUTOMATIC_REGISTERI_MAGE_PATH "../../image/automatic/"

#define SafeFree(p) { if ((p)) free(p); (p) = NULL; }
#define SafeArrayDelete(p) { if ((p)) delete [] (p); (p) = NULL; }
#define SafeDelete(p) { if ((p)) delete (p); (p) = NULL; }

typedef struct compare{
    IplImage* faceImg;
    ASF_SingleFaceInfo compareDetectedFaces;
    ASVLOFFSCREEN compareOffscreen;
    MInt32 faceID[5];
    MInt32 faceNum;
}compareInfo;
Q_DECLARE_METATYPE(compareInfo);

void CutIplImage(IplImage* src, IplImage* dst, int x, int y);
int exFeaturesOfFile(QString filePth, ASF_FaceFeature *OutFeature);

class workPthread : public QThread
{
    Q_OBJECT
public:
    explicit workPthread(QObject *parent = nullptr);
    void run();
    void addFace(persionInfo *p);
    void delFace(char *ID);
    void delFace(int pos);
    void addFrame(compareInfo frame);
    int getFaceNum();

signals:
    void faceCompareFinished(QString name, QString ID, float scores);

private slots:
    void slotDestroy(bool flag);

private:
    QQueue<compareInfo> compareInfoQeue;
    bool isDestroy;
    QVector<persionInfo> faces;
    compareInfo recvCompareInfo;
    ASF_FaceFeature copyfeature1;
    int lastID;
    QHash<int, int> pos_h;

    int ASFInit();
    int compareResult(compareInfo *info, ASF_FaceFeature *databaseFeature, float *scores);
    int exFeaturesOfImage(compareInfo* p, ASF_FaceFeature *feature);
};

#endif // WORKPTHREAD_H
