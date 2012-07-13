#pragma once
#include <QtGlobal>
#include <QByteArray>

template <typename T>
QByteArray toBytes(T num)
{
    QByteArray result;
    result.append( (char*)(&num),sizeof(T));
    return result;
}

template <typename T>
T fromBytes(QByteArray array)
{
    if (array.size()!=sizeof(T))
    {
        qDebug() << "fromBytes(): error";
        return 0;
    }
    T res=*(reinterpret_cast<T*>(array.data()));
    return res;
}
