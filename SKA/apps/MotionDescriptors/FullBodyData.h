//-----------------------------------------------------------------------------
// MotionDescriptors project - Builds with SKA Version 4.0
//-----------------------------------------------------------------------------
// FullBodyData.h
//			A class that holds low-level motion descriptors
//          that apply to the full body on frame by frame basis, rather than individual bones.
// Author: Trevor Martin
//-----------------------------------------------------------------------------

#ifndef FULLBODYDATA_DOT_H
#define FULLBODYDATA_DOT_H
//Includes for vars
#include <Core/SystemConfiguration.h>
#include <Math/Plane.h>
#include <Animation/Skeleton.h>

// SKA modules
struct boundingBox {
	Vector3D min, max;
};

class FullBodyData {

private:
	int frame;
	boundingBox bounding_box;
	pair<Vector3D, float> BoundingSphere; // center and radius
	Vector3D CoM; //center of mass 
	float QoM; //Quantity of Motion

public:
	//default constructor
	FullBodyData() : frame(0) {};

	void setFrame(int frm) { frame = frm; }
	int getFrame() { return frame; }

	void setBoundingBox(const Vector3D& _min, const Vector3D& _max) {
		bounding_box.min = _min, bounding_box.max = _max;
	}
	boundingBox getBoundingBox() { return bounding_box; }

	void setBoundingSphere(Vector3D center, float radius) {
		BoundingSphere = make_pair(center, radius);
	}
	void setBoundingSphere(const pair<Vector3D, float>& sphere) { BoundingSphere = sphere; }
	pair <Vector3D, float> getBoundingSphere() { return BoundingSphere; }

	void setCoM(const Vector3D& centerOfMass) { CoM = centerOfMass; }
	Vector3D getCoM() { return CoM; }
	
	void setQoM(float quantity) { QoM = quantity; }
	float getQoM() { return QoM; }
};


#endif //FULLBODYDATA_H
