#include "CimdrawSceneStateController.h"

#include "CimdrawView.h"

CimdrawView* CimdrawSceneStateController::view(CimdrawView* currentView) const
{
    return currentView;
}

void CimdrawSceneStateController::setView(CimdrawView*& currentView, CimdrawView* nextView) const
{
    if (nextView)
        currentView = nextView;
}

bool CimdrawSceneStateController::paintState(bool currentState) const
{
    return currentState;
}

void CimdrawSceneStateController::setPaintState(bool& currentState, bool nextState) const
{
    currentState = nextState;
}

bool CimdrawSceneStateController::interactiveTransformActive(bool currentState) const
{
    return currentState;
}

void CimdrawSceneStateController::setInteractiveTransformActive(bool& currentState, bool nextState) const
{
    currentState = nextState;
}

bool CimdrawSceneStateController::setWiringData(QHash<QString, QVariant>& wiringData,
                                           const QString& key,
                                           const QVariant& value) const
{
    if (key.isEmpty())
        return false;
    wiringData.insert(key, value);
    return true;
}

QVariant CimdrawSceneStateController::wiringData(const QHash<QString, QVariant>& wiringData,
                                            const QString& key) const
{
    return wiringData.value(key);
}
