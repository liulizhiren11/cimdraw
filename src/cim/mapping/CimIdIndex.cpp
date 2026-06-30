#include "CimIdIndex.h"

void CimIdIndex::bindGraphicItem(const QString& mrid, const QString& graphicItemId)
{
    const QString normalizedMrid = mrid.trimmed();
    const QString normalizedGraphicId = graphicItemId.trimmed();
    if (normalizedMrid.isEmpty() || normalizedGraphicId.isEmpty())
        return;

    mridToGraphicItem_.insert(normalizedMrid, normalizedGraphicId);
    graphicItemToMrid_.insert(normalizedGraphicId, normalizedMrid);
}

QString CimIdIndex::graphicItemIdForMrid(const QString& mrid) const
{
    return mridToGraphicItem_.value(mrid.trimmed());
}

QString CimIdIndex::mridForGraphicItemId(const QString& graphicItemId) const
{
    return graphicItemToMrid_.value(graphicItemId.trimmed());
}

bool CimIdIndex::hasBinding(const QString& mrid) const
{
    return mridToGraphicItem_.contains(mrid.trimmed());
}

int CimIdIndex::bindingCount() const
{
    return mridToGraphicItem_.size();
}
