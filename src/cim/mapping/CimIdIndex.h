#ifndef CIMIDINDEX_H
#define CIMIDINDEX_H

#include <QHash>
#include <QString>

class CimIdIndex
{
public:
    void bindGraphicItem(const QString& mrid, const QString& graphicItemId);
    QString graphicItemIdForMrid(const QString& mrid) const;
    QString mridForGraphicItemId(const QString& graphicItemId) const;
    bool hasBinding(const QString& mrid) const;
    int bindingCount() const;

private:
    QHash<QString, QString> mridToGraphicItem_;
    QHash<QString, QString> graphicItemToMrid_;
};

#endif // CIMIDINDEX_H
