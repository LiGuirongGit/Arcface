#include "dbbase.h"

/*
该类是数据库的基类，每个继承它的类都需要重写该类的虚函数。
至于为什么需要这个类？
因为数据库中不可能只有一张表，可能需要多张表，
那么每个表都需要单独写一个类去执行一些操作吗？
明显这是不可行的，在数据库中有多张表的情况下，因为对每个表的操作的SQL语句是一样的，
不同的是表的名字，每行/每列的字段不同而已，那么把这些不同的部分分别写在一个继承这个基类的子类中，
实现多态，传参的时候，只要将子类传过去就可以实现同一个操作，在不同表中都可以执行
*/

dbBase::dbBase(){}

dbBase::~dbBase(){}

QString dbBase::isTabExist() {
    return QString();
}

QString dbBase::createTab(){
    return QString();
}

QString dbBase::del(){
    return QString();
}

QString dbBase::find() {
    return QString();
}

QVariant dbBase::pkgInfo(QList<QVariant> &info) {
    return QVariant();
}

