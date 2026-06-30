#ifndef FRAMESHORTCUTBUILDER_H
#define FRAMESHORTCUTBUILDER_H

class QWidget;
class LzFrame;

class FrameShortcutBuilder
{
public:
    void installCommonShortcuts(QWidget* host, LzFrame* frame) const;
};

#endif
