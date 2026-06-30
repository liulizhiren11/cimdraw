#ifndef LZCONNECTIONPARAMETERS_H
#define LZCONNECTIONPARAMETERS_H

#include <QVector>
#include "LzConnectConfig.h"

namespace ConnectType
{
    typedef QVector<QGraphicsItem*> ConnectLines;
    struct LzConnectionParameters
    {
        ConnectLines connects;
    };
}

#endif;
