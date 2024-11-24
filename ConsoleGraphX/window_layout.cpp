#include "PCH_CGX.h"
#include <memory>
#include "window_layout.h"

namespace ConsoleGraphX
{
	void WindowLayout::AddWindow(AbstractWindow* window, const WindowPositioningRule& rule)
	{
		_m_positioningRules.insert({ window, rule });

        auto handle = window->OnWindowDestroyed.AddListener(this, &WindowLayout::_RemoveWindow);
	}

    void WindowLayout::ApplyLayout() 
    {
        for (auto& [window, rule] : _m_positioningRules) 
        {
            // Remove windows with invalid references
            if (rule.referenceWindow == nullptr) 
            {
                _RemoveWindow(window);
                continue;
            }

            const auto& refPos = rule.referenceWindow->GetWindowPosition();
            const auto& winPos = window->GetWindowPosition();

            int targetX = refPos.x;
            int targetY = refPos.y;

            // Determine target position based on anchor
            switch (rule.anchor) 
            {
            case Anchor::TopLeft:
                break;
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

            // Adjust position based on alignment
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
                // No adjustment
                break;
            }

            // Apply final offset
            targetX += rule.offset.m_xOffset;
            targetY += rule.offset.m_yOffset;

            window->SetWindowPosition(targetX, targetY);
        }
    }


    void WindowLayout::_RemoveWindow(AbstractWindow* window)
    {
        auto it = _m_positioningRules.find(window);

        if (it != _m_positioningRules.end())
        {
            _m_positioningRules.erase(it);
        }
    }


    bool AdjustZOrder(std::vector<WindowZOrder>& windows) {
        if (windows.empty()) {
            std::cerr << "No windows provided for Z-order adjustment.\n";
            return false;
        }

        // Sort the windows by their Z-order value (ascending: lower Z-order is closer to the top).
        std::sort(windows.begin(), windows.end(), [](const WindowZOrder& a, const WindowZOrder& b) {
            return a.zOrder < b.zOrder;
            });

        // Apply Z-order adjustments.

        // MAIN TOP
        // EDITOR
        // LOGGER
        // RTIP

        for (size_t i = 0; i < windows.size(); ++i) {
            HWND hwndInsertAfter = (i == 0) ? HWND_TOPMOST : windows[i - 1].hwnd;

            if (!SetWindowPos(windows[i].hwnd, hwndInsertAfter, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE)) {
                std::cerr << "Failed to adjust Z-order for window at index " << i << ". Error: "
                    << GetLastError() << "\n";
                return false;
            }
        }

        return true;
    }

};