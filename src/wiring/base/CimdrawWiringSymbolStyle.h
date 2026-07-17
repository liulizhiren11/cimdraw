#ifndef CIMDRAWWIRINGSYMBOLSTYLE_H
#define CIMDRAWWIRINGSYMBOLSTYLE_H

#include "CimdrawWiringTypes.h"

#include <QObject>
#include <QString>

/**
 * 全局接线图符号标准（美标 / IEC 欧标 / 国标）与「符号包」标识预留。
 *
 * - 默认值来自 cimdraw_data.json 的 wiring.symbolStandard；
 * - 主界面工具栏切换后写回配置文件并发出 standardChanged。
 */
class CimdrawWiringSymbolStyle : public QObject
{
    Q_OBJECT
public:
    static CimdrawWiringSymbolStyle& instance();

    CimdrawWiringSymbolStandard standard() const;
    void setStandard(CimdrawWiringSymbolStandard s);

    QString symbolPackId() const;
    void setSymbolPackId(const QString& id);

signals:
    void standardChanged(CimdrawWiringSymbolStandard standard);

private:
    explicit CimdrawWiringSymbolStyle(QObject* parent = nullptr);

    CimdrawWiringSymbolStandard standard_ = CimdrawWiringSymbolStandard::GB;
    QString symbolPackId_ = QStringLiteral("builtin.default");
};

#endif // CIMDRAWWIRINGSYMBOLSTYLE_H
