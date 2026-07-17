#ifndef CIMDRAWSCENESTATECONTROLLER_H
#define CIMDRAWSCENESTATECONTROLLER_H

#include <QHash>
#include <QVariant>

class CimdrawView;

class CimdrawSceneStateController
{
public:
    CimdrawView* view(CimdrawView* currentView) const;
    void setView(CimdrawView*& currentView, CimdrawView* nextView) const;

    bool paintState(bool currentState) const;
    void setPaintState(bool& currentState, bool nextState) const;

    bool interactiveTransformActive(bool currentState) const;
    void setInteractiveTransformActive(bool& currentState, bool nextState) const;

    bool setWiringData(QHash<QString, QVariant>& wiringData,
                       const QString& key,
                       const QVariant& value) const;
    QVariant wiringData(const QHash<QString, QVariant>& wiringData,
                        const QString& key) const;
};

#endif
