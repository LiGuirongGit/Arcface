#include "workpthread.h"

MHandle handle = NULL;

#if 1

void CutIplImage(IplImage* src, IplImage* dst, int x, int y)
{
    CvSize size = cvSize(dst->width, dst->height);//区域大小
    cvSetImageROI(src, cvRect(x, y, size.width, size.height));//设置源图像ROI
    cvCopy(src, dst); //复制图像
    cvResetImageROI(src);//源图像用完后，清空ROI
}

int exFeaturesOfFile(QString filePth, ASF_FaceFeature *OutFeature)
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
#endif

workPthread::workPthread(QObject *parent): QThread(parent)
{
    ASFInit();
    isDestroy = false;
}

void workPthread::run()
{
    int ret = -1;
    int pos = 0;
    int lastSize = 0;
    bool isFind = false;

    MFloat confidenceLevel;
    MRESULT res;
    ASF_FaceFeature featureImage = { 0 };

    getFacesInfo(faces);
    lastID = -1;
    lastSize = faces.size();


    while(1){

        if(isDestroy) {
            break;
        }
        if(!compareInfoQeue.isEmpty()) {
            recvCompareInfo = compareInfoQeue.back();

            ret = exFeaturesOfImage(&recvCompareInfo, &featureImage);

            //if(ret >= 0 /*|| lastSize != faces.size()*/) {
                lastSize = faces.size();
                copyfeature1.feature = (MByte *)malloc(2048);

                /*for(QHash<int, int>::const_iterator it = pos_h.constBegin(); it != pos_h.constEnd() && it.key() < faces.size(); ++it) {
                    copyfeature1.featureSize = faces[it.key()].featureLen;
                    memset(copyfeature1.feature, 0, 2048);
                    memcpy(copyfeature1.feature, faces[it.key()].feature, 2048-1);
                    res = ASFFaceFeatureCompare(handle, &featureImage, &copyfeature1, &confidenceLevel);
                    if(confidenceLevel >= 0.8){
                        pos = it.key();
                        if(copyfeature1.feature) {
                             SafeFree(copyfeature1.feature);
                         }
                        isFind = true;
                        break;
                    }
                }*/
                if(!isFind) {
                    for(int i=0; i<faces.size(); ++i) {
                        if(pos_h.size() > 0 && pos_h.find(i) != pos_h.constEnd()) {
                            qDebug() << "for continue";
                           // continue;
                        }
                        copyfeature1.featureSize = faces[i].featureLen;
                        memset(copyfeature1.feature, 0, 2048);
                        memcpy(copyfeature1.feature, faces[i].feature, 2048-1);

                        /*qDebug() <<"____________for len______________" <<faces[i].featureLen;
                        for(int j=0; j<faces[i].featureLen; ++j) {
                            qDebug( "faces[i].feature %x", (unsigned char)faces[i].feature[j]);
                        }*/

                        /*qDebug( "---------------------------");
                        for(int j=0; j<faces[i].featureLen; ++j) {
                            qDebug( "copyfeature1.feature %x", copyfeature1.feature[j]);
                        }
                        qDebug( "---------------------------");


                         qDebug() <<"____________end______________" ;*/

                        res = ASFFaceFeatureCompare(handle, &featureImage, &copyfeature1, &confidenceLevel);
                        if(confidenceLevel >= 0.8) {
                            pos = i;
                            if(pos_h.size() >= 100) {
                                qDebug() << "hash clear!!!";
                                pos_h.clear();
                            }
                            pos_h.insert(i, i);
                            if(copyfeature1.feature) {
                                 SafeFree(copyfeature1.feature);
                             }
                            break;
                        }
                    }
                }
                isFind = false;
           // }

            SafeFree(copyfeature1.feature);
            copyfeature1.featureSize = 0;
            compareInfoQeue.dequeue();

            if(confidenceLevel >= 0.8 && faces.size() > 0) {
                qDebug() << "confidenceLevel=" << confidenceLevel;
                emit faceCompareFinished(faces[pos].name,
                                        faces[pos].ID,
                                        confidenceLevel);
            } else {
                emit faceCompareFinished("", "", 0.0);
            }

        }

        usleep(100000);
    }

    res = ASFUninitEngine(handle);
    if (res != MOK)
        qDebug("ASFUninitEngine fail: %d\n", res);
    else
        qDebug("ASFUninitEngine sucess: %d\n", res);
    compareInfoQeue.clear();
}

void workPthread::slotDestroy(bool flag)
{
    isDestroy = flag;
}

void workPthread::addFrame(compareInfo frame)
{
    compareInfoQeue.enqueue(frame);
}


int workPthread::ASFInit()
{
    MRESULT res = MOK;
    handle = NULL;
    MInt32 mask = ASF_FACE_DETECT | ASF_FACERECOGNITION;
    res = ASFInitEngine(ASF_DETECT_MODE_VIDEO, ASF_OP_0_ONLY, NSCALE * 2, FACENUM, mask, &handle);
    if (res != MOK)
        qDebug("ASFInitEngine fail: %d\n", res);
    else
        qDebug("ASFInitEngine sucess: %d\n", res);
    return res;
}

int workPthread::exFeaturesOfImage(compareInfo* p, ASF_FaceFeature *feature)
{
    MRESULT res = MOK;

    if(p->faceNum <= 0) {
        qDebug() << "faceNum == 0";
        lastID = -1;
        emit faceCompareFinished("", "", 0.0);
        return -1;
    }

    /*if(lastID == p->faceID[0]) {
        return -1;
    }*/
    lastID = p->faceID[0];

    //emit faceCompareFinished("", "", 0.0);

    /*if(!p->faceImg) {
        qDebug("pImg is null");
        return -1;
    }*/

    res = ASFFaceFeatureExtractEx(handle, &p->compareOffscreen, &p->compareDetectedFaces, feature);
    if (res != MOK) {
        qDebug("%s ASFFaceFeatureExtractEx fail: %d\n", __func__, res);
    } /*else{
        //qDebug("%s ASFFaceFeatureExtractEx:%#x success: %d\n", __func__, feature->feature, res);
    }*/

    //cvReleaseImage(&p->faceImg);
    return res;
}


int workPthread::compareResult(compareInfo *info, ASF_FaceFeature *databaseFeature, float *scores)
{
    ASF_FaceFeature feature = { 0 };

    MRESULT res = ASFFaceFeatureExtractEx(handle, &info->compareOffscreen,
                                          &info->compareDetectedFaces, &feature);
    if (res != MOK) {
        qDebug("%s ASFFaceFeatureExtractEx fail: %d\n", __func__, res);
    } else{
        qDebug("%s ASFFaceFeatureExtractEx success: %d\n", __func__, res);
    }

    MFloat confidenceLevel;
    res = ASFFaceFeatureCompare(handle, &feature, databaseFeature, &confidenceLevel);
    if (res != MOK)
        qDebug("ASFFaceFeatureCompare fail: %d\n", res);
    else
        qDebug("ASFFaceFeatureCompare sucess: %lf\n", confidenceLevel);

    *scores = confidenceLevel;

    return res;
}

void workPthread::addFace(persionInfo *p) {
    persionInfo t;
    memset(t.feature, 0, sizeof(t.feature));
    memset(t.ID, 0, sizeof(t.ID));
    memset(t.name, 0, sizeof(t.name));

    memcpy(t.feature, p->feature, p->featureLen);
    strcpy(t.ID, p->ID);
    strcpy(t.name, p->name);
    t.featureLen = p->featureLen;

    faces.push_back(t);
    lastID = -1;
}

void workPthread::delFace(int pos) {
    faces.remove(pos);
    pos_h.remove(pos);
    lastID = -1;
}

void workPthread::delFace(char *ID) {
    for(int i=0; i<faces.size(); ++i) {
        if(strncmp(faces[i].ID, ID, strlen(ID)) == 0) {
            faces.removeAt(i);
            pos_h.remove(i);
            break;
        }
    }
    lastID = -1;

}

int workPthread::getFaceNum() {
    return faces.size();
}

