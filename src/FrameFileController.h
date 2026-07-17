#ifndef FRAMEFILECONTROLLER_H
#define FRAMEFILECONTROLLER_H

#include <functional>
#include <QString>

class QWidget;
class CimdrawScene;
class CimdrawView;

class FrameFileController
{
public:
    struct SaveResult
    {
        bool success = false;
        bool changed = false;
        QString tabName;
    };

    SaveResult saveCurrentView(CimdrawView* view) const;
    SaveResult saveCurrentViewAs(QWidget* host, CimdrawView* view, CimdrawScene* scene) const;
};

#endif
