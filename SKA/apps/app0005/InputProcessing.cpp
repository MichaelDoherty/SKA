//-----------------------------------------------------------------------------
// app0005 - Builds with SKA Version 3.1 - Sept 01, 2012 - Michael Doherty
//-----------------------------------------------------------------------------
// InputProcessing.cpp
//-----------------------------------------------------------------------------
#include "Core/SystemConfiguration.h"
#include "Input/InputManager.h"
#include "InputProcessing.h"
#include "AnimationControl.h"
#include "CameraControl.h"

#define ESC 27
#define SPACE 32

InputProcessor::InputProcessor()
{
	filter.addFilter('1', 0.2f, KEYBOARD);
	filter.addFilter('2', 0.2f, KEYBOARD);
	filter.addFilter('3', 0.2f, KEYBOARD);
	filter.addFilter('[', 0.2f, KEYBOARD);
}

void InputProcessor::processInputs(float elapsed_time)
{ 
	filter.advanceTime(elapsed_time);

	InputActions* actions = input_manager.getInput();
	
	// camera controls: 
	float fwd_thrust=0.0f;
	float hrz_thrust=0.0f;
	float vrt_thrust=0.0f;
	float yaw_thrust=0.0f;
	float pitch_thrust=0.0f;
	float roll_thrust=0.0f;
	bool move_camera = false;

	if (actions->mouse_button_state[0]) { fwd_thrust += 0.1f; move_camera= true; }
	if (actions->mouse_button_state[1]) { fwd_thrust -= 0.1f; move_camera= true; }
	//yaw_thrust = actions->mouse_xaxis/10.0f;
	//pitch_thrust = actions->mouse_yaxis/10.0f;

	for (short i=0; i<actions->num_keys_pressed; i++)
	{
		switch (filter.testInput(actions->keys_pressed[i]))
		{
		case ESC: exit(0); break;
		case 'w': fwd_thrust += 0.3f; move_camera= true; break;
		case 's': fwd_thrust -= 0.3f; move_camera= true; break;
		case 'q': vrt_thrust += 0.3f; move_camera= true; break;
		case 'e': vrt_thrust -= 0.3f; move_camera= true; break;
		case 'a': hrz_thrust += 0.3f; move_camera= true; break;
		case 'd': hrz_thrust -= 0.3f; move_camera= true; break;
		case 'i': pitch_thrust += 0.05f; move_camera= true;  break;
		case 'k': pitch_thrust -= 0.05f; move_camera= true; break;
		case 'j': yaw_thrust += 0.05f; move_camera= true; break;
		case 'l': yaw_thrust -= 0.05f; move_camera= true; break;
		case 'u': roll_thrust += 0.05f; move_camera= true; break;
		case 'o': roll_thrust -= 0.05f; move_camera= true; break;
		case '1': anim_ctrl.togglePause(); break;
		case '2': anim_ctrl.singleStep(); break;
		case '9': setCameraLeft(); break;
		case '8': setCameraFrontLeft(); break;
		case '7': setCameraFront(); break;
		case '[': anim_ctrl.slowDown(); break;
		case ']': anim_ctrl.fullSpeed(); break;
		}
	}
	if (move_camera)
	{
		camera.move(0.02f, fwd_thrust, hrz_thrust, vrt_thrust, yaw_thrust, pitch_thrust, roll_thrust);
	}
}
