#pragma once
#include <unordered_map>
#include <memory>
#include <vector>
#include "Engine/Core/Window/window.h"
#include "Engine/Core/Event/events.h" // for EventCallBackHandle

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

    struct WindowZOrder
    {
        std::shared_ptr<ConsoleGraphX::AbstractWindow> window;
        int zOrder = 0;
    };

    struct Offset
    {
        int m_xOffset = 0; // pixels
        int m_yOffset = 0; // pixels
    };

    enum class ZOrder
    {
        Top,
        Bottom,
        TopMost,
        BottomMost,
        Above,
        Below,
        None
    };

    struct WindowPositioningRule
    {
        std::weak_ptr<AbstractWindow> referenceWindow;

        Anchor anchor = Anchor::TopLeft;
        Alignment alignment = Alignment::Centered;
        Offset offset{};
        ZOrder zOrder = ZOrder::None;
    };

    bool AdjustZOrder(std::vector<WindowZOrder>& windows);

    class WindowLayout
    {
    public:
        void AddWindow(std::shared_ptr<AbstractWindow> window, const WindowPositioningRule& rule);
        void ApplyLayout();

    private:
        void _RemoveWindow(AbstractWindow* window);

    private:
        struct LayoutEntry
        {
            WindowPositioningRule rule{};
            ConsoleGraphX::CGXEventArgs<ConsoleGraphX::AbstractWindow*>::HandleType destroyedHandle{};
        };


        std::unordered_map<std::shared_ptr<AbstractWindow>, LayoutEntry> _m_positioningRules;
    };
}
