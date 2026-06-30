#include "LzSceneDocumentController.h"

#include <QDomDocument>
#include <QFile>

#include "LzFile.h"
#include "LzScene.h"
#include "LzView.h"

bool LzSceneDocumentController::load(LzScene* scene,
                                     const QString& filename,
                                     QString* inoutErrorMsg) const
{
    if (!scene)
        return false;

    QDomDocument domDoc;
    if (!toDomDocument(filename, &domDoc, inoutErrorMsg))
        return false;

    LzFile file(filename);
    return file.loadByXml(&domDoc, scene);
}

bool LzSceneDocumentController::toDomDocument(const QString& filename,
                                              QDomDocument* dom,
                                              QString* inoutErrorMsg) const
{
    if (!dom)
        return false;

    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        if (inoutErrorMsg)
            *inoutErrorMsg = QStringLiteral("无法打开文件");
        return false;
    }

    const QByteArray data = file.readAll();
    file.close();

    QString parseError;
    int errorLine = 0;
    int errorColumn = 0;
    if (!dom->setContent(data, &parseError, &errorLine, &errorColumn))
    {
        if (inoutErrorMsg)
        {
            *inoutErrorMsg = QStringLiteral("%1 (%2,%3)")
                                 .arg(parseError)
                                 .arg(errorLine)
                                 .arg(errorColumn);
        }
        return false;
    }
    return true;
}

bool LzSceneDocumentController::save(LzScene* scene, const QString& filename) const
{
    if (!scene || filename.isEmpty())
        return false;

    LzFile file(filename);
    const bool ok = file.save(scene);
    if (ok)
    {
        if (LzView* view = scene->getView())
            view->setModified(false);
    }
    return ok;
}

FILE_TYPE LzSceneDocumentController::sceneType(FILE_TYPE currentType) const
{
    return currentType;
}

void LzSceneDocumentController::setSceneType(FILE_TYPE* currentType, FILE_TYPE newType) const
{
    if (!currentType)
        return;
    *currentType = newType;
}
