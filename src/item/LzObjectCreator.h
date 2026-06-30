#ifndef LZOBJECTCREATOR_H
#define LZOBJECTCREATOR_H

#include <QGraphicsItem>

class LzObjectCreator
{
public:
    virtual QGraphicsItem* createObject() = 0;
};

#endif
