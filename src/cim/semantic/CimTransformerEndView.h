#ifndef CIMTRANSFORMERENDVIEW_H
#define CIMTRANSFORMERENDVIEW_H

#include "CimIdentifiedObjectView.h"

class CimTransformerEndView : public CimIdentifiedObjectView
{
public:
    explicit CimTransformerEndView(const CimObject* object = nullptr);

    bool isTransformerEnd() const;
    QString powerTransformerMrid() const;
    QString terminalMrid() const;
    int endNumber() const;
};

#endif // CIMTRANSFORMERENDVIEW_H
