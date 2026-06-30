#ifndef LZOBJECTFACTORY_H
#define LZOBJECTFACTORY_H

#include <QGraphicsItem>
#include <QMap>

#include "LzItemConfig.h"
#include "LzObjectCreator.h"

class LzScene;
class QDomElement;

class LzObjectFactory
{
public:
	static LzObjectFactory* instance();
    QGraphicsItem* createObject(const QString& shape);
	void registerCreator(const QString& shape, LzObjectCreator* creator);

private:
	QMap<QString, LzObjectCreator*> creatorMap;
	static LzObjectFactory* inst;

    LzObjectFactory();
    Q_DISABLE_COPY(LzObjectFactory)
};

#define REGISTER_OBJECT_CREATOR(classname, shapename)\
class LzObjectCreator##classname : public LzObjectCreator \
{ \
public: \
	LzObjectCreator##classname() \
	{ \
        LzObjectFactory::instance()->registerCreator(#shapename, this);\
	} \
    virtual QGraphicsItem* createObject() \
	{ \
		return new classname();\
	} \
}; \
LzObjectCreator##classname classname##creator;

QGraphicsItem* toObject(LzScene* scene,QDomElement *g, bool addToDoc);

#endif
