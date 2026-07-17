#include "CimdrawWiringItemBase.h"
#include "CimdrawWiringDrawPalette.h"
#include "CimdrawWiringSymbolStyle.h"
#include "CimdrawScene.h"
#include "Item/CimdrawConnectPoint.h"
#include "Item/CimdrawHandle.h"
#include "CimdrawConnectConfig.h"
#include "CimdrawItemConfig.h"
#include "cim/behavior/CimBehaviorResultVariant.h"
#include "cim/query/CimGraphicRenderStateQuery.h"

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

bool hasConductivePathForBehavior(const CimdrawWiringItemBase* wiring,
                                  int effectiveSwitchPosition)
{
    if (!wiring || !wiring->participatesInTopology())
        return false;

    switch (wiring->powerTopologyRole())
    {
    case PowerTopologyRole::Breaker:
    case PowerTopologyRole::Disconnector:
    case PowerTopologyRole::GroundSwitch:
        return effectiveSwitchPosition != 0;
    case PowerTopologyRole::Measurement:
    case PowerTopologyRole::SurgeArrester:
        return false;
    case PowerTopologyRole::Busbar:
    case PowerTopologyRole::Ground:
    case PowerTopologyRole::Transformer:
    case PowerTopologyRole::Reactor:
    case PowerTopologyRole::Compensation:
    case PowerTopologyRole::Feeder:
    case PowerTopologyRole::Conductor:
    case PowerTopologyRole::GenericEquipment:
    default:
        return true;
    }
}

CimBehaviorTerminalConnectivity terminalConnectivityForBehavior(const CimdrawWiringItemBase* wiring,
                                                                bool conductivity)
{
    if (!wiring || !wiring->participatesInTopology())
        return CimBehaviorTerminalConnectivity::Unknown;
    if (conductivity)
        return CimBehaviorTerminalConnectivity::Connected;
    if (wiring->supportsSwitchToggleOnDoubleClick())
        return CimBehaviorTerminalConnectivity::Disconnected;
    return CimBehaviorTerminalConnectivity::Unknown;
}


void purgeConnectPointChildren(QGraphicsItem* item, QVector<CimdrawConnectPoint*>& tracked)
{
    for (QGraphicsItem* ch : item->childItems())
    {
        if (auto* cp = qgraphicsitem_cast<CimdrawConnectPoint*>(ch))
        {
            tracked.removeAll(cp);
            delete cp;
        }
    }
    for (CimdrawConnectPoint* p : tracked)
        delete p;
    tracked.clear();
}

CimdrawWiringRunState clampRunState(int v)
{
    if (v < 0 || v > 3)
        return CimdrawWiringRunState::Normal;
    return static_cast<CimdrawWiringRunState>(v);
}

int overlappingEdgeHandleForPort(const CimdrawConnectPoint* port, const QRectF& itemRect)
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

void CimdrawWiringItemBase::installConnectPointsFromRect(const QRectF& pos)
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
            CimdrawConnectPoint* point = new CimdrawConnectPoint(this);
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

void CimdrawWiringItemBase::finalizeWiringConstruction()
{
    reconcileWiringConnectPoints();
}

void CimdrawWiringItemBase::refreshWiringCustomConnectPoints()
{
}

CimdrawConnectPoint* CimdrawWiringItemBase::ensureConnectPointAtScene(const QPointF& scenePos)
{
    Q_UNUSED(scenePos);
    return nullptr;
}

void CimdrawWiringItemBase::rebindAlongEdgeConnectPorts()
{
}

void CimdrawWiringItemBase::rebindWiringConnectPorts()
{
    if (wiringUsesCustomConnectPoints())
    {
        refreshWiringCustomConnectPoints();
        updatePoints();
        for (CimdrawConnectPoint* p : pointStruct.points)
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
        for (CimdrawConnectPoint* p : pointStruct.points)
        {
            if (p)
                p->update();
        }
        return;
    }

    for (int i = 0; i < pointStruct.points.size(); ++i)
    {
        if (CimdrawConnectPoint* p = pointStruct.points[i])
            configureDefaultWiringConnectPoint(p, i);
    }
    updatePoints();
    for (CimdrawConnectPoint* p : pointStruct.points)
    {
        if (p)
            p->update();
    }
}

void CimdrawWiringItemBase::reconcileWiringConnectPoints()
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
        if (qgraphicsitem_cast<CimdrawConnectPoint*>(ch))
            ++childPortCount;
    }

    if (static_cast<int>(pointStruct.points.size()) != want || childPortCount != want)
    {
        purgeConnectPointChildren(this, pointStruct.points);
        pointStruct.count = static_cast<ConnectCount>(want);
        pointStruct.points.reserve(want);
        for (int i = 0; i < want; ++i)
        {
            auto* point = new CimdrawConnectPoint(this);
            point->setDir(i);
            configureDefaultWiringConnectPoint(point, i);
            pointStruct.points.push_back(point);
        }
        updatePoints();
        return;
    }

    rebindWiringConnectPorts();
}
CimdrawWiringItemBase::CimdrawWiringItemBase(CimdrawWiringItemKind kind, QGraphicsItem* parent)
    : CimdrawItem(parent)
    , kind_(kind)
{
}

CimdrawWiringItemBase::CimdrawWiringItemBase(CimdrawWiringItemKind kind, const QRectF& pos, QGraphicsItem* parent)
    : CimdrawItem(parent)
    , kind_(kind)
{
    installConnectPointsFromRect(pos);
}

CimdrawWiringItemBase::~CimdrawWiringItemBase()
{
    if (wiringVisualTimer_)
        wiringVisualTimer_->stop();
    if (auto* sc = qobject_cast<CimdrawScene*>(scene()))
        disconnectSceneWiringSignals(sc);
}

void CimdrawWiringItemBase::setWiringDataKey(const QString& k)
{
    if (wiringDataKey_ == k)
        return;
    wiringDataKey_ = k;
    emit wiringDataKeyChanged(wiringDataKey_);
    if (auto* sc = qobject_cast<CimdrawScene*>(scene()))
        applyWiringPayload(sc->wiringData(wiringDataKey_));
    update();
    updateWiringVisualTimer();
}

void CimdrawWiringItemBase::setReferenceDesignator(const QString& text)
{
    if (referenceDesignator_ == text)
        return;
    referenceDesignator_ = text;
    emit referenceDesignatorChanged(referenceDesignator_);
    update();
}

QString CimdrawWiringItemBase::displayName() const
{
    if (!displayName_.isEmpty())
        return displayName_;
    return TmpBase<QGraphicsItem>::displayName();
}

void CimdrawWiringItemBase::setDisplayName(const QString& text)
{
    if (displayName_ == text)
        return;
    displayName_ = text;
    emit displayNameChanged(displayName_);
    update();
}

void CimdrawWiringItemBase::setValueText(const QString& text)
{
    if (valueText_ == text)
        return;
    valueText_ = text;
    emit valueTextChanged(valueText_);
    update();
}

void CimdrawWiringItemBase::setModelName(const QString& text)
{
    if (modelName_ == text)
        return;
    modelName_ = text;
    emit modelNameChanged(modelName_);
    update();
}

void CimdrawWiringItemBase::setWiringRunState(CimdrawWiringRunState s)
{
    if (runState_ == s)
        return;
    if (!applyingBehaviorResult_)
        behaviorResult_ = {};
    runState_ = s;
    emit wiringRunStateChanged(static_cast<int>(runState_));
    update();
    updateWiringVisualTimer();
    if (!applyingBehaviorResult_)
        publishCurrentBehaviorResultToScene();
}

void CimdrawWiringItemBase::applyBehaviorResult(const CimBehaviorResult& result)
{
    behaviorResult_ = result;
    if (!result.valid)
        return;

    CimGraphicRenderStateQuery query;
    const CimGraphicRenderStateSource source = graphicRenderStateSource();
    const CimGraphicRenderState renderState = query.resultForSource(source);

    applyingBehaviorResult_ = true;
    if (source.behaviorDrivesSwitchPosition)
        setSwitchPosition(renderState.switchPosition);
    setWiringRunState(renderState.runState);
    applyingBehaviorResult_ = false;
}

int CimdrawWiringItemBase::wiringRunStateInt() const
{
    return static_cast<int>(runState_);
}

CimBehaviorResult CimdrawWiringItemBase::effectiveBehaviorResult() const
{
    CimBehaviorResult result = currentBehaviorResult();
    if (result.valid)
    {
        if (result.objectId.isEmpty())
            result.objectId = cimdrawObjectId();
        if (result.runState == CimBehaviorRunState::Unknown)
        {
            CimGraphicRenderStateQuery query;
            result.runState =
                cimBehaviorRunStateFromGraphicRunState(query.resultForSource(graphicRenderStateSource()).runState);
        }
        return result;
    }

    result.objectId = cimdrawObjectId();
    result.valid = true;
    result.available = isVisible() && participatesInTopology();
    result.canOperate = supportsSwitchToggleOnDoubleClick();
    CimGraphicRenderStateQuery query;
    const CimGraphicRenderState renderState = query.resultForSource(graphicRenderStateSource());
    result.runState = cimBehaviorRunStateFromGraphicRunState(renderState.runState);
    result.energized = cimGraphicRenderStateEnergized(renderState);
    result.conductivity = hasConductivePathForBehavior(this, renderState.switchPosition);
    result.terminalConnectivity = terminalConnectivityForBehavior(this, result.conductivity);
    return result;
}

CimdrawWiringRunState CimdrawWiringItemBase::effectiveWiringRunState() const
{
    CimGraphicRenderStateQuery query;
    return query.resultForSource(graphicRenderStateSource()).runState;
}

bool CimdrawWiringItemBase::effectiveAlarmPulse() const
{
    CimGraphicRenderStateQuery query;
    const CimGraphicRenderState state = query.resultForSource(graphicRenderStateSource());
    return resolveGraphicAlarmPulse(state.runState);
}

int CimdrawWiringItemBase::effectiveFlowSign() const
{
    CimGraphicRenderStateQuery query;
    return query.resultForSource(graphicRenderStateSource()).flowSign;
}

CimdrawWiringVisualState CimdrawWiringItemBase::graphicRenderState() const
{
    CimGraphicRenderStateQuery query;
    const CimGraphicRenderStateSource source = graphicRenderStateSource();
    const CimGraphicRenderState state = query.resultForSource(source);
    return query.resultForSource(source, resolveGraphicAlarmPulse(state.runState));
}

CimGraphicRenderStateSource CimdrawWiringItemBase::graphicRenderStateSource() const
{
    CimGraphicRenderStateSource source;
    source.valid = true;
    source.runtimeRunState = runState_;
    source.manualSwitchPosition = switchPosition();
    source.flowSign = flowSign_;
    source.behaviorResult = currentBehaviorResult();
    source.behaviorDrivesSwitchPosition = supportsSwitchToggleOnDoubleClick() || source.behaviorResult.canOperate;
    return source;
}

bool CimdrawWiringItemBase::resolveGraphicAlarmPulse(CimdrawWiringRunState runState) const
{
    return runState == CimdrawWiringRunState::Alarm
        && ((QDateTime::currentMSecsSinceEpoch() / 400) % 2 == 0);
}

CimdrawWiringVisualState CimdrawWiringItemBase::graphicRenderStateForPaint() const
{
    return graphicRenderState();
}

CimdrawWiringVisualState CimdrawWiringItemBase::prepareGraphicRenderStateForPaint(QPainter* painter)
{
    prepare(painter);
    return graphicRenderStateForPaint();
}

void CimdrawWiringItemBase::setWiringRunStateInt(int v)
{
    setWiringRunState(clampRunState(v));
}

void CimdrawWiringItemBase::setFlowSign(int sign)
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
    if (auto* sc = qobject_cast<CimdrawScene*>(scene()))
        sc->scheduleTopologyRebuild();
}

int CimdrawWiringItemBase::switchPosition() const
{
    return switchContactsClosed_ ? 1 : 0;
}

int CimdrawWiringItemBase::effectiveSwitchPosition() const
{
    CimGraphicRenderStateQuery query;
    return query.resultForSource(graphicRenderStateSource()).switchPosition;
}

void CimdrawWiringItemBase::setSwitchPosition(int v)
{
    v = qBound(0, v, 1);
    const bool closed = (v == 1);
    if (switchContactsClosed_ == closed)
        return;
    if (!applyingBehaviorResult_)
        behaviorResult_ = {};
    switchContactsClosed_ = closed;
    emit switchPositionChanged(v);
    update();
    if (auto* sc = qobject_cast<CimdrawScene*>(scene()))
        sc->scheduleTopologyRebuild();
    if (!applyingBehaviorResult_)
        publishCurrentBehaviorResultToScene();
}

bool CimdrawWiringItemBase::breakerVisualOpen() const
{
    CimGraphicRenderStateQuery query;
    return cimGraphicRenderStateSwitchOpen(
        query.resultForSource(graphicRenderStateSource()),
        true);
}

bool CimdrawWiringItemBase::publishCurrentBehaviorResultToScene()
{
    auto* sc = qobject_cast<CimdrawScene*>(scene());
    if (!sc || wiringDataKey_.isEmpty())
        return false;

    // P1 / PowerSystemModel -> Behavior Projection / Graphic Projection:
    // user-driven graphic refreshes publish a typed behavior payload instead of
    // writing bare runtime state back into the scene data channel.
    const CimBehaviorResult result = effectiveBehaviorResult();
    const QVariantMap payload = cimBehaviorResultToVariantMap(result);
    if (sc->wiringData(wiringDataKey_).toMap() == payload)
        return true;
    sc->setWiringBehaviorResult(wiringDataKey_, result);
    return true;
}

bool CimdrawWiringItemBase::toggleSwitchPositionFromInteraction()
{
    if (!supportsSwitchToggleOnDoubleClick())
        return false;
    setSwitchPosition(switchPosition() == 0 ? 1 : 0);
    publishCurrentBehaviorResultToScene();
    return true;
}

void CimdrawWiringItemBase::copyWiringMetadataFrom(const CimdrawWiringItemBase* o)
{
    if (!o)
        return;
    referenceDesignator_ = o->referenceDesignator_;
    displayName_ = o->displayName_;
    valueText_ = o->valueText_;
    modelName_ = o->modelName_;
}

void CimdrawWiringItemBase::copyWiringRuntimeFrom(const CimdrawWiringItemBase* o)
{
    if (!o)
        return;
    wiringDataKey_ = o->wiringDataKey_;
    runState_ = o->runState_;
    flowSign_ = o->flowSign_;
    switchContactsClosed_ = o->switchContactsClosed_;
}

void CimdrawWiringItemBase::copyCustomDataFrom(const CimdrawWiringItemBase* o)
{
    Q_UNUSED(o);
}

void CimdrawWiringItemBase::saveCustomXml(QDomElement* g) const
{
    Q_UNUSED(g);
}

void CimdrawWiringItemBase::loadCustomXml(QDomElement* g)
{
    Q_UNUSED(g);
}

void CimdrawWiringItemBase::connectSceneWiringSignals(CimdrawScene* sc)
{
    if (!sc)
        return;
    QObject::connect(sc, &CimdrawScene::wiringDataChanged, this, &CimdrawWiringItemBase::onSceneWiringDataChanged, Qt::UniqueConnection);
    if (!wiringDataKey_.isEmpty())
        applyWiringPayload(sc->wiringData(wiringDataKey_));
}

void CimdrawWiringItemBase::disconnectSceneWiringSignals(CimdrawScene* sc)
{
    if (!sc)
        return;
    QObject::disconnect(sc, &CimdrawScene::wiringDataChanged, this, &CimdrawWiringItemBase::onSceneWiringDataChanged);
}

void CimdrawWiringItemBase::onSceneWiringDataChanged(const QString& key, const QVariant& value)
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

void CimdrawWiringItemBase::onWiringVisualTick()
{
    if (!scene() || !isVisible())
        return;
    update();
}

void CimdrawWiringItemBase::applyWiringPayload(const QVariant& value)
{
    if (value.isNull() || !value.isValid())
        return;
    const QVariantMap mapValue = value.toMap();
    if (!mapValue.isEmpty() && isCimBehaviorResultVariantMap(mapValue))
    {
        applyBehaviorResult(cimBehaviorResultFromVariantMap(mapValue));
        return;
    }
    if (value.canConvert<int>())
    {
        behaviorResult_ = {};
        runState_ = clampRunState(value.toInt());
        emit wiringRunStateChanged(static_cast<int>(runState_));
        updateWiringVisualTimer();
        return;
    }
    behaviorResult_ = {};
    const QString s = value.toString().trimmed();
    if (s == QLatin1String("energized") || s == QStringLiteral("带电"))
        runState_ = CimdrawWiringRunState::Energized;
    else if (s == QLatin1String("off") || s == QStringLiteral("停运") || s == QStringLiteral("分闸"))
        runState_ = CimdrawWiringRunState::Deenergized;
    else if (s == QLatin1String("alarm") || s == QStringLiteral("告警"))
        runState_ = CimdrawWiringRunState::Alarm;
    else if (s == QLatin1String("normal") || s == QStringLiteral("正常"))
        runState_ = CimdrawWiringRunState::Normal;
    emit wiringRunStateChanged(static_cast<int>(runState_));
    updateWiringVisualTimer();
}

void CimdrawWiringItemBase::updateWiringVisualTimer()
{
    const bool need = scene() != nullptr
        && (wiringUsesVisualTimer() || runState_ == CimdrawWiringRunState::Alarm);
    if (need)
    {
        if (!wiringVisualTimer_)
        {
            wiringVisualTimer_ = new QTimer(this);
            wiringVisualTimer_->setTimerType(Qt::CoarseTimer);
            QObject::connect(wiringVisualTimer_, &QTimer::timeout, this, &CimdrawWiringItemBase::onWiringVisualTick,
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

void CimdrawWiringItemBase::setState(HANDLE_STATE st)
{
    CimdrawItem::setState(st);
    if (st != HANDLE_ACTIVE || wiringUsesAlongEdgeConnectPoints())
        return;

    QSet<int> hideEdgeHandles;
    for (CimdrawConnectPoint* port : pointStruct.points)
    {
        const int handleDir = overlappingEdgeHandleForPort(port, itemPosition);
        if (handleDir != toInt(HANDLE_POINT::HANDLE_NONE))
            hideEdgeHandles.insert(handleDir);
    }
    for (CimdrawHandle* handle : handleStruct.handles)
    {
        if (handle && hideEdgeHandles.contains(handle->dir()))
            handle->hide();
    }
}

QVariant CimdrawWiringItemBase::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant& value)
{
    if (change == QGraphicsItem::ItemSceneChange)
    {
        if (auto* os = qobject_cast<CimdrawScene*>(scene()))
            disconnectSceneWiringSignals(os);
        if (auto* ns = qobject_cast<CimdrawScene*>(value.value<QGraphicsScene*>()))
            connectSceneWiringSignals(ns);
    }
    else if (change == QGraphicsItem::ItemSceneHasChanged)
    {
        updateWiringVisualTimer();
        if (scene())
            reconcileWiringConnectPoints();
    }
    return CimdrawItem::itemChange(change, value);
}

void CimdrawWiringItemBase::contextMenuEvent(QGraphicsSceneContextMenuEvent* event)
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
        const QString k = QInputDialog::getText(nullptr, tr("数据键"), tr("与 CimdrawScene::setWiringData 使用的键一致："), QLineEdit::Normal, wiringDataKey_, &ok);
        if (ok)
            setWiringDataKey(k.trimmed());
        return;
    }
    if (picked == aNorm)
        setWiringRunState(CimdrawWiringRunState::Normal);
    else if (picked == aOn)
        setWiringRunState(CimdrawWiringRunState::Energized);
    else if (picked == aOff)
        setWiringRunState(CimdrawWiringRunState::Deenergized);
    else if (picked == aAl)
        setWiringRunState(CimdrawWiringRunState::Alarm);
    else if (aFlow && picked == aFlow)
        setFlowSign(-flowSign_);
    publishCurrentBehaviorResultToScene();
}

void CimdrawWiringItemBase::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
    if (toggleSwitchPositionFromInteraction())
    {
        event->accept();
        return;
    }
    CimdrawItem::mouseDoubleClickEvent(event);
}

QPainterPath CimdrawWiringItemBase::shape() const
{
    return defaultRoutingObstaclePath();
}

int CimdrawWiringItemBase::defaultWiringConnectPointCount() const
{
    return 2;
}

void CimdrawWiringItemBase::configureDefaultWiringConnectPoint(CimdrawConnectPoint* point, int index) const
{
    if (!point)
        return;

    const qreal y = 0.0;
    if (index <= 0)
    {
        point->setDirection(LEFT_DIRECTION);
        point->setConnectionLeadOut(0.0);
        point->bindToItem(const_cast<CimdrawWiringItemBase*>(this), QPointF(-0.5, y));
        return;
    }

    point->setDirection(RIGHT_DIRECTION);
    point->setConnectionLeadOut(0.0);
    point->bindToItem(const_cast<CimdrawWiringItemBase*>(this), QPointF(0.5, y));
}

QPainterPath CimdrawWiringItemBase::defaultRoutingObstaclePath() const
{
    QPainterPath path;
    path.addRect(itemPosition.normalized());
    return path;
}

QRectF CimdrawWiringItemBase::boundingRect() const
{
    return itemPosition;
}

CimdrawWiringSymbolStandard CimdrawWiringItemBase::wiringSymbolStandard() const
{
    return CimdrawWiringSymbolStyle::instance().standard();
}

void CimdrawWiringItemBase::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    prepare(painter);
}

void CimdrawWiringItemBase::updateCoordinate()
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

bool CimdrawWiringItemBase::saveXml(QDomElement* g)
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
    return CimdrawItem::saveXml(g);
}

bool CimdrawWiringItemBase::loadXml(QDomElement* g)
{
    if (!CimdrawItem::loadXml(g))
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
    if (auto* sc = qobject_cast<CimdrawScene*>(scene()))
        applyWiringPayload(sc->wiringData(wiringDataKey_));
    return true;
}

void CimdrawWiringItemBase::stretch(int handle, double sx, double sy, const QPointF& origin)
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

bool CimdrawWiringItemBase::isTopologyGraphNode() const
{
    return topologyGraphNodeEnabled();
}

bool CimdrawWiringItemBase::participatesInTopology() const
{
    return topologyDomain() != CimdrawTopologyDomain::None;
}

CimdrawTopologyDomain CimdrawWiringItemBase::topologyDomain() const
{
    switch (kind_)
    {
    case CimdrawWiringItemKind::BusbarSection:
    case CimdrawWiringItemKind::CircuitBreaker:
    case CimdrawWiringItemKind::Disconnector:
    case CimdrawWiringItemKind::Transformer2W:
    case CimdrawWiringItemKind::Transformer3W:
    case CimdrawWiringItemKind::LoadBreak:
    case CimdrawWiringItemKind::ArcCoil:
    case CimdrawWiringItemKind::Arrester:
    case CimdrawWiringItemKind::AutoXfmr:
    case CimdrawWiringItemKind::Cable:
    case CimdrawWiringItemKind::EarthSwitch:
    case CimdrawWiringItemKind::EarthingXfmr:
    case CimdrawWiringItemKind::Ess:
    case CimdrawWiringItemKind::Generator:
    case CimdrawWiringItemKind::Grid:
    case CimdrawWiringItemKind::Motor:
    case CimdrawWiringItemKind::Load:
    case CimdrawWiringItemKind::PvInfeed:
    case CimdrawWiringItemKind::Reactor:
    case CimdrawWiringItemKind::SplitReactor:
    case CimdrawWiringItemKind::StationXfmr:
    case CimdrawWiringItemKind::SvgComp:
    case CimdrawWiringItemKind::WindInfeed:
    case CimdrawWiringItemKind::CurrentTransformer:
    case CimdrawWiringItemKind::VoltageTransformer:
    case CimdrawWiringItemKind::Fuse:
    case CimdrawWiringItemKind::Junction:
        return CimdrawTopologyDomain::PowerSystem;
    default:
        return CimdrawTopologyDomain::None;
    }
}

PowerTopologyRole CimdrawWiringItemBase::powerTopologyRole() const
{
    switch (kind_)
    {
    case CimdrawWiringItemKind::BusbarSection:
        return PowerTopologyRole::Busbar;
    case CimdrawWiringItemKind::CircuitBreaker:
        return PowerTopologyRole::Breaker;
    case CimdrawWiringItemKind::Disconnector:
    case CimdrawWiringItemKind::LoadBreak:
        return PowerTopologyRole::Disconnector;
    case CimdrawWiringItemKind::EarthSwitch:
        return PowerTopologyRole::GroundSwitch;
    case CimdrawWiringItemKind::Transformer2W:
    case CimdrawWiringItemKind::Transformer3W:
    case CimdrawWiringItemKind::StationXfmr:
    case CimdrawWiringItemKind::EarthingXfmr:
    case CimdrawWiringItemKind::AutoXfmr:
        return PowerTopologyRole::Transformer;
    case CimdrawWiringItemKind::ArcCoil:
    case CimdrawWiringItemKind::Reactor:
    case CimdrawWiringItemKind::SplitReactor:
        return PowerTopologyRole::Reactor;
    case CimdrawWiringItemKind::Capacitor:
    case CimdrawWiringItemKind::SvgComp:
        return PowerTopologyRole::Compensation;
    case CimdrawWiringItemKind::Arrester:
        return PowerTopologyRole::SurgeArrester;
    case CimdrawWiringItemKind::Generator:
    case CimdrawWiringItemKind::Load:
    case CimdrawWiringItemKind::Grid:
    case CimdrawWiringItemKind::PvInfeed:
    case CimdrawWiringItemKind::Ess:
    case CimdrawWiringItemKind::WindInfeed:
        return PowerTopologyRole::Feeder;
    case CimdrawWiringItemKind::Motor:
    case CimdrawWiringItemKind::CurrentTransformer:
    case CimdrawWiringItemKind::VoltageTransformer:
    case CimdrawWiringItemKind::Fuse:
        return PowerTopologyRole::GenericEquipment;
    case CimdrawWiringItemKind::Junction:
    case CimdrawWiringItemKind::Cable:
        return PowerTopologyRole::Conductor;
    default:
        return PowerTopologyRole::GenericEquipment;
    }
}

QHash<QString, bool> CimdrawWiringItemBase::powerTopologyInternalConnectivity(
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

QGraphicsItem* CimdrawWiringItemBase::duplicate()
{
    CimdrawWiringItemBase* item = cloneForDuplicate();
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

void CimdrawWiringItemBase::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsItem::mouseMoveEvent(event);
}
