#include "CGXPCH.h"
#include "frame_rate_controller.h"

namespace ConsoleGraphX
{
	FrameRateController::FrameRateController()
	{
		_m_prevCaptureTime = std::chrono::high_resolution_clock::now();
		_m_currentCaptureTime = std::chrono::high_resolution_clock::now();

		_m_deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(_m_currentCaptureTime - _m_prevCaptureTime);
		_m_deltaTimeNS = std::chrono::duration_cast<std::chrono::nanoseconds>(_m_currentCaptureTime - _m_prevCaptureTime);
		_m_elapsedTime = std::chrono::milliseconds(0);

		_m_frameCount = 0;
		_m_framesPerSecond = 0;
		_m_targetFrameRate = 60;
		_m_targetFrameDuration = std::chrono::milliseconds(1000 / _m_targetFrameRate);
	}

	FrameRateController::FrameRateController(int targetFrameRate)
	{
		_m_prevCaptureTime = std::chrono::high_resolution_clock::now();
		_m_currentCaptureTime = std::chrono::high_resolution_clock::now();

		_m_deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(_m_currentCaptureTime - _m_prevCaptureTime);
		_m_deltaTimeNS = std::chrono::duration_cast<std::chrono::nanoseconds>(_m_currentCaptureTime - _m_prevCaptureTime);
		_m_elapsedTime = std::chrono::milliseconds(0);

		_m_frameCount = 0;
		_m_framesPerSecond = 0;
		_m_targetFrameRate = targetFrameRate;
		_m_targetFrameDuration = std::chrono::milliseconds(1000 / _m_targetFrameRate);
	}

	void FrameRateController::CaptureFrame()
	{
		_m_currentCaptureTime = std::chrono::high_resolution_clock::now();

		_m_deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(_m_currentCaptureTime - _m_prevCaptureTime);
		_m_deltaTimeNS = _m_currentCaptureTime - _m_prevCaptureTime;

		_m_elapsedTime += _m_deltaTime;

		_m_prevCaptureTime = _m_currentCaptureTime;

		_m_frameCount++;
	}

	void FrameRateController::StartCapture()
	{
		_m_prevCaptureTime = std::chrono::high_resolution_clock::now();
	}

	void FrameRateController::EndCapture()
	{
		_m_currentCaptureTime = std::chrono::high_resolution_clock::now();

		_m_deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(_m_currentCaptureTime - _m_prevCaptureTime);
		_m_deltaTimeNS = _m_currentCaptureTime - _m_prevCaptureTime;
	}

	void FrameRateController::WaitForNextFrame(std::chrono::milliseconds deltaTime)
	{
		if (deltaTime <= _m_targetFrameDuration)
		{
			//std::this_thread::sleep_for(std::chrono::milliseconds(_m_targetFrameDuration - deltaTime) - std::chrono::milliseconds(2));
		}

		if (_m_elapsedTime >= std::chrono::milliseconds(1000))
		{
			_m_framesPerSecond = _m_frameCount;
			_m_frameCount = 0;
			_m_elapsedTime = std::chrono::milliseconds(0);
		}
	}

	void FrameRateController::ResetFrameCount()
	{
		_m_frameCount = 0;
	}

	int FrameRateController::GetFramesPerSecond() const
	{
		return _m_framesPerSecond;
	}

	std::chrono::nanoseconds FrameRateController::GetDeltaTimeNS() const
	{
		return _m_deltaTimeNS;
	}

	std::chrono::milliseconds FrameRateController::GetDeltaTimeMS() const
	{
		return _m_deltaTime;
	}

	std::chrono::microseconds FrameRateController::GetTargetFrameDuration()
	{
		return _m_targetFrameDuration;
	}

}
