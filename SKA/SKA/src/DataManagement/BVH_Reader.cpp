//-----------------------------------------------------------------------------
// BVH_Reader.cpp
//	 Reads Biovision Hierarchical data file (BVH)
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
#include "Core/SystemConfiguration.h"
#include "Core/SystemLog.h"
#include "Core/Utilities.h"
#include "DataManagement/DataManagementException.h"
#include "DataManagement/BVH_Reader.h"
#include <fstream>
#include <cstdlib>
using namespace std;

/*==========================================================================
This parser is based on the following grammar:

<BVH_FILE> ::= <BVH_HIERARCHY> <BVH_MOTION>
<BVH_HIERARCHY> ::= “HIERARCHY”  { <ROOT> }
<ROOT> ::= “ROOT” <NAME> “{“ <DECL> “}”
<DECL> ::= <OFFSET_SPEC> <CHANNELS_SPEC> { <JOINT> | <ENDSITE>}
<JOINT> ::= “JOINT” <NAME> “{“ <DECL> “}”
<ENDSITE> ::= “End Site” “{“ <OFFSET_SPEC> “}”
<OFFSET_SPEC> ::= “OFFSET” <REAL> <REAL> <REAL> [ <REAL> <REAL> <REAL> ]
<CHANNELS_SPEC> ::= “CHANNELS” <INTEGER> <CHANNEL_LABEL>
<CHANNEL_LABEL > ::= “Xposition” | “Yposition” | “Zposition” | “Xrotation” | “Yrotation” | “Zrotation”

==========================================================================*/

//===================================================================

struct BVH_OFFSET_SPEC
{
	double x,y,z;
};

struct BVH_CHANNELS_SPEC
{
	vector<string> channel_labels;
};

enum BVH_DECL_TYPE { BVH_ROOT, BVH_JOINT, BVH_ENDSITE, BVH_INVALID_DECL };
struct BVH_DECL
{
	string name;
	enum BVH_DECL_TYPE decl_type;
	BVH_OFFSET_SPEC* offset;
	BVH_CHANNELS_SPEC* channels;
	vector<BVH_DECL*> children;

	BVH_DECL() : decl_type(BVH_INVALID_DECL), offset(NULL), channels(NULL) { }
	~BVH_DECL() { 
		if (offset != NULL) delete offset; 
		if (channels!=NULL) delete channels;
		for (unsigned int i=0; i<children.size(); i++) 
			if (children[i]!=NULL) delete children[i]; 
	}
};

struct BVH_MOTION
{
	long frames;
	float frame_time;
	long channels;
	Array2D<float> frame_data;
	BVH_MOTION(long _frames, long _channels)
	: frames(_frames), channels(_channels) { frame_data.resize(frames, channels); }
};

struct BVH_HIERARCHY
{
	vector<BVH_DECL*> roots;

	BVH_HIERARCHY() { }
	~BVH_HIERARCHY() 
	{ for (unsigned int i=0; i<roots.size(); i++) if (roots[i]!=NULL) delete roots[i]; }
};

struct BVH_FILE {
	string filename;
	BVH_HIERARCHY* hierarchy;
	BVH_MOTION* motion;

	BVH_FILE() : hierarchy(NULL), motion(NULL) { }
	~BVH_FILE() 
	{ if (hierarchy!=NULL) delete hierarchy; if (motion!= NULL) delete motion; }
};

//===================================================================

class BVH_Reader_Local
{
public:
	BVH_Reader_Local() : SKA2BVH_channel_map(NULL), next_bone_id(0), line_scanner(NULL), size_scale(1.0f)
	{ }
	virtual ~BVH_Reader_Local() 
	{ 
		if (line_scanner != NULL) delete line_scanner; 
		if (SKA2BVH_channel_map != NULL) delete [] SKA2BVH_channel_map;
	}
	pair<SkeletonDefinition*, MotionSequence*> readBVH(const char* inputFilename);

private:

	// conversion to SKA skeleton
	SkeletonDefinition* createSkeleton(BVH_FILE* bvh_file);
	void createSkeleton(BVH_DECL* decl, SkeletonDefinition* skeleton, string parent_name);

	// BVH_index = channel_reindexing[SKA_index]
	short num_BVH_channels;
	short num_SKA_channels;     
	short* SKA2BVH_channel_map;
	void set_SKA2BVH_channel_map(BVH_DECL* decl, short BVH_index, short SKA_index);
	void configureChannels(short bone_id, BVH_DECL* decl, SkeletonDefinition* skel, short my_BVH_channels, bool noDOF);

	// non-terminals
	BVH_FILE* parse_BVH_FILE();
	BVH_HIERARCHY* parse_BVH_HIERARCHY();
	BVH_MOTION* parse_BVH_MOTION(long _channels);
	BVH_DECL* parse_DECL();
	BVH_OFFSET_SPEC* parse_OFFSET_SPEC();
	BVH_CHANNELS_SPEC* parse_CHANNELS_SPEC();
	
	// terminals
	string parse_NAME();
	double parse_REAL();
	long parse_INTEGER();
	bool parse_KEYWORD(string keyword);

	// misc
	long countChannels(BVH_HIERARCHY* hier);
	short next_bone_id;
	short* setupReindexing(BVH_HIERARCHY* hier);

	// tokenizer
	string currentToken();
	void consumeToken();
	void pushbackToken(string token);
	bool getMoreTokens();
	LineScanner* line_scanner;
	list<string> token_buffer;
	short token_index;

public:
	float size_scale;
	void setSizeScale(float s) { size_scale = s; }
};

//==========================================================================*/

ostream& operator<<(ostream& os, BVH_FILE* ptr);

pair<SkeletonDefinition*, MotionSequence*> BVH_Reader_Local::readBVH(const char* inputFilename)
{
	pair<SkeletonDefinition*, MotionSequence*> result;
	result.first = NULL;
	result.second = NULL;

	if (line_scanner != NULL) delete line_scanner;
	line_scanner = new LineScanner(inputFilename);
	if (!line_scanner->fileIsOpen()) return result;

	BVH_FILE* bvh_parse_tree = parse_BVH_FILE();
	bvh_parse_tree->filename = inputFilename;

	/*
	logout << "START BVH PARSE DUMP" << endl;
	logout << bvh_parse_tree;
	logout << "END BVH PARSE DUMP" << endl;
	*/

	SkeletonDefinition* skel = createSkeleton(bvh_parse_tree);

	BVH_MOTION* motion = bvh_parse_tree->motion;
	MotionSequence* ms = new MotionSequence();
	ms->setNumFrames((short)motion->frames);
	//ms->setFrameRate(1.0f/motion->frame_time);
	ms->setFrameRate(120);
	ms->buildChannelsFromSkeleton(skel);

	for (short f=0; f<motion->frames; f++)
	{
		for (short SKA_channel=0; SKA_channel<num_SKA_channels; SKA_channel++)
		{
			short BVH_channel = SKA2BVH_channel_map[SKA_channel];
			float v = motion->frame_data.get(f, BVH_channel);
			if (BVH_channel < 3) v *= size_scale;
			ms->setValueByChannelIndex(SKA_channel, f, v);
		}
	}

	delete bvh_parse_tree;

	result.first = skel;
	result.second = ms;
	return result;
}

// =============================================================
// conversion to SKA skeleton
SkeletonDefinition* BVH_Reader_Local::createSkeleton(BVH_FILE* parse_tree)
{
	SkeletonDefinition* skeleton = new SkeletonDefinition(parse_tree->filename);
	BVH_HIERARCHY* hier = parse_tree->hierarchy;
	if (hier->roots.size() < 1) 
	{
		return NULL; // throw exception
	}
	if (hier->roots.size() > 1) 
	{
		// Just a warning.
		logout << "BVH file " << parse_tree->filename 
			<< " contains multiple hierarchies. Only first hierarchy is used." << endl;
	}

	// BVH_index = channel_reindexing[SKA_index]
	num_BVH_channels = 0;
	num_SKA_channels = 0;     
	SKA2BVH_channel_map = new short[1000];

	createSkeleton(hier->roots[0], skeleton, string(""));

	return skeleton;
}

void BVH_Reader_Local::createSkeleton(BVH_DECL* decl, SkeletonDefinition* skeleton, string parent_name)
{
	if ((decl->decl_type != BVH_ROOT) && (decl->decl_type != BVH_JOINT)) return;
	
	short my_BVH_channels = num_BVH_channels;
	if (decl->decl_type == BVH_ROOT) num_BVH_channels += 6;
	else num_BVH_channels += 3;

	// FIXIT! Create a root bone that receives six root DOF, then create the root children (HIPS) with no DOF.
	if (parent_name == string(""))
	{
		short id = next_bone_id++;
		parent_name = "root";
		skeleton->setBoneName(id, parent_name);
		
		// BVH does not support root offsets
		skeleton->setRootPosition(0,0,0);
		skeleton->setRootOrientation(0,0,0);

		Vector3D off(0.0f,0.0f,0.0f);
		float length = 0.0f;
		Vector3D dir(0.0f,0.0f,1.0f);
		skeleton->setBoneLength(id, length);
		skeleton->setBoneDirection(id, dir.x, dir.y, dir.z);
		
		configureChannels(id, decl, skeleton, my_BVH_channels, false);
	}

	// process this bone once for each of its children and build skeleton structure
	// If BVH bone has multiple children, it will result in multiple SKA bones.
	for (unsigned short child=0; child<decl->children.size(); child++)
	{
		short id = next_bone_id++;

		// set bone name
		string BVH_name = decl->name;
		string SKA_name = decl->name;
		if (decl->children.size() > 1) 
		{
			SKA_name += string("__");
			SKA_name += toString(child);
		}
		skeleton->setBoneName(id, SKA_name);

		float offx=0.0f, offy=0.0f, offz=0.0f;
		float length=0.0f;

		// set length and direction from child's offset
		offx = (float)decl->children[child]->offset->x;
		offy = (float)decl->children[child]->offset->y;
		offz = (float)decl->children[child]->offset->z;
		Vector3D off(offx, offy, offz);
		length = off.magnitude()*size_scale;
		Vector3D dir(0.0f,0.0f,1.0f);
		if (length > EPSILON) dir = off.normalize();
		skeleton->setBoneLength(id, length);
		skeleton->setBoneDirection(id, dir.x, dir.y, dir.z);

		bool noDOF = !(decl->decl_type == BVH_JOINT);
		configureChannels(id, decl, skeleton, my_BVH_channels, noDOF);

		// connect to parent
		skeleton->addConnection(parent_name, SKA_name);
		// build child
		createSkeleton(decl->children[child], skeleton, SKA_name);
	}
}

void BVH_Reader_Local::configureChannels(short bone_id, BVH_DECL* decl, SkeletonDefinition* skel, short my_BVH_channels, bool noDOF)
{
	if (noDOF)
	{
		skel->setBoneChannels(bone_id, NULL, 0);
		//skel->setBoneAxis(bone_id, values, dof);	
		return;
	}

	// DOF application order comes from the "CHANNELS" line
	DOF_ID dof_order[6];
	short num_dof = decl->channels->channel_labels.size();
	for (unsigned short i=0; i<num_dof; i++)
	{
		string label = decl->channels->channel_labels[i];
		if (label == string("Xposition")) dof_order[i] = DOF_X;
		else if (label == string("Yposition")) dof_order[i] = DOF_Y;
		else if (label == string("Zposition")) dof_order[i] = DOF_Z;
		else if (label == string("Xrotation")) dof_order[i] = DOF_PITCH;	
		else if (label == string("Yrotation")) dof_order[i] = DOF_YAW;
		else if (label == string("Zrotation")) dof_order[i] = DOF_ROLL;
	}

	skel->setBoneChannels(bone_id, dof_order, num_dof);
	
	// axis of rotation is z-axis?
	float values[3] = { 0.0f, 0.0f, 0.0f };
	// assuming BVH always applies roll, pitch, yaw (vR = v(rYrXrZ))
	DOF_ID dof[3] = { DOF_ROLL, DOF_PITCH, DOF_YAW};

	short i=0, j=0;
	for (i=0; i<num_dof; i++)
	{
		if      (dof_order[i] == DOF_PITCH) { dof[j] = DOF_PITCH; /*values[j] = dirx;*/ j++; }
		else if (dof_order[i] == DOF_YAW)   { dof[j] = DOF_YAW;   /*values[j] = diry;*/ j++; }
		else if (dof_order[i] == DOF_ROLL)  { dof[j] = DOF_ROLL;  /*values[j] = dirz;*/ j++; }
	}
	
	//if (num_dof==3) { dof[0] = DOF_ROLL; dof[1] = DOF_PITCH; dof[2] = DOF_YAW; }
	skel->setBoneAxis(bone_id, values, dof);	

	// channel map
	short my_SKA_channels = num_SKA_channels;
	if (decl->decl_type == BVH_ROOT) num_SKA_channels += 6;
	else num_SKA_channels += 3;
	set_SKA2BVH_channel_map(decl, my_BVH_channels, my_SKA_channels);
}

// =============================================================
// non-terminals

BVH_FILE* BVH_Reader_Local::parse_BVH_FILE()
{
	token_buffer.clear();
	if (!getMoreTokens()) throw DataManagementException(string("BVH file has no tokens"));
	
	BVH_FILE* file = new BVH_FILE;
	file->hierarchy = parse_BVH_HIERARCHY();
	long channels = countChannels(file->hierarchy);
	file->motion = parse_BVH_MOTION(channels);
	return file;
}

BVH_HIERARCHY* BVH_Reader_Local::parse_BVH_HIERARCHY()
{
	if (!parse_KEYWORD(string("HIERARCHY"))) 
		throw DataManagementException(string("BVH file: Invalid HIERARCHY LABEL"));
	BVH_HIERARCHY* hierarchy = new BVH_HIERARCHY;
	BVH_DECL* root = NULL;
	while ((root=parse_DECL()) != NULL) 
	{
		if (root->decl_type == BVH_ROOT) hierarchy->roots.push_back(root);
		else delete root; // throw exception?
	}
	return hierarchy;
}

BVH_DECL* BVH_Reader_Local::parse_DECL()
{
	BVH_DECL* decl = new BVH_DECL;
	if (parse_KEYWORD(string("ROOT"))) decl->decl_type = BVH_ROOT;
	else if (parse_KEYWORD(string("JOINT"))) decl->decl_type = BVH_JOINT;
	else if (parse_KEYWORD(string("End")))
	{
		if (parse_KEYWORD(string("Site"))) decl->decl_type = BVH_ENDSITE;
		else pushbackToken(string("End"));
	}
	if (decl->decl_type == BVH_INVALID_DECL) { delete decl; return NULL; }
	
	if (decl->decl_type == BVH_ENDSITE)
	{   // end site only has an offset
		parse_KEYWORD(string("{"));
		decl->offset = parse_OFFSET_SPEC();
		parse_KEYWORD(string("}"));
	}
	else 
	{	// roots and joints have a name, offset, channels and children
		decl->name = parse_NAME();
		parse_KEYWORD(string("{"));
		decl->offset = parse_OFFSET_SPEC();
		decl->channels = parse_CHANNELS_SPEC();
		BVH_DECL* child = NULL;
		while ((child=parse_DECL()) != NULL) decl->children.push_back(child);
		parse_KEYWORD(string("}"));
	}

	return decl;
}

BVH_OFFSET_SPEC* BVH_Reader_Local::parse_OFFSET_SPEC()
{
	if (!parse_KEYWORD(string("OFFSET"))) return NULL;
	BVH_OFFSET_SPEC* offset = new BVH_OFFSET_SPEC;
	offset->x = parse_REAL();
	offset->y = parse_REAL();
	offset->z = parse_REAL();
	return offset;
}

BVH_CHANNELS_SPEC* BVH_Reader_Local::parse_CHANNELS_SPEC()
{
	if (!parse_KEYWORD(string("CHANNELS"))) return NULL;
	BVH_CHANNELS_SPEC* channels = new BVH_CHANNELS_SPEC;
	string label;
	long num_channels = parse_INTEGER();
	for (long i=0; i<num_channels; i++)
	{
		label = parse_NAME();
		if ((label == string("Xposition")) ||
			(label == string("Yposition")) ||
			(label == string("Zposition")) ||
			(label == string("Xrotation")) ||
			(label == string("Yrotation")) ||
			(label == string("Zrotation")))		
			channels->channel_labels.push_back(label);
		else
		{
			pushbackToken(label);
			break;
		}
	}
	//+BVH::DEBUG+
	logout << "BVH_Reader_Local::parse_CHANNELS_SPEC ";
	for (unsigned short i=0; i<channels->channel_labels.size(); i++)
	{
		logout << channels->channel_labels[i] << " ";
	}
	logout << endl;
	//-BVH::DEBUG-

	return channels;
}

BVH_MOTION* BVH_Reader_Local::parse_BVH_MOTION(long _channels)
{
	if (!parse_KEYWORD(string("MOTION"))) return false;
	if (!parse_KEYWORD(string("Frames:"))) return false;
	long frames = parse_INTEGER();
	if (!parse_KEYWORD(string("Frame"))) return false;
	if (!parse_KEYWORD(string("Time:"))) return false;
	BVH_MOTION* motion = new BVH_MOTION(frames, _channels);
	motion->frame_time = (float)parse_REAL();
	long f,c;
	for (f=0; f<frames; f++)
		for (c=0; c<_channels; c++)
		{
			float v = (float)parse_REAL();
			// HACK! assume channels 3 and higher are angles in degrees. 
			// (only the root has translation channels)
			if (c > 2) v = deg2rad(v);
			// load channels in file order. does not attempt to reorder based on DOF order.
			motion->frame_data.set(f, c, v);
		}
	return motion;
}
//==================================================
// misc

long countChannelsRecursive(BVH_DECL* decl)
{
	long c=0;
	if ((decl->decl_type == BVH_ROOT)  || (decl->decl_type == BVH_JOINT))
	{
		c += decl->channels->channel_labels.size();
		for (unsigned long i=0; i<decl->children.size(); i++)
			c += countChannelsRecursive(decl->children[i]);
	}
	return c;
}

long BVH_Reader_Local::countChannels(BVH_HIERARCHY* hier)
{
	long c = 0;
	for (unsigned long i=0; i<hier->roots.size(); i++)
		c += countChannelsRecursive(hier->roots[i]);
	return c;
}

// BVH_index = channel_reindexing[SKA_index]
void BVH_Reader_Local::set_SKA2BVH_channel_map(BVH_DECL* decl, short BVH_index, short SKA_index)
{
	if ((decl->decl_type == BVH_ROOT)  || (decl->decl_type == BVH_JOINT))
	{
		BVH_CHANNELS_SPEC* cspec = decl->channels;
		short b = BVH_index;
		short s = SKA_index;
		short r = 0;
		short num_channels = cspec->channel_labels.size();
		if (num_channels == 6)
		{
			// translations
			if (cspec->channel_labels[r+0] == "Xposition")      SKA2BVH_channel_map[s]   = b;
			else if (cspec->channel_labels[r+0] == "Yposition") SKA2BVH_channel_map[s+1] = b;
			else if (cspec->channel_labels[r+0] == "Zposition") SKA2BVH_channel_map[s+2] = b;
			if (cspec->channel_labels[r+1] == "Xposition")      SKA2BVH_channel_map[s]   = b+1;
			else if (cspec->channel_labels[r+1] == "Yposition") SKA2BVH_channel_map[s+1] = b+1;
			else if (cspec->channel_labels[r+1] == "Zposition") SKA2BVH_channel_map[s+2] = b+1;
			if (cspec->channel_labels[r+2] == "Xposition")      SKA2BVH_channel_map[s ] =  b+2;
			else if (cspec->channel_labels[r+2] == "Yposition") SKA2BVH_channel_map[s+1] = b+2;
			else if (cspec->channel_labels[r+2] == "Zposition") SKA2BVH_channel_map[s+2] = b+2;
			r += 3;
			b += 3;
			s += 3;
		}
		// rotations
		if (cspec->channel_labels[r+0] == "Xrotation")      SKA2BVH_channel_map[s]   = b;
		else if (cspec->channel_labels[r+0] == "Yrotation") SKA2BVH_channel_map[s+1] = b;
		else if (cspec->channel_labels[r+0] == "Zrotation") SKA2BVH_channel_map[s+2] = b;
		if (cspec->channel_labels[r+1] == "Xrotation")      SKA2BVH_channel_map[s]   = b+1;
		else if (cspec->channel_labels[r+1] == "Yrotation") SKA2BVH_channel_map[s+1] = b+1;
		else if (cspec->channel_labels[r+1] == "Zrotation") SKA2BVH_channel_map[s+2] = b+1;
		if (cspec->channel_labels[r+2] == "Xrotation")      SKA2BVH_channel_map[s]   = b+2;
		else if (cspec->channel_labels[r+2] == "Yrotation") SKA2BVH_channel_map[s+1] = b+2;
		else if (cspec->channel_labels[r+2] == "Zrotation") SKA2BVH_channel_map[s+2] = b+2;
	}
}

//==================================================
// terminals

string BVH_Reader_Local::parse_NAME()
{
	string token = currentToken();
	consumeToken();
	return token;
}

double BVH_Reader_Local::parse_REAL()
{
	string token = currentToken();
	double v = atof(token.c_str());
	consumeToken();
	return v;
}

long BVH_Reader_Local::parse_INTEGER()
{
	string token = currentToken();
	long v = atoi(token.c_str());
	consumeToken();
	return v;
}

bool BVH_Reader_Local::parse_KEYWORD(string keyword)
{
	if (currentToken() == keyword)
	{
		consumeToken();
		return true;
	}
	return false;
}

//==================================================
// tokenizer

string BVH_Reader_Local::currentToken()
{
	if (token_buffer.size() < 1) return string("EOF");
	return token_buffer.front();
}

void BVH_Reader_Local::consumeToken()
{
	if (token_buffer.size() > 0) token_buffer.pop_front();
	if (token_buffer.size() < 1) getMoreTokens();
}

void BVH_Reader_Local::pushbackToken(string token)
{
	token_buffer.push_front(token);
}

bool BVH_Reader_Local::getMoreTokens()
{
	string line;
	if (!line_scanner->getNextLine(line)) return false;
	bool got_one = false;
	unsigned int i=0;
	string s;
	while (true)
	{
		while ((i<line.length()) && AAPU::iswhitespace(line[i])) i++;
		if (i>=line.length()) return true;
		s = "";
		while ((i<line.length()) && !AAPU::iswhitespace(line[i])) 
			s += line[i++];
		token_buffer.push_back(s);
		got_one = true;
	}
	return got_one;
}

//==================================================
// debugging

void indent(ostream& os, short indentation)
{
	for (short i=0; i<indentation; i++) os << '.';
}

void print(BVH_OFFSET_SPEC* offset, ostream& os, short indentation=0)
{
	os << endl;
	indent(os, indentation);
	if (offset == NULL) return;
	os << "OFFSET " << offset->x << " " << offset->y << " " << offset->z << " ";
}

void print(BVH_CHANNELS_SPEC* channels, ostream& os, short indentation=0)
{
	os << endl;
	indent(os, indentation);
	if (channels == NULL) return;
	os << "CHANNELS ";
	for (unsigned int i=0; i<channels->channel_labels.size(); i++)
		os << channels->channel_labels[i] << " ";
}

void print(BVH_DECL* decl, ostream& os, short indentation=0)
{
	if (decl == NULL) return;
	os << endl;
	indent(os, indentation);
	if (decl->decl_type == BVH_ROOT) os << "ROOT ";
	else if (decl->decl_type == BVH_JOINT) os << "JOINT ";
	else if (decl->decl_type == BVH_ENDSITE) os << "End Site ";
	if ((decl->decl_type == BVH_ROOT) || (decl->decl_type == BVH_JOINT))
		os << decl->name << " ";
	os << endl;
	print(decl->offset, os, indentation+1);
	if ((decl->decl_type == BVH_ROOT) || (decl->decl_type == BVH_JOINT))
	{
		os << endl;
		print(decl->channels, os, indentation+1);
		for (unsigned int i=0; i<decl->children.size(); i++)
			print(decl->children[i], os, indentation+1);
	}
}

void print(BVH_HIERARCHY* ptr, ostream& os)
{
	if (ptr == NULL) return;
	for (unsigned int i=0; i<ptr->roots.size(); i++) 
		print(ptr->roots[i], os);
}

void print(BVH_MOTION* ptr, ostream& os)
{
	if (ptr == NULL) return;
	os << "MOTION frames = " << ptr->frames << " channels = " << ptr->frame_data.getColumns()
		<< " frame time = " << ptr->frame_time << endl;
	long f = 0;
	long c;
	for (c=0; c<ptr->frame_data.getColumns(); c++)
		cout << "[" << f << "," << c << "] = " << ptr->frame_data.get(f,c) << endl;
	f = ptr->frames-1;
	for (c=0; c<ptr->frame_data.getColumns(); c++)
		cout << "[" << f << "," << c << "] = " << ptr->frame_data.get(f,c) << endl;
}

ostream& operator<<(ostream& os, BVH_FILE* ptr)
{
	if (ptr == NULL) return os;
	os << "BVH_FILE: " << ptr->filename << endl;
	print(ptr->hierarchy, os);
	print(ptr->motion, os);
	return os;
}

//==================================================
// Interface facade class

BVH_Reader::BVH_Reader() { }

BVH_Reader::~BVH_Reader() { } 

pair<SkeletonDefinition*, MotionSequence*> BVH_Reader::readBVH(const char* inputFilename)
{
	BVH_Reader_Local reader;
	reader.setSizeScale(size_scale);
	pair<SkeletonDefinition*, MotionSequence*> answer = reader.readBVH(inputFilename);
	return answer;
}

