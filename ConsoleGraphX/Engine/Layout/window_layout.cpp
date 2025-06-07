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

            const auto refPos = refWindow->GetWindowPosition();
            const auto winPos = window->GetWindowPosition();

            int targetX = refPos.x;
            int targetY = refPos.y;

            // Anchor logic
            switch (rule.anchor)
            {
            case Anchor::TopLeft: break;
            case Anchor::TopCenter:
                targetX += (refPos.width - winPos.width) / 2;
                break;
            case Anchor::TopRight:
                targetX += refPos.width - winPos.width;
                break;
            case Anchor::CenterLeft:
                targetY += (refPos.height - winPos.height) / 2;
                break;
            case Anchor::Center:
                targetX += (refPos.width - winPos.width) / 2;
                targetY += (refPos.height - winPos.height) / 2;
                break;
            case Anchor::CenterRight:
                targetX += refPos.width - winPos.width;
                targetY += (refPos.height - winPos.height) / 2;
                break;
            case Anchor::BottomLeft:
                targetY += refPos.height - winPos.height;
                break;
            case Anchor::BottomCenter:
                targetX += (refPos.width - winPos.width) / 2;
                targetY += refPos.height - winPos.height;
                break;
            case Anchor::BottomRight:
                targetX += refPos.width - winPos.width;
                targetY += refPos.height - winPos.height;
                break;
            }

            // Alignment logic
            switch (rule.alignment)
            {
            case Alignment::Above:
                targetY -= winPos.height + rule.offset.m_yOffset;
                break;
            case Alignment::Below:
                targetY += refPos.height + rule.offset.m_yOffset;
                break;
            case Alignment::LeftOf:
                targetX -= winPos.width + rule.offset.m_xOffset;
                break;
            case Alignment::RightOf:
                targetX += refPos.width + rule.offset.m_xOffset;
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

            const AbstractWindow* window = windows[i].window.get();
            const WindowPositionData& postion = window->GetWindowPosition();

            // You can adjust position/size here if needed
            hdwp = DeferWindowPos(hdwp, window->GetHWND(), insertAfter,
                postion.x, postion.y, 
                postion.width, postion.height, 
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
