//-----------------------------------------------------------------------------
// Textures.h
//	 Class to load bitmap (.bmp) files into OpenGL 2D textures.
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
// Version 3.0 - July 18, 2014 - Michael Doherty
//-----------------------------------------------------------------------------
#ifndef TEXTURES_DOT_H
#define TEXTURES_DOT_H
#include "Core/SystemConfiguration.h"
#include <string>
#include <vector>
using namespace std;
#include "Core/BasicException.h"

class TextureException : public BasicException
{
public:
	TextureException() : BasicException(string("undefined exception")) { }
	TextureException(const string& _msg) : BasicException(_msg) { }
};

const short MAX_TEXTURES = 100;
typedef unsigned short TextureId;
typedef short TextureIndex;

class Texture
{
private:
	TextureId id; 
	string filename;
public:
	Texture(TextureId _id, char* _file)
		: id(_id), filename(_file)
	{
	}
	virtual ~Texture() { }
	TextureId getId() { return id; }
	string getFilename() { return filename; }
};

class TextureManager
{
private:
	Texture* textures[MAX_TEXTURES];
	vector<char*> filepaths;
	bool opengl_initialized;
public:
	TextureManager();
	virtual ~TextureManager();
	void addTextureFilepath(char* filepath);
	TextureIndex loadTextureBMP(char* filename);
	void releaseTexture(TextureIndex index);
	void releaseAllTextures();
	void selectTexture(TextureIndex index);
	void disableTextures();
};

extern TextureManager texture_manager;

SKA_LIB_DECLSPEC void addTextureFilepath(char* filepath);

#endif
