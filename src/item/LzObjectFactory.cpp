#include <QDomElement>

#include "LzObjectFactory.h"
#include "LzScene.h"
#include "LzGroup.h"
#include "LzItem.h"

LzObjectFactory* LzObjectFactory::inst = nullptr;
LzObjectFactory* LzObjectFactory::instance()
{
	if (!inst)
	{
		inst = new LzObjectFactory;
	}

	return inst;
}

LzObjectFactory::LzObjectFactory()
{

}

QGraphicsItem* LzObjectFactory::createObject(const QString& shape)
{
	if (creatorMap.contains(shape))
	{
		return creatorMap.value(shape)->createObject();
	}

	return nullptr;
}
void LzObjectFactory::registerCreator(const QString& shape, LzObjectCreator* creator)
{
	creatorMap.insert(shape, creator);
}


QGraphicsItem* toObject(LzScene* scene, QDomElement* g, bool addToDoc)
{
    if (!g)
        return nullptr;

    QString type = g->attribute(QStringLiteral("type"));
    if (type.isEmpty())
        type = g->attribute(QStringLiteral("shape"));
    if (type.isEmpty())
        return nullptr;

    QGraphicsItem* obj = LzObjectFactory::instance()->createObject(type);
    if (!obj)
        return nullptr;

    if (auto* li = dynamic_cast<LzItem*>(obj)) {
        if (!li->loadXml(g)) {
            delete li;
            return nullptr;
        }
        if (scene && addToDoc)
            scene->addItem(li);
        return li;
    }
    if (auto* grp = dynamic_cast<LzGroup*>(obj)) {
        if (!grp->loadXml(g)) {
            delete grp;
            return nullptr;
        }
        if (scene && addToDoc)
            scene->addItem(grp);
        return grp;
    }

    delete obj;
    return nullptr;
}
