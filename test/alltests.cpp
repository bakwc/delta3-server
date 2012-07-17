#include "alltests.h"


AllTests::AllTests()
{
}

#include "../server/utils.h"
using namespace delta3;
void AllTests::testUtils1()
{
    #define mycmp(T,l,r) QCOMPARE( fromBytes<T>( toBytes<T>(l) ), r )
    mycmp(int,5,5);
    qint64 input2=983;
    mycmp(qint64, input2, input2);
    double input3 = 2.718281828;
    mycmp(double, input3, input3);
}
#include "../server/netextract.h"
void AllTests::testNetExtract1() {
    // testing Пакет авторизации клиента
    qint8 protoId = 1;
    qint8 protoVersion = 1;
    Cspyp1Command cmdID =  CMD1_AUTH ;
    QByteArray clientHash = QByteArray("abcdefghabcdefgh");
    QCOMPARE(clientHash.length(), 16);
    QString os = QString("Android").rightJustified(20);
    QString desc = QString("description").rightJustified(20);
    QCOMPARE(desc.length(), 20);
    QByteArray testMessage = QByteArray().append(
       QString("%1%2%3%4%5%6").arg((char)protoId).arg((char)protoVersion).arg((char)cmdID)
                .arg(QString(clientHash)).arg(os).arg(desc) );
    QCOMPARE(testMessage.length(), 59);

    QCOMPARE( getProtoId(testMessage), protoId);
    QCOMPARE( getProtoVersion(testMessage), protoVersion);
    QCOMPARE( getCommand(testMessage), cmdID);
    QCOMPARE( getClientHash(testMessage), clientHash);
    QCOMPARE( getClientOs(testMessage), os);
    QCOMPARE( getClientDevice(testMessage), desc);
}

void AllTests::testNetExtract2() {
    // Пакет авторизации администратора
    qint8 protoId = 1;
    qint8 protoVersion = 1;
    Cspyp1Command cmdID =  CMD1_AUTH ;
    QString login = QString('a').leftJustified(22, QChar('2'));
    QString pass  = QString('b').leftJustified(22, QChar('4'));
    QCOMPARE(login.length(), 22);
    QCOMPARE(pass.length(),  22);
    QByteArray testMessage = QByteArray().append(
       QString("%1%2%3%4%5").arg((char)protoId).arg((char)protoVersion).arg((char)cmdID)
                .arg(login).arg(pass) );
    //qDebug() << "testMsg" << testMessage;
    QCOMPARE(testMessage.length(), 47);

    QCOMPARE( getProtoId(testMessage), protoId);
    QCOMPARE( getProtoVersion(testMessage), protoVersion);
    QCOMPARE( getCommand(testMessage), cmdID);
    QCOMPARE( getAdminLogin(testMessage), login);
    QCOMPARE( getAdminPassword(testMessage), pass);
}

void AllTests::testNetExtract3() {
    // Пакет запроса списка клиентов
    qint8 protoId = 1;
    qint8 protoVersion = 1;
    Cspyp1Command cmdID =  CMD1_LIST ;
    QByteArray testMessage = QByteArray().append(
       QString("%1%2%3").arg((char)protoId).arg((char)protoVersion).arg((char)cmdID) );
    //qDebug() << "testMsg" << testMessage;
    QCOMPARE(testMessage.length(), 3);

    QCOMPARE( getProtoId(testMessage), protoId);
    QCOMPARE( getProtoVersion(testMessage), protoVersion);
    QCOMPARE( getCommand(testMessage), cmdID);
}
