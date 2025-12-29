#include "PCH_CGX.h"

#include <algorithm>
#include <iostream>
#include <limits>
#include <memory>
#include <vector>

#include "Engine/Core/Window/Layout/window_layout.h"
#include "Engine/Core/Window/window_manager.h"
#include "Engine/Core/Logger/logger_manager.h"
#include "../WinCore/WinCore.h"

namespace ConsoleGraphX
{
    void WindowLayout::AddWindow(std::shared_ptr<AbstractWindow> window, const WindowPositioningRule& rule)
    {
        if (!window)
            return;

        LayoutEntry entry{};
        entry.rule = rule;

        // IMPORTANT: store the handle so the subscription stays alive.
        entry.destroyedHandle =
            window->OnWindowDestroyed.AddListenerLambda([this](AbstractWindow* destroyed)
                {
                    this->_RemoveWindow(destroyed);
                });

        // Insert or replace
        _m_positioningRules[window] = std::move(entry);
    }

    void WindowLayout::ApplyLayout()
    {
        for (auto it = _m_positioningRules.begin(); it != _m_positioningRules.end(); )
        {
            auto& window = it->first;
            auto& entry = it->second;

            if (!window)
            {
                it = _m_positioningRules.erase(it);
                continue;
            }

            auto refWindow = entry.rule.referenceWindow.lock();
            if (!refWindow)
            {
                it = _m_positioningRules.erase(it);
                continue;
            }

            // These are your logical target sizes (whatever your engine defines them as)
            const Vector2i refSize = refWindow->GetTargetWindowSize();
            const Vector2i winSize = window->GetTargetWindowSize();

            // Get the reference window's OUTER rect position in pixels.
            int x = 0, y = 0, w = 0, h = 0;
            if (!WinCore::GetWindowOuterPosSizePx(refWindow->GetHWND(), x, y, w, h))
            {
                ++it;
                continue;
            }

            int32_t targetX = x;
            int32_t targetY = y;

            // Anchor
            switch (entry.rule.anchor)
            {
            case Anchor::TopLeft: break;

            case Anchor::TopCenter:
                targetX += (refSize.x - winSize.x) / 2;
                break;

            case Anchor::TopRight:
                targetX += (refSize.x - winSize.x);
                break;

            case Anchor::CenterLeft:
                targetY += (refSize.y - winSize.y) / 2;
                break;

            case Anchor::Center:
                targetX += (refSize.x - winSize.x) / 2;
                targetY += (refSize.y - winSize.y) / 2;
                break;

            case Anchor::CenterRight:
                targetX += (refSize.x - winSize.x);
                targetY += (refSize.y - winSize.y) / 2;
                break;

            case Anchor::BottomLeft:
                targetY += (refSize.y - winSize.y);
                break;

            case Anchor::BottomCenter:
                targetX += (refSize.x - winSize.x) / 2;
                targetY += (refSize.y - winSize.y);
                break;

            case Anchor::BottomRight:
                targetX += (refSize.x - winSize.x);
                targetY += (refSize.y - winSize.y);
                break;

            case Anchor::None:
            default:
                break;
            }

            // Alignment
            switch (entry.rule.alignment)
            {
            case Alignment::Above:    targetY -= winSize.y; break;
            case Alignment::Below:    targetY += refSize.y; break;
            case Alignment::LeftOf:   targetX -= winSize.x; break;
            case Alignment::RightOf:  targetX += refSize.x; break;
            case Alignment::Centered: break;
            case Alignment::None:
            default:
                break;
            }

            // Offsets once.
            targetX += entry.rule.offset.m_xOffset;
            targetY += entry.rule.offset.m_yOffset;

            window->MoveWindow(targetX, targetY);
            ++it;
        }
    }

    void WindowLayout::_RemoveWindow(AbstractWindow* window)
    {
        if (!window)
            return;

        for (auto it = _m_positioningRules.begin(); it != _m_positioningRules.end(); ++it)
        {
            const auto& key = it->first;
            if (key && key.get() == window)
            {
                _m_positioningRules.erase(it);
                return;
            }
        }
    }

    // Z-order only. No size “correction”, no move/resize, no querying width/height.
    bool AdjustZOrder(std::vector<WindowZOrder>& windows)
    {
        if (windows.empty())
            return false;

        std::sort(windows.begin(), windows.end(),
            [](const WindowZOrder& a, const WindowZOrder& b)
            {
                return a.zOrder < b.zOrder;
            });

        const size_t count = windows.size();
        constexpr int maxInt = (std::numeric_limits<int>::max)();
        const int deferCount = (count > (size_t)maxInt) ? maxInt : (int)count;

        HDWP hdwp = BeginDeferWindowPos(deferCount);
        if (!hdwp)
            return false;

        for (size_t i = 0; i < windows.size(); ++i)
        {
            auto& wz = windows[i];
            if (!wz.window)
                continue;

            HWND hwnd = wz.window->GetHWND();
            if (!hwnd || !IsWindow(hwnd))
                continue;

            HWND insertAfter = HWND_BOTTOM;
            if (i != 0 && windows[i - 1].window)
            {
                HWND prev = windows[i - 1].window->GetHWND();
                if (prev && IsWindow(prev))
                    insertAfter = prev;
            }

            hdwp = DeferWindowPos(
                hdwp,
                hwnd,
                insertAfter,
                0, 0, 0, 0,
                SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER
            );

            if (!hdwp)
                return false;
        }

        if (!EndDeferWindowPos(hdwp))
            return false;

        return true;
    }
}
