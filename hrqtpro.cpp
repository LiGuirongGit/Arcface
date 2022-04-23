#include "hrqtpro.h"
#include "ui_hrqtpro.h"

#define APPID ""
#define SDKKEY ""

hrQtPro::hrQtPro(QWidget *parent) :
    QWidget(parent),
    handle(NULL),
    ui(new Ui::hrQtPro)
{
    ui->setupUi(this);
    ui->nameLineEdit->setPlaceholderText("Name");
    ui->IDlineEdit->setPlaceholderText("ID");
    ui->picregisterBtn->hide();

    //数据库初始化
    dbInit();
    createFaceInfoTab();

    /*注册按钮*/
    connect(ui->registerBtn, SIGNAL(clicked()), this, SLOT(slotRegisterFace()));
    /*图片自动注册按钮*/
    connect(ui->picregisterBtn, SIGNAL(clicked()), this, SLOT(slotPicregister()));
    connect(ui->delFaceBtn, SIGNAL(clicked()), this, SLOT(slotDeleteFace()));

#if 1
    /*注册消息类型*/
    qRegisterMetaType<QVariant>("QVariant");
    /*单个人脸对比标志位*/
    faceCompareflag = true;

    /*人脸特征对比*/
    thread1 = new workPthread();

    connect(thread1, SIGNAL(faceCompareFinished(QString , QString ,float)),
            this, SLOT(slotFaceCompareFinished(QString , QString ,float)));
     connect(this, SIGNAL(workThreadDestroy(bool)),
             thread1, SLOT(slotDestroy(bool)));

    thread1->start();

    /*自动注册*/
    automaticRegister = new registerThread(this);
    connect(automaticRegister, SIGNAL(signalInsertInfoFinish(QVariant)),
                this, SLOT(slotFacesUpdate(QVariant)));


#endif
    cvTimer = new QTimer(this);
    connect(cvTimer, SIGNAL(timeout()), this, SLOT(slotReadFarme()));  // 时间到，读取当前摄像头信息
    ASFInit();
    OpenCamera();
    ui->faceNumLab->setText(QString("人数：%1").arg(QString::number(thread1->getFaceNum())));

}

hrQtPro::~hrQtPro()
{
    emit workThreadDestroy(true);
    thread1->quit();
    thread1->wait(300);
    MRESULT res = ASFUninitEngine(handle);
    if (res != MOK)
        printf("ASFUninitEngine fail: %d\n", res);
    else
        printf("ASFUninitEngine sucess: %d\n", res);
    if(thread1) {
        delete thread1;
        thread1 = nullptr;
    }
    automaticRegister->stop();
    automaticRegister->wait(300);
    automaticRegister->ASFDeInit();
    if(automaticRegister) {
        delete automaticRegister;
        automaticRegister = nullptr;
    }
    cap.release();
    capture.release();
    delete ui;
}

int hrQtPro::ASFInit()
{
    MRESULT res = MOK;
#if 0
    res = ASFOnlineActivation(APPID, SDKKEY);
    if (MOK != res && MERR_ASF_ALREADY_ACTIVATED != res)
        qDebug("ASFOnlineActivation fail: %d\n", res);
    else
        qDebug("ASFOnlineActivation sucess: %d\n", res);
#endif
    MInt32 mask = ASF_FACE_DETECT | ASF_FACERECOGNITION;
    res = ASFInitEngine(ASF_DETECT_MODE_VIDEO, ASF_OP_0_ONLY, NSCALE, FACENUM, mask, &handle);
    if (res != MOK)
        qDebug("ASFInitEngine fail: %d\n", res);
    else
        qDebug("ASFInitEngine sucess: %d\n", res);
    return res;
}

int hrQtPro::getCameraParameter()
{
    double brightness = 0;        //亮度
    double contrast = 0;        //对比度
    double saturation = 0;        //饱和度
    double hue = 0;                //色调
    double gain = 0;            //增益
    double exposure = 0;        //曝光
    double white_balance = 0;    //白平衡

    double pos_msec = 0;        //当前视频位置(ms)
    double pos_frame = 0;        //从0开始下一帧的索引
    double pos_avi_ratio = 0;    //视频中的相对位置(范围为0.0到1.0)
    double frame_width = 0;        //视频帧的像素宽度
    double frame_height = 0;    //视频帧的像素高度
    double fps = 0;                //帧速率
    double frame_count = 0;        //视频总帧数
    double video_duration = 0.00;    //视频时长
    double format = 0;            //格式

    frame_width = capture.get(cv::CAP_PROP_FRAME_WIDTH);
    frame_height = capture.get(cv::CAP_PROP_FRAME_HEIGHT);
    fps = capture.get(cv::CAP_PROP_FPS);
    frame_count = capture.get(cv::CAP_PROP_FRAME_COUNT);
    format = capture.get(cv::CAP_PROP_FORMAT);
    pos_avi_ratio = capture.get(cv::CAP_PROP_POS_AVI_RATIO);
    video_duration = frame_count / fps;

    qDebug() << "---------------------------------------------";
    qDebug() << "视频中的相对位置(范围为0.0到1.0):" << pos_avi_ratio ;
    qDebug() << "视频帧的像素宽度:" << frame_width;
    qDebug() << "视频帧的像素高度:" << frame_height;
    qDebug() << "录制视频的帧速率(帧/秒):" << fps ;
    qDebug() << "视频文件总帧数:" << frame_count ;
    qDebug() << "图像的格式:" << format ;
    qDebug() << "视频时长:" << video_duration;
    qDebug() << "---------------------------------------------";

    brightness = capture.get(cv::CAP_PROP_BRIGHTNESS);
    contrast= capture.get(cv::CAP_PROP_CONTRAST);
    saturation = capture.get(cv::CAP_PROP_SATURATION);
    hue = capture.get(cv::CAP_PROP_HUE);
    gain = capture.get(cv::CAP_PROP_GAIN);
    exposure = capture.get(cv::CAP_PROP_EXPOSURE);
    white_balance = capture.get(cv::CAP_PROP_WHITE_BALANCE_BLUE_U);

    qDebug() << "---------------------------------------------";
    qDebug() << "摄像头亮度：" << brightness;
    qDebug() << "摄像头对比度：" << contrast;
    qDebug() << "摄像头饱和度：" << saturation;
    qDebug() << "摄像头色调：" << hue;
    qDebug() << "摄像头增益：" << gain;
    qDebug() << "摄像头曝光度：" << exposure;
    qDebug() << "摄像头白平衡：" << white_balance;
    qDebug() << "---------------------------------------------" ;


    pos_msec = capture.get(cv::CAP_PROP_POS_MSEC);
    pos_frame = capture.get(cv::CAP_PROP_POS_FRAMES);
    pos_avi_ratio = capture.get(cv::CAP_PROP_POS_AVI_RATIO);

    qDebug() << "---------------------------------------------";
    qDebug() << "视频文件中当前位置(ms):" << pos_msec;
    qDebug() << "从0开始下一帧的索引:" << pos_frame;
    qDebug() << "视频中的相对位置(范围为0.0到1.0):" << pos_avi_ratio;
    qDebug() << "---------------------------------------------";

}

int hrQtPro::OpenCamera()
{
    //不知道填啥的，/dev/video*
    if(capture.open(0)) {  //从摄像头读入视频如果设备只有一个摄像头就传入参数0
        qDebug() << "open success";
    } else {
        qDebug() << "open error";
    }
    if (!capture.isOpened()) {//先判断是否打开摄像头
         qDebug("err");
         return -1;
    }

    getCameraParameter();
    /* 设置格式 */
    /* 如果不设置格式：执行 ret, frame = cap.read()时终端输出 select timeout
     * 或者 VIDIOC_DQBUF: Resource temporarily unavailable ，
     * 导致无法从摄像头获取图像，frame为空。*/
    /* CPU/GPU处理能力有限，或者是主流的usb摄像头驱动uvc有一些限制，最终造成图像数据无法很好处理。*/
    /* 由于opencv使用uvc读取usb摄像机流，
     * 而cv2.VideoCapture可能默认为未压缩的流，
     * 例如YUYV，因此我们需要将流格式更改为MJPG之类的内容，
     * 具体取决于摄像机是否支持该格式
     */
    capture.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M','J','P','G'));
    cvTimer->start(20);

    return 0;
}

int hrQtPro::slotReadFarme()
{
   capture.read(cap);
   if(!cap.data || cap.empty()) {
       return -1;
   }
   //Mat转换为QImage
   //为什么要转换？
   //因为Mat的格式是BRG,而QImage是RBG格式的，
   //想要通过label显示出来，就要转换为RGB，
   //RGB，对应的是红绿蓝三原色，BGR，对应的是蓝绿红，转换就需要红蓝通道交换

   qImg = Mat2QImage(cap);
   if(qImg.isNull())
       return -1;

   IplImage ipi(cap);
   IplImage* img = cvCreateImage(cvSize(ipi.width - ipi.width % 4,
                                        ipi.height), IPL_DEPTH_8U, ipi.nChannels);

   CutIplImage(&ipi, img, 0, 0);

   ASVLOFFSCREEN offscreen = { 0 };
   offscreen.u32PixelArrayFormat = ASVL_PAF_RGB24_B8G8R8;
   offscreen.i32Width = img->width;
   offscreen.i32Height = img->height;
   offscreen.pi32Pitch[0] = img->widthStep;
   offscreen.ppu8Plane[0] = (MUInt8*)img->imageData;

   if (img) {
       MRESULT res = ASFDetectFacesEx(handle, &offscreen, &detectedFaces);
       if (MOK != res) {
           qDebug("ASFDetectFacesEx failed: %d\n", res);
       } else {
           for (int i = 0; i < detectedFaces.faceNum; i++) {
               if(detectedFaces.faceID[0] != faceid) {
                   faceid = detectedFaces.faceID[0];
                   faceName = "";
                   faceID = "";
               }
               if(faceCompareflag && detectedFaces.faceNum > 0 ) {
                   faceCompareflag = false;
                   compareInfo t;

                   t.faceImg = NULL;
                   t.faceImg = cvCreateImage(cvSize(ipi.width - ipi.width % 4,
                                                           ipi.height), IPL_DEPTH_8U, ipi.nChannels);
                   CutIplImage(&ipi, t.faceImg, 0, 0);
                   t.compareDetectedFaces.faceRect.left = detectedFaces.faceRect[0].left;
                   t.compareDetectedFaces.faceRect.top = detectedFaces.faceRect[0].top;
                   t.compareDetectedFaces.faceRect.right = detectedFaces.faceRect[0].right;
                   t.compareDetectedFaces.faceRect.bottom = detectedFaces.faceRect[0].bottom;
                   t.compareDetectedFaces.faceOrient = detectedFaces.faceOrient[0];
                   t.faceID[0] = detectedFaces.faceID[0];
                   t.faceNum = detectedFaces.faceNum;
                   memcpy(&t.compareOffscreen, &offscreen, sizeof(offscreen));
                   thread1->addFrame(t);
                   cvReleaseImage(&t.faceImg);
               }
           }
       }
       cvReleaseImage(&img);
   }

}

// 图片转换
QImage hrQtPro::Mat2QImage(Mat &src) {
    if(src.type() == CV_8UC3) {//为3通道的彩色图片
        //得到图像的的首地址
        const uchar *pSrc = (const uchar*)src.data;
        //以src构造图片
        QImage qImage(pSrc,src.cols,src.rows,src.step,QImage::Format_RGB888);
        //在不改变实际图像数据的条件下，交换红蓝通道
        return qImage.rgbSwapped();
    }
    return QImage();
}

void hrQtPro::paintEvent(QPaintEvent* e)
{
    if(!qImg.isNull()) {
        QPainter painter(&qImg);
        // 设置画笔颜色
        if(detectedFaces.faceNum > 0) {
            painter.setPen(QColor(255, 0, 0));
            painter.drawText(QPoint(detectedFaces.faceRect[0].left, detectedFaces.faceRect[0].top - 20),
                    faceName);
            painter.drawRect(detectedFaces.faceRect[0].left,
                    detectedFaces.faceRect[0].top,
                    detectedFaces.faceRect[0].right - detectedFaces.faceRect[0].left,
                    detectedFaces.faceRect[0].bottom - detectedFaces.faceRect[0].top);//绘制人脸的框
        }
        ui->previewLabel->setPixmap(QPixmap::fromImage(qImg));
    }
}

void hrQtPro::slotFaceCompareFinished(QString name, QString ID, float scores)
{
    if(scores >= 0.8) {
        faceName = name;
        faceID = ID;
    } else {
        faceName = "";
        faceID = "";
    }
    faceCompareflag = true;
}


/*
 * 函数作用：根据姓名和id，注册一个人员
 * 思路：截取当前帧作为一张图片，将这张图片的特征值取出来，并将这些特征值转换为base64，
 * 存到数据库中，最后更新缓存区的人脸数据
*/
void hrQtPro::slotRegisterFace()
{
    QString name = ui->nameLineEdit->text();
    QString ID = ui->IDlineEdit->text();
    if(name.isEmpty() || name.isNull()) {
        QMessageBox::information(NULL, "Please enter your name",
                                 "warning",QMessageBox::Yes);
        return;
    } else if(ID.isEmpty() || ID.isNull()) {
        QMessageBox::information(NULL, "Please enter your ID",
                                 "warning",QMessageBox::Yes);
        return;
    }
    QDateTime time = QDateTime::currentDateTime();
    int ms = time.time().msec();
    qint64 epochTime = time.toMSecsSinceEpoch();

    QString imgStr = QString("%1%2-%3-%4.jpg")
            .arg(MANUAL_REGISTER_IMAGE_PATH)
            .arg(name)
            .arg(ID)
            .arg(QString::number(epochTime));

    if(qImg.isNull()) {
        qDebug("[%s:%d] img is NULL!!!", __func__, __LINE__);
        return;
    }
    if(!qImg.save(imgStr)) {
        qDebug("[%s:%d] save %s failed!!!", __func__, __LINE__, imgStr.toUtf8().data());
        return;
    }

    ASF_FaceFeature feature = { 0 };
    if(exFeaturesOfFile(imgStr, &feature) == 0){
        persionInfo registerInfo;

        strcpy(registerInfo.name, name.toUtf8().data());
        strcpy(registerInfo.ID, ID.toUtf8().data());
        memset(registerInfo.feature, 0, sizeof(registerInfo.feature));

        memcpy(registerInfo.feature, feature.feature, feature.featureSize);
        registerInfo.featureLen = feature.featureSize;


        /*for(int i=0; i<feature.featureSize; ++i) {
            qDebug( "registerInfo.feature %x", (unsigned char)registerInfo.feature[i]);
        }*/

        addFaceInfo(&registerInfo);
        //更新缓存区的人脸数据
        thread1->addFace(&registerInfo);
        ui->faceNumLab->setText(QString("人数：%1").arg(QString::number(thread1->getFaceNum())));
    }
    QFile::remove(imgStr);
}

void hrQtPro::slotFacesUpdate(QVariant p) {
    int curNum = automaticRegister->getCurNum();
    int sumNum = automaticRegister->getSumNum();
    QString text;
    if(p.canConvert<persionInfo>()) {
        persionInfo t = p.value<persionInfo>();
        thread1->addFace(&t);
        ui->faceNumLab->setText(QString("人数：%1").arg(QString::number(thread1->getFaceNum())));
        if(curNum == sumNum) {
            text = QString("完成(%1/%2)").arg(QString::number(sumNum)).arg(QString::number(curNum));

        } else {
            text = QString("暂停(%1/%2)").arg(QString::number(sumNum)).arg(QString::number(curNum));
        }
        ui->picregisterBtn->setText(text);
    } else {
        qDebug("[%s:%d] can not convert persionInfo", __func__, __LINE__);
    }
}

void hrQtPro::slotPicregister() {
    static bool state = false;
    state = !state;
    if(state) {
        qDebug() << "start";
        automaticRegister->start();
    } else {
        qDebug() << "stop";
        automaticRegister->stop();
        ui->picregisterBtn->setText("自动注册");
    }
}

void hrQtPro::slotDeleteFace()
{
    QString faceId = ui->IDlineEdit->text();
    if(faceId.isNull() || faceId.isEmpty()) {
        QMessageBox::information(NULL, "Please enter ID",
                                 "warning",QMessageBox::Yes);
        return;
    }
    delFaceInfo("ID", faceId);
    thread1->delFace(faceId.toUtf8().data());
    ui->faceNumLab->setText(QString("人数：%1").arg(QString::number(thread1->getFaceNum())));

}
