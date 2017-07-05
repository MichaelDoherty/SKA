//-----------------------------------------------------------------------------
// Bone.cpp
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

#include <Core/SystemConfiguration.h>
#include <Animation/Bone.h>
#include <Animation/Skeleton.h>

Bone::Bone(int _id, Skeleton* skeleton_description)
	: id(_id), 
	name(NULL), length(1.0f), direction(0.0f,0.0f,1.0f), axis(0.0f,0.0f,1.0f),
	children(NULL), num_children(0), max_children(10)
{
	name = strClone("UNNAMED");
	axis_order[0] = CT_TX;
	axis_order[1] = CT_TY;
	axis_order[2] = CT_TZ;
	channel_order[0] = CT_INVALID;
	channel_order[1] = CT_INVALID;
	channel_order[2] = CT_INVALID;
	channel_order[3] = CT_INVALID;
	channel_order[4] = CT_INVALID;
	channel_order[5] = CT_INVALID;
	children = new Bone*[max_children];
	skeleton = skeleton_description;
	bone_object = NULL; 
	base_box = NULL;
	tip_box = NULL;
	parent = NULL; 
	bone_data_initialized=false; 
}

Bone::~Bone() 
{
	strDelete(name);
	// skeleton will delete children
}

void Bone::update()
{
	if (parent == NULL) 
	{
		Matrix4x4 base_xform = skeleton->getWorldTransformation();
		computeOffsetTransform();			// Computes B
		computeRotationTransform();			// Computes M
		L = B*M;
		W = base_xform*L;
		end_position.x = W.m[12];
		end_position.y = W.m[13];
		end_position.z = W.m[14];
	}

	else
	{
		computeRotationTransform();
		L = C*M*Cinv*B;
		W = parent->W * L;

		// NOTE: W for this bone includes translate to end of bone

		// extract bone's global coordinates for rendering
		position = parent->end_position;
		end_position.x = W.m[12];
		end_position.y = W.m[13];
		end_position.z = W.m[14];
		bone_object->setEndpoints(position, end_position);
	}
	if (base_box != NULL) base_box->moveTo(position);
	if (tip_box != NULL) tip_box->moveTo(end_position);

	// update children
	for (short i = 0; i<num_children; i++) children[i]->update();
}

// Compute the rotation due to current AMC frame angles
void Bone::computeRotationTransform()
{
	//logout << name << " ";
	M = Matrix4x4::identity();
	Matrix4x4 r;
	for (short d=0; d<6; d++)
	{
		if (channel_order[d] != CT_INVALID)
		{
			short channel_type = channel_order[d];
			if (channel_params[channel_type].valid)
			{
				switch (channel_type)
				{
				case CT_RX:
					//logout << "Rx";
					r = Matrix4x4::rotationPitch(orientation.pitch);
					M = r * M; 
					break;
				case CT_RY:
					//logout << "Ry";
					r = Matrix4x4::rotationYaw(orientation.yaw);
					M = r * M; 
					break;
				case CT_RZ:
					//logout << "Rz";
					r = Matrix4x4::rotationRoll(orientation.roll);
					M = r * M; 
					break;
				default:
					break;
				}

			}
		}
	}
	//logout << endl;
}

//====================================================================
// Bone Setup functions
//====================================================================
void Bone::addChild(Bone* _child)
{
	if (children == NULL)
	{
		max_children = 10;
		num_children = 0;
		children = new Bone*[max_children];
	}
	else if (num_children == max_children)
	{
		max_children *= 2;
		Bone** tmp = children;
		children = new Bone*[max_children];
		for (short i=0; i<num_children; i++) children[i] = tmp[i];
		delete [] tmp;
	}
	children[num_children++] = _child;
}

void Bone::precalcData()
{
	// C and Cinv from axis data
	computeLocalAxisTransform();		
	// use direction and length to create transform B from bone base to bone tip
	computeOffsetTransform();
	// precalc children
	for (short i = 0; i<num_children; i++) children[i]->precalcData();
	bone_data_initialized = true;
}

void Bone::setPose(Vector3D p, Vector3D a)
{
	short d;
	for (d=0; d<6; d++)
	{
		if (!channel_params[d].valid) continue;
		switch (d)
		{
		case 0: position.x = p.x; break;
		case 1: position.y = p.y; break;
		case 2: position.z = p.z; break;
		case 3: orientation.pitch = a.pitch; break;
		case 4: orientation.yaw = a.yaw; break;
		case 5: orientation.roll = a.roll; break;
		}
	}
}

// compute axis transformations from bone axis angles
void Bone::computeLocalAxisTransform()
{
	CHANNEL_TYPE* axis_order = getAxisOrder();
	Vector3D axis = getAxis();

	C = Matrix4x4::identity();
	for (short d=0; d<3; d++)
	{
		Matrix4x4 r;
		switch(axis_order[d])
		{
		case CT_RX:
			r = Matrix4x4::rotationPitch(axis.pitch);
			C = r * C;
			break;
		case CT_RY:
			r = Matrix4x4::rotationYaw(axis.yaw);
			C = r * C;
			break;
		case CT_RZ:
			r = Matrix4x4::rotationRoll(axis.roll);
			C = r * C;
			break;
		default:
			break;
		}
	}
	Cinv = C.cheapInverse(true);
}

void Bone::computeOffsetTransform()
{
	if (parent == NULL)
	{
		B = Matrix4x4::translationXYZ(position);
	}
	else
	{
		Vector3D scaled_dir = getDirection() * getLength();
		B = Matrix4x4::translationXYZ(scaled_dir);		
	}
}

//********************************************************************************
// Model Setup
//********************************************************************************

void Bone::constructRenderObject(Color color)
{
	if (bone_object != NULL) delete bone_object;
	bone_object = NULL;

	ModelSpecification bonespec("Bone",color);

	bone_object = new BoneObject(bonespec,
			Vector3D(0.0f, 0.0f, 0.0f), 
			Vector3D(0.0f, 0.0f, 0.0f));

	bone_object->setLength(getLength());
	bone_object->drawAsMesh();

	if ((name == string("lfemur")) || (name == string("lhumerus"))) 
		bone_object->special();

	if (skeleton->jointBoxesVisible() && (id > 0))
	{
		Color color = Color(1.0f,1.0f,1.0f);
		ModelSpecification tip_spec("Box", color);
		tip_spec.addSpec("length", "0.5");
		tip_spec.addSpec("width", "0.5");
		tip_spec.addSpec("height", "0.5");

		color = Color(0.0f,1.0f,0.0f);
		ModelSpecification base_spec("Box", color);
		base_spec.addSpec("length", "0.5");
		base_spec.addSpec("width", "0.5");
		base_spec.addSpec("height", "0.5");

		tip_box = new Object(tip_spec,
			Vector3D(0.0f, 0.0f, 0.0f), 
			Vector3D(0.0f, 0.0f, 0.0f));
		base_box = new Object(base_spec,
			Vector3D(0.0f, 0.0f, 0.0f), 
			Vector3D(0.0f, 0.0f, 0.0f));
		if (tip_box != NULL) 
		{
			tip_box->scaleTo(Vector3D(0.5f, 1.0f, 1.0f));
			tip_box->rotateTo(Vector3D(0.0f, HALF_PI, 0.0f));
		}
		if (base_box != NULL) 
		{
			base_box->scaleTo(Vector3D(1.0f, 1.0f, 0.5f));
			tip_box->rotateTo(Vector3D(0.0f, 0.0f, HALF_PI));
		}
	}
}

//********************************************************************************
// Debugging Dumps
//********************************************************************************

ostream& operator<<(ostream& out, Bone& bone)
{
	out << endl << "DUMP BONE " << bone.name << " id " << bone.id << " DEFINITION:" << endl;

	out << "Name: " << bone.name << endl;
	out << "Direction: " << bone.direction << endl;
	out << "Length: " << bone.length << endl;

	out << "Axis: " << bone.axis << " " 
		<< bone.axis_order[0] << bone.axis_order[1] << bone.axis_order[2] <<endl;

	for (short d=0; d<6; d++)
	{
		if (!bone.channel_params[d].valid)
			out << "channel " << d << " is not enabled" << endl;
		else
			out << "channel " << d << " limits(" 
				<< bone.channel_params[d].min << "," << bone.channel_params[d].max << ")" << endl;
	}

	out << endl << "DUMP BONE " << bone.name << " id " << bone.id << " DATA: " << endl;

	out << "start position: " << bone.position << endl;
	out << "end position: " << bone.end_position << endl;
	out << "input orientation: " << bone.orientation << endl;

	float r,p,y;

	out << "M (motion data - dynamic angles):" << endl;
	out << bone.M;
	bone.M.factorEulerZYX(p, y, r);
	out << "M angles(deg) " << rad2deg(p) << "," << rad2deg(y) << "," << rad2deg(r) << endl;

	out << "B (translation relative to parent bone):" << endl;
	out << bone.B;
	bone.B.factorEulerZYX(p, y, r);
	out << "B angles(deg) " << rad2deg(p) << "," << rad2deg(y) << "," << rad2deg(r) << endl;

	out << "C (local axis transform - relative to parent bone):" << endl;
	out << bone.C;
	bone.C.factorEulerZYX(p, y, r);
	out << "C angles(deg) " << rad2deg(p) << "," << rad2deg(y) << "," << rad2deg(r) << endl;
	/*
	out << "Cinv (inverse of local axis transform):" << endl;
	out << bone.Cinv;
	bone.Cinv.factorEulerZYX(p, y, r);
	out << "Cinv angles(deg) " << rad2deg(p) << "," << rad2deg(y) << "," << rad2deg(r) << endl;
	*/
	out << "L (complete local transform):" << endl;
	out << bone.L;
	bone.L.factorEulerZYX(p, y, r);
	out << "L angles(deg) " << rad2deg(p) << "," << rad2deg(y) << "," << rad2deg(r) << endl;

	out << "W (complete world transform):" << endl;
	out << bone.W;
	bone.W.factorEulerZYX(p, y, r);
	out << "W angles(deg) " << rad2deg(p) << "," << rad2deg(y) << "," << rad2deg(r) << endl;

	return out;
}

