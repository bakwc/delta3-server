#ifndef ALLTESTS_H
#define ALLTESTS_H

#include <QObject>
#include <QByteArray>
#include <QtTest>

class AllTests : public QObject
{
    Q_OBJECT

public:
    AllTests();

private Q_SLOTS:
    void testUtils1();

    void testNetExtract1();
    void testNetExtract2();
    void testNetExtract3();

};

#endif // ALLTESTS_H
