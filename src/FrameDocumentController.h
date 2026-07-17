#ifndef FRAMEDOCUMENTCONTROLLER_H
#define FRAMEDOCUMENTCONTROLLER_H

#include <functional>

class QString;
class QWidget;
class CimdrawCenterWidget;
class CimdrawScene;
class CimdrawView;

class FrameDocumentController
{
public:
    struct NewSceneContext
    {
        CimdrawCenterWidget* centerWidget = nullptr;
        std::function<void(CimdrawView*, CimdrawScene*)> afterCreate;
        std::function<void(const QString&)> renameCurrentTab;
    };

    struct OpenSceneContext
    {
        QWidget* host = nullptr;
        std::function<CimdrawView*()> createEmptyScene;
        std::function<void(const QString&)> renameCurrentTab;
        std::function<void()> afterOpen;
    };

    CimdrawView* createPowerScene(const NewSceneContext& context) const;
    CimdrawView* openScene(const QString& fileName, const OpenSceneContext& context) const;
    bool chooseAndOpenScene(QWidget* host, const OpenSceneContext& context) const;
    bool finalizeOpenedScene(CimdrawView* view,
                             const QString& fileNamePath,
                             const std::function<void(const QString&)>& renameCurrentTab,
                             const std::function<void()>& afterOpen) const;
};

#endif
