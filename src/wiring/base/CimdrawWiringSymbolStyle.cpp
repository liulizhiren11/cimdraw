#include "CimdrawWiringSymbolStyle.h"

CimdrawWiringSymbolStyle& CimdrawWiringSymbolStyle::instance()
{
    static CimdrawWiringSymbolStyle inst;
    return inst;
}

CimdrawWiringSymbolStyle::CimdrawWiringSymbolStyle(QObject* parent)
    : QObject(parent)
{
}

CimdrawWiringSymbolStandard CimdrawWiringSymbolStyle::standard() const
{
    return standard_;
}

void CimdrawWiringSymbolStyle::setStandard(CimdrawWiringSymbolStandard s)
{
    if (standard_ == s)
        return;
    standard_ = s;
    emit standardChanged(standard_);
}

QString CimdrawWiringSymbolStyle::symbolPackId() const
{
    return symbolPackId_;
}

void CimdrawWiringSymbolStyle::setSymbolPackId(const QString& id)
{
    symbolPackId_ = id;
}
