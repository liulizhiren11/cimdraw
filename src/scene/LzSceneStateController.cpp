#include "LzSceneStateController.h"

#include "LzView.h"

LzView* LzSceneStateController::view(LzView* currentView) const
{
    return currentView;
}

void LzSceneStateController::setView(LzView*& currentView, LzView* nextView) const
{
    if (nextView)
        currentView = nextView;
}

bool LzSceneStateController::paintState(bool currentState) const
{
    return currentState;
}

void LzSceneStateController::setPaintState(bool& currentState, bool nextState) const
{
    currentState = nextState;
}

bool LzSceneStateController::interactiveTransformActive(bool currentState) const
{
    return currentState;
}

void LzSceneStateController::setInteractiveTransformActive(bool& currentState, bool nextState) const
{
    currentState = nextState;
}

bool LzSceneStateController::setWiringData(QHash<QString, QVariant>& wiringData,
                                           const QString& key,
                                           const QVariant& value) const
{
    if (key.isEmpty())
        return false;
    wiringData.insert(key, value);
    return true;
}

QVariant LzSceneStateController::wiringData(const QHash<QString, QVariant>& wiringData,
                                            const QString& key) const
{
    return wiringData.value(key);
}
