//-----------------------------------------------------------------------------
// InputManager.h
//	 Monitors input devices and returns structure containing current inputs.
//-----------------------------------------------------------------------------
// This software is part of the Skeleton Animation Toolkit (SKA) developed 
// at the University of the Pacific, under the guidance of Michael Doherty.
// For information please contact mdoherty@pacific.edu.
//-----------------------------------------------------------------------------
// This is open software. You are free to use it as you see fit.
// The University of the Pacific and identified authors would appreciate
// being credited for any significant use, particularly if used for
// commercial projects or academic research publications.
//-----------------------------------------------------------------------------

#ifndef INPUTMANAGER_DOT_H
#define INPUTMANAGER_DOT_H
#include <Core/SystemConfiguration.h>
#include <cstdlib>

// An instance of InputActions structure contains all user inputs 
// accumulated between calls to InputManager::getInput().

struct SKA_LIB_DECLSPEC InputActions
{
	unsigned short num_keys_pressed;
	unsigned char keys_pressed[256];
	bool mouse_button_state[3];
	unsigned short num_joy_buttons_pressed;
	unsigned char* joy_buttons_pressed;
	float mouse_xaxis;
	float mouse_yaxis;
	float mouse_zaxis;
	float joy_xaxis;
	float joy_yaxis;
	float joy_zaxis;
	float joy_xrot;
	float joy_yrot;
	float joy_zrot;
	float joy_slider;
	long  joy_pov;
	char* joy_product_name;
	char* joy_type;

	InputActions() : 
		num_keys_pressed(0), 
		num_joy_buttons_pressed(0), joy_buttons_pressed(NULL),
		mouse_xaxis(0.0), mouse_yaxis(0.0), mouse_zaxis(0.0),
		joy_xaxis(0.0),   joy_yaxis(0.0),   joy_zaxis(0.0),
		joy_xrot(0.0),    joy_yrot(0.0),    joy_zrot(0.0),  
		joy_slider(0.0),  joy_pov(-1),
		joy_product_name(NULL), joy_type(NULL)
	{ 
		mouse_button_state[0] = false;
		mouse_button_state[1] = false;
		mouse_button_state[2] = false;
	}

	virtual ~InputActions()
	{
	}
};

class SKA_LIB_DECLSPEC InputManager
{
public:
	InputManager()
	{
		mouse_button_state[0] = false;
		mouse_button_state[1] = false;
		mouse_button_state[2] = false;
		num_pressed_keys = 0;
		last_mouse_x_pos = last_mouse_y_pos = 0;
		first_mouse_move = true;
	}
	InputActions* getInput()
	{
		current_actions.mouse_button_state[0] = mouse_button_state[0];
		current_actions.mouse_button_state[1] = mouse_button_state[1];
		current_actions.mouse_button_state[2] = mouse_button_state[2];
		current_actions.mouse_xaxis = mouse_x_axis;
		current_actions.mouse_yaxis = mouse_y_axis;
		current_actions.mouse_zaxis = mouse_z_axis;

		mouse_x_axis = mouse_y_axis = mouse_z_axis = 0.0f;

		current_actions.num_keys_pressed = num_pressed_keys;
		for (short i=0; i<num_pressed_keys; i++)
			current_actions.keys_pressed[i] = pressed_keys[i];
		return &current_actions;
	}
	void registerGlutHandlers();

	bool keyIsPressed(unsigned char key);
	bool setKeyPressed(unsigned char key);
	bool setKeyReleased(unsigned char key);
	void mouseMotion(int x, int y);
public:
	InputActions current_actions;
	bool mouse_button_state[3];
	float mouse_x_axis, mouse_y_axis, mouse_z_axis;
	unsigned char pressed_keys[256];
	short num_pressed_keys;
	int last_mouse_x_pos, last_mouse_y_pos;
	bool first_mouse_move;
};

// single global instance
SKA_LIB_DECLSPEC extern InputManager input_manager;

#endif