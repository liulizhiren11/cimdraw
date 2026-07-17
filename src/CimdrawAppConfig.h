#ifndef CIMDRAWAPPCONFIG_H
#define CIMDRAWAPPCONFIG_H

#include "wiring/base/CimdrawWiringTypes.h"

class QString;
class QJsonObject;

/** 从 cimdraw_data.json 读取/写入应用级配置（与数据源配置同文件） */
namespace CimdrawAppConfig {

/** 从根对象应用 wiring.symbolStandard；缺省为 GB */
void applyFromJsonRoot(const QJsonObject& root);

/** 将当前符号标准写入配置文件（保留 sources 等其它字段） */
bool saveWiringSymbolStandard(const QString& absolutePath, CimdrawWiringSymbolStandard standard);

QString symbolStandardToString(CimdrawWiringSymbolStandard standard);
CimdrawWiringSymbolStandard symbolStandardFromString(const QString& text, bool* ok = nullptr);

/** 界面显示名 */
QString symbolStandardDisplayName(CimdrawWiringSymbolStandard standard);

} // namespace CimdrawAppConfig

#endif
