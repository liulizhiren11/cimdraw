#ifndef FRAMEDOCUMENTCONTROLLER_H
#define FRAMEDOCUMENTCONTROLLER_H

#include <functional>

class QString;
class QWidget;
class LzCenterWidget;
class LzScene;
class LzView;

class FrameDocumentController
{
public:
    struct NewSceneContext
    {
        LzCenterWidget* centerWidget = nullptr;
        std::function<void(LzView*, LzScene*)> afterCreate;
        std::function<void(const QString&)> renameCurrentTab;
    };

    struct OpenSceneContext
    {
        QWidget* host = nullptr;
        std::function<LzView*()> createEmptyScene;
        std::function<void(const QString&)> renameCurrentTab;
        std::function<void()> afterOpen;
    };

    LzView* createPowerScene(const NewSceneContext& context) const;
    LzView* openScene(const QString& fileName, const OpenSceneContext& context) const;
    bool chooseAndOpenScene(QWidget* host, const OpenSceneContext& context) const;
    bool finalizeOpenedScene(LzView* view,
                             const QString& fileNamePath,
                             const std::function<void(const QString&)>& renameCurrentTab,
                             const std::function<void()>& afterOpen) const;
};

#endif
