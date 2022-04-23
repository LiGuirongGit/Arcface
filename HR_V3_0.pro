#-------------------------------------------------
#
# Project created by QtCreator 2021-04-29T14:24:25
#
#-------------------------------------------------

QT       += core gui sql
CONFIG += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = HR_V3_0
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        hrqtpro.cpp \
    services/detect/workpthread.cpp \
    services/register/registerthread.cpp \
    db/dbAPI/dbapi.cpp \
    db/dbBase/dbbase.cpp \
    db/dbSql/dbsql.cpp \
    db/dbTables/faceinfo.cpp

HEADERS += \
        hrqtpro.h \
    ASF/inc/merror.h \
    ASF/inc/asvloffscreen.h \
    ASF/inc/arcsoft_face_sdk.h \
    ASF/inc/amcomdef.h \
    services/detect/workpthread.h \
    services/register/registerthread.h \
    db/dbAPI/dbapi.h \
    db/dbBase/dbbase.h \
    db/dbSql/dbsql.h \
    db/dbTables/faceinfo.h

FORMS += \
        hrqtpro.ui

LIBS += /usr/local/lib/libopencv_highgui.so \
        /usr/local/lib/libopencv_core.so    \
        /usr/local/lib/libopencv_imgproc.so \
        /usr/local/lib/libopencv_imgcodecs.so\
        /usr/local/lib/libopencv_objdetect.so\
        /usr/local/lib/libopencv_shape.so \
        /usr/local/lib/libopencv_videoio.so


win32:CONFIG(release, debug|release): LIBS += -L$$PWD/ASF/linux_so/release/ -larcsoft_face_engine
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/ASF/linux_so/debug/ -larcsoft_face_engine
else:unix: LIBS += -L$$PWD/ASF/linux_so/ -larcsoft_face_engine


win32:CONFIG(release, debug|release): LIBS += -L$$PWD/ASF/linux_so/release/ -larcsoft_face
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/ASF/linux_so/debug/ -larcsoft_face
else:unix: LIBS += -L$$PWD/ASF/linux_so/ -larcsoft_face

