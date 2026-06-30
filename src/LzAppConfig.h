#ifndef LZAPPCONFIG_H
#define LZAPPCONFIG_H

#include "wiring/base/LzWiringTypes.h"

class QString;
class QJsonObject;

/** 从 lz_data.json 读取/写入应用级配置（与数据源配置同文件） */
namespace LzAppConfig {

/** 从根对象应用 wiring.symbolStandard；缺省为 GB */
void applyFromJsonRoot(const QJsonObject& root);

/** 将当前符号标准写入配置文件（保留 sources 等其它字段） */
bool saveWiringSymbolStandard(const QString& absolutePath, LzWiringSymbolStandard standard);

QString symbolStandardToString(LzWiringSymbolStandard standard);
LzWiringSymbolStandard symbolStandardFromString(const QString& text, bool* ok = nullptr);

/** 界面显示名 */
QString symbolStandardDisplayName(LzWiringSymbolStandard standard);

} // namespace LzAppConfig

#endif
