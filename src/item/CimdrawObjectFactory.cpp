#include <QDomElement>

#include "CimdrawObjectFactory.h"
#include "CimdrawScene.h"
#include "CimdrawGroup.h"
#include "CimdrawItem.h"

CimdrawObjectFactory* CimdrawObjectFactory::inst = nullptr;
CimdrawObjectFactory* CimdrawObjectFactory::instance()
{
	if (!inst)
	{
		inst = new CimdrawObjectFactory;
	}

	return inst;
}

CimdrawObjectFactory::CimdrawObjectFactory()
{

}

QGraphicsItem* CimdrawObjectFactory::createObject(const QString& shape)
{
	if (creatorMap.contains(shape))
	{
		return creatorMap.value(shape)->createObject();
	}

	return nullptr;
}
void CimdrawObjectFactory::registerCreator(const QString& shape, CimdrawObjectCreator* creator)
{
	creatorMap.insert(shape, creator);
}


QGraphicsItem* toObject(CimdrawScene* scene, QDomElement* g, bool addToDoc)
{
    if (!g)
        return nullptr;

    QString type = g->attribute(QStringLiteral("type"));
    if (type.isEmpty())
        type = g->attribute(QStringLiteral("shape"));
    if (type.isEmpty())
        return nullptr;

    QGraphicsItem* obj = CimdrawObjectFactory::instance()->createObject(type);
    if (!obj)
        return nullptr;

    if (auto* li = dynamic_cast<CimdrawItem*>(obj)) {
        if (!li->loadXml(g)) {
            delete li;
            return nullptr;
        }
        if (scene && addToDoc)
            scene->addItem(li);
        return li;
    }
    if (auto* grp = dynamic_cast<CimdrawGroup*>(obj)) {
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
