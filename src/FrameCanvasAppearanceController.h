#ifndef FRAMECANVASAPPEARANCECONTROLLER_H
#define FRAMECANVASAPPEARANCECONTROLLER_H

#include <QString>

class QColor;
class QPushButton;
class QTabWidget;
class QWidget;
class CimdrawView;

class FrameCanvasAppearanceController
{
public:
    CimdrawView* currentView(QTabWidget* tabs) const;
    bool applyBackgroundColor(CimdrawView* view, QPushButton* button, const QColor& color) const;
    bool applyBackgroundColorToCurrentView(QTabWidget* tabs, QPushButton* button, const QColor& color) const;
    bool applyBackgroundImage(CimdrawView* view, const QString& imagePath) const;
    bool applyBackgroundImageToCurrentView(QTabWidget* tabs, const QString& imagePath) const;
    bool chooseAndApplyBackgroundColor(QWidget* host, QTabWidget* tabs, QPushButton* button) const;
    bool chooseAndApplyBackgroundImage(QWidget* host, QTabWidget* tabs) const;
    QString defaultImageDirectory(const QWidget* host) const;
    void syncBackgroundButtonFromCurrentView(QTabWidget* tabs, QPushButton* button) const;
};

#endif
