#ifndef CIMDRAWCONNECTIONPARAMETERS_H
#define CIMDRAWCONNECTIONPARAMETERS_H

#include <QVector>
#include "CimdrawConnectConfig.h"

namespace ConnectType
{
    typedef QVector<QGraphicsItem*> ConnectLines;
    struct CimdrawConnectionParameters
    {
        ConnectLines connects;
    };
}

#endif;
