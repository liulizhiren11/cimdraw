#ifndef CIMPOWERTOPOLOGYPREVIEWQUERY_H
#define CIMPOWERTOPOLOGYPREVIEWQUERY_H

#include "topology/PowerTopologyTypes.h"

class QString;
class CimdrawScene;

class CimPowerTopologyPreviewQuery
{
public:
    PowerTopologySwitchChangePreview previewPowerSwitchTopologyChange(
        CimdrawScene* scene,
        const QString& deviceId,
        int toSwitchPosition) const;
    PowerTopologyProtectionRangePreview previewPowerProtectionRange(
        CimdrawScene* scene,
        const QString& deviceId,
        int toSwitchPosition,
        const QString& sourceDeviceId) const;
    PowerTopologyOperationPreview previewPowerSwitchOperation(
        CimdrawScene* scene,
        const QString& deviceId,
        int toSwitchPosition,
        const QString& supplySourceDeviceId,
        const QString& supplyTargetDeviceId,
        const QString& groundSourceDeviceId) const;
};

#endif
