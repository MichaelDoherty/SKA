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

#ifndef TEXTURES_DOT_H
#define TEXTURES_DOT_H
#include <Core/SystemConfiguration.h>
#include <vector>
using namespace std;
#include <Core/BasicException.h>
#include <Core/Utilities.h>

class SKA_LIB_DECLSPEC TextureException : public BasicException
{
public:
	TextureException() : BasicException("Unspecified Texture exception") { }
	TextureException(const char* _msg) : BasicException(_msg) { }
	TextureException(const TextureException& _other) : BasicException(_other) { }
};

typedef unsigned short TextureId;
typedef short TextureIndex;

class SKA_LIB_DECLSPEC Texture
{
private:
	TextureId id; 
	char* filename;
public:
	Texture(TextureId _id, char* _file)
		: id(_id), filename(NULL)
	{
		filename = strClone(_file);
	}
	virtual ~Texture() { strDelete(filename); }
	TextureId getId() { return id; }
	string getFilename() { return filename; }
};

class SKA_LIB_DECLSPEC TextureManager
{
private:
	struct TextureManagerData* data;
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

SKA_LIB_DECLSPEC extern TextureManager texture_manager;

#endif
