
#include <Core/Inputs/Input.hh>
#include <Configuration.hpp>
#include <Threads/ThreadManager.hpp>
#include <Threads/MainThread.hpp>

#ifdef USE_IMGUI
#include <Utils/Age_Imgui.hpp>
#endif

namespace AGE
{

	Input::Input() :
		_mousePosX(0),
		_mousePosY(0),
		_mouseDelX(0),
		_mouseDelY(0),
		_mouseWheelX(0),
		_mouseWheelY(0)
	{
		for (int i = 0; i < AGE_INPUT_NUMBER; ++i)
			_inputs[i] = false;
		for (int i = 0; i < AGE_KEY_NUMBER; ++i)
			_keyInputs[i] = 0;
	}

	void	Input::frameUpdate()
	{
		std::lock_guard<AGE::SpinLock> lock(_mutex);

		for (int i = 0; i < AGE_KEY_NUMBER; ++i)
		{
			if (AGE_MAPPED_KEY_JUST_PRESSED(_keyInputs[i]))
			{
				_keyInputs[i] = AGE_RESET_MAPPED_STATE(_keyInputs[i]);
				_keyInputs[i] = AGE_SET_MAPPED_KEY_PRESSED(_keyInputs[i]);
			}
			else if (AGE_MAPPED_KEY_JUST_RELEASED(_keyInputs[i]))
			{
				_keyInputs[i] = AGE_RESET_MAPPED_STATE(_keyInputs[i]);
			}
			if (AGE_PHYSICAL_KEY_JUST_PRESSED(_keyInputs[i]))
			{
				_keyInputs[i] = AGE_RESET_PHYSICAL_STATE(_keyInputs[i]);
				_keyInputs[i] = AGE_SET_PHYSICAL_KEY_PRESSED(_keyInputs[i]);
			}
			else if (AGE_PHYSICAL_KEY_JUST_RELEASED(_keyInputs[i]))
			{
				_keyInputs[i] = AGE_RESET_PHYSICAL_STATE(_keyInputs[i]);
			}
		}
	}

	void 	Input::addInput(AgeInputs input)
	{
		std::lock_guard<AGE::SpinLock> lock(_mutex);

		_inputs[input] = true;
	}

	void 	Input::removeInput(AgeInputs input)
	{
		std::lock_guard<AGE::SpinLock> lock(_mutex);

		_inputs[input] = false;
	}

	void	Input::keyInputPressed(AgeKeys mappedInput, AgeKeys physicalInput)
	{
		_mutex.lock();
		_keyInputs[mappedInput] = AGE_RESET_MAPPED_STATE(_keyInputs[mappedInput]);
		_keyInputs[mappedInput] = AGE_SET_MAPPED_KEY_JUST_PRESSED(_keyInputs[mappedInput]);
		_keyInputs[physicalInput] = AGE_RESET_MAPPED_STATE(_keyInputs[physicalInput]);
		_keyInputs[physicalInput] = AGE_SET_PHYSICAL_KEY_JUST_PRESSED(_keyInputs[physicalInput]);
		_mutex.unlock();
#ifdef USE_IMGUI
		GetMainThread()->getQueue()->emplaceTask<ImGuiKeyEvent>(mappedInput, true);
#endif
	}

	void 	Input::keyInputReleased(AgeKeys mappedInput, AgeKeys physicalInput)
	{
		_mutex.lock();
		_keyInputs[mappedInput] = AGE_RESET_MAPPED_STATE(_keyInputs[mappedInput]);
		_keyInputs[mappedInput] = AGE_SET_MAPPED_KEY_JUST_RELEASED(_keyInputs[mappedInput]);
		_keyInputs[physicalInput] = AGE_RESET_MAPPED_STATE(_keyInputs[physicalInput]);
		_keyInputs[physicalInput] = AGE_SET_PHYSICAL_KEY_JUST_RELEASED(_keyInputs[physicalInput]);
		_mutex.unlock();
#ifdef USE_IMGUI
		GetMainThread()->getQueue()->emplaceTask<ImGuiKeyEvent>(mappedInput, false);
#endif
	}

	void 				Input::setMousePosition(glm::ivec2 const &pos, glm::ivec2 const &rel)
	{
		std::lock_guard<AGE::SpinLock> lock(_mutex);

		_mouseDelX = rel.x;
		_mouseDelY = rel.y;
		_mousePosX = pos.x;
		_mousePosY = pos.y;
	}

	void				Input::setMouseWheel(glm::ivec2 const &delta)
	{
		std::lock_guard<AGE::SpinLock> lock(_mutex);

		_mouseWheelX = delta.x;
		_mouseWheelY = delta.y;
	}

	void				Input::addJoystick(std::string const &name, uint32_t joyId)
	{
		std::lock_guard<AGE::SpinLock> lock(_mutex);

		_joysticks[joyId].connected = true;
	}

	void				Input::removeJoystick(uint32_t joyId)
	{
		std::lock_guard<AGE::SpinLock> lock(_mutex);

		_joysticks[joyId].disconnect();
	}

	void	Input::setJoystickAxis(uint32_t joyId, AgeJoystickAxis joyAxis, float value)
	{
		std::lock_guard<AGE::SpinLock> lock(_mutex);

		assert(_joysticks[joyId].connected);
		_joysticks[joyId].axis[joyAxis] = value;
	}

	void	Input::setJoystickTrackBall(uint32_t joyId, glm::ivec2 const &pos)
	{
		std::lock_guard<AGE::SpinLock> lock(_mutex);

		assert(_joysticks[joyId].connected);
		_joysticks[joyId].trackBall = pos;
	}

	void	Input::setJoystickHat(uint32_t joyId, uint32_t joyHat, AgeJoystickHatDirections dir)
	{
		std::lock_guard<AGE::SpinLock> lock(_mutex);

		assert(_joysticks[joyId].connected);
		_joysticks[joyId].hats[joyHat] = dir;
	}

	void	Input::joystickButtonPressed(uint32_t joyId, AgeJoystickButtons joyButton)
	{
		std::lock_guard<AGE::SpinLock> lock(_mutex);

		assert(_joysticks[joyId].connected);
		_joysticks[joyId].buttons[joyButton] = true;
	}

	void	Input::joystickButtonReleased(uint32_t joyId, AgeJoystickButtons joyButton)
	{
		std::lock_guard<AGE::SpinLock> lock(_mutex);

		assert(_joysticks[joyId].connected);
		_joysticks[joyId].buttons[joyButton] = false;
	}

	void Input::sendMouseStateToIMGUI()
	{
#ifdef USE_IMGUI
		glm::ivec2 mousePosition(_mousePosX, _mousePosY);
		GetMainThread()->getQueue()->emplaceTask<ImGuiMouseStateEvent>(mousePosition,
			getInput(AGE_MOUSE_LEFT),
			getInput(AGE_MOUSE_MIDDLE),
			getInput(AGE_MOUSE_RIGHT));
#endif
	}

	glm::ivec2   	    Input::getMouseWheel()
	{
		std::lock_guard<AGE::SpinLock> lock(_mutex);

		return glm::ivec2(_mouseWheelX, _mouseWheelY);
	}

	glm::ivec2      	Input::getMousePosition()
	{
		std::lock_guard<AGE::SpinLock> lock(_mutex);

		return glm::ivec2(_mousePosX, _mousePosY);
	}

	glm::ivec2       	Input::getMouseDelta()
	{
		std::lock_guard<AGE::SpinLock> lock(_mutex);

		return glm::ivec2(_mouseDelX, _mouseDelY);
	}

	bool 	Input::getInput(AgeInputs input)
	{
		std::lock_guard<AGE::SpinLock> lock(_mutex);

		return (_inputs[input]);
	}

	bool 	Input::getPhysicalKeyPressed(AgeKeys input)
	{
		std::lock_guard<AGE::SpinLock> lock(_mutex);

		return (AGE_PHYSICAL_KEY_PRESSED(_keyInputs[input]));
	}
	bool 	Input::getMappedKeyPressed(AgeKeys input)
	{
		std::lock_guard<AGE::SpinLock> lock(_mutex);

		return (AGE_MAPPED_KEY_PRESSED(_keyInputs[input]));
	}

	bool 	Input::getPhysicalKeyJustPressed(AgeKeys input)
	{
		std::lock_guard<AGE::SpinLock> lock(_mutex);

		return (AGE_PHYSICAL_KEY_JUST_PRESSED(_keyInputs[input]));
	}

	bool 	Input::getMappedKeyJustPressed(AgeKeys input)
	{
		std::lock_guard<AGE::SpinLock> lock(_mutex);

		return (AGE_MAPPED_KEY_JUST_PRESSED(_keyInputs[input]));
	}

	bool 	Input::getPhysicalKeyJustReleased(AgeKeys input)
	{
		std::lock_guard<AGE::SpinLock> lock(_mutex);

		return (AGE_PHYSICAL_KEY_JUST_RELEASED(_keyInputs[input]));
	}

	bool 	Input::getMappedKeyJustReleased(AgeKeys input)
	{
		std::lock_guard<AGE::SpinLock> lock(_mutex);

		return (AGE_MAPPED_KEY_JUST_RELEASED(_keyInputs[input]));
	}

	bool	Input::getJoystick(uint32_t joyId)
	{
		std::lock_guard<AGE::SpinLock> lock(_mutex);

		return (_joysticks[joyId].connected);
	}

	bool	Input::getJoystick(uint32_t joyId, Joystick &joystickInfos)
	{
		std::lock_guard<AGE::SpinLock> lock(_mutex);

		if (_joysticks[joyId].connected)
		{
			joystickInfos = _joysticks[joyId];
			return (true);
		}
		else
			return (false);
	}
}