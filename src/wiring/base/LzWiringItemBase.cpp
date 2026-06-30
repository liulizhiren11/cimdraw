#include "LzWiringItemBase.h"
#include "LzWiringDrawPalette.h"
#include "LzWiringSymbolStyle.h"
#include "LzScene.h"
#include "Item/LzConnectPoint.h"
#include "Item/LzHandle.h"
#include "LzConnectConfig.h"
#include "LzItemConfig.h"

#include <QAction>
#include <QDateTime>
#include <QSet>
#include <QGraphicsSceneMouseEvent>
#include <QInputDialog>
#include <QMenu>
#include <QPainter>
#include <QPainterPath>
#include <QTimer>
#include <QVector>
#include <QtMath>

namespace {

QHash<QString, bool> fullyConnectedTerminalPairs(const QVector<QString>& terminalIds)
{
    QHash<QString, bool> connectivity;
    for (int i = 0; i < terminalIds.size(); ++i)
    {
        for (int j = i + 1; j < terminalIds.size(); ++j)
            connectivity.insert(terminalIds.at(i) + QStringLiteral("|") + terminalIds.at(j), true);
    }
    return connectivity;
}

QHash<QString, bool> adjacentTerminalPairs(const QVector<QString>& terminalIds)
{
    QHash<QString, bool> connectivity;
    for (int i = 0; i + 1 < terminalIds.size(); ++i)
        connectivity.insert(terminalIds.at(i) + QStringLiteral("|") + terminalIds.at(i + 1), true);
    return connectivity;
}


void purgeConnectPointChildren(QGraphicsItem* item, QVector<LzConnectPoint*>& tracked)
{
    for (QGraphicsItem* ch : item->childItems())
    {
        if (auto* cp = qgraphicsitem_cast<LzConnectPoint*>(ch))
        {
            tracked.removeAll(cp);
            delete cp;
        }
    }
    for (LzConnectPoint* p : tracked)
        delete p;
    tracked.clear();
}

LzWiringRunState clampRunState(int v)
{
    if (v < 0 || v > 3)
        return LzWiringRunState::Normal;
    return static_cast<LzWiringRunState>(v);
}

int overlappingEdgeHandleForPort(const LzConnectPoint* port, const QRectF& itemRect)
{
    if (!port)
        return toInt(HANDLE_POINT::HANDLE_NONE);

    constexpr qreal kOverlapTolerance = 8.0;
    const QPointF portCenter = port->mapToParent(port->connectionCenterLocal());
    const QPointF rectCenter = itemRect.center();

    auto isNear = [&](const QPointF& handleCenter) {
        return qAbs(portCenter.x() - handleCenter.x()) <= kOverlapTolerance
            && qAbs(portCenter.y() - handleCenter.y()) <= kOverlapTolerance;
    };

    switch (port->getDirection())
    {
    case TOP_DIRECTION:
        if (isNear(QPointF(rectCenter.x(), itemRect.top())))
            return toInt(HANDLE_POINT::TOP);
        break;
    case RIGHT_DIRECTION:
        if (isNear(QPointF(itemRect.right(), rectCenter.y())))
            return toInt(HANDLE_POINT::RIGHT);
        break;
    case BOTTOM_DIRECTION:
        if (isNear(QPointF(rectCenter.x(), itemRect.bottom())))
            return toInt(HANDLE_POINT::BOTTOM);
        break;
    case LEFT_DIRECTION:
        if (isNear(QPointF(itemRect.left(), rectCenter.y())))
            return toInt(HANDLE_POINT::LEFT);
        break;
    default:
        break;
    }

    return toInt(HANDLE_POINT::HANDLE_NONE);
}
} // namespace

void LzWiringItemBase::installConnectPointsFromRect(const QRectF& pos)
{
    setUseFont(false);
    itemWidth = pos.width();
    itemHeight = pos.height();
    itemPosition = QRectF(0, 0, itemWidth, itemHeight);
    itemInitialRect = itemPosition;

    purgeConnectPointChildren(this, pointStruct.points);

    if (wiringUsesCustomConnectPoints())
    {
        pointStruct.count = static_cast<ConnectCount>(qMax(0, customWiringConnectPointCount()));
    }
    else if (wiringUsesAlongEdgeConnectPoints())
    {
        pointStruct.count = 0;
    }
    else
    {
        const int count = qMax(0, defaultWiringConnectPointCount());
        pointStruct.count = static_cast<ConnectCount>(count);
        pointStruct.points.reserve(count);
        for (int i = 0; i < count; ++i)
        {
            LzConnectPoint* point = new LzConnectPoint(this);
            point->setDir(i);
            configureDefaultWiringConnectPoint(point, i);
            pointStruct.points.push_back(point);
        }
    }
    updateCoordinate();
    if (wiringUsesCustomConnectPoints())
        refreshWiringCustomConnectPoints();
    updatePoints();
}

void LzWiringItemBase::finalizeWiringConstruction()
{
    reconcileWiringConnectPoints();
}

void LzWiringItemBase::refreshWiringCustomConnectPoints()
{
}

LzConnectPoint* LzWiringItemBase::ensureConnectPointAtScene(const QPointF& scenePos)
{
    Q_UNUSED(scenePos);
    return nullptr;
}

void LzWiringItemBase::rebindAlongEdgeConnectPorts()
{
}

void LzWiringItemBase::rebindWiringConnectPorts()
{
    if (wiringUsesCustomConnectPoints())
    {
        refreshWiringCustomConnectPoints();
        updatePoints();
        for (LzConnectPoint* p : pointStruct.points)
        {
            if (p)
                p->update();
        }
        return;
    }

    if (wiringUsesAlongEdgeConnectPoints())
    {
        rebindAlongEdgeConnectPorts();
        updatePoints();
        for (LzConnectPoint* p : pointStruct.points)
        {
            if (p)
                p->update();
        }
        return;
    }

    for (int i = 0; i < pointStruct.points.size(); ++i)
    {
        if (LzConnectPoint* p = pointStruct.points[i])
            configureDefaultWiringConnectPoint(p, i);
    }
    updatePoints();
    for (LzConnectPoint* p : pointStruct.points)
    {
        if (p)
            p->update();
    }
}

void LzWiringItemBase::reconcileWiringConnectPoints()
{
    if (wiringUsesCustomConnectPoints())
    {
        refreshWiringCustomConnectPoints();
        updatePoints();
        return;
    }

    if (wiringUsesAlongEdgeConnectPoints())
    {
        rebindAlongEdgeConnectPorts();
        updatePoints();
        return;
    }

    const int want = defaultWiringConnectPointCount();
    if (want <= 0)
    {
        purgeConnectPointChildren(this, pointStruct.points);
        pointStruct.count = 0;
        updatePoints();
        return;
    }

    int childPortCount = 0;
    for (QGraphicsItem* ch : childItems())
    {
        if (qgraphicsitem_cast<LzConnectPoint*>(ch))
            ++childPortCount;
    }

    if (static_cast<int>(pointStruct.points.size()) != want || childPortCount != want)
    {
        purgeConnectPointChildren(this, pointStruct.points);
        pointStruct.count = static_cast<ConnectCount>(want);
        pointStruct.points.reserve(want);
        for (int i = 0; i < want; ++i)
        {
            auto* point = new LzConnectPoint(this);
            point->setDir(i);
            configureDefaultWiringConnectPoint(point, i);
            pointStruct.points.push_back(point);
        }
        updatePoints();
        return;
    }

    rebindWiringConnectPorts();
}
LzWiringItemBase::LzWiringItemBase(LzWiringItemKind kind, QGraphicsItem* parent)
    : LzItem(parent)
    , kind_(kind)
{
}

LzWiringItemBase::LzWiringItemBase(LzWiringItemKind kind, const QRectF& pos, QGraphicsItem* parent)
    : LzItem(parent)
    , kind_(kind)
{
    installConnectPointsFromRect(pos);
}

LzWiringItemBase::~LzWiringItemBase()
{
    if (wiringVisualTimer_)
        wiringVisualTimer_->stop();
    if (auto* sc = qobject_cast<LzScene*>(scene()))
        disconnectSceneWiringSignals(sc);
}

void LzWiringItemBase::setWiringDataKey(const QString& k)
{
    if (wiringDataKey_ == k)
        return;
    wiringDataKey_ = k;
    emit wiringDataKeyChanged(wiringDataKey_);
    if (auto* sc = qobject_cast<LzScene*>(scene()))
        applyWiringPayload(sc->wiringData(wiringDataKey_));
    update();
    updateWiringVisualTimer();
}

void LzWiringItemBase::setReferenceDesignator(const QString& text)
{
    if (referenceDesignator_ == text)
        return;
    referenceDesignator_ = text;
    emit referenceDesignatorChanged(referenceDesignator_);
    update();
}

QString LzWiringItemBase::displayName() const
{
    if (!displayName_.isEmpty())
        return displayName_;
    return TmpBase<QGraphicsItem>::displayName();
}

void LzWiringItemBase::setDisplayName(const QString& text)
{
    if (displayName_ == text)
        return;
    displayName_ = text;
    emit displayNameChanged(displayName_);
    update();
}

void LzWiringItemBase::setValueText(const QString& text)
{
    if (valueText_ == text)
        return;
    valueText_ = text;
    emit valueTextChanged(valueText_);
    update();
}

void LzWiringItemBase::setModelName(const QString& text)
{
    if (modelName_ == text)
        return;
    modelName_ = text;
    emit modelNameChanged(modelName_);
    update();
}

void LzWiringItemBase::setWiringRunState(LzWiringRunState s)
{
    if (runState_ == s)
        return;
    runState_ = s;
    emit wiringRunStateChanged(static_cast<int>(runState_));
    update();
    updateWiringVisualTimer();
}

int LzWiringItemBase::wiringRunStateInt() const
{
    return static_cast<int>(runState_);
}

void LzWiringItemBase::setWiringRunStateInt(int v)
{
    setWiringRunState(clampRunState(v));
}

void LzWiringItemBase::setFlowSign(int sign)
{
    const int ns = (sign < 0) ? -1 : 1;
    if (flowSign_ == ns)
        return;
    flowSign_ = ns;
    emit flowSignChanged(flowSign_);
    if (flowSignAffectsConnectPoints())
        reconcileWiringConnectPoints();
    update();
    updateWiringVisualTimer();
    if (auto* sc = qobject_cast<LzScene*>(scene()))
        sc->scheduleTopologyRebuild();
}

int LzWiringItemBase::switchPosition() const
{
    return switchContactsClosed_ ? 1 : 0;
}

void LzWiringItemBase::setSwitchPosition(int v)
{
    v = qBound(0, v, 1);
    const bool closed = (v == 1);
    if (switchContactsClosed_ == closed)
        return;
    switchContactsClosed_ = closed;
    emit switchPositionChanged(v);
    update();
    if (auto* sc = qobject_cast<LzScene*>(scene()))
        sc->scheduleTopologyRebuild();
}

bool LzWiringItemBase::breakerVisualOpen() const
{
    return runState_ == LzWiringRunState::Deenergized || !switchContactsClosed_;
}

void LzWiringItemBase::copyWiringMetadataFrom(const LzWiringItemBase* o)
{
    if (!o)
        return;
    referenceDesignator_ = o->referenceDesignator_;
    displayName_ = o->displayName_;
    valueText_ = o->valueText_;
    modelName_ = o->modelName_;
}

void LzWiringItemBase::copyWiringRuntimeFrom(const LzWiringItemBase* o)
{
    if (!o)
        return;
    wiringDataKey_ = o->wiringDataKey_;
    runState_ = o->runState_;
    flowSign_ = o->flowSign_;
    switchContactsClosed_ = o->switchContactsClosed_;
}

void LzWiringItemBase::copyCustomDataFrom(const LzWiringItemBase* o)
{
    Q_UNUSED(o);
}

void LzWiringItemBase::saveCustomXml(QDomElement* g) const
{
    Q_UNUSED(g);
}

void LzWiringItemBase::loadCustomXml(QDomElement* g)
{
    Q_UNUSED(g);
}

void LzWiringItemBase::connectSceneWiringSignals(LzScene* sc)
{
    if (!sc)
        return;
    QObject::connect(sc, &LzScene::wiringDataChanged, this, &LzWiringItemBase::onSceneWiringDataChanged, Qt::UniqueConnection);
    if (!wiringDataKey_.isEmpty())
        applyWiringPayload(sc->wiringData(wiringDataKey_));
}

void LzWiringItemBase::disconnectSceneWiringSignals(LzScene* sc)
{
    if (!sc)
        return;
    QObject::disconnect(sc, &LzScene::wiringDataChanged, this, &LzWiringItemBase::onSceneWiringDataChanged);
}

void LzWiringItemBase::onSceneWiringDataChanged(const QString& key, const QVariant& value)
{
    if (wiringDataKey_.isEmpty() || key != wiringDataKey_)
        return;
    applyWiringPayload(value);
    updateWiringVisualTimer();
    if (!scene())
        return;
    QTimer::singleShot(0, this, [this]() {
        if (scene())
            update();
    });
}

void LzWiringItemBase::onWiringVisualTick()
{
    if (!scene() || !isVisible())
        return;
    update();
}

void LzWiringItemBase::applyWiringPayload(const QVariant& value)
{
    if (value.isNull() || !value.isValid())
        return;
    if (value.canConvert<int>())
    {
        runState_ = clampRunState(value.toInt());
        emit wiringRunStateChanged(static_cast<int>(runState_));
        updateWiringVisualTimer();
        return;
    }
    const QString s = value.toString().trimmed();
    if (s == QLatin1String("energized") || s == QStringLiteral("带电"))
        runState_ = LzWiringRunState::Energized;
    else if (s == QLatin1String("off") || s == QStringLiteral("停运") || s == QStringLiteral("分闸"))
        runState_ = LzWiringRunState::Deenergized;
    else if (s == QLatin1String("alarm") || s == QStringLiteral("告警"))
        runState_ = LzWiringRunState::Alarm;
    else if (s == QLatin1String("normal") || s == QStringLiteral("正常"))
        runState_ = LzWiringRunState::Normal;
    emit wiringRunStateChanged(static_cast<int>(runState_));
    updateWiringVisualTimer();
}

void LzWiringItemBase::updateWiringVisualTimer()
{
    const bool need = scene() != nullptr
        && (wiringUsesVisualTimer() || runState_ == LzWiringRunState::Alarm);
    if (need)
    {
        if (!wiringVisualTimer_)
        {
            wiringVisualTimer_ = new QTimer(this);
            wiringVisualTimer_->setTimerType(Qt::CoarseTimer);
            QObject::connect(wiringVisualTimer_, &QTimer::timeout, this, &LzWiringItemBase::onWiringVisualTick,
                             Qt::QueuedConnection);
        }
        if (!wiringVisualTimer_->isActive())
            wiringVisualTimer_->start(120);
    }
    else if (wiringVisualTimer_)
    {
        wiringVisualTimer_->stop();
    }
}

void LzWiringItemBase::setState(HANDLE_STATE st)
{
    LzItem::setState(st);
    if (st != HANDLE_ACTIVE || wiringUsesAlongEdgeConnectPoints())
        return;

    QSet<int> hideEdgeHandles;
    for (LzConnectPoint* port : pointStruct.points)
    {
        const int handleDir = overlappingEdgeHandleForPort(port, itemPosition);
        if (handleDir != toInt(HANDLE_POINT::HANDLE_NONE))
            hideEdgeHandles.insert(handleDir);
    }
    for (LzHandle* handle : handleStruct.handles)
    {
        if (handle && hideEdgeHandles.contains(handle->dir()))
            handle->hide();
    }
}

QVariant LzWiringItemBase::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant& value)
{
    if (change == QGraphicsItem::ItemSceneChange)
    {
        if (auto* os = qobject_cast<LzScene*>(scene()))
            disconnectSceneWiringSignals(os);
        if (auto* ns = qobject_cast<LzScene*>(value.value<QGraphicsScene*>()))
            connectSceneWiringSignals(ns);
    }
    else if (change == QGraphicsItem::ItemSceneHasChanged)
    {
        updateWiringVisualTimer();
        if (scene())
            reconcileWiringConnectPoints();
    }
    return LzItem::itemChange(change, value);
}

void LzWiringItemBase::contextMenuEvent(QGraphicsSceneContextMenuEvent* event)
{
    event->accept();
    QMenu menu;
    QAction* aKey = menu.addAction(tr("设置数据绑定键…"));
    QAction* aNorm = menu.addAction(tr("状态：正常"));
    QAction* aOn = menu.addAction(tr("状态：带电"));
    QAction* aOff = menu.addAction(tr("状态：停运"));
    QAction* aAl = menu.addAction(tr("状态：告警"));
    QAction* aFlow = nullptr;
    if (supportsFlowDirectionAction())
        aFlow = menu.addAction(tr("反转进线箭头方向"));
    QAction* picked = menu.exec(event->screenPos());
    if (!picked)
        return;
    if (picked == aKey)
    {
        bool ok = false;
        const QString k = QInputDialog::getText(nullptr, tr("数据键"), tr("与 LzScene::setWiringData 使用的键一致："), QLineEdit::Normal, wiringDataKey_, &ok);
        if (ok)
            setWiringDataKey(k.trimmed());
        return;
    }
    if (picked == aNorm)
        setWiringRunState(LzWiringRunState::Normal);
    else if (picked == aOn)
        setWiringRunState(LzWiringRunState::Energized);
    else if (picked == aOff)
        setWiringRunState(LzWiringRunState::Deenergized);
    else if (picked == aAl)
        setWiringRunState(LzWiringRunState::Alarm);
    else if (aFlow && picked == aFlow)
        setFlowSign(-flowSign_);
    if (!wiringDataKey_.isEmpty())
    {
        if (auto* sc = qobject_cast<LzScene*>(scene()))
            sc->setWiringData(wiringDataKey_, static_cast<int>(runState_));
    }
}

void LzWiringItemBase::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
    if (supportsSwitchToggleOnDoubleClick())
    {
        setSwitchPosition(switchPosition() == 0 ? 1 : 0);
        event->accept();
        return;
    }
    LzItem::mouseDoubleClickEvent(event);
}

QPainterPath LzWiringItemBase::shape() const
{
    return defaultRoutingObstaclePath();
}

int LzWiringItemBase::defaultWiringConnectPointCount() const
{
    return 2;
}

void LzWiringItemBase::configureDefaultWiringConnectPoint(LzConnectPoint* point, int index) const
{
    if (!point)
        return;

    const qreal y = 0.0;
    if (index <= 0)
    {
        point->setDirection(LEFT_DIRECTION);
        point->setConnectionLeadOut(0.0);
        point->bindToItem(const_cast<LzWiringItemBase*>(this), QPointF(-0.5, y));
        return;
    }

    point->setDirection(RIGHT_DIRECTION);
    point->setConnectionLeadOut(0.0);
    point->bindToItem(const_cast<LzWiringItemBase*>(this), QPointF(0.5, y));
}

QPainterPath LzWiringItemBase::defaultRoutingObstaclePath() const
{
    QPainterPath path;
    path.addRect(itemPosition.normalized());
    return path;
}

QRectF LzWiringItemBase::boundingRect() const
{
    return itemPosition;
}

LzWiringSymbolStandard LzWiringItemBase::wiringSymbolStandard() const
{
    return LzWiringSymbolStyle::instance().standard();
}

void LzWiringItemBase::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    prepare(painter);
}

void LzWiringItemBase::updateCoordinate()
{
    QPointF pt1, pt2, delta;
    pt1 = mapToScene(transformOriginPoint());
    pt2 = mapToScene(itemPosition.center());
    delta = pt1 - pt2;
    prepareGeometryChange();
    if (!parentItem())
    {
        itemPosition = QRectF(-itemWidth / 2, -itemHeight / 2, itemWidth, itemHeight);
        itemWidth = itemPosition.width();
        itemHeight = itemPosition.height();
        setTransform(transform().translate(delta.x(), delta.y()));
        setTransformOriginPoint(itemPosition.center());
        moveBy(-delta.x(), -delta.y());
        setTransform(transform().translate(-delta.x(), -delta.y()));
        updateHandles();
        updatePoints();
    }
    itemInitialRect = itemPosition;
}

bool LzWiringItemBase::saveXml(QDomElement* g)
{
    g->setAttribute(QStringLiteral("shape"), static_cast<int>(drawTypeForXml()));
    g->setAttribute(QStringLiteral("referenceDesignator"), referenceDesignator_);
    g->setAttribute(QStringLiteral("displayName"), displayName_);
    g->setAttribute(QStringLiteral("valueText"), valueText_);
    g->setAttribute(QStringLiteral("modelName"), modelName_);
    g->setAttribute(QStringLiteral("wiringDataKey"), wiringDataKey_);
    g->setAttribute(QStringLiteral("wiringRunState"), static_cast<int>(runState_));
    g->setAttribute(QStringLiteral("wiringFlowSign"), flowSign_);
    g->setAttribute(QStringLiteral("wiringSwitchClosed"), switchContactsClosed_ ? 1 : 0);
    saveCustomXml(g);
    return LzItem::saveXml(g);
}

bool LzWiringItemBase::loadXml(QDomElement* g)
{
    if (!LzItem::loadXml(g))
        return false;
    // XML stores the item's scene position in the same coordinates returned by
    // saveXml(), where wiring items are already centered around their local
    // origin. Re-running updateCoordinate() here would center them again and
    // shift the item away from the saved connect-line anchors on load.
    itemPosition = QRectF(-itemWidth / 2, -itemHeight / 2, itemWidth, itemHeight);
    itemInitialRect = itemPosition;
    setTransformOriginPoint(itemPosition.center());
    updateHandles();
    updatePoints();
    referenceDesignator_ = g->attribute(QStringLiteral("referenceDesignator"));
    displayName_ = g->attribute(QStringLiteral("displayName"));
    valueText_ = g->attribute(QStringLiteral("valueText"));
    modelName_ = g->attribute(QStringLiteral("modelName"));
    wiringDataKey_ = g->attribute(QStringLiteral("wiringDataKey"));
    runState_ = clampRunState(g->attribute(QStringLiteral("wiringRunState"), QStringLiteral("0")).toInt());
    flowSign_ = g->attribute(QStringLiteral("wiringFlowSign"), QStringLiteral("1")).toInt();
    if (flowSign_ == 0)
        flowSign_ = 1;
    switchContactsClosed_ = g->attribute(QStringLiteral("wiringSwitchClosed"), QStringLiteral("1")).toInt() != 0;
    loadCustomXml(g);
    reconcileWiringConnectPoints();
    if (auto* sc = qobject_cast<LzScene*>(scene()))
        applyWiringPayload(sc->wiringData(wiringDataKey_));
    return true;
}

void LzWiringItemBase::stretch(int handle, double sx, double sy, const QPointF& origin)
{
    QTransform trans;
    switch (handle)
    {
    case RIGHT:
    case LEFT:
        sy = 1;
        break;
    case TOP:
    case BOTTOM:
        sx = 1;
        break;
    default:
        break;
    }

    trans.translate(origin.x(), origin.y());
    trans.scale(sx, sy);
    trans.translate(-origin.x(), -origin.y());

    prepareGeometryChange();
    itemPosition = trans.mapRect(itemInitialRect);
    itemWidth = itemPosition.width();
    itemHeight = itemPosition.height();
    updateHandles();
    rebindWiringConnectPorts();
    refreshConnectedLines();
    update();
}

bool LzWiringItemBase::isTopologyGraphNode() const
{
    return topologyGraphNodeEnabled();
}

bool LzWiringItemBase::participatesInTopology() const
{
    return topologyDomain() != LzTopologyDomain::None;
}

LzTopologyDomain LzWiringItemBase::topologyDomain() const
{
    switch (kind_)
    {
    case LzWiringItemKind::BusbarSection:
    case LzWiringItemKind::CircuitBreaker:
    case LzWiringItemKind::Disconnector:
    case LzWiringItemKind::Transformer2W:
    case LzWiringItemKind::Transformer3W:
    case LzWiringItemKind::PowerFeeder:
    case LzWiringItemKind::LoadBreak:
    case LzWiringItemKind::ArcCoil:
    case LzWiringItemKind::Arrester:
    case LzWiringItemKind::AutoXfmr:
    case LzWiringItemKind::Cable:
    case LzWiringItemKind::EarthSwitch:
    case LzWiringItemKind::EarthingXfmr:
    case LzWiringItemKind::Ess:
    case LzWiringItemKind::Generator:
    case LzWiringItemKind::Grid:
    case LzWiringItemKind::Motor:
    case LzWiringItemKind::Load:
    case LzWiringItemKind::PvInfeed:
    case LzWiringItemKind::Reactor:
    case LzWiringItemKind::SplitReactor:
    case LzWiringItemKind::StationXfmr:
    case LzWiringItemKind::SvgComp:
    case LzWiringItemKind::WindInfeed:
    case LzWiringItemKind::CurrentTransformer:
    case LzWiringItemKind::VoltageTransformer:
    case LzWiringItemKind::Fuse:
    case LzWiringItemKind::Junction:
        return LzTopologyDomain::PowerSystem;
    default:
        return LzTopologyDomain::None;
    }
}

PowerTopologyRole LzWiringItemBase::powerTopologyRole() const
{
    switch (kind_)
    {
    case LzWiringItemKind::BusbarSection:
        return PowerTopologyRole::Busbar;
    case LzWiringItemKind::CircuitBreaker:
        return PowerTopologyRole::Breaker;
    case LzWiringItemKind::Disconnector:
    case LzWiringItemKind::LoadBreak:
        return PowerTopologyRole::Disconnector;
    case LzWiringItemKind::EarthSwitch:
        return PowerTopologyRole::GroundSwitch;
    case LzWiringItemKind::Transformer2W:
    case LzWiringItemKind::Transformer3W:
    case LzWiringItemKind::StationXfmr:
    case LzWiringItemKind::EarthingXfmr:
    case LzWiringItemKind::AutoXfmr:
        return PowerTopologyRole::Transformer;
    case LzWiringItemKind::ArcCoil:
    case LzWiringItemKind::Reactor:
    case LzWiringItemKind::SplitReactor:
        return PowerTopologyRole::Reactor;
    case LzWiringItemKind::Capacitor:
    case LzWiringItemKind::SvgComp:
        return PowerTopologyRole::Compensation;
    case LzWiringItemKind::Arrester:
        return PowerTopologyRole::SurgeArrester;
    case LzWiringItemKind::PowerFeeder:
    case LzWiringItemKind::Generator:
    case LzWiringItemKind::Load:
    case LzWiringItemKind::Grid:
    case LzWiringItemKind::PvInfeed:
    case LzWiringItemKind::Ess:
    case LzWiringItemKind::WindInfeed:
        return PowerTopologyRole::Feeder;
    case LzWiringItemKind::Motor:
    case LzWiringItemKind::CurrentTransformer:
    case LzWiringItemKind::VoltageTransformer:
    case LzWiringItemKind::Fuse:
        return PowerTopologyRole::GenericEquipment;
    case LzWiringItemKind::Junction:
    case LzWiringItemKind::Cable:
        return PowerTopologyRole::Conductor;
    default:
        return PowerTopologyRole::GenericEquipment;
    }
}

QHash<QString, bool> LzWiringItemBase::powerTopologyInternalConnectivity(
    const QVector<QString>& terminalIds,
    int effectiveSwitchPosition) const
{
    if (terminalIds.size() < 2)
        return {};

    switch (powerTopologyRole())
    {
    case PowerTopologyRole::Breaker:
    case PowerTopologyRole::Disconnector:
    case PowerTopologyRole::GroundSwitch:
        return effectiveSwitchPosition == 1 ? adjacentTerminalPairs(terminalIds) : QHash<QString, bool>{};
    case PowerTopologyRole::Measurement:
    case PowerTopologyRole::SurgeArrester:
        return {};
    case PowerTopologyRole::Transformer:
    case PowerTopologyRole::Reactor:
    case PowerTopologyRole::Compensation:
    case PowerTopologyRole::Feeder:
    case PowerTopologyRole::GenericEquipment:
        return adjacentTerminalPairs(terminalIds);
    case PowerTopologyRole::Busbar:
    case PowerTopologyRole::Ground:
    case PowerTopologyRole::Conductor:
        return fullyConnectedTerminalPairs(terminalIds);
    }

    return {};
}

QGraphicsItem* LzWiringItemBase::duplicate()
{
    LzWiringItemBase* item = cloneForDuplicate();
    if (!item)
        return nullptr;
    item->itemWidth = getWidth();
    item->itemHeight = getHeight();
    // 复制分组内图元时尚无 parent；此时 updateCoordinate() 会走无父分支并重置
    // itemPosition/transform，addToGroup 后连接点与符号错位。直接拷贝几何状态。
    item->itemPosition = itemPosition;
    item->itemInitialRect = itemInitialRect;
    item->setPos(pos().x(), pos().y());
    item->setTransform(transform());
    item->setTransformOriginPoint(transformOriginPoint());
    item->setRotation(rotation());
    item->setScale(scale());
    item->setZValue(zValue());
    item->copyWiringMetadataFrom(this);
    item->copyWiringRuntimeFrom(this);
    item->copyCustomDataFrom(this);
    item->reconcileWiringConnectPoints();
    item->updateHandles();
    item->updatePoints();
    return item;
}

void LzWiringItemBase::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsItem::mouseMoveEvent(event);
}
