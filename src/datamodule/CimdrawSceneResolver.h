#ifndef CIMDRAWSCENERESOLVER_H
#define CIMDRAWSCENERESOLVER_H

#include <functional>

class CimdrawScene;

/** 由主窗口提供，用于在收到数据时解析当前应写入的 CimdrawScene（通常为当前标签页） */
using CimdrawSceneResolver = std::function<CimdrawScene*()>;

#endif
