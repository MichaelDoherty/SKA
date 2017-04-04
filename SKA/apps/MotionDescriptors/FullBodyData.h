//-----------------------------------------------------------------------------
// MotionDescriptors project - Builds with SKA Version 4.0
//-----------------------------------------------------------------------------
// FullBodyData.h
//			A class that holds low-level motion descriptors
//          that apply to the full body, rather than individual bones.
// Author: Trevor Martin
//
//
//-----------------------------------------------------------------------------

#ifndef FULLBODYDATA_H
#define FULLBODYDATA_H
//Includes for vars
#include <Core/SystemConfiguration.h>
#include "Plane.h"
#include "Animation/Skeleton.h"
#include "BoneData.h"
// SKA modules
struct boundingBox {
	float max_x, max_y, max_z = 0.0;
	float min_x, min_y, min_z = 0.0;
};

class FullBodyData {
private:
	int frame = 0;
	boundingBox BoundingBox;

public:
	//default constructor
	FullBodyData() {};

	//basic set and get funcs
	void setFrame(int frm) { frame = frm; }
	int getFrame() { return frame; }

	//get bounding box params
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