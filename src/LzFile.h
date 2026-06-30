#ifndef LZFILE_H
#define LZFILE_H

#include <QScopedPointer>
#include <QString>
#include "LzViewConfig.h"

class QString;
class LzFilePrivate;
class LzScene;
class QDomDocument;

class LzFile
{
public:
    explicit LzFile(const QString& filePath);

    virtual ~LzFile();

    virtual bool save(LzScene* scene);

    virtual bool saveAsXml(LzScene* scene);

    /// 与 saveAsXml 相同内容，便于与 PDW 命名约定对齐（扩展名可为 .pdw）
    bool savePdw(LzScene* scene);

    virtual bool load(QDomDocument* dom,QString* errMsg = nullptr);

    virtual bool loadByXml(QDomDocument* dom, LzScene* scene);

    /// 从 .pdw/.xml 读入 DOM 并调用 loadByXml
    bool loadPdw(const QString& filePath, LzScene* scene, QString* errMsg = nullptr);

    SAVE_FILE_SUFFIX getFileTypeByName(const QString& fileName);

private:
    Q_DECLARE_PRIVATE(LzFile);
    QScopedPointer<LzFilePrivate> d_ptr;
};
#endif // LZFILE_H
