#ifndef CIMSERIESCOMPENSATORVIEW_H
#define CIMSERIESCOMPENSATORVIEW_H

#include "CimEquipmentView.h"

class CimSeriesCompensatorView : public CimEquipmentView
{
public:
    explicit CimSeriesCompensatorView(const CimObject* object = nullptr);

    bool isSeriesCompensator() const;
    bool looksLikeCapacitor() const;
    bool looksLikeReactor() const;
};

#endif // CIMSERIESCOMPENSATORVIEW_H
