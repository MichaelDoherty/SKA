//-----------------------------------------------------------------------------
// Textures.cpp
//	 Class to load bitmap (.bmp) files into OpenGL 2D textures.
//   Depends on the EasyBMP library.
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
#include <Core/Utilities.h>
#include <Graphics/Textures.h>
#include <GL/glut.h>
#include <Core/SystemLog.h>
#include <Graphics/GraphicsInterface.h>
#include <EasyBMP/EasyBMP.h>

TextureManager texture_manager;

const short MAX_TEXTURES = 100;

struct TextureManagerData
{
	Texture* textures[MAX_TEXTURES];
	GLuint texture_names[MAX_TEXTURES];
	vector<char*> filepaths;
	bool opengl_initialized;
};

// A Texture's id is an index into the openGL texture names array.
// External users get back TextureIndexes, not texture IDs.

TextureManager::TextureManager()
{
	data = new TextureManagerData;
	for (int i=0; i<MAX_TEXTURES; i++) data->textures[i] = NULL;
	data->opengl_initialized = false;
}

TextureManager::~TextureManager()
{
	releaseAllTextures();
	delete data;
}

void TextureManager::addTextureFilepath(char* filepath)
{
	char* s = strClone(filepath);
	data->filepaths.push_back(s);
}

void TextureManager::releaseAllTextures()
{
	for (int i=0; i<MAX_TEXTURES; i++) 
		if (data->textures[i] != NULL) 
		{
			releaseTexture(data->textures[i]->getId());
			delete data->textures[i];
			data->textures[i] = NULL;
		}
}

void TextureManager::releaseTexture(TextureIndex index)
{
	if (data->textures[index] == NULL)
	{
		throw TextureException("TextureManager::releaseTexture() - Invalid TextureIndex");
		return;
	}
	glDeleteTextures(1, &(data->texture_names[data->textures[index]->getId()]));
	delete data->textures[index];
	data->textures[index] = NULL;
}

void TextureManager::selectTexture(TextureIndex index)
{
	if (!data->opengl_initialized)
	{
		throw TextureException("TextureManager::selectTexture() - selecting Texture before API initialization");
		return;
	}

	if (data->textures[index] == NULL)
	{
		throw TextureException("TextureManager::selectTexture() - Invalid TextureIndex");
		return;
	}
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	GLuint texname = data->texture_names[data->textures[index]->getId()];
	glBindTexture(GL_TEXTURE_2D, texname);
	glEnable(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void TextureManager::disableTextures()
{
	glDisable(GL_TEXTURE_2D);
}

// BMP loading is based on code from
// http://www.gamedeception.net/threads/5425-Loading-a-BMP-without-AuxDIBImageLoad()
TextureIndex TextureManager::loadTextureBMP(char* filename)
{
	if (!data->opengl_initialized)
	{
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glGenTextures(MAX_TEXTURES, data->texture_names);
		data->opengl_initialized = true;
	}

	checkOpenGLError(0);
	unsigned long img_width = 0;	
	unsigned long img_height = 0;		
	unsigned char* bmp_data = NULL;

	TextureIndex new_index = -1;
	// find a free index
	for (TextureIndex i=0; i<MAX_TEXTURES; i++)
	{
		if (data->textures[i] == NULL) 
		{
			new_index = i;
			break;
		}
	}
	if (new_index == -1) return new_index;

	GLuint texture_name = data->texture_names[new_index];

	char s[3000];
	sprintf(s, "Loading texture %s index %d name %d", filename, new_index, texture_name);
	logout << s << endl;
	
	BMP bmp;
	bool file_found = false;

	SetEasyBMPwarningsOff();
	file_found = bmp.ReadFromFile(filename);
	if (!file_found)
	{
		for (unsigned short i=0; i<data->filepaths.size(); i++)
		{
			char* s = new char[strlen(data->filepaths[i])+strlen(filename)+1];
			sprintf(s, "%s/%s", data->filepaths[i], filename);
			file_found = bmp.ReadFromFile(s);
			if (file_found) break;
		}
	}
	SetEasyBMPwarningsOn();

	if (!file_found) return -1;

	img_width = bmp.TellWidth();
	img_height = bmp.TellHeight();
	int bit_depth = bmp.TellBitDepth(); // 24
	int bytes_per_pixel = bit_depth/8;
	int data_size = img_width*img_height*bytes_per_pixel;
	bmp_data = (unsigned char*)malloc(data_size);
	for (unsigned int i=0; i<img_width; i++)
	{
		for (unsigned int j=0; j<img_height; j++)
		{
			RGBApixel* p = bmp(i,j);
			int k = (i+j*img_width)*bytes_per_pixel;
			bmp_data[k] = p->Red;
			bmp_data[k+1] = p->Green;
			bmp_data[k+2] = p->Blue;
		}
	}

	checkOpenGLError(1);
	glBindTexture(GL_TEXTURE_2D, texture_name);
	checkOpenGLError(2);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	checkOpenGLError(3);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	checkOpenGLError(4);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	checkOpenGLError(5);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	checkOpenGLError(6);
	glEnable(GL_TEXTURE_2D);
	checkOpenGLError(7);

	glTexImage2D(GL_TEXTURE_2D, 
		0, GL_RGB, 
		img_width, img_height, 
		0, GL_RGB, 
		GL_UNSIGNED_BYTE, bmp_data);
	free(bmp_data);

	checkOpenGLError(8);

	data->textures[new_index] = new Texture(new_index, filename);
	return new_index;
}
