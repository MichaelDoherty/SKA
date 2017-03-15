//-----------------------------------------------------------------------------
// GraphicsInterface.cpp
//	 Implements functions dependent on OpenGL.
//   Most methods defined here are for classes defined elsewhere.
//   The intention is to isolate the OpenGL dependencies.
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
#include <Core/SystemLog.h>
#include <Graphics/GraphicsInterface.h>
#include <Camera/Camera.h>
#include <Models/Models.h>
#include <Models/CodedModels.h>
#include <Graphics/Textures.h>
#include <GL/glut.h>

float time_fade_factor = 1.0f;

static GLfloat black_color[ ] = { 0.0f, 0.0f, 0.0f, 1.0f };
static GLfloat white_color[ ] = { 1.0f, 1.0f, 1.0f, 1.0f };
static GLfloat red_color[ ]   = { 1.0f, 0.0f, 0.0f, 1.0f };
static GLfloat green_color[ ] = { 0.0f, 1.0f, 0.0f, 1.0f };
static GLfloat blue_color[ ]  = { 0.0f, 0.0f, 1.0f, 1.0f };
static GLfloat base_shininess = 1.0f;

void checkOpenGLError(int placecode)
{
	GLenum error_code = glGetError();
	if (error_code != GL_NO_ERROR)
	{
		const GLubyte *errString;
		errString = gluErrorString(error_code);
		logout << "openGL error at " << placecode 
			<< " error is " << error_code << " " << errString << endl; 
	}
}

//IMPROVEIT! This is a bit ugly. 
// These methods should be in their class files, accessing a more abstract rendering interface.
// The only reason they are here now is because they contain openGL commands.

void LookAtCamera::setSceneView()
{
	gluPerspective(vert_fov, aspect_ratio, 
		near_plane_distance, far_plane_distance);
	gluLookAt(eye_point.x, eye_point.y, eye_point.z,
		lookat_point.x, lookat_point.y, lookat_point.z,
		up_vector.x, up_vector.y, up_vector.z);
}

static GLfloat* fadeColor(GLfloat c[4])
{
	if (time_fade_factor < 1.0f)
	{
		c[0] *= time_fade_factor;
		c[1] *= time_fade_factor;
		c[2] *= time_fade_factor;
		c[3] *= time_fade_factor;
	}
	return c;
}

void MeshModel::render(Matrix4x4& world_xform) 
{
	// apply xforms
    glPushMatrix(); 	
	glMultMatrixf(world_xform.m);
	
	glBegin(GL_TRIANGLES);

	for (unsigned int i=0; i<faces->faces.size(); i++)
	{
		float n[3];
		n[0] = faces->faces[i].normal.x;
		n[1] = faces->faces[i].normal.y;
		n[2] = faces->faces[i].normal.z;
		glNormal3fv(n);
		
		GLfloat face_color[] = { faces->faces[i].color.r, faces->faces[i].color.g, faces->faces[i].color.b, faces->faces[i].color.a };
		fadeColor(face_color);
		glMaterialfv(GL_FRONT, GL_AMBIENT, face_color);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, face_color);
		glMaterialfv(GL_FRONT, GL_SPECULAR, white_color);
		glMaterialfv(GL_FRONT, GL_EMISSION, black_color);
		glMaterialf(GL_FRONT, GL_SHININESS, base_shininess);

		glVertex3f(faces->faces[i].point[0].x, faces->faces[i].point[0].y, faces->faces[i].point[0].z);
		glVertex3f(faces->faces[i].point[1].x, faces->faces[i].point[1].y, faces->faces[i].point[1].z);
		glVertex3f(faces->faces[i].point[2].x, faces->faces[i].point[2].y, faces->faces[i].point[2].z);
	}
	glEnd();
	checkOpenGLError(405);

	glBegin(GL_TRIANGLES);
	for (unsigned int i=0; i<faces->pcfaces.size(); i++)
	{
		float n[3];
		n[0] = faces->pcfaces[i].normal.x;
		n[1] = faces->pcfaces[i].normal.y;
		n[2] = faces->pcfaces[i].normal.z;
		glNormal3fv(n);
		
		GLfloat color1[] = { faces->pcfaces[i].color[0].r, faces->pcfaces[i].color[0].g, faces->pcfaces[i].color[0].b, 1.0f };
		GLfloat color2[] = { faces->pcfaces[i].color[1].r, faces->pcfaces[i].color[1].g, faces->pcfaces[i].color[1].b, 1.0f };
		GLfloat color3[] = { faces->pcfaces[i].color[2].r, faces->pcfaces[i].color[2].g, faces->pcfaces[i].color[2].b, 1.0f };

		glMaterialfv(GL_FRONT, GL_SPECULAR, white_color);
		glMaterialfv(GL_FRONT, GL_EMISSION, black_color);
		glMaterialf(GL_FRONT, GL_SHININESS, base_shininess);

		glMaterialfv(GL_FRONT, GL_AMBIENT, color1);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, color1);
		glVertex3f(faces->pcfaces[i].point[0].x, faces->pcfaces[i].point[0].y, faces->pcfaces[i].point[0].z);
		glMaterialfv(GL_FRONT, GL_AMBIENT, color2);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, color2);
		glVertex3f(faces->pcfaces[i].point[1].x, faces->pcfaces[i].point[1].y, faces->pcfaces[i].point[1].z);
		glMaterialfv(GL_FRONT, GL_AMBIENT, color3);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, color3);
		glVertex3f(faces->pcfaces[i].point[2].x, faces->pcfaces[i].point[2].y, faces->pcfaces[i].point[2].z);
	}
	glEnd();
	checkOpenGLError(405);


	if (faces->tfaces.size() > 0)
	{
		// assumes all faces use same texture
		texture_manager.selectTexture(faces->tfaces[0].texture_id);
	}

	checkOpenGLError(406);
	glBegin(GL_TRIANGLES);

	for (unsigned int i=0; i<faces->tfaces.size(); i++)
	{
		float n[3];
		n[0] = faces->tfaces[i].normal.x;
		n[1] = faces->tfaces[i].normal.y;
		n[2] = faces->tfaces[i].normal.z;
		glNormal3fv(n);
		
		GLfloat face_color[]  = { faces->tfaces[i].color.r, faces->tfaces[i].color.g, faces->tfaces[i].color.b, 1.0f };
		fadeColor(face_color);	
		glMaterialfv( GL_FRONT, GL_AMBIENT, face_color );
		glMaterialfv( GL_FRONT, GL_DIFFUSE, face_color );
		glMaterialfv( GL_FRONT, GL_SPECULAR, white_color );
		glMaterialfv( GL_FRONT, GL_EMISSION, black_color );
		glMaterialf( GL_FRONT, GL_SHININESS, base_shininess);
	
		glTexCoord2f(faces->tfaces[i].texture_uv[0].u, faces->tfaces[i].texture_uv[0].v);
		glVertex3f(faces->tfaces[i].point[0].x, faces->tfaces[i].point[0].y, faces->tfaces[i].point[0].z);
		glTexCoord2f(faces->tfaces[i].texture_uv[1].u, faces->tfaces[i].texture_uv[1].v);
		glVertex3f(faces->tfaces[i].point[1].x, faces->tfaces[i].point[1].y, faces->tfaces[i].point[1].z);
		glTexCoord2f(faces->tfaces[i].texture_uv[2].u, faces->tfaces[i].texture_uv[2].v);
		glVertex3f(faces->tfaces[i].point[2].x, faces->tfaces[i].point[2].y, faces->tfaces[i].point[2].z);
	}
	glEnd();
	checkOpenGLError(410);

	if (faces->tfaces.size() > 0) 
	{
		texture_manager.disableTextures();
	}
	glPopMatrix(); 
	checkOpenGLError(412);
}

void LineModel::render(Matrix4x4& world_xform)
{
	glBegin(GL_LINES);
	GLfloat line_color[]  = { color.r, color.g, color.b, color.a };
	glMaterialfv( GL_FRONT, GL_AMBIENT, line_color );
	glMaterialfv( GL_FRONT, GL_DIFFUSE, line_color );
	glMaterialfv( GL_FRONT, GL_SPECULAR, white_color );
	glMaterialfv( GL_FRONT, GL_EMISSION, black_color );
	glMaterialf( GL_FRONT, GL_SHININESS, base_shininess);
	glVertex3f(start.x, start.y, start.z);
	glVertex3f(end.x, end.y, end.z);
	glEnd();
}

void CoordinateAxisModel::render(Matrix4x4& world_xform)
{
	if (!add_arrowheads)
	{
		glBegin(GL_LINES);

		glMaterialfv( GL_FRONT, GL_SPECULAR, white_color );
		glMaterialfv( GL_FRONT, GL_EMISSION, black_color );
		glMaterialf( GL_FRONT, GL_SHININESS, base_shininess);

		glMaterialfv( GL_FRONT, GL_AMBIENT, red_color );
		glMaterialfv( GL_FRONT, GL_DIFFUSE, red_color );
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(length, 0., 0.);

		glMaterialfv( GL_FRONT, GL_AMBIENT, green_color );
		glMaterialfv( GL_FRONT, GL_DIFFUSE, green_color );
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, length, 0.0f);
	
		glMaterialfv( GL_FRONT, GL_AMBIENT, blue_color );
		glMaterialfv( GL_FRONT, GL_DIFFUSE, blue_color );
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, 0.0f, length);

		glEnd();
	}
	else
	{
		//0.2  = 1
		//0.15 = .75
		//0.04 = .2
		float a = length;
		float b = 0.75*length;
		float c = 0.2*length;

		glBegin(GL_LINES);
		glMaterialfv( GL_FRONT, GL_SPECULAR, white_color );
		glMaterialfv( GL_FRONT, GL_EMISSION, black_color );
		glMaterialf( GL_FRONT, GL_SHININESS, base_shininess);

		glMaterialfv( GL_FRONT, GL_AMBIENT, red_color );
		glMaterialfv( GL_FRONT, GL_DIFFUSE, red_color );
		glVertex3f(-a, 0.0f, 0.0f);
		glVertex3f( a, 0.0f, 0.0f);
		glVertex3f( a, 0.0f, 0.0f); // TOP PIECE OF ARROWHEAD
		glVertex3f( b, c, 0.0f);
		glVertex3f( a, 0.0f, 0.0f); // BOTTOM PIECE OF ARROWHEAD
		glVertex3f( b, -c, 0.0f);

		glMaterialfv( GL_FRONT, GL_AMBIENT, green_color );
		glMaterialfv( GL_FRONT, GL_DIFFUSE, green_color );
		glVertex3f( 0.0f, a, 0.0f);
		glVertex3f( 0.0f, -a, 0.0f);
		glVertex3f( 0.0f, a, 0.0f); // TOP PIECE OF ARROWHEAD
		glVertex3f( c, b, 0.0f);
		glVertex3f( 0.0f, a, 0.0f); // BOTTOM PIECE OF ARROWHEAD
		glVertex3f( -c, b, 0.0f);

		glMaterialfv( GL_FRONT, GL_AMBIENT, blue_color );
		glMaterialfv( GL_FRONT, GL_DIFFUSE, blue_color );
		glVertex3f( 0.0f, 0.0f, a);
		glVertex3f( 0.0f, 0.0f, -a);
		glVertex3f( 0.0f, 0.0f, a); // TOP PIECE OF ARROWHEAD
		glVertex3f( 0.0f, c, b);
		glVertex3f( 0.0f, 0.0f, a); // BOTTOM PIECE OF ARROWHEAD
		glVertex3f( 0.0f, -c, b);
		glEnd();
	}
	checkOpenGLError(311);	
}
