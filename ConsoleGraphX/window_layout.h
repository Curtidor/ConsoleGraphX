#pragma once
#include <unordered_map>
#include "window.h"

namespace ConsoleGraphX
{
    enum class Anchor 
    {
        None,
        TopLeft,
        TopCenter,
        TopRight,
        CenterLeft,
        Center,
        CenterRight,
        BottomLeft,
        BottomCenter,
        BottomRight
    };

    enum class Alignment 
    {
        None,
        Above,
        Below,
        LeftOf,
        RightOf,
        Centered
    };

    struct Offset 
    {
        int m_xOffset; // Horizontal offset in pixels
        int m_yOffset; // Vertical offset in pixels
    };

    struct WindowPositioningRule
    {
        AbstractWindow* referenceWindow;
        Anchor anchor;
        Alignment alignment;
        Offset offset;
    };

    class WindowLayout
    {
    public:
        void AddWindow(AbstractWindow* window, const WindowPositioningRule& rule);
        void ApplyLayout();

    private:
        void _RemoveWindow(AbstractWindow* window);
    private:
        std::unordered_map<AbstractWindow*, WindowPositioningRule> _m_positioningRules;
    };
};
