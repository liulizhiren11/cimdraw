#ifndef LZATTRIBUTEMANAGER_H
#define LZATTRIBUTEMANAGER_H

#include <QObject>
#include <QScopedPointer>
#include <QColor>
#include <QFont>

class LzAttributeManager
{
public:
    static LzAttributeManager* getInstance();
    ~LzAttributeManager();
    static inline bool hoverSize_ = false;

    static inline bool usePen_ = true;
    static inline QColor penColor_ = QColor(Qt::black);
    static inline int penWidth_ = 2;
    static inline int penStyle_ = Qt::PenStyle(Qt::SolidLine);

    static inline bool useBrush_ = true;
    static inline QColor brushColor_ = QColor(Qt::white);
    static inline int brushStyle_ = Qt::BrushStyle(Qt::SolidPattern);

    static inline bool useFont_ = false;
    static inline QFont fontStyle_ = QFont();
private:
    static LzAttributeManager* attributeManager_;
    LzAttributeManager();
};

#endif
