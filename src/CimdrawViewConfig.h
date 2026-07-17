#ifndef CIMDRAWVIEWCONFIG_H
#define CIMDRAWVIEWCONFIG_H

//保存文件格式
using SAVE_FILE_SUFFIX = enum
{
    CIMDRAW_UNKNOWN = -1,
    CIMDRAW_XML = 0,
    CIMDRAW_PDW = 1,
};

//视图模式
using FILE_TYPE = enum
{
    //绘制模式
    CIMDRAW_DRAW = 0,
    //展示模式
    CIMDRAW_DISPLAY,
    //动态模式
    CIMDRAW_DYNAMIC,
};

#define FILE_PATH    "data/file"
#define FILE_IMAGE   "data/image"

#endif
