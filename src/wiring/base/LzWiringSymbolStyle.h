#ifndef LZWIRINGSYMBOLSTYLE_H
#define LZWIRINGSYMBOLSTYLE_H

#include "LzWiringTypes.h"

#include <QObject>
#include <QString>

/**
 * 全局接线图符号标准（美标 / IEC 欧标 / 国标）与「符号包」标识预留。
 *
 * - 默认值来自 lz_data.json 的 wiring.symbolStandard；
 * - 主界面工具栏切换后写回配置文件并发出 standardChanged。
 */
class LzWiringSymbolStyle : public QObject
{
    Q_OBJECT
public:
    static LzWiringSymbolStyle& instance();

    LzWiringSymbolStandard standard() const;
    void setStandard(LzWiringSymbolStandard s);

    QString symbolPackId() const;
    void setSymbolPackId(const QString& id);

signals:
    void standardChanged(LzWiringSymbolStandard standard);

private:
    explicit LzWiringSymbolStyle(QObject* parent = nullptr);

    LzWiringSymbolStandard standard_ = LzWiringSymbolStandard::GB;
    QString symbolPackId_ = QStringLiteral("builtin.default");
};

#endif // LZWIRINGSYMBOLSTYLE_H
