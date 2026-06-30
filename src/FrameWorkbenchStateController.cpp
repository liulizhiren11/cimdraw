#include "FrameWorkbenchStateController.h"

#include <QComboBox>
#include <QLineEdit>
#include <QTabWidget>

#include "LzScene.h"
#include "LzView.h"
#include "wiring/base/LzWiringItemBase.h"
#include "wiring/base/LzWiringTypes.h"

void FrameWorkbenchStateController::syncWiringStandardCombo(QComboBox* combo,
                                                            LzWiringSymbolStandard standard) const
{
    if (!combo)
        return;

    const int want = static_cast<int>(standard);
    for (int i = 0; i < combo->count(); ++i)
    {
        if (combo->itemData(i).toInt() == want)
        {
            combo->setCurrentIndex(i);
            break;
        }
    }
}

void FrameWorkbenchStateController::refreshAllWiringItemPaint(QTabWidget* tabs) const
{
    if (!tabs)
        return;

    for (int i = 0; i < tabs->count(); ++i)
    {
        auto* view = qobject_cast<LzView*>(tabs->widget(i));
        if (!view || !view->getScene())
            continue;

        const QList<QGraphicsItem*> items = view->getScene()->items();
        for (QGraphicsItem* item : items)
        {
            if (auto* wiringItem = dynamic_cast<LzWiringItemBase*>(item))
                wiringItem->update();
        }
    }
}

void FrameWorkbenchStateController::syncPointerPosition(QLineEdit* xLineEdit,
                                                        QLineEdit* yLineEdit,
                                                        const QPointF& point) const
{
    if (xLineEdit)
        xLineEdit->setText(QStringLiteral("x=") + QString::number(point.x()));
    if (yLineEdit)
        yLineEdit->setText(QStringLiteral("y=") + QString::number(point.y()));
}

LzView* FrameWorkbenchStateController::currentViewFromTabs(QTabWidget* tabs) const
{
    if (!tabs)
        return nullptr;
    return qobject_cast<LzView*>(tabs->currentWidget());
}
