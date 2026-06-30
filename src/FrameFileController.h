#ifndef FRAMEFILECONTROLLER_H
#define FRAMEFILECONTROLLER_H

#include <functional>
#include <QString>

class QWidget;
class LzScene;
class LzView;

class FrameFileController
{
public:
    struct SaveResult
    {
        bool success = false;
        bool changed = false;
        QString tabName;
    };

    SaveResult saveCurrentView(LzView* view) const;
    SaveResult saveCurrentViewAs(QWidget* host, LzView* view, LzScene* scene) const;
};

#endif
