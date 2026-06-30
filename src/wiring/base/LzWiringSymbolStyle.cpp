#include "LzWiringSymbolStyle.h"

LzWiringSymbolStyle& LzWiringSymbolStyle::instance()
{
    static LzWiringSymbolStyle inst;
    return inst;
}

LzWiringSymbolStyle::LzWiringSymbolStyle(QObject* parent)
    : QObject(parent)
{
}

LzWiringSymbolStandard LzWiringSymbolStyle::standard() const
{
    return standard_;
}

void LzWiringSymbolStyle::setStandard(LzWiringSymbolStandard s)
{
    if (standard_ == s)
        return;
    standard_ = s;
    emit standardChanged(standard_);
}

QString LzWiringSymbolStyle::symbolPackId() const
{
    return symbolPackId_;
}

void LzWiringSymbolStyle::setSymbolPackId(const QString& id)
{
    symbolPackId_ = id;
}
