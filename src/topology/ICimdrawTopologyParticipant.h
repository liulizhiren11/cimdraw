#ifndef ILZTOPOLOGYPARTICIPANT_H
#define ILZTOPOLOGYPARTICIPANT_H

#include "TopologyTypes.h"

class QGraphicsItem;

class ICimdrawTopologyParticipant
{
public:
    virtual ~ICimdrawTopologyParticipant() = default;

    virtual bool participatesInTopology() const = 0;
    virtual CimdrawTopologyDomain topologyDomain() const = 0;
};

ICimdrawTopologyParticipant* cimdrawTopologyParticipantForItem(QGraphicsItem* item);
const ICimdrawTopologyParticipant* cimdrawTopologyParticipantForItem(const QGraphicsItem* item);

#endif // ILZTOPOLOGYPARTICIPANT_H
