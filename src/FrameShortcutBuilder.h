#ifndef FRAMESHORTCUTBUILDER_H
#define FRAMESHORTCUTBUILDER_H

class QWidget;
class CimdrawFrame;

class FrameShortcutBuilder
{
public:
    void installCommonShortcuts(QWidget* host, CimdrawFrame* frame) const;
};

#endif
