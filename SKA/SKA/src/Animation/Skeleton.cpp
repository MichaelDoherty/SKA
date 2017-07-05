//-----------------------------------------------------------------------------
// Skeleton.cpp
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

#include <Core/SystemConfiguration.h>
#include <map>
using namespace std;
#include <Animation/AnimationException.h>
#include <Animation/Skeleton.h>
#include <Animation/RawMotionController.h>
#include <Core/SystemLog.h>
#include <Core/Utilities.h>

// SkeletonLocalData hides some attributes from the SKA library interface.
// This was done to avoid STL complexities when building Microsoft DLLs.
class SkeletonLocalData
{
public:
	map<char*, char*> units;
	list<pair<char*,char*> > connections; // parent -> child
	map<char*, short> bone_name_to_id;
	map<short, char*> bone_id_to_name;

	SkeletonLocalData() {	}
	~SkeletonLocalData()
	{
		map<char*,char*>::iterator units_iter = units.begin();
		while (units_iter != units.end())
		{
			strDelete((*units_iter).first);
			strDelete((*units_iter).second);
			units_iter++;
		}
		map<char*,short>::iterator bni_iter = bone_name_to_id.begin();
		while (bni_iter != bone_name_to_id.end())
		{
			strDelete((*bni_iter).first);
			bni_iter++;
		}
		map<short,char*>::iterator inb_iter = bone_id_to_name.begin();
		while (inb_iter != bone_id_to_name.end())
		{
			strDelete((*inb_iter).second);
			inb_iter++;
		}
		list<pair<char*,char*> >::iterator conn_iter = connections.begin();
		while (conn_iter != connections.end())
		{
			strDelete((*conn_iter).first);
			strDelete((*conn_iter).second);
			conn_iter++;
		}
	}
};

Skeleton::Skeleton()
	: description1(NULL), description2(NULL)
{
	skel_id = strClone("UNNAMED");
	documentation = strClone("");
	source = strClone("");
	num_bones = 0;
	for (short b=0; b<MAX_SKELETON_BONES; b++) bone_array[b] = NULL;
	offset_position = Vector3D(0.0f,0.0f,0.0f);
	offset_rotation = Vector3D(0.0f,0.0f,0.0f);
	show_joint_boxes = true;
	local_data = new SkeletonLocalData;
}

Skeleton::Skeleton(const char* _id)
	: description1(NULL), description2(NULL)
{
	skel_id = strClone(_id);
	documentation = strClone("");
	source = strClone("");
	num_bones = 0;
	for (short b=0; b<MAX_SKELETON_BONES; b++) bone_array[b] = NULL;
	offset_position = Vector3D(0.0f,0.0f,0.0f);
	offset_rotation = Vector3D(0.0f,0.0f,0.0f);
	show_joint_boxes = true;
	local_data = new SkeletonLocalData;
}

Skeleton::~Skeleton()
{
	strDelete(skel_id);
	strDelete(documentation);
	strDelete(source);
	strDelete(description1);
	strDelete(description2);
	for (int b=0; b<num_bones; b++)
	{
		if (bone_array[b] != NULL)
		{
			delete bone_array[b];
			bone_array[b] = NULL;
		}
	}
	delete local_data;
}

char* Skeleton::getId() { return skel_id; }

short Skeleton::boneIdFromName(const char* name)
{
	map<char*,short>::iterator bni_iter = local_data->bone_name_to_id.begin();
	while (bni_iter != local_data->bone_name_to_id.end())
	{
		if (strcmp(name, (*bni_iter).first) == 0) return (*bni_iter).second;
		bni_iter++;
	}
	return -1;
}

char* Skeleton::boneNameFromId(short id)
{
	map<short,char*>::iterator iter = local_data->bone_id_to_name.find(id);
	if (iter == local_data->bone_id_to_name.end()) return NULL;
	return (*iter).second;
}

int Skeleton::numBones() { return num_bones; }

bool Skeleton::isActiveChannel(short bone_index, short channel_type)
{
	if (bone_array[bone_index] == NULL) return false;
	return bone_array[bone_index]->isValidChannel(channel_type);
}

short Skeleton::getParentBoneId(short id)
{
	char* child_name = boneNameFromId(id);
	list<pair<char*,char*> >::iterator iter = local_data->connections.begin();
	while (iter != local_data->connections.end())
	{
		if (strcmp((*iter).second,child_name) == 0)
			return boneIdFromName((*iter).first);
		iter++;
	}
	return -1;
}

Vector3D Skeleton::getRootPosition()
{
	return init_root_position;
}

Vector3D Skeleton::getRootOrientation()
{
	return init_root_orientation;
}

list<pair<char*,char*> >& Skeleton::getConnections()
{
	return local_data->connections;
}

void Skeleton::setId(const char* _id) 
{ 
	strDelete(skel_id);
	skel_id = strClone(_id); 
}

void Skeleton::clearDocumentation() 
{ 
	strDelete(documentation);
	documentation = strClone(""); 
}

void Skeleton::addDocumentation(const char* d) 
{ 
	if (documentation == NULL) 
	{
		documentation = strClone(d);
	}
	else
	{
		char* tmp = documentation;
		documentation = new char[strlen(tmp)+strlen(d)+1];
		strcpy(documentation, tmp);
		strcat(documentation, d);
		strDelete(tmp);
	}
}

char* Skeleton::getDocumentation() 
{ 
	return documentation; 
}

void Skeleton::setSource(const char* s)
{ 
	strDelete(source);
	source = strClone(s); 
}

char* Skeleton::getSource() 
{ 
	return source; 
}

void Skeleton::addUnits(const char* unit_name, const char* unit_value)
{
	char* uname = strClone(unit_name);
	char* uvalue = strClone(unit_value);
	local_data->units.insert(pair<char*,char*>(uname, uvalue));
}

void Skeleton::setRootPosition(float x, float y, float z)
{
	init_root_position = Vector3D(x, y, z);
}

void Skeleton::setRootOrientation(float pitch, float yaw, float roll)
{
	init_root_orientation = Vector3D(pitch, yaw, roll);
}

void Skeleton::createBone(short id, const char* name)
{
	bone_array[id] = new Bone(id, this);
	bone_array[id]->setName(name);
	local_data->bone_name_to_id.insert(pair<char*,short>(strClone(name),id));
	local_data->bone_id_to_name.insert(pair<short,char*>(id,strClone(name)));
	// This is a bit of a hack. It assumes that the given ids are all consequtive.
	if (id>num_bones-1) num_bones = id+1;
}

void Skeleton::setBoneDirection(short id, float x, float y, float z)
{
	if (bone_array[id] == NULL) throw AnimationException("Skeleton::setBoneDirection called for undefined bone");
	Vector3D dir(x,y,z);
	bone_array[id]->setDirection(dir);
}

void Skeleton::setBoneLength(short id, float length)
{
	if (bone_array[id] == NULL) throw AnimationException("Skeleton::setBoneLength called for undefined bone");
	bone_array[id]->setLength(length);
}
	
void Skeleton::setBoneAxis(short id, float values[3], CHANNEL_TYPE dof[3])
{
	if (bone_array[id] == NULL) throw AnimationException("Skeleton::setBoneAxis called for undefined bone");
	bone_array[id]->setAxisOrder(dof[0],0);
	bone_array[id]->setAxisOrder(dof[1],1);
	bone_array[id]->setAxisOrder(dof[2],2);
	Vector3D axis;
	for (short d=0; d<3; d++)
	{
		switch (dof[d])
		{
			case CT_RX: axis.x = values[d]; break;
			case CT_RY: axis.y = values[d]; break;
			case CT_RZ: axis.z = values[d]; break;
			default: break;
		}
	}
	bone_array[id]->setAxis(axis);
}

void Skeleton::setBoneChannels(short id, CHANNEL_TYPE* channels, short num_channels)
{
	if (bone_array[id] == NULL) throw AnimationException("Skeleton::setBoneChannels called for undefined bone");
	for (short d=0; d<num_channels; d++)
	{
		bone_array[id]->setChannelOrder(d, channels[d]);
		bone_array[id]->setValidChannel(channels[d],true);
	}	
}

void Skeleton::setBoneChannelLimit(short id, short idx, float min, float max)
{
	if (bone_array[id] == NULL) throw AnimationException("Skeleton::setBoneChannelLimit called for undefined bone");
	short channel_type = bone_array[id]->getChannelOrder(idx);
	bone_array[id]->setChannelLowerLimit(channel_type, min);
	bone_array[id]->setChannelUpperLimit(channel_type, max);
}

void Skeleton::addConnection(const char* parent, const char* child)
{
	local_data->connections.push_back(pair<char*,char*>(strClone(parent),strClone(child)));
}

void Skeleton::scaleBoneLengths(float _scale)
{
	if (num_bones == 0) return;
	int b;
	for (b=0; b<num_bones; b++)	
	{
		float length = bone_array[b]->getLength();
		bone_array[b]->setLength(_scale*length);
	}
	bone_array[0]->precalcData(); // recursively calls other bones
}

void Skeleton::interpretAMCData(int _bone_id, float _data[6], Vector3D& _pos, Vector3D& _angles)
{
	for (short d=0; d<6; d++)
	{
		CHANNEL_TYPE channel_type = bone_array[_bone_id]->getChannelOrder(d);
		if (channel_type != CT_INVALID)
		{
			if (bone_array[_bone_id]->isValidChannel(channel_type))
			{
				switch (channel_type)
				{
				case CT_TX: _pos.x = _data[d]; break;
				case CT_TY: _pos.y = _data[d]; break;
				case CT_TZ: _pos.z = _data[d]; break;
				case CT_RX: _angles.pitch = _data[d]; break;
				case CT_RY: _angles.yaw = _data[d]; break;
				case CT_RZ: _angles.roll = _data[d]; break;
				default: break;
				}
			}
		}
	}
}

void Skeleton::finalizeInitialization()
{
	short b;
	for (b=0; b<num_bones; b++)
	{
		short parent_id = getParentBoneId(b);
		if (parent_id < 0) continue;
		bone_array[parent_id]->addChild(bone_array[b]);
		bone_array[b]->setParent(bone_array[parent_id]);
	}
	bone_array[0]->precalcData(); // recursively processes all bones
}

void Skeleton::update(float _time)
{ 
	if (motion_controller == NULL) 
	{
		char s[1000];
		sprintf(s, "skeleton %s has no motion controller", getId());
		logout << s << endl;
		throw AnimationException(s);
		return;
	}

	// compute root transforms from offsets
	Matrix4x4 translation_xform = Matrix4x4::translationXYZ(offset_position);
	Matrix4x4 rotation_xform = Matrix4x4::rotationZXY(offset_rotation);

	world_xform = translation_xform*rotation_xform;

	for (short id=0; id<num_bones; id++)
	{
		Bone* bone = bone_array[id];
		if (bone != NULL)
		{
			short bid = bone->getID();
			Vector3D p, a;
			CHANNEL_ID cx(BONE_ID(bid),CT_TX);
			CHANNEL_ID cy(BONE_ID(bid),CT_TY);
			CHANNEL_ID cz(BONE_ID(bid),CT_TZ);
			CHANNEL_ID cpitch(BONE_ID(bid),CT_RX);
			CHANNEL_ID cyaw(BONE_ID(bid),CT_RY);
			CHANNEL_ID croll(BONE_ID(bid),CT_RZ);

			if (motion_controller->isValidChannel(cx, _time))
				p.x = motion_controller->getValue(cx, _time);
			if (motion_controller->isValidChannel(cy, _time))
				p.y = motion_controller->getValue(cy, _time);
			if (motion_controller->isValidChannel(cz, _time))
				p.z = motion_controller->getValue(cz, _time);
			if (motion_controller->isValidChannel(cpitch, _time))
				a.pitch = motion_controller->getValue(cpitch, _time);
			if (motion_controller->isValidChannel(cyaw, _time))
				a.yaw = motion_controller->getValue(cyaw, _time);
			if (motion_controller->isValidChannel(croll, _time))
				a.roll = motion_controller->getValue(croll, _time);
	
			if (id==0) p += offset_position;
			bone->setPose(p, a);
		}
		else
		{
			logout << "Missing BONE (id" << id << ")  in AnimSkeleton::update" << endl;
		}
	}
	bone_array[0]->update(); 
}

void Skeleton::constructRenderObject(list<Object*>& objects, Color color)
{
	for (short id=0; id<num_bones; id++)
	{
		Bone* bone = bone_array[id];
		bone->constructRenderObject(color);
		objects.push_back(bone->getBoneObject());
		if (bone->getBaseBox() != NULL) objects.push_back(bone->getBaseBox());
		if (bone->getTipBox() != NULL) objects.push_back(bone->getTipBox());
	}
}

void Skeleton::getBonePositions(const char* bone_name, Vector3D& start, Vector3D& end)
{
	short bone_id = boneIdFromName(bone_name);
	getBonePositions(bone_id, start, end);
}

void Skeleton::getBonePositions(int bone_id, Vector3D& start, Vector3D& end)
{
	Bone* bone = bone_array[bone_id];
	start = bone->getPosition();
	end = bone->getEndPosition();
}

void Skeleton::getBoneOrientation(const char* bone_name, Vector3D& orientation)
{
	short bone_id = boneIdFromName(bone_name);
	getBoneOrientation(bone_id, orientation);
}

void Skeleton::getBoneOrientation(int bone_id, Vector3D& orientation)
{
	Bone* bone = bone_array[bone_id];
	orientation = bone->getOrientation();
}

Bone* Skeleton::getBone(int bone_id)
{
	return bone_array[bone_id];
}

Bone* Skeleton::getBone(const char* bone_name)
{
	short bone_id = boneIdFromName(bone_name);
	if (bone_id == -1) return NULL;
	return bone_array[bone_id];
}

float Skeleton::getChannelValue(CHANNEL_ID& _channel)
{
	Bone* bone = bone_array[_channel.bone_id];
	if (bone == NULL) return 0.0f;
	switch (_channel.channel_type)
	{
	case CT_TX: return bone->getPosition().x;
	case CT_TY: return bone->getPosition().y;
	case CT_TZ: return bone->getPosition().z;
	case CT_RX: return bone->getOrientation().pitch;
	case CT_RY: return bone->getOrientation().yaw;
	case CT_RZ: return bone->getOrientation().roll;
	default: return 0.0f;
	}
	return 0.0f;
}

// Cycle through all frames in motion sequence and record
// all joint positions in the pmat table.

bool Skeleton::buildPositionMatrix(Array2D<float>& pmat)
{ 
	if (motion_controller == NULL) 
	{
		char s[1000];
		sprintf(s, "AnimSkeleton::buildPositionMatrix failed - skeleton %s has no motion controller", getId());
		logout << s << endl;
		throw AnimationException(s);
		return false;
	}

	// DANGEROUS! - (but this is only special case code)
	RawMotionController* rawctrl = (RawMotionController*)motion_controller;
	int num_frames = rawctrl->numFrames();

	pmat.resize(num_frames, 3*num_bones);

	for (int frame=0; frame<num_frames; frame++)
	{
		// compute skeletal transform for offsets
		Matrix4x4 translation_xform = Matrix4x4::translationXYZ(offset_position);
		Matrix4x4 rotation_xform = Matrix4x4::rotationZXY(offset_rotation);
		world_xform = translation_xform*rotation_xform;

		for (short id=0; id<num_bones; id++)
		{
			Bone* bone = bone_array[id];
			if (bone != NULL)
			{
				short bid = bone->getID();
				Vector3D p, a;
				CHANNEL_ID cx(BONE_ID(bid),CT_TX);
				CHANNEL_ID cy(BONE_ID(bid),CT_TY);
				CHANNEL_ID cz(BONE_ID(bid),CT_TZ);
				CHANNEL_ID cpitch(BONE_ID(bid),CT_RX);
				CHANNEL_ID cyaw(BONE_ID(bid),CT_RY);
				CHANNEL_ID croll(BONE_ID(bid),CT_RZ);
				
				if (rawctrl->isValidChannel(cx))
					p.x = rawctrl->getValueByFrame(cx, frame);
				if (rawctrl->isValidChannel(cy))
					p.y = rawctrl->getValueByFrame(cy, frame);
				if (rawctrl->isValidChannel(cz))
					p.z = rawctrl->getValueByFrame(cz, frame);
				if (rawctrl->isValidChannel(cpitch))
					a.pitch = rawctrl->getValueByFrame(cpitch, frame);
				if (rawctrl->isValidChannel(cyaw))
					a.yaw = rawctrl->getValueByFrame(cyaw, frame);
				if (rawctrl->isValidChannel(croll))
					a.roll = rawctrl->getValueByFrame(croll, frame);

				if (id==0) p += offset_position;
				bone->setPose(p, a);
			}
		}
		bone_array[0]->update(); 

		// now cycle through bones and record positions
		for (short id=0; id<num_bones; id++)
		{
			Vector3D start, end;
			getBonePositions(id, start, end);
			pmat.set(frame, id*3,   end.x);
			pmat.set(frame, id*3+1, end.y);
			pmat.set(frame, id*3+2, end.z);
		}
	}

	return true;
}

void Skeleton::dumpBoneList(ostream& ostr)
{
	for (int b=0; b<num_bones; b++)
	{
		string bone_name = boneNameFromId(b);
		ostr << bone_name << ", " << b << endl;
	}
}

ostream& operator<<(ostream& out, Skeleton& skel)
{
	out << "Skeleton dump =================================" << endl;
	out << "ID: " << skel.getId() << endl;
	out << "Documentation:" << skel.documentation << endl;
	out << "Units:" << endl;
	map<char*,char*>::iterator units_iter = skel.local_data->units.begin();
	while (units_iter != skel.local_data->units.end())
	{
		out << "\t" << (*units_iter).first << " = " << (*units_iter).second << endl;
		units_iter++;
	}

	for (short b=0; b<skel.num_bones; b++)
	{
		out <<  "BONE " << b << " --------------------------------" << endl;
		out << *(skel.bone_array[b]);
	}

	out << "HIERARCHY --------------------------" << endl;
	list<pair<char*,char*> >::iterator citer = skel.local_data->connections.begin();
	while (citer != skel.local_data->connections.end())
	{
		out << "\t" << (*citer).first << " " << (*citer).second << endl;
		citer++;
	}
	out << "======================== end Skeleton dump" << endl;

	return out;
}
