//-----------------------------------------------------------------------------
// Skeleton.h
//	 The skeleton of an animated character.
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

#ifndef SKELETON_DOT_H
#define SKELETON_DOT_H
#include <Core/SystemConfiguration.h>
#include <iostream>
#include <list>
#include <map>
using namespace std;
#include <float.h>
#include <Animation/Channel.h>
#include <Animation/MotionController.h>
#include <Animation/Bone.h>
#include <Math/Math.h>
#include <Math/Vector3D.h>
#include <Math/Matrix4x4.h>
#include <Core/SystemLog.h>
#include <Core/Utilities.h>
#include <Core/Array2D.h>

// SkeletonLocalData is a kludge to hide STL data structures
// from the Windows DLL interface.
class SkeletonLocalData;

static const short MAX_SKELETON_BONES = 50;

class SKA_LIB_DECLSPEC Skeleton
{
	SKA_LIB_DECLSPEC friend ostream& operator<<(ostream& out, Skeleton& skel);

private:

	// ========== data defined by skeleton loader ===========
	
	char* skel_id;			// unique identifier name
	char* documentation;	// comments - not used for animation
	char* source;			// comments - not used for animation
	
	short num_bones;
	Bone* bone_array[MAX_SKELETON_BONES];

	Vector3D init_root_position;
	Vector3D init_root_orientation;
	SkeletonLocalData* local_data;

	// ========== data defined by application program ===========

	char* description1;
	char* description2;

	MotionController* motion_controller;

	Vector3D offset_position;
	Vector3D offset_rotation;

	Matrix4x4 world_xform;

	bool show_joint_boxes;

public:

	Skeleton();
	Skeleton(const char* _id);
	~Skeleton();

	// ========== functions called by skeleton loader ===========
	void setId(const char* _id);
	void clearDocumentation();
	void addDocumentation(const char* d);
	void setSource(const char* s);
	void addUnits(const char* unit_name, const char* unit_value);
	void setRootPosition(float x, float y, float z);
	void setRootOrientation(float pitch, float yaw, float roll);
	// createBone must be called before any of the setBone* methods
	void createBone(short id, const char* name);
	void setBoneDirection(short id, float x, float y, float z);
	void setBoneLength(short id, float length);
	void setBoneAxis(short id, float values[3], CHANNEL_TYPE dof[3]);
	void setBoneChannels(short id, CHANNEL_TYPE* channels, short num_channels);
	void setBoneChannelLimit(short id, short idx, float min, float max);
	void addConnection(const char* parent, const char* child);
	void scaleBoneLengths(float _scale=1.0f);
	void interpretAMCData(int _bone_id, float _data[6], Vector3D& _pos, Vector3D& _angles);
	void finalizeInitialization();

	// ========== functions for accessing data defined by skeleton loader ===========
	char* getId();
	char* getDocumentation();
	short boneIdFromName(const char* name);
	char* boneNameFromId(short id);
	int numBones();
	bool isActiveChannel(short bone_index, short dof_index); // should this accept a CHANNEL_ID parameter?
	short getParentBoneId(short id);
	char* getSource();
	Vector3D getRootPosition();
	Vector3D getRootOrientation();
	list<pair<char*,char*> >& getConnections();

	// === run-time interface ===
	char* getDescription1() { return description1; }
	char* getDescription2() { return description2; }
	void setDescription1(const char* _d) 
	{
		strDelete(description1);
		description1 = strClone(_d);
	}
	void setDescription2(const char* _d) 
	{
		strDelete(description2);
		description2 = strClone(_d);
	}

	void update(float _time);

	void getBonePositions(const char* bone_name, Vector3D& start, Vector3D& end);
	void getBonePositions(int bone_id, Vector3D& start, Vector3D& end);
	void getBoneOrientation(const char* bone_name, Vector3D& orientation);
	void getBoneOrientation(int bone_id, Vector3D& orientation);
	Bone* getBone(const char* bone_name);
	Bone* getBone(int bone_id);
	float getChannelValue(CHANNEL_ID& _channel);

	bool buildPositionMatrix(Array2D<float>& pmat);

	MotionController* getMotionController() { return motion_controller; }

	void attachMotionController(MotionController* _controller)
	{
		motion_controller = _controller;
	}

	void setOffsetPosition(Vector3D& offset)
	{
		offset_position = offset;
	}

	void setOffsetRotation(Vector3D& rotation)
	{
		offset_rotation = rotation;
	}

	Matrix4x4& getWorldTransformation()
	{
		return world_xform;
	}

	void showJointBoxes(bool flag=true)
	{ 
		show_joint_boxes = flag; 
	}

	bool jointBoxesVisible()
	{
		return show_joint_boxes;
	}

	// === run-time setup interface ===

	void constructRenderObject(list<Object*>& objects, Color color=Color(1.0f,1.0f,0.0f));

	void dumpBoneList(ostream& ostr);
};

#endif
