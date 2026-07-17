#ifndef CIMSHUNTCOMPENSATORVIEW_H
#define CIMSHUNTCOMPENSATORVIEW_H

#include "CimEquipmentView.h"

#include <initializer_list>

class CimShuntCompensatorView : public CimEquipmentView
{
public:
    explicit CimShuntCompensatorView(const CimObject* object = nullptr);

    bool isShuntCompensatorFamily() const;
    bool looksLikeSvg() const;
    bool looksLikeArcCoil() const;
    bool looksLikeReactor() const;
    bool looksLikeSplitReactor() const;

private:
    double firstNumericAttribute(std::initializer_list<const char*> keys, bool* okOut = nullptr) const;
    int inferredSectionCount() const;
};

#endif // CIMSHUNTCOMPENSATORVIEW_H
