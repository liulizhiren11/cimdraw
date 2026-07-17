#ifndef CIMDRAWWIRINGDRAWPALETTE_H
#define CIMDRAWWIRINGDRAWPALETTE_H

#include "CimdrawWiringTypes.h"

#include <QColor>

/**
 * 接线图 / 单线图配色定义：
 * 正常态使用偏冷的钢蓝色导体，带电 / 失电 / 告警保持清晰语义区分，
 * 母线使用偏黄铜色调。
 */
namespace CimdrawWiringDrawPalette {

inline QColor conductorStroke(CimdrawWiringRunState s)
{
    switch (s)
    {
    case CimdrawWiringRunState::Energized:
        return QColor(0, 142, 82);
    case CimdrawWiringRunState::Deenergized:
        return QColor(118, 128, 148);
    case CimdrawWiringRunState::Alarm:
        return QColor(220, 58, 46);
    default:
        return QColor(48, 82, 138);
    }
}

inline QColor alarm()
{
    return conductorStroke(CimdrawWiringRunState::Alarm);
}

inline QColor busbarFill(CimdrawWiringRunState s, bool alarmPulse)
{
    switch (s)
    {
    case CimdrawWiringRunState::Energized:
        return QColor(78, 148, 108);
    case CimdrawWiringRunState::Deenergized:
        return QColor(152, 158, 172);
    case CimdrawWiringRunState::Alarm:
        return alarmPulse ? QColor(228, 76, 58) : QColor(168, 58, 52);
    default:
        return QColor(206, 168, 92);
    }
}

/** 工具箱图标预览色，和正常态导体颜色保持一致 */
inline QColor iconStroke()
{
    return conductorStroke(CimdrawWiringRunState::Normal);
}

inline QColor earthGround()
{
    return QColor(118, 86, 58);
}

inline QColor solarAccent()
{
    return QColor(240, 168, 38);
}

inline QColor windAccent()
{
    return QColor(58, 128, 196);
}

inline QColor svgWave()
{
    return QColor(32, 162, 178);
}

/** 电缆折线段强调色；水平引线保持导体色，告警态使用导体红色 */
inline QColor cableZigAccent(CimdrawWiringRunState s)
{
    switch (s)
    {
    case CimdrawWiringRunState::Alarm:
        return conductorStroke(s);
    case CimdrawWiringRunState::Energized:
        return QColor(0, 148, 128);
    case CimdrawWiringRunState::Deenergized:
        return QColor(108, 148, 168);
    default:
        return QColor(52, 148, 188);
    }
}

inline QColor essPlus()
{
    return QColor(34, 132, 86);
}

inline QColor essMinus()
{
    return QColor(72, 108, 188);
}

inline QColor capacitorPlate(CimdrawWiringRunState s)
{
    switch (s)
    {
    case CimdrawWiringRunState::Alarm:
        return conductorStroke(s);
    case CimdrawWiringRunState::Energized:
        return QColor(28, 112, 188);
    default:
        return QColor(58, 124, 198);
    }
}

inline QColor reactorCoil(CimdrawWiringRunState s)
{
    switch (s)
    {
    case CimdrawWiringRunState::Alarm:
        return conductorStroke(s);
    case CimdrawWiringRunState::Energized:
        return QColor(128, 92, 178);
    default:
        return QColor(104, 82, 148);
    }
}

inline QColor generatorFill()
{
    return QColor(255, 238, 210);
}

} // 接线图配色命名空间 CimdrawWiringDrawPalette

#endif // CIMDRAWWIRINGDRAWPALETTE_H
