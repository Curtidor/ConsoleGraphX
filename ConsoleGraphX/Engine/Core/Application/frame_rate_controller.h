#pragma once
#include <chrono>

namespace ConsoleGraphX
{
    class FrameRateController
    {
    private:
        std::chrono::high_resolution_clock::time_point _m_prevCaptureTime;
        std::chrono::high_resolution_clock::time_point _m_currentCaptureTime;
        std::chrono::milliseconds _m_deltaTime;
        std::chrono::nanoseconds _m_deltaTimeNS;
        std::chrono::milliseconds _m_elapsedTime;

        int _m_frameCount;
        int _m_framesPerSecond;
        int _m_targetFrameRate;
        std::chrono::milliseconds _m_targetFrameDuration;

    public:
        FrameRateController();
        FrameRateController(int targetFrameRate);

        void CaptureFrame();
        void StartCapture();
        void EndCapture();
        void WaitForNextFrame(std::chrono::milliseconds deltaTime);
        void ResetFrameCount();

        int GetFramesPerSecond() const;
        std::chrono::nanoseconds GetDeltaTimeNS() const;
        std::chrono::milliseconds GetDeltaTimeMS() const;
        std::chrono::microseconds GetTargetFrameDuration();
    };
}