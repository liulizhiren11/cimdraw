#include "FrameShortcutBuilder.h"

#include <QKeySequence>
#include <QShortcut>
#include <QWidget>

#include "CimdrawFrame.h"

void FrameShortcutBuilder::installCommonShortcuts(QWidget* host, CimdrawFrame* frame) const
{
    if (!host || !frame)
        return;

    QShortcut* leftShortcut = new QShortcut(QKeySequence(Qt::Key_Left), host);
    QObject::connect(leftShortcut, &QShortcut::activated, frame, &CimdrawFrame::onShortcutLeft);

    QShortcut* rightShortcut = new QShortcut(QKeySequence(Qt::Key_Right), host);
    QObject::connect(rightShortcut, &QShortcut::activated, frame, &CimdrawFrame::onShortcutRight);

    QShortcut* upShortcut = new QShortcut(QKeySequence(Qt::Key_Up), host);
    QObject::connect(upShortcut, &QShortcut::activated, frame, &CimdrawFrame::onShortcutUp);

    QShortcut* downShortcut = new QShortcut(QKeySequence(Qt::Key_Down), host);
    QObject::connect(downShortcut, &QShortcut::activated, frame, &CimdrawFrame::onShortcutDown);
}
