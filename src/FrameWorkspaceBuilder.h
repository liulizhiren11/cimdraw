#ifndef FRAMEWORKSPACEBUILDER_H
#define FRAMEWORKSPACEBUILDER_H

class QComboBox;
class QLineEdit;
class QMainWindow;
class QPushButton;
class QStatusBar;
class QToolBar;

class FrameWorkspaceBuilder
{
public:
    struct CanvasToolbarResult
    {
        QToolBar* toolBar = nullptr;
        QPushButton* backgroundColorButton = nullptr;
        QPushButton* backgroundImageButton = nullptr;
    };

    struct WiringToolbarResult
    {
        QToolBar* toolBar = nullptr;
        QComboBox* standardCombo = nullptr;
    };

    struct StatusBarResult
    {
        QStatusBar* statusBar = nullptr;
        QLineEdit* xLineEdit = nullptr;
        QLineEdit* yLineEdit = nullptr;
        QLineEdit* messageLineEdit = nullptr;
    };

    CanvasToolbarResult createCanvasToolbar(QMainWindow* host) const;
    WiringToolbarResult createWiringToolbar(QMainWindow* host) const;
    StatusBarResult createStatusBar(QMainWindow* host) const;
};

#endif
