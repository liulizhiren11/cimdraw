#ifndef CIMDRAWOBJECTFACTORY_H
#define CIMDRAWOBJECTFACTORY_H

#include <QGraphicsItem>
#include <QMap>

#include "CimdrawItemConfig.h"
#include "CimdrawObjectCreator.h"

class CimdrawScene;
class QDomElement;

class CimdrawObjectFactory
{
public:
	static CimdrawObjectFactory* instance();
    QGraphicsItem* createObject(const QString& shape);
	void registerCreator(const QString& shape, CimdrawObjectCreator* creator);

private:
	QMap<QString, CimdrawObjectCreator*> creatorMap;
	static CimdrawObjectFactory* inst;

    CimdrawObjectFactory();
    Q_DISABLE_COPY(CimdrawObjectFactory)
};

#define REGISTER_OBJECT_CREATOR(classname, shapename)\
class CimdrawObjectCreator##classname : public CimdrawObjectCreator \
{ \
public: \
	CimdrawObjectCreator##classname() \
	{ \
        CimdrawObjectFactory::instance()->registerCreator(#shapename, this);\
	} \
    virtual QGraphicsItem* createObject() \
	{ \
		return new classname();\
	} \
}; \
CimdrawObjectCreator##classname classname##creator;

QGraphicsItem* toObject(CimdrawScene* scene,QDomElement *g, bool addToDoc);

#endif
