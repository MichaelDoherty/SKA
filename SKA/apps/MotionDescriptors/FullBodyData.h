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
#include "BoneData.h"
// SKA modules
struct boundingBox {
	float max_x, max_y, max_z = 0.0;
	float min_x, min_y, min_z = 0.0;
};

class FullBodyData {

private:
	std::pair <Vector3D, float> BoundingSphere;
	int frame = 0;
	boundingBox BoundingBox;
	Vector3D CoM; //center of mass (default const is all 0s)
	float QoM = 0.0; //Quantity of Motion

public:
	//default constructor
	FullBodyData() {};

	//basic set functions
	void setFrame(int frm) { frame = frm; }
	void setCoM(Vector3D centerOfMass) { CoM = centerOfMass; }
	void setQoM(float quantity) { QoM = quantity; }

	//Set boundingSphere with center point and radiur
	void setBoundingSphere(Vector3D center, float radius){
		BoundingSphere = make_pair(center, radius);
	}

	//Set boundingSphere with  pre-existing pair
	void setBoundingSphere(pair <Vector3D, float> sphere) { BoundingSphere = sphere; }

	//basic get functions
	std::pair <Vector3D, float> getBoundingSphere() { return BoundingSphere; }
	float getQoM() { return QoM; }
	int getFrame() { return frame; }
	Vector3D getCoM() { return CoM; }

	//get bounding box params
	//(max value or min value in selected direction)
	float getBoxParam(bool max, char plane) {
		float val = 0.0;
		if (plane == 'x') {
			max ? val=  BoundingBox.max_x : val = BoundingBox.min_x;
		}
		else if (plane == 'y') {
			max ? val = BoundingBox.max_y : val = BoundingBox.min_y;
		}
		else if (plane == 'z') {
			max ? val = BoundingBox.max_z : val = BoundingBox.min_z;
		}
		return val;
	}

	//Returns a boundingBox struct object (defined in FullBodyData.h)
	//In most cases, getBoxParam will suffice for individual values
	boundingBox getBox() { return BoundingBox; }

	//Set function for bounding box with individual values
	void setBoxParam(char plane, bool max, float val) {
		if (plane == 'x') {
			max ? BoundingBox.max_x = val : BoundingBox.min_x = val;
		}
		else if (plane == 'y') {
			max ? BoundingBox.max_y = val : BoundingBox.min_y = val;
		}
		else if (plane == 'z') {
			max ? BoundingBox.max_z = val : BoundingBox.min_z = val;
		}
	}
	//Set function for bounding box with array
	void setBoxParam(float arr[6]) {
		BoundingBox.max_x = arr[0];
		BoundingBox.max_y = arr[1];
		BoundingBox.max_z = arr[2];
		BoundingBox.min_x = arr[3];
		BoundingBox.min_y = arr[4];
		BoundingBox.min_z = arr[5];
	}
};


#endif //FULLBODYDATA_H