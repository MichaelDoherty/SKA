//-----------------------------------------------------------------------------
// Graphics2D.cpp
//	 Functions for 2D graphics (currently string rendering).
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
#include <Graphics/Graphics2D.h>
#include <vector>
using namespace std;
#include <Math/Vector3D.h>
#include <GL/glut.h>
#include <Graphics/GraphicsInterface.h>

static const GLfloat black_color[ ] = { 0.0f, 0.0f, 0.0f, 1.0f };
static const GLfloat white_color[ ] = { 1.0f, 1.0f, 1.0f, 1.0f };
static const GLfloat red_color[ ]   = { 1.0f, 0.0f, 0.0f, 1.0f };
static const GLfloat green_color[ ] = { 0.0f, 1.0f, 0.0f, 1.0f };
static const GLfloat blue_color[ ]  = { 0.0f, 0.0f, 1.0f, 1.0f };
static const GLfloat base_shininess = 1.0f;

// bitmap string rendering code adapted from 
// http://www.lighthouse3d.com/opengl/glut/index.php?bmpfont and
// http://www.lighthouse3d.com/opengl/glut/index.php?bmpfontortho

void renderBitmapString(float x, float y, float z, void *font, Color color, const char *string) 
{  
	int placecode = 801;
	glMaterialfv( GL_FRONT, GL_SPECULAR, white_color );
	checkOpenGLError(placecode++);
	glMaterialfv( GL_FRONT, GL_EMISSION, black_color );
	checkOpenGLError(placecode++);
	glMaterialf( GL_FRONT, GL_SHININESS, base_shininess);
	checkOpenGLError(placecode++);
	glMaterialfv( GL_FRONT, GL_AMBIENT, color.c );
	checkOpenGLError(placecode++);
	glMaterialfv( GL_FRONT, GL_DIFFUSE, color.c );
	checkOpenGLError(placecode++);
	glRasterPos3f(x, y, z);
	checkOpenGLError(placecode++);
	for (short i = 0; string[i] != '\0'; i++) 
	{
		glutBitmapCharacter(font, string[i]);
		checkOpenGLError(placecode++);
	}
}

void renderString(float x, float y, float z, Color color, const char* string)
{
	int placecode = 901;
	glPushMatrix();		//save orientation of coordinate systems
	checkOpenGLError(placecode++);
	glLoadIdentity();
	checkOpenGLError(placecode++);
	renderBitmapString(x , y, z, GLUT_BITMAP_HELVETICA_18, color, string);
	checkOpenGLError(placecode++);
	glPopMatrix();		//restore orientation of coordinate systems
	checkOpenGLError(placecode++);
}