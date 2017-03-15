//-----------------------------------------------------------------------------
// InputManager.cpp
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

#include <Core/SystemConfiguration.h>
#include <Input/InputManager.h>
#include <GL/glut.h>

void mouseHandler(int button, int state, int x, int y)
{
	//mouse.x = x;
	//mouse.y = y;
	switch (button)
	{
	case GLUT_LEFT_BUTTON:
		if (state == GLUT_UP) input_manager.mouse_button_state[0] = false;
		if (state == GLUT_DOWN) input_manager.mouse_button_state[0] = true; 
		break;
	case GLUT_MIDDLE_BUTTON:
		if (state == GLUT_UP) input_manager.mouse_button_state[2] = false;
		if (state == GLUT_DOWN) input_manager.mouse_button_state[2] = true; 
		break;
	case GLUT_RIGHT_BUTTON:
		if (state == GLUT_UP) input_manager.mouse_button_state[1] = false;
		if (state == GLUT_DOWN) input_manager.mouse_button_state[1] = true; 
		break;
	}
}

void keyDownHandler(unsigned char key, int x, int y)
{ 
	//printf("key down %c\n", key);
	input_manager.setKeyPressed(key);
}

void keyUpHandler(unsigned char key, int x, int y)
{ 
	//printf("key up %c\n", key);
	input_manager.setKeyReleased(key);
}

void keySpecialDownHandler(int key, int x, int y)
{ 
	//printf("special key down %d\n", key);
	input_manager.setKeyPressed(key);
}

void keySpecialUpHandler(int key, int x, int y)
{ 
	//printf("special key up %d\n", key);
	input_manager.setKeyReleased(key);
}

void mouseMotionHander(int x, int y)
{
	input_manager.mouseMotion(x,y);
}

void mousePassiveMotionHander(int x, int y)
{
	input_manager.mouseMotion(x,y);
}

bool InputManager::keyIsPressed(unsigned char key)
{
	for (short i=0; i<num_pressed_keys; i++)
		if (pressed_keys[i] == key) return true;
	return false;
}

bool InputManager::setKeyPressed(unsigned char key)
{
	if (keyIsPressed(key)) return true;
	pressed_keys[num_pressed_keys] = key;
	num_pressed_keys++;
	return true;
}

bool InputManager::setKeyReleased(unsigned char key)
{
	for (short i=0; i<num_pressed_keys; i++)
		if (pressed_keys[i] == key) 
		{
			pressed_keys[i] = pressed_keys[num_pressed_keys-1];
			num_pressed_keys--;
			return true;
		}
	return false;
}

void InputManager::mouseMotion(int x, int y)
{
	if (first_mouse_move) 
	{
		first_mouse_move = false;
	}
	else
	{
		mouse_x_axis = x - last_mouse_x_pos;
		mouse_y_axis = y - last_mouse_y_pos;
	}
	last_mouse_x_pos = x;
	last_mouse_y_pos = y;
}

void InputManager::registerGlutHandlers()
{
	glutIgnoreKeyRepeat(true);
	glutKeyboardFunc(keyDownHandler);
	glutKeyboardUpFunc(keyUpHandler);
	glutSpecialFunc(keySpecialDownHandler);
	glutSpecialUpFunc(keySpecialUpHandler);

	glutMouseFunc(mouseHandler);
	glutMotionFunc(mouseMotionHander);	
	glutPassiveMotionFunc(mousePassiveMotionHander);	

}

// single global instance
InputManager input_manager;