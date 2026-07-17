#ifndef CIMDRAWOBJECTCREATOR_H
#define CIMDRAWOBJECTCREATOR_H

#include <QGraphicsItem>

class CimdrawObjectCreator
{
public:
    virtual QGraphicsItem* createObject() = 0;
};

#endif
