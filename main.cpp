#include "hrqtpro.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    hrQtPro w;
    w.show();

    return a.exec();
}
