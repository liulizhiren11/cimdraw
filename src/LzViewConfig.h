#ifndef LZVIEWCONFIG_H
#define LZVIEWCONFIG_H

//保存文件格式
using SAVE_FILE_SUFFIX = enum
{
    LZ_UNKNOWN = -1,
    LZ_XML = 0,
    LZ_PDW = 1,
};

//视图模式
using FILE_TYPE = enum
{
    //绘制模式
    LZ_DRAW = 0,
    //展示模式
    LZ_DISPLAY,
    //动态模式
    LZ_DYNAMIC,
};

#define FILE_PATH    "data/file"
#define FILE_IMAGE   "data/image"

#endif
