/**
 * В этом файле размещено несколько шаблонных функций
 * для сериализации и десериализации различных типов.
 * Т. е. для преобразования, к примеру, числа типа int
 * в список из четырёх байт QByteArray. Функции сделаны
 * шаблонными для того, чтобы они могли работать с
 * различными типами (qint18, qint32, и т. п.).
*/
#pragma once

#include <QtGlobal>
#include <QByteArray>
#include <QString>
#include <QDebug>

namespace delta3
{
    template <typename T>
    inline QByteArray toBytes(T num)
    {
        QByteArray result;
        result.append( (char*)(&num),sizeof(T));
        return result;
    }

    template <typename T>
    inline T fromBytes(const QByteArray& array)
    {
        if (array.size()!=sizeof(T))
        {
            qDebug() << "fromBytes(): error";
            return (T) 0;
        }
        const T* tmp=(reinterpret_cast<const T*>(array.data()));
        return *tmp;
    }

    inline QByteArray toBytes(const QString& str, quint16 len)
    {
        QByteArray res=str.toLocal8Bit();
        res.leftJustified(len, 0, true);
        return res;
    }
}
