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

#include <Core/SystemConfiguration.h>
#include <Core/SystemLog.h>
#include <Core/Utilities.h>
#include <Core/Array2D.h>
#include <DataManagement/DataManagementException.h>
#include <DataManagement/BVH_Reader.h>
#include <fstream>
#include <cstdlib>
#include <vector>
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
	short channels;
	Array2D<float> frame_data;
	BVH_MOTION(long _frames, short _channels)
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
	unsigned short num_channels;
	BVH_MOTION* motion;

	BVH_FILE() : hierarchy(NULL), num_channels(0), motion(NULL) { }
	~BVH_FILE() 
	{ if (hierarchy!=NULL) delete hierarchy; if (motion!= NULL) delete motion; }
};

ostream& operator<<(ostream& os, BVH_FILE* ptr);
ostream& operator<<(ostream& os, BVH_HIERARCHY* ptr);
ostream& operator<<(ostream& os, BVH_MOTION* ptr);

//===================================================================

class BVH_Reader_Local
{
public:
	BVH_Reader_Local() : SKA2BVH_channel_map(NULL), next_bone_id(0), line_scanner(NULL)
	{ }
	virtual ~BVH_Reader_Local() 
	{ 
		if (line_scanner != NULL) delete line_scanner; 
		if (SKA2BVH_channel_map != NULL) delete [] SKA2BVH_channel_map;
	}
	pair<Skeleton*, MotionSequence*> readBVH(const char* inputFilename);

private:

	// conversion to SKA skeleton
	Skeleton* createSkeleton(BVH_FILE* bvh_file);
	void createSkeleton(BVH_DECL* decl, Skeleton* skeleton, string parent_name);

	// BVH_index = channel_reindexing[SKA_index]
	short num_BVH_channels;
	short num_SKA_channels;     
	short* SKA2BVH_channel_map;
	vector<CHANNEL_ID> channel_ids;
	void set_SKA2BVH_channel_map(BVH_DECL* decl, short BVH_index, short SKA_index);
	void configureChannels(short bone_id, BVH_DECL* decl, Skeleton* skel, short my_BVH_channels, bool noDOF);

	// non-terminals
	BVH_FILE* parse_BVH_FILE();
	BVH_HIERARCHY* parse_BVH_HIERARCHY();
	BVH_MOTION* parse_BVH_MOTION(short _channels);
	BVH_DECL* parse_DECL();
	BVH_OFFSET_SPEC* parse_OFFSET_SPEC();
	BVH_CHANNELS_SPEC* parse_CHANNELS_SPEC();
	
	// terminals
	string parse_NAME();
	double parse_REAL();
	long parse_INTEGER();
	bool parse_KEYWORD(string keyword);

	// misc
	short countChannels(BVH_HIERARCHY* hier);
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
};

//==========================================================================
void postParseProcess(BVH_DECL* decl, 
	short* channel_remap, bool* channel_isangle, 
	short& old_chan, short& new_chan)
{
	if (decl->channels != NULL)
	{	
		if (decl->decl_type == BVH_ROOT)
		{
			// keep all channels
			for (unsigned short i=0; i<decl->channels->channel_labels.size(); i++)
			{
				if ((decl->channels->channel_labels[i] == "Xposition") ||
					(decl->channels->channel_labels[i] == "Yposition") ||
					(decl->channels->channel_labels[i] == "Zposition"))
				{
					channel_remap[new_chan] = old_chan;
					channel_isangle[new_chan] = false;
					new_chan++;
					old_chan++;
				}
				else if ((decl->channels->channel_labels[i] == "Xrotation") ||
  					(decl->channels->channel_labels[i] == "Yrotation") ||
					(decl->channels->channel_labels[i] == "Zrotation"))
				{
					channel_remap[new_chan] = old_chan;
					channel_isangle[new_chan] = true;
					new_chan++;
					old_chan++;
				}
			}
		}
		else if (decl->decl_type == BVH_JOINT)
		{
			// remove translation channels
			unsigned short i=0;
			while (i<decl->channels->channel_labels.size())
			{
				if ((decl->channels->channel_labels[i] == "Xposition") ||
					(decl->channels->channel_labels[i] == "Yposition") ||
					(decl->channels->channel_labels[i] == "Zposition"))
				{
					for (unsigned int j=i; j<decl->channels->channel_labels.size()-1; j++)
						decl->channels->channel_labels[j] = decl->channels->channel_labels[j+1];
					decl->channels->channel_labels.pop_back();
					old_chan++;
				}
				else if ((decl->channels->channel_labels[i] == "Xrotation") ||
  					(decl->channels->channel_labels[i] == "Yrotation") ||
					(decl->channels->channel_labels[i] == "Zrotation"))
				{
					channel_remap[new_chan] = old_chan;
					channel_isangle[new_chan] = true;
					new_chan++;
					old_chan++;
					i++;
				}
			}
		}
		for (unsigned int i=0; i<decl->children.size(); i++)
			postParseProcess(decl->children[i], channel_remap, channel_isangle, old_chan, new_chan);
	}
}

short postParseProcess(BVH_HIERARCHY* hier, short* channel_remap, bool* channel_isangle)
{
	short old_chan = 0;
	short new_chan = 0;
	for (unsigned short i=0; i<hier->roots.size(); i++)
		postParseProcess(hier->roots[i], channel_remap, channel_isangle, old_chan, new_chan);
	return new_chan;
}

BVH_MOTION* postParseProcess(BVH_MOTION* old_mo, short* channel_remap, bool* channel_isangle, short new_chans)
{
	BVH_MOTION* new_mo = new BVH_MOTION(old_mo->frames, new_chans);
	new_mo->frame_time = old_mo->frame_time;
	for (unsigned short f=0; f<new_mo->frames; f++)
	{
		for (unsigned short c=0; c<new_mo->channels; c++)
		{
			short old_chan = channel_remap[c];
			float value = old_mo->frame_data.get(f,old_chan);
			if (channel_isangle[c]) value = deg2rad(value);
			new_mo->frame_data.set(f,c,value);
		}
	}
	return new_mo;
}

//==========================================================================

pair<Skeleton*, MotionSequence*> BVH_Reader_Local::readBVH(const char* inputFilename)
{
	pair<Skeleton*, MotionSequence*> result;
	result.first = NULL;
	result.second = NULL;

	if (line_scanner != NULL) delete line_scanner;
	line_scanner = new LineScanner(inputFilename);
	if (!line_scanner->fileIsOpen()) return result;

	// Interpret the BVH file, exactly as it is written
	BVH_FILE* bvh_parse_tree = parse_BVH_FILE();
	bvh_parse_tree->filename = inputFilename;

	// Adjust BVH data:
	// Remove any non-root translation channels.
	// Convert rotation channels from degrees to radians.
	
	short* channel_remap = new short[bvh_parse_tree->num_channels];
	bool* channel_isangle = new bool[bvh_parse_tree->num_channels];
	
	bvh_parse_tree->num_channels = postParseProcess(bvh_parse_tree->hierarchy, channel_remap, channel_isangle);
	
	BVH_MOTION* old_mo = bvh_parse_tree->motion;
	BVH_MOTION* new_mo = postParseProcess(old_mo, channel_remap, channel_isangle, bvh_parse_tree->num_channels);
	bvh_parse_tree->motion = new_mo;

	delete old_mo;
	delete [] channel_remap;
	delete [] channel_isangle;

	// Convert to SKA structures

	Skeleton* skel = createSkeleton(bvh_parse_tree);

	BVH_MOTION* motion = bvh_parse_tree->motion;
	MotionSequence* ms = new MotionSequence();
	ms->setNumFrames(motion->frames);
	ms->setFrameRate(1.0f/motion->frame_time);

	CHANNEL_ID* cid = new CHANNEL_ID[channel_ids.size()];
	for (unsigned short c=0; c<channel_ids.size(); c++)	cid[c] = channel_ids[c];
	Array2D<float> bulk_data(motion->frames, channel_ids.size());
	for (long f=0; f<motion->frames; f++)
	{
		for (short SKA_channel=0; SKA_channel<num_SKA_channels; SKA_channel++)
		{
			short BVH_channel = SKA2BVH_channel_map[SKA_channel];
			float v = motion->frame_data.get(f, BVH_channel);
			bulk_data.set(f, SKA_channel, v);
		}
	}
	ms->bulkBuild(cid, short(channel_ids.size()), bulk_data);
	delete [] cid;

	delete bvh_parse_tree;

	skel->finalizeInitialization();
	result.first = skel;
	result.second = ms;
	return result;
}

// =============================================================
// conversion to SKA skeleton
Skeleton* BVH_Reader_Local::createSkeleton(BVH_FILE* parse_tree)
{
	Skeleton* skeleton = new Skeleton(parse_tree->filename.c_str());
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

// This function is a bit tricky, since it needs to convert BVH joints to bones.
// To do this it inserts some intermediate bones at joints that connect multiple children.
void BVH_Reader_Local::createSkeleton(BVH_DECL* decl, Skeleton* skeleton, string parent_name)
{
	if ((decl->decl_type != BVH_ROOT) && (decl->decl_type != BVH_JOINT)) return;
	
	short my_BVH_channels = num_BVH_channels;
	if (decl->decl_type == BVH_ROOT) num_BVH_channels += 6;
	else num_BVH_channels += 3;
	
	//logout << "BVH_Reader_Local::createSkeleton " << decl->name 
	//	<< " " << my_BVH_channels
	//	<< " " << num_BVH_channels << endl;

	if (parent_name == string(""))
	{
		short id = next_bone_id++;
		parent_name = "root";
		skeleton->createBone(id, parent_name.c_str());
		
		// BVH does not support root offsets
		skeleton->setRootPosition(0,0,0);
		skeleton->setRootOrientation(0,0,0);

		float length = 0.0f;
		Vector3D dir(0.0f,0.0f,1.0f);
		skeleton->setBoneLength(id, length);
		skeleton->setBoneDirection(id, dir.x, dir.y, dir.z);
		
		configureChannels(id, decl, skeleton, my_BVH_channels, false);
	}

	// process this joint once for each of its children and build skeleton structure
	// If BVH joint has multiple children, it will result in multiple SKA bones.
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
		skeleton->createBone(id, SKA_name.c_str());

		float offx=0.0f, offy=0.0f, offz=0.0f;
		float length=0.0f;

		// set length and direction from child's offset
		offx = (float)decl->children[child]->offset->x;
		offy = (float)decl->children[child]->offset->y;
		offz = (float)decl->children[child]->offset->z;
		Vector3D off(offx, offy, offz);
		length = off.magnitude();
		Vector3D dir(0.0f,0.0f,1.0f);
		if (length > EPSILON) dir = off.normalize();
		skeleton->setBoneLength(id, length);
		skeleton->setBoneDirection(id, dir.x, dir.y, dir.z);

		bool noDOF = !(decl->decl_type == BVH_JOINT);

		configureChannels(id, decl, skeleton, my_BVH_channels, noDOF);

		// connect to parent
		skeleton->addConnection(parent_name.c_str(), SKA_name.c_str());
		// build child
		createSkeleton(decl->children[child], skeleton, SKA_name);
	}
}

void BVH_Reader_Local::configureChannels(short bone_id, BVH_DECL* decl, Skeleton* skel, short my_BVH_channels, bool noDOF)
{
	// BVH files do not specify axis angles 
	float axis_angles[3] = { 0.0f, 0.0f, 0.0f };
	CHANNEL_TYPE dof[3] = { CT_INVALID, CT_INVALID, CT_INVALID }; 
	skel->setBoneAxis(bone_id, axis_angles, dof);	

	if (noDOF)
	{
		skel->setBoneChannels(bone_id, NULL, 0);
		return;
	}

	// DOF application order comes from the "CHANNELS" line
	CHANNEL_TYPE dof_order[6];
	short num_dof = short(decl->channels->channel_labels.size());
	for (unsigned short i=0; i<num_dof; i++)
	{
		string label = decl->channels->channel_labels[i];
		if (label == string("Xposition")) 
		{ 
			dof_order[i] = CT_TX; 
			CHANNEL_ID c(bone_id, CT_TX);
			channel_ids.push_back(c); 
		}
		else if (label == string("Yposition")) 
		{ 
			dof_order[i] = CT_TY; 
			CHANNEL_ID c(bone_id, CT_TY);
			channel_ids.push_back(c); 
		}
		else if (label == string("Zposition"))
		{ 
			dof_order[i] = CT_TZ; 
			CHANNEL_ID c(bone_id, CT_TZ);
			channel_ids.push_back(c); 
		}
		else if (label == string("Xrotation"))	
		{ 
			dof_order[i] = CT_RX; 
			CHANNEL_ID c(bone_id, CT_RX);
			channel_ids.push_back(c); 
		}
		else if (label == string("Yrotation")) 
		{ 
			dof_order[i] = CT_RY; 
			CHANNEL_ID c(bone_id, CT_RY);
			channel_ids.push_back(c); 
		}
		else if (label == string("Zrotation")) 
		{ 
			dof_order[i] = CT_RZ; 
			CHANNEL_ID c(bone_id, CT_RZ);
			channel_ids.push_back(c); 
		}
	}
	// reverse angle application order. 
	// BVH defines in left-to-right order. SKA expects right-to-left order.
	if (num_dof == 6)
	{
		CHANNEL_TYPE tmp = dof_order[5];
		dof_order[5] = dof_order[3];
		dof_order[3] = tmp;
	}
	else if (num_dof == 3)
	{
		CHANNEL_TYPE tmp = dof_order[2];
		dof_order[2] = dof_order[0];
		dof_order[0] = tmp;
	}
	skel->setBoneChannels(bone_id, dof_order, num_dof);

	// channel map - used to translate file order channels to motion sequence channels
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
	if (!getMoreTokens()) throw DataManagementException("BVH file has no tokens");
	
	BVH_FILE* file = new BVH_FILE;
	file->hierarchy = parse_BVH_HIERARCHY();
	file->num_channels = countChannels(file->hierarchy);
	file->motion = parse_BVH_MOTION(file->num_channels);
	return file;
}

BVH_HIERARCHY* BVH_Reader_Local::parse_BVH_HIERARCHY()
{
	if (!parse_KEYWORD(string("HIERARCHY"))) 
		throw DataManagementException("BVH file: Invalid HIERARCHY LABEL");
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
	return channels;
}

BVH_MOTION* BVH_Reader_Local::parse_BVH_MOTION(short _channels)
{
	if (!parse_KEYWORD(string("MOTION"))) return NULL;
	if (!parse_KEYWORD(string("Frames:"))) return NULL;
	long frames = parse_INTEGER();
	if (!parse_KEYWORD(string("Frame"))) return NULL;
	if (!parse_KEYWORD(string("Time:"))) return NULL;
	BVH_MOTION* motion = new BVH_MOTION(frames, _channels);
	motion->frame_time = (float)parse_REAL();
	long f;
	short c;
	for (f=0; f<frames; f++)
		for (c=0; c<_channels; c++)
		{
			float v = (float)parse_REAL();
			// Load channels in file order.
			// Do not reorder based on DOF order.
			// Angles are left in degrees, they will be converted to radians during post-parse processing.
			motion->frame_data.set(f, c, v);
		}
	return motion;
}
//==================================================
// misc

short countChannelsRecursive(BVH_DECL* decl)
{
	short c=0;
	if ((decl->decl_type == BVH_ROOT)  || (decl->decl_type == BVH_JOINT))
	{
		c += short(decl->channels->channel_labels.size());
		for (unsigned short i=0; i<decl->children.size(); i++)
			c += countChannelsRecursive(decl->children[i]);
	}
	return c;
}

short BVH_Reader_Local::countChannels(BVH_HIERARCHY* hier)
{
	short c = 0;
	for (unsigned short i=0; i<hier->roots.size(); i++)
		c += countChannelsRecursive(hier->roots[i]);
	return c;
}

// BVH_index = channel_reindexing[SKA_index]
void BVH_Reader_Local::set_SKA2BVH_channel_map(BVH_DECL* decl, short BVH_index, short SKA_index)
{
	if ((decl->decl_type == BVH_ROOT)  || (decl->decl_type == BVH_JOINT))
	{
		for (unsigned int i=0; i<decl->channels->channel_labels.size(); i++) 
			SKA2BVH_channel_map[SKA_index+i] = BVH_index+i;
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
		while ((i<line.length()) && ParsingUtilities::iswhitespace(line[i])) i++;
		if (i>=line.length()) return true;
		s = "";
		while ((i<line.length()) && !ParsingUtilities::iswhitespace(line[i])) 
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
ostream& operator<<(ostream& os, BVH_HIERARCHY* ptr)
{
	if (ptr == NULL) return os;
	for (unsigned int i=0; i<ptr->roots.size(); i++) 
		print(ptr->roots[i], os);
	return os;
}

ostream& operator<<(ostream& os, BVH_MOTION* ptr)
{
	if (ptr == NULL) return os;
	os << "MOTION frames = " << ptr->frames << " channels = " << ptr->frame_data.getColumns()
		<< " frame time = " << ptr->frame_time << endl;
	long f = 0;
	long c;
	for (c=0; c<ptr->frame_data.getColumns(); c++)
		os << "[" << f << "," << c << "] = " << ptr->frame_data.get(f,c) << endl;
	f = ptr->frames-1;
	for (c=0; c<ptr->frame_data.getColumns(); c++)
		os << "[" << f << "," << c << "] = " << ptr->frame_data.get(f,c) << endl;
	return os;
}

ostream& operator<<(ostream& os, BVH_FILE* ptr)
{
	if (ptr == NULL) return os;
	os << "BVH_FILE: " << ptr->filename << " num_channels: " << ptr->num_channels << endl;
	os << ptr->hierarchy;
	os << ptr->motion;
	return os;
}

//==================================================
// Interface facade class

BVH_Reader::BVH_Reader() { }

BVH_Reader::~BVH_Reader() { } 

pair<Skeleton*, MotionSequence*> BVH_Reader::readBVH(const char* inputFilename)
{
	BVH_Reader_Local reader;
	pair<Skeleton*, MotionSequence*> answer = reader.readBVH(inputFilename);
	return answer;
}

