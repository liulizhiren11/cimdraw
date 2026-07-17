#include "CimProfileSet.h"

void CimProfileSet::addProfile(const QString& profileName)
{
    const QString normalized = profileName.trimmed();
    if (normalized.isEmpty() || profiles_.contains(normalized))
        return;
    profiles_.push_back(normalized);
}

bool CimProfileSet::contains(const QString& profileName) const
{
    return profiles_.contains(profileName.trimmed());
}

QStringList CimProfileSet::allProfiles() const
{
    return profiles_;
}
