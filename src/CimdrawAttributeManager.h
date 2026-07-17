#ifndef CIMDRAWATTRIBUTEMANAGER_H
#define CIMDRAWATTRIBUTEMANAGER_H

#include <QObject>
#include <QScopedPointer>
#include <QColor>
#include <QFont>

class CimdrawAttributeManager
{
public:
    static CimdrawAttributeManager* getInstance();
    ~CimdrawAttributeManager();
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
    static CimdrawAttributeManager* attributeManager_;
    CimdrawAttributeManager();
};

#endif
