#include "FrameWorkbenchStateController.h"

#include <QComboBox>
#include <QLineEdit>
#include <QTabWidget>

#include "CimdrawScene.h"
#include "CimdrawView.h"
#include "wiring/base/CimdrawWiringItemBase.h"
#include "wiring/base/CimdrawWiringTypes.h"

void FrameWorkbenchStateController::syncWiringStandardCombo(QComboBox* combo,
                                                            CimdrawWiringSymbolStandard standard) const
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
        auto* view = qobject_cast<CimdrawView*>(tabs->widget(i));
        if (!view || !view->getScene())
            continue;

        const QList<QGraphicsItem*> items = view->getScene()->items();
        for (QGraphicsItem* item : items)
        {
            if (auto* wiringItem = dynamic_cast<CimdrawWiringItemBase*>(item))
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

CimdrawView* FrameWorkbenchStateController::currentViewFromTabs(QTabWidget* tabs) const
{
    if (!tabs)
        return nullptr;
    return qobject_cast<CimdrawView*>(tabs->currentWidget());
}
