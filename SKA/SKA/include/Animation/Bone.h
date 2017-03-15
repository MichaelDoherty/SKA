//-----------------------------------------------------------------------------
// Bone.h
//	 One bone in a character skeleton.
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

#ifndef BONE_DOT_H
#define BONE_DOT_H
#include <Core/SystemConfiguration.h>
#include <cfloat>
#include <iostream>
using namespace std;
#include <Animation/Channel.h>
#include <Objects/BoneObject.h>
#include <Core/Utilities.h>

class Skeleton;

class SKA_LIB_DECLSPEC Bone
{
	SKA_LIB_DECLSPEC friend ostream& operator<<(ostream& out, Bone& bone);

private:

	//==== STATIC ATTRIBUTES - defined when skeleton is loaded

	// identification
	short id;		// id is also the bone index in the skeleton
	char* name;

	// physical definition of the bone
	float  length;				// length of bone
	Vector3D direction;			// direction vector (in parent's coordinate system)

	// axis of rotation - only used with ASF skeletons
	Vector3D axis;				// axis of rotation
	CHANNEL_TYPE axis_order[3];	// axis angle application order (right to left)

	// parameters for each possible channel, indexed by CHANNEL_TYPE
	struct SKA_LIB_DECLSPEC ChannelParameters
	{
		bool valid;
		float min;
		float max;
		ChannelParameters() : valid(false), min(-1.0f*FLT_MAX), max(FLT_MAX) { }
	} channel_params[NUMBER_OF_CHANNEL_TYPES];
	
	// channel application order (right to left)
	CHANNEL_TYPE channel_order[NUMBER_OF_CHANNEL_TYPES]; 

	// skeleton structure
	Skeleton* skeleton;
	Bone* parent;			// if parent==NULL, this is the root 
	Bone** children;	
	short num_children;
	short max_children;

	//==== ANIMATION COMPUTATION VARIABLES

	// C and Cinv are only needed when axis is used
	Matrix4x4 C;		// local axis transform (relative to parent bone)
	Matrix4x4 Cinv;		// inverse of local axis transform

	Matrix4x4 B;		// translation relative to parent bone (from direction and length)
						// translation from start_position to end_position

	Matrix4x4 M;		// motion data (dynamic angles)
	Matrix4x4 L;		// complete local transform
	Matrix4x4 W;		// complete world transform

	Vector3D position;			// base of bone (world coordinates)
	Vector3D end_position;		// tip of bone (world coordinates) 
	Vector3D orientation;

	bool  bone_data_initialized;	// indicates that precalcData() has been run
	
	//==== CONNECTION TO GRAPHICS SYSTEM

	BoneObject* bone_object;
	Object* base_box;
	Object* tip_box;

public:
	Bone(int _id, Skeleton* skeleton_definition);
	virtual ~Bone();
	
	//==== STATIC ATTRIBUTES

	// identification
	short			getID() { return id; }
	const char*		getName() { return name; }
	void			setName(const char* _name) { strDelete(name); name = strClone(_name); }

	// physical definition of the bone
	float			getLength() { return length; }
	void			setLength(float _length) { length = _length; }
	Vector3D		getDirection() { return direction; }
	void			setDirection(Vector3D _dir) { direction = _dir; }

	// axis of rotation - only used with ASF skeletons
	Vector3D		getAxis() { return axis; }
	void			setAxis(Vector3D _axis) { axis = _axis; }
	CHANNEL_TYPE*	getAxisOrder() { return axis_order; }
	CHANNEL_TYPE	getAxisOrder(int index) { return axis_order[index]; }
	void			setAxisOrder(CHANNEL_TYPE ct, int index) { axis_order[index] = ct; }

	// parameters for each possible channel, indexed by CHANNEL_TYPE
	bool			isValidChannel(int c) { return channel_params[c].valid; }
	void			setValidChannel(int c, bool v) { channel_params[c].valid = v; }
	float			getChannelLowerLimit(int c) { return channel_params[c].min; }
	void			setChannelLowerLimit(int c, float v) { channel_params[c].min = v; }
	float			getChannelUpperLimit(int c) { return channel_params[c].max; }
	void			setChannelUpperLimit(int c, float v) { channel_params[c].max = v; }

	// order that channels should be applied
	CHANNEL_TYPE	getChannelOrder(int index) { return channel_order[index]; }
	void			setChannelOrder(int index, CHANNEL_TYPE c) { channel_order[index] = c; }

	// skeleton structure
	Bone*			getParent() { return parent; }
	void			addChild(Bone* _child);
	void			setParent(Bone* _parent) { parent = _parent; }
	
	//==== ANIMATION COMPUTATIONS

	Vector3D		getPosition() { return position; }
	Vector3D		getEndPosition() { return end_position; } 
	Vector3D		getOrientation() { return orientation; }

	void			setPose(Vector3D p, Vector3D a);
	void			update();

	void			computeRotationTransform();
	void			computeOffsetTransform();
	void			computeLocalAxisTransform();
	void			precalcData();

	// These internal transformation are exported because they are needed
	// when removing bone axes. They are used to process the motion data to
	// pre-rotate the bone angles by the axis that is being removed.
	// C, Cinv and M are computed by precalcData(), so that method must be
	// called before attempting to use these transformations.
	Matrix4x4		getC() { return C; } 
	Matrix4x4		getCinv() { return Cinv; } 
	Matrix4x4		getM() { return M; }

	//==== CONNECTION TO GRAPHICS SYSTEM

	// constructRenderObject() must be called before the three get methods
	// can return valid objects.
	void			constructRenderObject(Color color=Color(1.0f,1.0f,0.0f));
	BoneObject*		getBoneObject() { return bone_object; }
	Object*			getBaseBox() { return base_box; }
	Object*			getTipBox() { return tip_box; }
};

#endif
