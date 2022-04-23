#ifndef HRQTPRO_H
#define HRQTPRO_H

#include <QWidget>
#include <QDebug>
#include <QImage>
#include <QPainter>
#include <QObject>
#include <QThreadPool>
#include <QMessageBox>
#include <QTimer>

#include "services/detect/workpthread.h"
#include "services/register/registerthread.h"

using namespace cv;

namespace Ui {
class hrQtPro;
}

class hrQtPro : public QWidget
{
    Q_OBJECT

public:
    explicit hrQtPro(QWidget *parent = 0);
    ~hrQtPro();

protected:
    void paintEvent(QPaintEvent* e);

public slots:

private slots:
    int slotReadFarme();
    void slotFaceCompareFinished(QString name, QString ID, float scores);
    void slotRegisterFace();
    void slotPicregister();
    void slotFacesUpdate(QVariant);
    void slotDeleteFace();

signals:
    void workThreadDestroy(bool);
    void signalDatabaseInsertInfo(persionInfo);
    void signalDatabaseDeleteInfoByID(QString);

private:
    Ui::hrQtPro *ui;
    //facePthreadPool *threadPool;

    VideoCapture capture;
    QTimer  *cvTimer;
    MHandle handle;
    Mat cap;
    QImage qImg;
    ASF_MultiFaceInfo detectedFaces;

    workPthread *thread1;

    registerThread *automaticRegister;

    //face info
    int faceRect[4];
    QString faceName;
    QString faceID;
    int faceid;
    bool faceCompareflag;

    int ASFInit();
    int OpenCamera();
    QImage Mat2QImage(Mat &src);
    int getCameraParameter();

};

#endif // HRQTPRO_H
