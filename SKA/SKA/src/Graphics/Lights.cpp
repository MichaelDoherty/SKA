//-----------------------------------------------------------------------------
// Lights.cpp
//	 Function to initialize default lighting.
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

#include <GL/glut.h>
#include <Graphics/GraphicsInterface.h>
#include <Graphics/Lights.h>

void initializeDefaultLighting()
{
	GLfloat light_ambient[]   = { 0.8f, 0.8f, 0.8f, 1.0f };
	GLfloat light0_diffuse[]  = {0.5f, 0.5f, 0.5f, 1.0f};
	GLfloat light0_specular[] = {0.0f, 0.0f, 0.0f, 1.0f};
	GLfloat light0_position[] = {-100.0, 100.0, -100.0, 0.0};
	GLfloat light1_diffuse[]  = {0.5f, 0.5f, 0.5f, 1.0f};
	GLfloat light1_specular[] = {0.0f, 0.0f, 0.0f, 1.0f};
	GLfloat light1_position[] = {100.0, 100.0, 100.0, 0.0};
	glEnable(GL_LIGHTING);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, light_ambient);
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, true);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light1_specular);
	glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
	glEnable(GL_LIGHT1);
	checkOpenGLError(201);
}
