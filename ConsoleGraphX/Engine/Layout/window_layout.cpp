#include "PCH_CGX.h"
#include <thread>
#include <span>
#include <memory>
#include "Engine\Core\Window\window_manager.h"
#include "Engine\Core\Logger\logger_manager.h"

namespace ConsoleGraphX
{
    void WindowLayout::AddWindow(std::shared_ptr<AbstractWindow> window, const WindowPositioningRule& rule)
    {
        _m_positioningRules.insert({ window, rule });

        EventCallBackHandle callbackHandle = window->OnWindowDestroyed.AddListenerLambda([this](AbstractWindow* destroyed) {
            this->_RemoveWindow(destroyed);
            });
    }

    void WindowLayout::ApplyLayout()
    {
        for (auto it = _m_positioningRules.begin(); it != _m_positioningRules.end(); )
        {
            auto window = it->first;
            auto refWindow = it->second.referenceWindow;

            // If either window expired, erase entry
            if (!refWindow)
            {
                it = _m_positioningRules.erase(it);
                continue;
            }

            const auto rule = it->second;

            const Vector2 refSize = refWindow->GetTargetWindowSizeInPixels();
            const Vector2 winSize = window->GetTargetWindowSizeInPixels();

			const auto refPosition = refWindow->GetWindowPosition();
			const auto winPosition = window->GetWindowPosition();

            int targetX = refPosition.x;
            int targetY = refPosition.y;

            // Anchor logic
            switch (rule.anchor)
            {
            case Anchor::TopLeft: break;
            case Anchor::TopCenter:
                targetX += (refSize.x - winSize.x) / 2;
                break;
            case Anchor::TopRight:
                targetX += refSize.x - winSize.x;
                break;
            case Anchor::CenterLeft:
                targetY += (refSize.y - winSize.y) / 2;
                break;
            case Anchor::Center:
                targetX += (refSize.x - winSize.x) / 2;
                targetY += (refSize.y - winSize.y) / 2;
                break;
            case Anchor::CenterRight:
                targetX += refSize.x - winSize.x;
                targetY += (refSize.y - winSize.y) / 2;
                break;
            case Anchor::BottomLeft:
                targetY += refSize.y - winSize.y;
                break;
            case Anchor::BottomCenter:
                targetX += (refSize.x - winSize.x) / 2;
                targetY += refSize.y - winSize.y;
                break;
            case Anchor::BottomRight:
                targetX += refSize.x - winSize.x;
                targetY += refSize.y - winSize.y;
                break;
            }

            // Alignment logic
            switch (rule.alignment)
            {
            case Alignment::Above:
                targetY -= winSize.y + rule.offset.m_yOffset;
                break;
            case Alignment::Below:
                targetY += refSize.y + rule.offset.m_yOffset;
                break;
            case Alignment::LeftOf:
                targetX -= winSize.x + rule.offset.m_xOffset;
                break;
            case Alignment::RightOf:
                targetX += refSize.x + rule.offset.m_xOffset;
                break;
            case Alignment::Centered:
                break;
            }

            targetX += rule.offset.m_xOffset;
            targetY += rule.offset.m_yOffset;

            window->SetWindowPosition(targetX, targetY);
            ++it;
        }
    }

    void WindowLayout::_RemoveWindow(AbstractWindow* window)
    {
        for (auto it = _m_positioningRules.begin(); it != _m_positioningRules.end(); ++it)
        {
            auto locked = it->first;
            if (locked && locked.get() == window)
            {
                _m_positioningRules.erase(it);
                return;
            }
        }
    }

    bool AdjustZOrder(std::vector<WindowZOrder>& windows)
    {
        if (windows.empty()) {
            std::cerr << "No windows to reorder.\n";
            return false;
        }

        // Sort by ascending Z-order (bottom to top)
        std::sort(windows.begin(), windows.end(), [](const WindowZOrder& a, const WindowZOrder& b) {
            return a.zOrder < b.zOrder;
            });

        HDWP hdwp = BeginDeferWindowPos(static_cast<int>(windows.size()));
        if (!hdwp) {
            std::cerr << "BeginDeferWindowPos failed.\n";
            return false;
        }

        for (size_t i = 0; i < windows.size(); ++i) {
            HWND insertAfter = (i == 0) ? HWND_BOTTOM : windows[i - 1].window.get()->GetHWND();

            AbstractWindow* window = windows[i].window.get();
            const WindowPositionData& position = window->GetWindowPosition(); // dont trust the width and height values
            const Vector2 size = window->GetTargetWindowSizeInPixels();

            // good chance the window HWND is either not set or invalid
            if (position.width == 0 && position.height == 0 && position.x == 0 && position.y ==  0)
            {
                int x = 10;
            }

            if (window->GetWindowName() == "Main")
            {
                int x = 10;
            }
            // check on the return width and height values if they dont match some funny stuff is going on
            if (position.width != size.x || position.height != size.y)
            {
                if (window->m_deferWindowPosCountWithWrongSize < (std::numeric_limits<uint32_t>::max)())
                    window->m_deferWindowPosCountWithWrongSize++;
                
                if (window->m_deferWindowPosCountWithWrongSize > 20) // give the window 20 chances to have the right size
                {
                    //why hasnt the window been updated this is  bad news
                    int x = 10;
                }
            }
            else
            {
                window->m_deferWindowPosCountWithWrongSize = 0;
            }

            hdwp = DeferWindowPos(hdwp, window->GetHWND(), insertAfter,
                position.x, position.y,
                size.x, size.y,
                SWP_NOACTIVATE | SWP_SHOWWINDOW);


            if (!hdwp) {
                std::cerr << "DeferWindowPos failed at index " << i << ".\n";
                return false;
            }
        }

        if (!EndDeferWindowPos(hdwp)) {
            std::cerr << "EndDeferWindowPos failed.\n";
            return false;
        }

        return true;
    }
}
