#include "CimdrawSceneDocumentController.h"

#include <QDomDocument>
#include <QFile>

#include "CimdrawFile.h"
#include "CimdrawScene.h"
#include "CimdrawView.h"

bool CimdrawSceneDocumentController::load(CimdrawScene* scene,
                                     const QString& filename,
                                     QString* inoutErrorMsg) const
{
    if (!scene)
        return false;

    QDomDocument domDoc;
    if (!toDomDocument(filename, &domDoc, inoutErrorMsg))
        return false;

    CimdrawFile file(filename);
    return file.loadByXml(&domDoc, scene);
}

bool CimdrawSceneDocumentController::toDomDocument(const QString& filename,
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

bool CimdrawSceneDocumentController::save(CimdrawScene* scene, const QString& filename) const
{
    if (!scene || filename.isEmpty())
        return false;

    CimdrawFile file(filename);
    const bool ok = file.save(scene);
    if (ok)
    {
        if (CimdrawView* view = scene->getView())
            view->setModified(false);
    }
    return ok;
}

FILE_TYPE CimdrawSceneDocumentController::sceneType(FILE_TYPE currentType) const
{
    return currentType;
}

void CimdrawSceneDocumentController::setSceneType(FILE_TYPE* currentType, FILE_TYPE newType) const
{
    if (!currentType)
        return;
    *currentType = newType;
}
