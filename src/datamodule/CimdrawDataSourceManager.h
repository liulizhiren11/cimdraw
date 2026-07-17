#ifndef CIMDRAWDATASOURCEMANAGER_H
#define CIMDRAWDATASOURCEMANAGER_H

#include "CimdrawSceneResolver.h"

#include <QList>
#include <QObject>
#include <QString>

class CimdrawIDataSource;

/**
 * @brief 从 cimdraw_data.json 加载并管理多路数据源。
 *
 * 默认配置文件路径：与可执行文件同目录下的 cimdraw_data.json。
 * 若文件不存在或解析失败，不创建任何源（不视为致命错误）。
 */
class CimdrawDataSourceManager : public QObject
{
public:
    explicit CimdrawDataSourceManager(QObject* parent = nullptr);
    ~CimdrawDataSourceManager() override;

    void setSceneResolver(CimdrawSceneResolver resolver);

    /** 读取 JSON 配置并重建数据源列表；会先 stop 并销毁旧源 */
    bool loadFromFile(const QString& absolutePath);

    void startAll();
    void stopAll();

    QString lastConfigPath() const { return lastConfigPath_; }
    QString lastError() const { return lastError_; }

private:
    void clearSources();

    CimdrawSceneResolver sceneResolver_;
    QList<CimdrawIDataSource*> sources_;
    QString lastConfigPath_;
    QString lastError_;
};

#endif
