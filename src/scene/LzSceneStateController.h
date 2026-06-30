#ifndef LZSCENESTATECONTROLLER_H
#define LZSCENESTATECONTROLLER_H

#include <QHash>
#include <QVariant>

class LzView;

class LzSceneStateController
{
public:
    LzView* view(LzView* currentView) const;
    void setView(LzView*& currentView, LzView* nextView) const;

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
