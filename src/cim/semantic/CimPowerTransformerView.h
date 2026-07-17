#ifndef CIMPOWERTRANSFORMERVIEW_H
#define CIMPOWERTRANSFORMERVIEW_H

#include "CimEquipmentView.h"

class CimPowerTransformerView : public CimEquipmentView
{
public:
    explicit CimPowerTransformerView(const CimObject* object = nullptr);

    bool isPowerTransformer() const;
    int transformerEndCount() const;
    bool looksLikeAutoTransformer() const;
    bool looksLikeEarthingTransformer() const;
    bool looksLikeStationTransformer() const;
};

#endif // CIMPOWERTRANSFORMERVIEW_H
