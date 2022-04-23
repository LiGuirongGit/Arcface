#include "registerthread.h"

registerThread::registerThread(QObject *parent) : QThread(parent)
{
    qRegisterMetaType<persionInfo>("persionInfo");
    sumNum = 0;
    curNum = 0;
    isStop = false;
    ASFInit();
}

int registerThread::ASFInit()
{
    MRESULT res = MOK;
    handle = NULL;
    MInt32 mask = ASF_FACE_DETECT | ASF_FACERECOGNITION;
    res = ASFInitEngine(ASF_DETECT_MODE_VIDEO, ASF_OP_0_ONLY, NSCALE * 2, 1, mask, &handle);
    if (res != MOK)
        qDebug("[%s:%d] ASFInitEngine fail: %d\n", __func__, __LINE__, res);
    else
        qDebug("[%s:%d] ASFInitEngine sucess: %d\n", __func__, __LINE__, res);
    return res;
}

void registerThread::ASFDeInit() {
    MRESULT res  = ASFUninitEngine(handle);
    if (res != MOK)
        qDebug("[%s:%d] ASFUninitEngine fail: %d\n", __func__, __LINE__, res);
    else
        qDebug("[%s:%d] ASFUninitEngine sucess: %d\n", __func__, __LINE__, res);
}

int registerThread::registerExFeaturesOfFile(QString filePth, ASF_FaceFeature *OutFeature)
{
    if(filePth.isEmpty() || filePth.isNull()) {
        qDebug("filePth is NULL");
        return -1;
    }

    MRESULT res = MOK;
    IplImage* pImg = cvLoadImage(filePth.toUtf8().data());
    IplImage* img = NULL;

    ASF_FaceFeature feature = { 0 };

    if(pImg) {
        img = cvCreateImage(cvSize(pImg->width - pImg->width % 4,
                                             pImg->height), IPL_DEPTH_8U, pImg->nChannels);
    } else {
        qDebug("pImg is null");
        return -1;
    }

    if(img) {
        CutIplImage(pImg, img, 0, 0);
    } else {
        qDebug("img is null");
        cvReleaseImage(&pImg);
        return -1;
    }

    if(img) {
        ASVLOFFSCREEN offscreen = { 0 };
        offscreen.u32PixelArrayFormat = ASVL_PAF_RGB24_B8G8R8;
        offscreen.i32Width = img->width;
        offscreen.i32Height = img->height;
        offscreen.pi32Pitch[0] = img->widthStep;
        offscreen.ppu8Plane[0] = (MUInt8*)img->imageData;
        ASF_MultiFaceInfo detectedFaces;
        res = ASFDetectFacesEx(handle, &offscreen, &detectedFaces);
        if (MOK != res) {
            qDebug("[%s]ASFDetectFacesEx failed: %d\n", __func__, res);
        } else {
            //ColorSpaceConversion(img->width, img->height, ASVL_PAF_NV21, (MUInt8*)img->imageData, offscreen);
            ASF_SingleFaceInfo SingleDetectedFaces = { 0 };

            SingleDetectedFaces.faceRect.left = detectedFaces.faceRect[0].left;
            SingleDetectedFaces.faceRect.top = detectedFaces.faceRect[0].top;
            SingleDetectedFaces.faceRect.right = detectedFaces.faceRect[0].right;
            SingleDetectedFaces.faceRect.bottom = detectedFaces.faceRect[0].bottom;
            SingleDetectedFaces.faceOrient = detectedFaces.faceOrient[0];
            res = ASFFaceFeatureExtractEx(handle, &offscreen, &SingleDetectedFaces, &feature);
            if (res != MOK) {
                qDebug("%s ASFFaceFeatureExtractEx fail: %d\n", filePth.toUtf8().data(), res);
            } else{
                OutFeature->featureSize = feature.featureSize;
                OutFeature->feature = (MByte *)malloc(feature.featureSize);
                memset(OutFeature->feature, 0, feature.featureSize);
                memcpy(OutFeature->feature, feature.feature, feature.featureSize);

                qDebug("%s ASFFaceFeatureExtractEx:%#x success: %d\n", filePth.toUtf8().data(), feature.feature,res);
            }
        }
        cvReleaseImage(&img);
    } else {
        qDebug("cutImg is null %s", filePth.toUtf8().data());
    }

    cvReleaseImage(&pImg);

    return res;

}

void registerThread::run()
{
    QDir *dir=new QDir(AUTOMATIC_REGISTERI_MAGE_PATH);
    ASF_FaceFeature feature = { 0 };
    QString name, ID;
    QStringList tmpName;
    QStringList filter;
    QDateTime time;
    qint64 epochTime;
    persionInfo registerInfo;
    QVariant var;
    filter << "*.jpg" << "*.png";
    QList<QFileInfo> *fileInfo=new QList<QFileInfo>(dir->entryInfoList(filter, QDir::Files | QDir::NoSymLinks));
    sumNum = fileInfo->count();

    for(int i = 0; !isStop && i<fileInfo->count(); i++) {

        memset(registerInfo.feature, 0, sizeof(registerInfo.feature));
        memset(registerInfo.ID, 0, sizeof(registerInfo.ID));
        memset(registerInfo.name, 0, sizeof(registerInfo.name));
        registerInfo.featureLen  = 0;

        tmpName = fileInfo->at(i).fileName().split(".");
        name = tmpName.at(0);
        time = QDateTime::currentDateTime();
        epochTime = time.toMSecsSinceEpoch();
        ID = QString::number(epochTime);

        if(registerExFeaturesOfFile(fileInfo->at(i).filePath(), &feature) == 0) {
            strncpy(registerInfo.name, name.toUtf8().data(), sizeof(registerInfo.name));
            strncpy(registerInfo.ID, ID.toUtf8().data(), sizeof(registerInfo.ID));
            memcpy(registerInfo.feature, feature.feature, feature.featureSize);
            registerInfo.featureLen = feature.featureSize;

            addFaceInfo(&registerInfo);
            var.setValue(registerInfo);
            curNum = i+1;
            emit signalInsertInfoFinish(var);
            if(feature.feature) {
                free(feature.feature);
                feature.feature = NULL;
            }
        }
        usleep(700*1000);
        QFile::remove(fileInfo->at(i).filePath());
        var.clear();
    }

    isStop = false;
    curNum = 0;
    sumNum = 0;
    delete dir;
    dir = nullptr;
    delete fileInfo;
    fileInfo = nullptr;
}

int registerThread::getSumNum() {
    return sumNum;
}

int registerThread::getCurNum() {
    return curNum;
}

void registerThread::stop() {
    isStop = true;
}
