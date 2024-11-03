#include "PCH_CGX.h"
#include <memory>
#include "window_layout.h"

namespace ConsoleGraphX
{
	void WindowLayout::AddWindow(Window* window, const WindowPositioningRule& rule)
	{
		_m_positioningRules.insert({ window, rule });

        auto handle = window->OnWindowDestroyed.AddListener(this, &WindowLayout::_RemoveWindow);
	}

    void WindowLayout::ApplyLayout()
    {
        for (auto& [window, rule] : _m_positioningRules)
        {
            // handle the case where the ref window has been destroyed
            if (rule.referenceWindow == nullptr)
            {
                _RemoveWindow(window);
                continue;
            }

            WindowPositionData windowPositionRef = rule.referenceWindow->GetWindowPosition();
            int refX = windowPositionRef.x;
            int refY = windowPositionRef.y;
            int refWidth = windowPositionRef.width;
            int refHeight = windowPositionRef.height;

            WindowPositionData windowPosition = window->GetWindowPosition();

            int targetX = refX;
            int targetY = refY;

            // Determine position based on anchor
            switch (rule.anchor) {
            case Anchor::TopLeft:
                targetX = refX;
                targetY = refY;
                break;
            case Anchor::TopCenter:
                targetX = refX + refWidth / 2 - windowPosition.width / 2;
                targetY = refY;
                break;
            case Anchor::TopRight:
                targetX = refX + refWidth - windowPosition.width;
                targetY = refY;
                break;
            case Anchor::CenterLeft:
                targetX = refX;
                targetY = refY + refHeight / 2 - windowPosition.height / 2;
                break;
            case Anchor::Center:
                targetX = refX + refWidth / 2 - windowPosition.width / 2;
                targetY = refY + refHeight / 2 - windowPosition.height / 2;
                break;
            case Anchor::CenterRight:
                targetX = refX + refWidth - windowPosition.width;
                targetY = refY + refHeight / 2 - windowPosition.height / 2;
                break;
            case Anchor::BottomLeft:
                targetX = refX;
                targetY = refY + refHeight - windowPosition.height;
                break;
            case Anchor::BottomCenter:
                targetX = refX + refWidth / 2 - windowPosition.width / 2;
                targetY = refY + refHeight - windowPosition.height;
                break;
            case Anchor::BottomRight:
                targetX = refX + refWidth - windowPosition.width;
                targetY = refY + refHeight - windowPosition.height;
                break;
            }

            // adjust position based on alignment
            switch (rule.alignment) 
            {
            case Alignment::Above:
                targetY -= windowPosition.height + rule.offset.m_yOffset;
                break;
            case Alignment::Below:
                targetY += refHeight + rule.offset.m_yOffset;
                break;
            case Alignment::LeftOf:
                targetX -= windowPosition.width + rule.offset.m_xOffset;
                break;
            case Alignment::RightOf:
                targetX += refWidth + rule.offset.m_xOffset;
                break;
            case Alignment::Centered:
                // centered alignment does not change the position after anchor positioning
                break;
            }

            // apply the final offset
            targetX += rule.offset.m_xOffset;
            targetY += rule.offset.m_yOffset;

            window->SetWindowPosition(targetX, targetY);
        }
    }

    void WindowLayout::_RemoveWindow(Window* window)
    {
        auto it = _m_positioningRules.find(window);

        if (it != _m_positioningRules.end())
        {
            _m_positioningRules.erase(it);
        }
    }


};