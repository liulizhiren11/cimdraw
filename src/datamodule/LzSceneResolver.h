#ifndef LZSCENERESOLVER_H
#define LZSCENERESOLVER_H

#include <functional>

class LzScene;

/** 由主窗口提供，用于在收到数据时解析当前应写入的 LzScene（通常为当前标签页） */
using LzSceneResolver = std::function<LzScene*()>;

#endif
