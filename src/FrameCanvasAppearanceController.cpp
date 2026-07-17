#include "FrameCanvasAppearanceController.h"

#include <QColor>
#include <QCoreApplication>
#include <QFileDialog>
#include <QColorDialog>
#include <QPalette>
#include <QPushButton>
#include <QTabWidget>
#include <QWidget>

#include "CimdrawView.h"
#include "CimdrawViewConfig.h"

CimdrawView* FrameCanvasAppearanceController::currentView(QTabWidget* tabs) const
{
    if (!tabs)
        return nullptr;
    return qobject_cast<CimdrawView*>(tabs->currentWidget());
}

bool FrameCanvasAppearanceController::applyBackgroundColor(CimdrawView* view,
                                                           QPushButton* button,
                                                           const QColor& color) const
{
    if (!view || !button || !color.isValid())
        return false;

    QPalette palette = button->palette();
    palette.setColor(QPalette::Button, color);
    button->setPalette(palette);
    view->setBackgroundCol(color);
    return true;
}

bool FrameCanvasAppearanceController::applyBackgroundColorToCurrentView(QTabWidget* tabs,
                                                                        QPushButton* button,
                                                                        const QColor& color) const
{
    return applyBackgroundColor(currentView(tabs), button, color);
}

bool FrameCanvasAppearanceController::applyBackgroundImage(CimdrawView* view, const QString& imagePath) const
{
    if (!view || imagePath.trimmed().isEmpty())
        return false;

    view->setBackgroundImage(imagePath);
    return true;
}

bool FrameCanvasAppearanceController::applyBackgroundImageToCurrentView(QTabWidget* tabs,
                                                                        const QString& imagePath) const
{
    return applyBackgroundImage(currentView(tabs), imagePath);
}

bool FrameCanvasAppearanceController::chooseAndApplyBackgroundColor(QWidget* host,
                                                                    QTabWidget* tabs,
                                                                    QPushButton* button) const
{
    QColor color;
    color = QColorDialog::getColor(color, host, QObject::tr("画布颜色"));
    if (!color.isValid())
        return false;

    return applyBackgroundColorToCurrentView(tabs, button, color);
}

bool FrameCanvasAppearanceController::chooseAndApplyBackgroundImage(QWidget* host, QTabWidget* tabs) const
{
    const QString filePathName = defaultImageDirectory(host);
    const QString openFileName = QFileDialog::getOpenFileName(host,
                                                              QObject::tr("打开文件"),
                                                              filePathName,
                                                              QObject::tr("Images Files(*.png *.jpg *.jpeg *.bmp)"));
    if (openFileName.isEmpty())
        return false;

    return applyBackgroundImageToCurrentView(tabs, openFileName);
}

QString FrameCanvasAppearanceController::defaultImageDirectory(const QWidget* host) const
{
    Q_UNUSED(host);
    return QStringLiteral("%1/../../%2/").arg(QCoreApplication::applicationDirPath()).arg(FILE_IMAGE);
}

void FrameCanvasAppearanceController::syncBackgroundButtonFromCurrentView(QTabWidget* tabs,
                                                                          QPushButton* button) const
{
    if (!button)
        return;

    QColor color = Qt::white;
    if (CimdrawView* view = currentView(tabs))
    {
        color = view->getBackgroundCol();
        if (!color.isValid())
            color = Qt::white;
    }

    QPalette palette = button->palette();
    palette.setColor(QPalette::Button, color);
    button->setPalette(palette);
}
