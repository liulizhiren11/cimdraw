#include "CimdrawPowerArresterTool.h"
#include "CimdrawPowerArresterItem.h"
#include "CimdrawWiringToolUtils.h"
#include "CimdrawScene.h"
#include "wiring/base/CimdrawWiringSymbolStyle.h"

#include <QGraphicsSceneMouseEvent>
#include <QPainter>

CimdrawPowerArresterTool::CimdrawPowerArresterTool(QObject* parent)
    : CimdrawTool(parent)
{
    name = tr("避雷器 MOA");
    createMode = CREATE_MODE_DRAG;
}

CimdrawPowerArresterTool::~CimdrawPowerArresterTool() = default;

CimdrawDrawTypeId CimdrawPowerArresterTool::getDrawType() const
{
    return CIMDRAW_WSYM_ARRESTER;
}

QGraphicsItem* CimdrawPowerArresterTool::createObject(CimdrawScene* scene, QRectF* pos, bool addToScene)
{
    auto* item = new CimdrawPowerArresterItem(*pos);
    item->setDrawTool(this);
    if (item && addToScene && scene)
    {
        scene->cleanSelection();
        scene->addItem(item);
    }
    return item;
}

QIcon CimdrawPowerArresterTool::getIcon(const QSize& size, const QVariant& para) const
{
    Q_UNUSED(para);
    QPixmap pm(size);
    pm.fill(Qt::transparent);
    QPainter painter(&pm);
    painter.setRenderHint(QPainter::Antialiasing, true);
    const qreal margin = 4.0;
    const QRectF avail(margin, margin, qMax(1.0, size.width() - margin * 2), qMax(1.0, size.height() - margin * 2));
    const QSizeF def = CimdrawPowerArresterItem::defaultSize();
    const qreal scale = qMin(avail.width() / def.width(), avail.height() / def.height());
    const QRectF iconRect(avail.center().x() - def.width() * scale * 0.5,
                          avail.center().y() - def.height() * scale * 0.5,
                          def.width() * scale,
                          def.height() * scale);
    CimdrawPowerArresterItem::drawSymbol(
        &painter, iconRect, CimdrawWiringRunState::Normal, false, CimdrawWiringSymbolStyle::instance().standard());
    return QIcon(pm);
}

void CimdrawPowerArresterTool::onMousePress(CimdrawScene* scene, QGraphicsSceneMouseEvent* evt)
{
    if (evt->button() == Qt::RightButton)
    {
        CimdrawToolManager::getInstance()->changeTool(CIMDRAW_SELECTION);
        scene->cleanSelection();
        return;
    }
    const QPointF local = evt->scenePos();
    QRectF rc(local, CimdrawPowerArresterItem::defaultSize());
    QGraphicsItem* pObj = createObject(scene, &rc);
    cimdrawSldPushAdd(scene, pObj, evt);
}

