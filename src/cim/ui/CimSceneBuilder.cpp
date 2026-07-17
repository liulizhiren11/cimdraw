#include "CimSceneBuilder.h"

#include <QGraphicsItem>
#include <QLineF>
#include <QStringList>
#include <QSet>
#include <QtGlobal>
#include <limits>

#include "CimdrawScene.h"
#include "CimdrawToolBoxManager.h"
#include "Tool/CimdrawTool.h"
#include "Item/TmpBase.h"
#include "item/CimdrawConnectLine.h"
#include "wiring/base/CimdrawWiringItemBase.h"
#include "wiring/power/CimdrawPowerAutoXfmrItem.h"
#include "wiring/power/CimdrawPowerBusbarSectionItem.h"
#include "wiring/power/CimdrawPowerCapacitorItem.h"
#include "wiring/power/CimdrawPowerEarthingXfmrItem.h"
#include "wiring/power/CimdrawPowerJunctionItem.h"
#include "wiring/power/CimdrawPowerStationXfmrItem.h"
#include "wiring/power/CimdrawPowerTransformer2wItem.h"
#include "wiring/power/CimdrawPowerTransformer3wItem.h"
#include "cim/mapping/CimGraphicMapper.h"
#include "cim/model/CimModel.h"
#include "cim/model/CimObject.h"
#include "cim/semantic/CimConnectivityNodeView.h"
#include "cim/semantic/CimDiagramObjectPointView.h"
#include "cim/semantic/CimDiagramObjectView.h"
#include "cim/semantic/CimSwitchView.h"
#include "cim/semantic/CimTerminalView.h"
#include "cim/semantic/CimTransformerEndView.h"

namespace {

QColor cimGeneratedLinkColor()
{
    return QColor(0x7a, 0x87, 0x9a);
}

QString cimDisplayName(const CimObject& object)
{
    const QString name = object.name.trimmed();
    if (!name.isEmpty())
        return name;
    return object.className + QStringLiteral(" ") + object.mrid;
}

QString cimGraphicItemRuntimeId(const QGraphicsItem* item)
{
    if (!item)
        return QString();
    return QStringLiteral("gfx:%1").arg(quintptr(item), 0, 16);
}

void hideWiringPorts(TmpShape* shape)
{
    if (!shape)
        return;

    const QVector<CimdrawConnectPoint*>& ports = shape->wiringConnectPorts();
    for (CimdrawConnectPoint* port : ports)
    {
        if (port)
        {
            port->setVisible(false);
            port->setAcceptedMouseButtons(Qt::NoButton);
            port->setAcceptHoverEvents(false);
        }
    }
}

CimdrawConnectPoint* preferredWiringPort(TmpShape* shape, CONNECT_DIRECTION direction)
{
    if (!shape)
        return nullptr;

    const QVector<CimdrawConnectPoint*>& ports = shape->wiringConnectPorts();
    for (CimdrawConnectPoint* port : ports)
    {
        if (port && port->getDirection() == direction)
            return port;
    }
    for (CimdrawConnectPoint* port : ports)
    {
        if (port)
            return port;
    }
    return nullptr;
}

bool cimSwitchClass(const QString& className)
{
    return className == QStringLiteral("Breaker")
        || className == QStringLiteral("Disconnector")
        || className == QStringLiteral("LoadBreakSwitch")
        || className == QStringLiteral("GroundDisconnector");
}

bool cimSourceClass(const QString& className)
{
    return className == QStringLiteral("SynchronousMachine")
        || className == QStringLiteral("SolarGeneratingUnit")
        || className == QStringLiteral("WindGeneratingUnit")
        || className == QStringLiteral("BatteryUnit")
        || className == QStringLiteral("ExternalNetworkInjection")
        || className == QStringLiteral("EquivalentInjection");
}

bool cimLoadClass(const QString& className)
{
    return className == QStringLiteral("EnergyConsumer");
}

bool cimMotorClass(const QString& className)
{
    return className == QStringLiteral("AsynchronousMachine");
}

bool cimMeasurementClass(const QString& className)
{
    return className == QStringLiteral("CurrentTransformer")
        || className == QStringLiteral("PotentialTransformer")
        || className == QStringLiteral("VoltageTransformer");
}

bool cimShuntCompensationClass(const QString& className)
{
    return className == QStringLiteral("LinearShuntCompensator")
        || className == QStringLiteral("ShuntCompensator")
        || className == QStringLiteral("NonlinearShuntCompensator")
        || className == QStringLiteral("StaticVarCompensator")
        || className == QStringLiteral("PetersenCoil");
}

bool cimCapacitorLikeShuntClass(const QString& className)
{
    return className == QStringLiteral("LinearShuntCompensator")
        || className == QStringLiteral("ShuntCompensator")
        || className == QStringLiteral("NonlinearShuntCompensator");
}

bool cimFuseClass(const QString& className)
{
    return className == QStringLiteral("Fuse");
}

bool cimJunctionClass(const QString& className)
{
    return className == QStringLiteral("Junction")
        || className == QStringLiteral("Connector");
}

QSizeF preferredSceneItemSizeForDrawType(CimdrawDrawTypeId drawType, const QSizeF& fallback)
{
    switch (drawType)
    {
    case CIMDRAW_WSYM_CAPACITOR:
        return QSizeF(44.0, 48.0);
    case CIMDRAW_WSYM_REACTOR:
        return QSizeF(96.0, 40.0);
    case CIMDRAW_WSYM_SPLIT_REACTOR:
        return QSizeF(120.0, 44.0);
    case CIMDRAW_WSYM_SVG_COMP:
        return QSizeF(64.0, 50.0);
    case CIMDRAW_WSYM_ARC_COIL:
        return QSizeF(92.0, 44.0);
    default:
        return fallback;
    }
}

bool cimLineLikeClass(const QString& className)
{
    return className == QStringLiteral("BusbarSection")
        || className == QStringLiteral("ACLineSegment");
}

struct CimDiagramPoint
{
    int sequence = 0;
    QPointF position;
};

struct CimDiagramGeometry
{
    QVector<CimDiagramPoint> points;
    qreal rotation = 0.0;
    bool hasRotation = false;
    QString diagramObjectMrid;
};

enum class CimDiagramTargetKind
{
    DirectEquipment = 0,
    TerminalEquipment = 1,
};

QPointF diagramAnchorForGeometry(const CimDiagramGeometry& geometry);
bool diagramGeometryPreferredOver(const CimDiagramGeometry& candidate,
                                  CimDiagramTargetKind candidateKind,
                                  const CimDiagramGeometry& current,
                                  CimDiagramTargetKind currentKind);

struct CimDiagramTarget
{
    QString equipmentMrid;
    CimDiagramTargetKind kind = CimDiagramTargetKind::TerminalEquipment;
};

struct CimDiagramPlacement
{
    QPointF center;
    QSizeF size;
    qreal rotation = 0.0;
    bool valid = false;
    bool hasRotation = false;
};

struct CimDiagramLayoutTransform
{
    qreal scale = 1.0;
    QPointF offset;
    bool valid = false;
};

struct CimTerminalBinding
{
    QString terminalMrid;
    QString equipmentMrid;
    QString nodeMrid;
    int terminalSequence = -1;
    int transformerEndNumber = -1;
};

const CimObject* transformerEndForTerminal(const CimObject& terminal,
                                           const QHash<QString, const CimObject*>& objectByMrid)
{
    const CimTerminalView terminalView(&terminal);
    const QString transformerEndMrid = terminalView.transformerEndMrid();
    if (!transformerEndMrid.isEmpty())
        return objectByMrid.value(transformerEndMrid, nullptr);

    for (auto it = objectByMrid.constBegin(); it != objectByMrid.constEnd(); ++it)
    {
        const CimObject* object = it.value();
        if (!object)
            continue;
        const CimTransformerEndView transformerEndView(object);
        if (!transformerEndView.isTransformerEnd())
            continue;
        if (transformerEndView.terminalMrid() == terminal.mrid)
            return object;
    }
    return nullptr;
}

QString resolveDiagramEquipmentMrid(const CimObject& identifiedObject)
{
    const CimTerminalView terminalView(&identifiedObject);
    if (terminalView.isTerminal())
        return terminalView.conductingEquipmentMrid();
    return identifiedObject.mrid;
}

bool orderedTransformerShape(TmpShape* shape)
{
    return dynamic_cast<CimdrawPowerTransformer2wItem*>(shape) != nullptr
        || dynamic_cast<CimdrawPowerAutoXfmrItem*>(shape) != nullptr
        || dynamic_cast<CimdrawPowerEarthingXfmrItem*>(shape) != nullptr
        || dynamic_cast<CimdrawPowerStationXfmrItem*>(shape) != nullptr
        || dynamic_cast<CimdrawPowerTransformer3wItem*>(shape) != nullptr;
}

QHash<QString, CimDiagramGeometry> buildDiagramGeometryByEquipmentMrid(const QVector<CimObject>& objects)
{
    QHash<QString, const CimObject*> objectByMrid;
    for (const CimObject& object : objects)
        objectByMrid.insert(object.mrid, &object);

    QHash<QString, CimDiagramTarget> diagramTargetByDiagramObjectMrid;
    QHash<QString, qreal> rotationByDiagramObjectMrid;
    QHash<QString, QVector<CimDiagramPoint>> pointsByDiagramObjectMrid;

    for (const CimObject& object : objects)
    {
        const CimDiagramObjectView diagramObjectView(&object);
        if (diagramObjectView.isDiagramObject())
        {
            const QString identifiedMrid = diagramObjectView.identifiedObjectMrid();
            if (identifiedMrid.isEmpty())
                continue;

            const CimObject* identifiedObject = objectByMrid.value(identifiedMrid, nullptr);
            if (!identifiedObject)
                continue;

            const QString equipmentMrid = resolveDiagramEquipmentMrid(*identifiedObject);
            if (equipmentMrid.isEmpty())
                continue;

            CimDiagramTarget target;
            target.equipmentMrid = equipmentMrid;
            target.kind = identifiedObject->className == QStringLiteral("Terminal")
                              ? CimDiagramTargetKind::TerminalEquipment
                              : CimDiagramTargetKind::DirectEquipment;
            diagramTargetByDiagramObjectMrid.insert(object.mrid, target);

            if (diagramObjectView.hasRotation())
                rotationByDiagramObjectMrid.insert(object.mrid, diagramObjectView.rotation());
            continue;
        }

        const CimDiagramObjectPointView diagramObjectPointView(&object);
        if (!diagramObjectPointView.isDiagramObjectPoint())
            continue;

        const QString diagramObjectMrid = diagramObjectPointView.diagramObjectMrid();
        if (diagramObjectMrid.isEmpty())
            continue;

        if (!diagramObjectPointView.hasPosition())
            continue;

        CimDiagramPoint point;
        point.sequence = diagramObjectPointView.sequenceNumber(
            pointsByDiagramObjectMrid.value(diagramObjectMrid).size() + 1);
        point.position = diagramObjectPointView.position();
        pointsByDiagramObjectMrid[diagramObjectMrid].push_back(point);
    }

    QHash<QString, CimDiagramGeometry> geometryByEquipmentMrid;
    QHash<QString, CimDiagramTargetKind> targetKindByEquipmentMrid;
    for (auto it = diagramTargetByDiagramObjectMrid.constBegin(); it != diagramTargetByDiagramObjectMrid.constEnd(); ++it)
    {
        CimDiagramGeometry geometry;
        geometry.points = pointsByDiagramObjectMrid.value(it.key());
        geometry.diagramObjectMrid = it.key();
        std::sort(geometry.points.begin(), geometry.points.end(), [](const CimDiagramPoint& lhs, const CimDiagramPoint& rhs) {
            return lhs.sequence < rhs.sequence;
        });
        if (geometry.points.isEmpty())
            continue;

        if (rotationByDiagramObjectMrid.contains(it.key()))
        {
            geometry.rotation = rotationByDiagramObjectMrid.value(it.key());
            geometry.hasRotation = true;
        }

        const QString& equipmentMrid = it->equipmentMrid;
        const CimDiagramTargetKind targetKind = it->kind;
        const auto existing = geometryByEquipmentMrid.constFind(equipmentMrid);
        const auto existingKind = targetKindByEquipmentMrid.constFind(equipmentMrid);
        const bool shouldReplace =
            existing == geometryByEquipmentMrid.constEnd()
            || existingKind == targetKindByEquipmentMrid.constEnd()
            || diagramGeometryPreferredOver(geometry, targetKind, existing.value(), existingKind.value());

        if (shouldReplace)
        {
            geometryByEquipmentMrid.insert(equipmentMrid, geometry);
            targetKindByEquipmentMrid.insert(equipmentMrid, targetKind);
        }
    }

    return geometryByEquipmentMrid;
}

CimDiagramPlacement diagramPlacementForObject(const CimObject& object,
                                              const QSizeF& fallbackSize,
                                              const QHash<QString, CimDiagramGeometry>& geometryByEquipmentMrid)
{
    CimDiagramPlacement placement;
    const auto geometryIt = geometryByEquipmentMrid.constFind(object.mrid);
    if (geometryIt == geometryByEquipmentMrid.constEnd() || geometryIt->points.isEmpty())
        return placement;

    const CimDiagramGeometry& geometry = geometryIt.value();
    placement.valid = true;
    placement.size = fallbackSize;
    placement.hasRotation = geometry.hasRotation;
    placement.rotation = geometry.rotation;

    if (geometry.points.size() == 1)
    {
        placement.center = geometry.points.first().position;
        return placement;
    }

    const QPointF first = geometry.points.first().position;
    const QPointF last = geometry.points.last().position;
    placement.center = QPointF((first.x() + last.x()) * 0.5, (first.y() + last.y()) * 0.5);

    if (cimLineLikeClass(object.className))
    {
        const qreal length = QLineF(first, last).length();
        if (object.className == QStringLiteral("BusbarSection"))
            placement.size.setWidth(qMax<qreal>(24.0, length));
        else if (object.className == QStringLiteral("ACLineSegment"))
            placement.size.setWidth(qMax<qreal>(48.0, length));

        if (!placement.hasRotation)
        {
            const qreal dx = last.x() - first.x();
            const qreal dy = last.y() - first.y();
            if (qAbs(dy) > qAbs(dx))
                placement.rotation = 90.0;
            else if (dx < 0.0)
                placement.rotation = 180.0;
            else
                placement.rotation = 0.0;
            placement.hasRotation = true;
        }
    }

    return placement;
}

CimDiagramLayoutTransform buildDiagramLayoutTransform(const QHash<QString, CimDiagramGeometry>& geometryByEquipmentMrid)
{
    qreal minX = 0.0;
    qreal minY = 0.0;
    qreal maxX = 0.0;
    qreal maxY = 0.0;
    bool hasPoint = false;
    for (auto it = geometryByEquipmentMrid.constBegin(); it != geometryByEquipmentMrid.constEnd(); ++it)
    {
        for (const CimDiagramPoint& point : it->points)
        {
            if (!hasPoint)
            {
                minX = maxX = point.position.x();
                minY = maxY = point.position.y();
                hasPoint = true;
            }
            else
            {
                minX = qMin(minX, point.position.x());
                minY = qMin(minY, point.position.y());
                maxX = qMax(maxX, point.position.x());
                maxY = qMax(maxY, point.position.y());
            }
        }
    }

    CimDiagramLayoutTransform transform;
    if (!hasPoint)
        return transform;

    const qreal width = qMax<qreal>(1.0, maxX - minX);
    const qreal height = qMax<qreal>(1.0, maxY - minY);
    const qreal targetWidth = 720.0;
    const qreal targetHeight = 520.0;
    transform.scale = qBound<qreal>(1.0, qMin(targetWidth / width, targetHeight / height), 2.4);
    transform.offset = QPointF(80.0, 80.0) - QPointF(minX, minY) * transform.scale;
    transform.valid = true;
    return transform;
}

QPointF applyDiagramLayoutTransform(const QPointF& point, const CimDiagramLayoutTransform& transform)
{
    if (!transform.valid)
        return point;
    return point * transform.scale + transform.offset;
}

QSizeF scaledDiagramFallbackSize(const QSizeF& baseSize, const CimDiagramLayoutTransform& transform)
{
    if (!transform.valid)
        return baseSize;

    const qreal factor = qBound<qreal>(0.45, 0.9 / transform.scale, 1.0);
    return QSizeF(qMax<qreal>(12.0, baseSize.width() * factor),
                  qMax<qreal>(12.0, baseSize.height() * factor));
}

QPointF diagramAnchorForGeometry(const CimDiagramGeometry& geometry)
{
    if (geometry.points.isEmpty())
        return {};
    if (geometry.points.size() == 1)
        return geometry.points.first().position;

    const QPointF first = geometry.points.first().position;
    const QPointF last = geometry.points.last().position;
    return QPointF((first.x() + last.x()) * 0.5, (first.y() + last.y()) * 0.5);
}

bool diagramGeometryPreferredOver(const CimDiagramGeometry& candidate,
                                  CimDiagramTargetKind candidateKind,
                                  const CimDiagramGeometry& current,
                                  CimDiagramTargetKind currentKind)
{
    if (candidateKind != currentKind)
        return candidateKind < currentKind;

    if (candidate.points.size() != current.points.size())
        return candidate.points.size() > current.points.size();

    if (candidate.hasRotation != current.hasRotation)
        return candidate.hasRotation;

    const QPointF candidateAnchor = diagramAnchorForGeometry(candidate);
    const QPointF currentAnchor = diagramAnchorForGeometry(current);
    if (!qFuzzyCompare(candidateAnchor.y() + 1.0, currentAnchor.y() + 1.0))
        return candidateAnchor.y() < currentAnchor.y();
    if (!qFuzzyCompare(candidateAnchor.x() + 1.0, currentAnchor.x() + 1.0))
        return candidateAnchor.x() < currentAnchor.x();

    return candidate.diagramObjectMrid < current.diagramObjectMrid;
}

} // namespace

CimSceneBuilder::BuildResult CimSceneBuilder::populateSceneFromCimModel(CimdrawScene* scene, const CimModel& model) const
{
    BuildResult result;
    if (!scene)
        return result;

    CimGraphicMapper mapper;
    QVector<CimObject> objects = model.allObjects();
    QVector<CimObject> busbars;
    QVector<CimObject> switchingDevices;
    QVector<CimObject> transformers;
    QVector<CimObject> sources;
    QVector<CimObject> loads;
    QVector<CimObject> motors;
    QVector<CimObject> measurements;
    QVector<CimObject> fuses;
    QVector<CimObject> junctions;
    QVector<CimObject> conductors;
    QVector<CimObject> shunts;
    QVector<CimObject> seriesCompensators;
    QVector<CimObject> arresters;
    QVector<CimObject> grounds;
    QVector<CimObject> genericOthers;
    QHash<QString, const CimObject*> objectByMrid;
    const QHash<QString, CimDiagramGeometry> diagramGeometryByEquipmentMrid =
        buildDiagramGeometryByEquipmentMrid(objects);
    const CimDiagramLayoutTransform diagramLayoutTransform =
        buildDiagramLayoutTransform(diagramGeometryByEquipmentMrid);
    QHash<QString, QPointF> diagramAnchorByMrid;
    for (auto it = diagramGeometryByEquipmentMrid.constBegin(); it != diagramGeometryByEquipmentMrid.constEnd(); ++it)
        diagramAnchorByMrid.insert(it.key(), applyDiagramLayoutTransform(diagramAnchorForGeometry(it.value()),
                                                                         diagramLayoutTransform));

    for (const CimObject& object : objects)
    {
        objectByMrid.insert(object.mrid, &object);
        const CimGraphicMappingResult mapping = mapper.mapObject(object);
        if (!mapping.isMappable())
            continue;

        switch (mapping.role)
        {
        case CimGraphicMappingRole::Busbar:
            busbars.push_back(object);
            break;
        case CimGraphicMappingRole::SwitchingDevice:
            switchingDevices.push_back(object);
            break;
        case CimGraphicMappingRole::Transformer:
            transformers.push_back(object);
            break;
        case CimGraphicMappingRole::Source:
            sources.push_back(object);
            break;
        case CimGraphicMappingRole::Load:
            loads.push_back(object);
            break;
        case CimGraphicMappingRole::Motor:
            motors.push_back(object);
            break;
        case CimGraphicMappingRole::Measurement:
            measurements.push_back(object);
            break;
        case CimGraphicMappingRole::Fuse:
            fuses.push_back(object);
            break;
        case CimGraphicMappingRole::Junction:
            junctions.push_back(object);
            break;
        case CimGraphicMappingRole::Conductor:
            conductors.push_back(object);
            break;
        case CimGraphicMappingRole::ShuntCompensator:
            shunts.push_back(object);
            break;
        case CimGraphicMappingRole::SeriesCompensator:
            seriesCompensators.push_back(object);
            break;
        case CimGraphicMappingRole::SurgeArrester:
            arresters.push_back(object);
            break;
        case CimGraphicMappingRole::Ground:
            grounds.push_back(object);
            break;
        case CimGraphicMappingRole::Other:
            genericOthers.push_back(object);
            break;
        case CimGraphicMappingRole::Unsupported:
            break;
        }
    }

    const QPointF origin(120.0, 120.0);
    const qreal rowSpacing = 150.0;
    const qreal columnSpacing = 120.0;
    const qreal branchSpacing = 180.0;
    QHash<QString, TmpShape*> shapesByMrid;

    auto createGraphicItem = [&](const CimObject& object, const QPointF& topLeft, const QSizeF& size) {
        const CimGraphicMappingResult mapping = mapper.mapObject(object);
        if (!mapping.isMappable())
            return static_cast<TmpShape*>(nullptr);

        const CimdrawDrawTypeId drawType = mapping.drawType;
        const QSizeF preferredSize = preferredSceneItemSizeForDrawType(drawType, size);
        const CimDiagramPlacement placement =
            diagramPlacementForObject(object, preferredSize, diagramGeometryByEquipmentMrid);
        const QSizeF fallbackSize = scaledDiagramFallbackSize(preferredSize, diagramLayoutTransform);
        QSizeF actualSize = placement.valid ? placement.size : fallbackSize;
        QPointF actualPos = placement.valid ? placement.center : topLeft;

        if (placement.valid)
        {
            actualPos = applyDiagramLayoutTransform(actualPos, diagramLayoutTransform);
            if (!cimLineLikeClass(object.className))
                actualSize = scaledDiagramFallbackSize(actualSize, diagramLayoutTransform);
        }

        QRectF rect(QPointF(0.0, 0.0), actualSize);
        QGraphicsItem* graphicsItem = nullptr;
        if (mapping.requiresDedicatedCapacitorItemFactory)
        {
            // Capacitor-like CIM objects keep their dedicated graphic factory.
            auto* capacitor = new CimdrawPowerCapacitorItem(rect);
            scene->addItem(capacitor);
            graphicsItem = capacitor;
        }
        else
        {
            CimdrawTool* tool = CimdrawToolManager::getInstance()->changeTool(drawType);
            if (!tool)
                return static_cast<TmpShape*>(nullptr);
            graphicsItem = tool->createObject(scene, &rect, true);
        }
        if (!graphicsItem)
            return static_cast<TmpShape*>(nullptr);
        graphicsItem->setPos(actualPos);
        graphicsItem->setData(CimMridDataKey, object.mrid);
        const QString graphicItemId = cimGraphicItemRuntimeId(graphicsItem);
        graphicsItem->setData(CimGraphicItemIdDataKey, graphicItemId);
        auto* shape = dynamic_cast<TmpShape*>(graphicsItem);
        if (!shape)
            return static_cast<TmpShape*>(nullptr);

        if (placement.valid && placement.hasRotation)
            shape->setItemDegree(placement.rotation);

        shape->setCimdrawObjectId(object.mrid);
        shape->setTopologyNodeStableId(object.mrid);
        shapesByMrid.insert(object.mrid, shape);
        result.shapeByMrid.insert(object.mrid, graphicsItem);
        result.idIndex.bindGraphicItem(object.mrid, graphicItemId);

        if (auto* wiring = dynamic_cast<CimdrawWiringItemBase*>(shape))
        {
            wiring->setDisplayName(cimDisplayName(object));
            wiring->setReferenceDesignator(object.name);

            const CimSwitchView switchView(&object);
            if (switchView.isSwitch())
            {
                wiring->setSwitchPosition(switchView.isOpen() ? 0 : 1);
            }
        }
        hideWiringPorts(shape);
        return shape;
    };

    auto placeRow = [&](const QVector<CimObject>& rowObjects, qreal y, const QSizeF& itemSize, qreal spacing) {
        QVector<TmpShape*> rowShapes;
        if (rowObjects.isEmpty())
            return rowShapes;

        const qreal totalWidth = rowObjects.size() * itemSize.width()
                                 + qMax(0, rowObjects.size() - 1) * spacing;
        qreal x = origin.x();
        if (!busbars.isEmpty())
            x += qMax(0.0, (300.0 - totalWidth) * 0.5);

        for (const CimObject& object : rowObjects)
        {
            TmpShape* shape = createGraphicItem(object, QPointF(x, y), itemSize);
            if (shape)
                rowShapes.push_back(shape);
            x += itemSize.width() + spacing;
        }
        return rowShapes;
    };

    qreal currentY = origin.y();
    const QSizeF busbarSize(300.0, 12.0);
    const QSizeF switchSize(96.0, 64.0);
    const QSizeF transformerSize(112.0, 68.0);
    const QSizeF sourceSize(168.0, 36.0);
    const QSizeF loadSize(96.0, 36.0);
    const QSizeF motorSize(72.0, 72.0);
    const QSizeF measurementSize(84.0, 54.0);
    const QSizeF fuseSize(88.0, 34.0);
    const QSizeF junctionSize(28.0, 28.0);
    const QSizeF otherSize(96.0, 60.0);
    const QSizeF conductorSize(132.0, 44.0);
    const QSizeF shuntSize(56.0, 56.0);
    const QSizeF reactorSize(96.0, 40.0);
    const QSizeF arresterSize(52.0, 58.0);
    const QSizeF groundSize(40.0, 96.0);

    for (int i = 0; i < busbars.size(); ++i)
        createGraphicItem(busbars.at(i), QPointF(origin.x() + i * (busbarSize.width() + branchSpacing), currentY), busbarSize);
    if (!busbars.isEmpty())
        currentY += rowSpacing;

    placeRow(switchingDevices, currentY, switchSize, columnSpacing);
    if (!switchingDevices.isEmpty())
        currentY += rowSpacing;

    placeRow(transformers, currentY, transformerSize, branchSpacing);
    if (!transformers.isEmpty())
        currentY += rowSpacing;

    placeRow(sources, currentY, sourceSize, branchSpacing);
    if (!sources.isEmpty())
        currentY += rowSpacing;

    placeRow(loads, currentY, loadSize, branchSpacing);
    if (!loads.isEmpty())
        currentY += rowSpacing;

    placeRow(motors, currentY, motorSize, branchSpacing);
    if (!motors.isEmpty())
        currentY += rowSpacing;

    placeRow(measurements, currentY, measurementSize, branchSpacing);
    if (!measurements.isEmpty())
        currentY += rowSpacing;

    placeRow(fuses, currentY, fuseSize, branchSpacing);
    if (!fuses.isEmpty())
        currentY += rowSpacing;

    placeRow(junctions, currentY, junctionSize, columnSpacing);
    if (!junctions.isEmpty())
        currentY += rowSpacing;

    placeRow(conductors, currentY, conductorSize, branchSpacing);
    if (!conductors.isEmpty())
        currentY += rowSpacing;

    placeRow(shunts, currentY, shuntSize, branchSpacing);
    if (!shunts.isEmpty())
        currentY += rowSpacing;

    placeRow(seriesCompensators, currentY, reactorSize, branchSpacing);
    if (!seriesCompensators.isEmpty())
        currentY += rowSpacing;

    placeRow(arresters, currentY, arresterSize, branchSpacing);
    if (!arresters.isEmpty())
        currentY += rowSpacing;

    placeRow(grounds, currentY, groundSize, branchSpacing);
    if (!grounds.isEmpty())
        currentY += rowSpacing;

    placeRow(genericOthers, currentY, otherSize, branchSpacing);

    QHash<QString, QVector<CimTerminalBinding>> terminalsByNode;
    for (const CimObject& object : objects)
    {
        const CimTerminalView terminalView(&object);
        if (!terminalView.isTerminal())
            continue;

        const QString equipmentMrid = terminalView.conductingEquipmentMrid();
        QString nodeMrid = terminalView.topologicalNodeMrid();
        if (nodeMrid.isEmpty())
            nodeMrid = terminalView.connectivityNodeMrid();
        if (nodeMrid.isEmpty())
        {
            const QString connectivityNodeMrid = terminalView.connectivityNodeMrid();
            if (const CimObject* connectivityNode = objectByMrid.value(connectivityNodeMrid, nullptr))
            {
                const CimConnectivityNodeView connectivityNodeView(connectivityNode);
                nodeMrid = connectivityNodeView.topologicalNodeMrid();
            }
        }

        if (equipmentMrid.isEmpty() || nodeMrid.isEmpty())
            continue;

        CimTerminalBinding binding;
        binding.terminalMrid = object.mrid;
        binding.equipmentMrid = equipmentMrid;
        binding.nodeMrid = nodeMrid;
        binding.terminalSequence = terminalView.sequenceNumber();
        if (const CimObject* transformerEnd = transformerEndForTerminal(object, objectByMrid))
        {
            const CimTransformerEndView transformerEndView(transformerEnd);
            binding.transformerEndNumber = transformerEndView.endNumber();
        }
        terminalsByNode[nodeMrid].push_back(binding);
    }

    QHash<TmpShape*, QSet<CimdrawConnectPoint*>> usedPortsByShape;

    auto resolveNodeAnchor = [&](const QString& nodeMrid, const QVector<CimTerminalBinding>& bindings) {
        const auto nodeAnchorIt = diagramAnchorByMrid.constFind(nodeMrid);
        if (nodeAnchorIt != diagramAnchorByMrid.constEnd())
            return nodeAnchorIt.value();

        QPointF sum;
        int count = 0;
        for (const CimTerminalBinding& binding : bindings)
        {
            if (TmpShape* shape = shapesByMrid.value(binding.equipmentMrid, nullptr))
            {
                sum += shape->sceneBoundingRect().center();
                ++count;
            }
        }
        return count > 0 ? QPointF(sum.x() / count, sum.y() / count) : QPointF();
    };

    auto preferredPortForBinding = [&](TmpShape* shape, const QPointF& nodeAnchor, const CimTerminalBinding* binding) {
        if (!shape)
            return static_cast<CimdrawConnectPoint*>(nullptr);

        if (auto* busbar = dynamic_cast<CimdrawPowerBusbarSectionItem*>(shape))
            return busbar->ensureConnectPointAtScene(nodeAnchor);

        const QVector<CimdrawConnectPoint*>& ports = shape->wiringConnectPorts();
        if (ports.isEmpty())
            return static_cast<CimdrawConnectPoint*>(nullptr);

        const QSet<CimdrawConnectPoint*>& usedPorts = usedPortsByShape[shape];

        if (binding && orderedTransformerShape(shape))
        {
            int preferredIndex = -1;
            if (binding->transformerEndNumber >= 1 && binding->transformerEndNumber <= ports.size())
                preferredIndex = binding->transformerEndNumber - 1;
            else if (binding->terminalSequence >= 1 && binding->terminalSequence <= ports.size())
                preferredIndex = binding->terminalSequence - 1;

            if (preferredIndex >= 0)
            {
                CimdrawConnectPoint* preferred = ports.value(preferredIndex, nullptr);
                if (preferred && !usedPorts.contains(preferred))
                {
                    usedPortsByShape[shape].insert(preferred);
                    return preferred;
                }
                if (preferred)
                    return preferred;
            }
        }

        CimdrawConnectPoint* best = nullptr;
        qreal bestDistance = std::numeric_limits<qreal>::max();

        auto tryPick = [&](bool allowUsed) {
            for (CimdrawConnectPoint* port : ports)
            {
                if (!port)
                    continue;
                if (!allowUsed && usedPorts.contains(port))
                    continue;
                const qreal distance = QLineF(port->connectionCenterInScene(), nodeAnchor).length();
                if (!best || distance < bestDistance)
                {
                    best = port;
                    bestDistance = distance;
                }
            }
        };

        tryPick(false);
        if (!best)
            tryPick(true);
        if (best)
            usedPortsByShape[shape].insert(best);
        return best;
    };

    auto ensureHiddenNodeHub = [&](const QString& nodeMrid, const QPointF& nodeAnchor) -> TmpShape* {
        TmpShape* existing = shapesByMrid.value(nodeMrid, nullptr);
        if (existing)
            return existing;

        auto* hub = new CimdrawPowerBusbarSectionItem(QRectF(0.0, 0.0, 18.0, 4.0));
        hub->setPos(nodeAnchor);
        hub->setVisible(false);
        hub->setAcceptedMouseButtons(Qt::NoButton);
        hub->setAcceptHoverEvents(false);
        hub->setData(CimGeneratedVisualHelperDataKey, true);
        hub->setCimdrawObjectId(nodeMrid);
        hub->setTopologyNodeStableId(nodeMrid);
        hideWiringPorts(hub);
        scene->addItem(hub);
        shapesByMrid.insert(nodeMrid, hub);
        return hub;
    };

    auto connectShapes = [&](TmpShape* startShape, CONNECT_DIRECTION startDir,
                             TmpShape* endShape, CONNECT_DIRECTION endDir,
                             const CimTerminalBinding* startBinding = nullptr,
                             const CimTerminalBinding* endBinding = nullptr,
                             const QPointF& nodeAnchor = QPointF()) {
        if (!startShape || !endShape || startShape == endShape)
            return;

        for (QGraphicsItem* item : startShape->connectedItems())
        {
            auto* line = qgraphicsitem_cast<CimdrawConnectLine*>(item);
            if (!line)
                continue;
            if ((line->getStartItem() == startShape && line->getEndItem() == endShape)
                || (line->getStartItem() == endShape && line->getEndItem() == startShape))
            {
                return;
            }
        }

        const QPointF effectiveAnchor = nodeAnchor.isNull()
            ? QPointF((startShape->sceneBoundingRect().center().x() + endShape->sceneBoundingRect().center().x()) * 0.5,
                      (startShape->sceneBoundingRect().center().y() + endShape->sceneBoundingRect().center().y()) * 0.5)
            : nodeAnchor;
        CimdrawConnectPoint* startPort = preferredPortForBinding(startShape, effectiveAnchor, startBinding);
        CimdrawConnectPoint* endPort = preferredPortForBinding(endShape, effectiveAnchor, endBinding);
        if (!startPort)
            startPort = preferredWiringPort(startShape, startDir);
        if (!endPort)
            endPort = preferredWiringPort(endShape, endDir);
        if (!startPort || !endPort)
            return;

        auto* line = new CimdrawConnectLine(QRectF(0, 0, 1, 1));
        scene->addItem(line);
        line->setConnectStrategy(BROKEN_CONNECTION);
        line->setCornerStyle(CimdrawConnectLine::Sharp);
        line->setStartItem(startShape);
        line->setStartConnectPort(startPort);
        startShape->addConnect(line);
        line->attachEndAndRecompute(endShape, endPort);
        line->recomputePathFromCurrentEndpoints(false);
        endShape->addConnect(line);
        line->setData(CimGeneratedVisualHelperDataKey, true);
        line->setUsePen(true);
        line->setItemPenColor(cimGeneratedLinkColor());
        line->setItemPenWidth(1);
        line->setZValue(CimdrawConnectLineLayerZ);
        ++result.generatedHelperLineCount;
    };

    auto fallbackConnectByRows = [&]() {
        QVector<TmpShape*> orderedShapes;
        auto appendExistingShapes = [&](const QVector<CimObject>& rowObjects) {
            for (const CimObject& object : rowObjects)
            {
                if (TmpShape* shape = shapesByMrid.value(object.mrid, nullptr))
                    orderedShapes.push_back(shape);
            }
        };

        appendExistingShapes(busbars);
        appendExistingShapes(switchingDevices);
        appendExistingShapes(transformers);
        appendExistingShapes(sources);
        appendExistingShapes(loads);
        appendExistingShapes(motors);
        appendExistingShapes(measurements);
        appendExistingShapes(fuses);
        appendExistingShapes(junctions);
        appendExistingShapes(conductors);
        appendExistingShapes(shunts);
        appendExistingShapes(seriesCompensators);
        appendExistingShapes(arresters);
        appendExistingShapes(grounds);
        appendExistingShapes(genericOthers);

        if (orderedShapes.size() < 2)
            return;

        TmpShape* anchor = orderedShapes.first();
        for (int i = 1; i < orderedShapes.size(); ++i)
        {
            TmpShape* next = orderedShapes.at(i);
            const bool anchorIsBusbar = dynamic_cast<CimdrawPowerBusbarSectionItem*>(anchor) != nullptr;
            const bool nextIsBusbar = dynamic_cast<CimdrawPowerBusbarSectionItem*>(next) != nullptr;

            if (anchorIsBusbar)
                connectShapes(anchor, BOTTOM_DIRECTION, next, TOP_DIRECTION);
            else if (nextIsBusbar)
                connectShapes(anchor, TOP_DIRECTION, next, BOTTOM_DIRECTION);
            else
                connectShapes(anchor, BOTTOM_DIRECTION, next, TOP_DIRECTION);

            anchor = next;
        }
    };

    QStringList orderedNodeMrids = terminalsByNode.keys();
    std::sort(orderedNodeMrids.begin(), orderedNodeMrids.end());
    for (const QString& nodeMrid : orderedNodeMrids)
    {
        QVector<TmpShape*> memberShapes;
        TmpShape* busbarShape = nullptr;
        QVector<TmpShape*> junctionShapes;
        const QVector<CimTerminalBinding>& bindings = terminalsByNode.value(nodeMrid);
        const QPointF nodeAnchor = resolveNodeAnchor(nodeMrid, bindings);
        for (const CimTerminalBinding& binding : bindings)
        {
            TmpShape* shape = shapesByMrid.value(binding.equipmentMrid, nullptr);
            if (!shape)
                continue;
            if (!memberShapes.contains(shape))
                memberShapes.push_back(shape);
            if (dynamic_cast<CimdrawPowerBusbarSectionItem*>(shape))
                busbarShape = shape;
            if (dynamic_cast<CimdrawPowerJunctionItem*>(shape))
                junctionShapes.push_back(shape);
        }

        if (memberShapes.size() < 2)
        {
            if (memberShapes.size() == 1)
            {
                TmpShape* loneShape = memberShapes.first();
                if (orderedTransformerShape(loneShape))
                {
                    for (const CimTerminalBinding& binding : bindings)
                    {
                        if (shapesByMrid.value(binding.equipmentMrid, nullptr) != loneShape)
                            continue;
                        TmpShape* hub = ensureHiddenNodeHub(binding.nodeMrid, nodeAnchor);
                        connectShapes(hub,
                                      BOTTOM_DIRECTION,
                                      loneShape,
                                      TOP_DIRECTION,
                                      nullptr,
                                      &binding,
                                      nodeAnchor);
                    }
                }
            }
            continue;
        }

        if (busbarShape)
        {
            for (const CimTerminalBinding& binding : bindings)
            {
                TmpShape* shape = shapesByMrid.value(binding.equipmentMrid, nullptr);
                if (shape == busbarShape)
                    continue;
                connectShapes(busbarShape, BOTTOM_DIRECTION, shape, TOP_DIRECTION, nullptr, &binding, nodeAnchor);
            }
            continue;
        }

        if (!junctionShapes.isEmpty())
        {
            TmpShape* junctionHub = junctionShapes.first();
            for (const CimTerminalBinding& binding : bindings)
            {
                TmpShape* shape = shapesByMrid.value(binding.equipmentMrid, nullptr);
                if (shape == junctionHub)
                    continue;
                connectShapes(junctionHub, BOTTOM_DIRECTION, shape, TOP_DIRECTION, nullptr, &binding, nodeAnchor);
            }
            continue;
        }

        if (memberShapes.size() == 2)
        {
            const CimTerminalBinding* firstBinding = nullptr;
            const CimTerminalBinding* secondBinding = nullptr;
            for (const CimTerminalBinding& binding : bindings)
            {
                if (!firstBinding && shapesByMrid.value(binding.equipmentMrid, nullptr) == memberShapes.at(0))
                    firstBinding = &binding;
                else if (!secondBinding && shapesByMrid.value(binding.equipmentMrid, nullptr) == memberShapes.at(1))
                    secondBinding = &binding;
            }
            connectShapes(memberShapes.at(0),
                          BOTTOM_DIRECTION,
                          memberShapes.at(1),
                          TOP_DIRECTION,
                          firstBinding,
                          secondBinding,
                          nodeAnchor);
            continue;
        }

        for (const CimTerminalBinding& binding : bindings)
        {
            TmpShape* shape = shapesByMrid.value(binding.equipmentMrid, nullptr);
            if (!shape)
                continue;
            TmpShape* hub = ensureHiddenNodeHub(nodeMrid, nodeAnchor);
            connectShapes(hub, BOTTOM_DIRECTION, shape, TOP_DIRECTION, nullptr, &binding, nodeAnchor);
        }
    }

    bool hasAnyConnectionLine = false;
    for (QGraphicsItem* item : scene->items())
    {
        if (qgraphicsitem_cast<CimdrawConnectLine*>(item))
        {
            hasAnyConnectionLine = true;
            break;
        }
    }
    if (!hasAnyConnectionLine)
        fallbackConnectByRows();

    scene->rebuildTopologyIndex();
    return result;
}
