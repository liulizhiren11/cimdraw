#ifndef CIMDRAWFILE_H
#define CIMDRAWFILE_H

#include <QScopedPointer>
#include <QString>
#include "CimdrawViewConfig.h"

class QString;
class CimdrawFilePrivate;
class CimdrawScene;
class QDomDocument;

class CimdrawFile
{
public:
    explicit CimdrawFile(const QString& filePath);

    virtual ~CimdrawFile();

    virtual bool save(CimdrawScene* scene);

    virtual bool saveAsXml(CimdrawScene* scene);

    /// 与 saveAsXml 相同内容，便于与 PDW 命名约定对齐（扩展名可为 .pdw）
    bool savePdw(CimdrawScene* scene);

    virtual bool load(QDomDocument* dom,QString* errMsg = nullptr);

    virtual bool loadByXml(QDomDocument* dom, CimdrawScene* scene);

    /// 从 .pdw/.xml 读入 DOM 并调用 loadByXml
    bool loadPdw(const QString& filePath, CimdrawScene* scene, QString* errMsg = nullptr);

    SAVE_FILE_SUFFIX getFileTypeByName(const QString& fileName);

private:
    Q_DECLARE_PRIVATE(CimdrawFile);
    QScopedPointer<CimdrawFilePrivate> d_ptr;
};
#endif // CIMDRAWFILE_H
