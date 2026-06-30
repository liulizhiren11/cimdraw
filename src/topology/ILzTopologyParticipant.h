#ifndef ILZTOPOLOGYPARTICIPANT_H
#define ILZTOPOLOGYPARTICIPANT_H

#include "TopologyTypes.h"

class QGraphicsItem;

class ILzTopologyParticipant
{
public:
    virtual ~ILzTopologyParticipant() = default;

    virtual bool participatesInTopology() const = 0;
    virtual LzTopologyDomain topologyDomain() const = 0;
};

ILzTopologyParticipant* lzTopologyParticipantForItem(QGraphicsItem* item);
const ILzTopologyParticipant* lzTopologyParticipantForItem(const QGraphicsItem* item);

#endif // ILZTOPOLOGYPARTICIPANT_H
