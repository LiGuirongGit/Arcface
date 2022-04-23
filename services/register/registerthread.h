#ifndef REGISTERTHREAD_H
#define REGISTERTHREAD_H

#include <QObject>
#include <QThread>
#include <QVariant>
#include <QFileDialog>
#include <QDateTime>
#include "../detect/workpthread.h"

class registerThread : public QThread
{
    Q_OBJECT
public:
    explicit registerThread(QObject *parent = nullptr);
    void run();
    int getSumNum();
    int getCurNum();
    void stop();
    void ASFDeInit();

signals:
    void signalInsertInfoFinish(QVariant);

private:
    int sumNum;
    int curNum;
    bool isStop;
    MHandle handle = NULL;

    int ASFInit();
    int registerExFeaturesOfFile(QString filePth, ASF_FaceFeature *OutFeature);

};

#endif // REGISTERTHREAD_H
