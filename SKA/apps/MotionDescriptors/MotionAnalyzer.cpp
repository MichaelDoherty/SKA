//-----------------------------------------------------------------------------
// MotionDescriptors project - Builds with SKA Version 4.0
//-----------------------------------------------------------------------------
// MotionAnalyzer.cpp
// Author:     Trevor Martin - April 2017
// Refactored: Michael Doherty - May 2017
//-----------------------------------------------------------------------------
// SKA configuration
#include <Core/SystemConfiguration.h>
// local application
#include "MotionAnalyzer.h"

void MotionAnalyzer::initialize(long _num_frames, float _frame_duration, Skeleton* _skel) {
	
	// Reset storage structures
	bone_data.clear();
	body_data.clear();
	frame_data_valid.clear();

	if (_skel == NULL)
	{
		ourSkel = NULL;
		num_frames = 0;
		frame_duration = 0.0;
		animation_frame = 0;
	}
	else
	{
		ourSkel = _skel;
		num_frames = _num_frames;
		frame_duration = _frame_duration;
		animation_frame = 0;

		// resize the storage structure to fit the new animation
		int i, j;
		int bone_count = ourSkel->numBones();
		//resize outer vector...
		bone_data.resize(num_frames + 1);
		body_data.resize(num_frames + 1);
		frame_data_valid.resize(num_frames + 1);
		for (i = 0; i < (int)frame_data_valid.size(); i++) {
			frame_data_valid[i] = false;
		}
		for (i = 0; i < (int)bone_data.size(); i++) {
			//...and each inner vector
			bone_data[i].resize(bone_count);
		}
		//empty bone to initialize vector
		for (i = 0; i < num_frames + 1; i++) {
			for (j = 0; j < bone_count; j++) {
				Bone *tmpBone = ourSkel->getBone(j);
				BoneData this_bone(tmpBone, i);
				bone_data[i][j] = this_bone;
			}
		}

		// reinitialize bone parameters (weight maps)
		//Init QoMWeightMap (all 1's for now)
		QoMWeightMap.clear();
		for (i = 0; i < bone_count; i++) {
			std::string boneName = bone_data[0][i].getBone()->getName();
			QoMWeightMap[boneName] = 1.0;
		}
		//Initializaing WeightMap for each animation
		weightMap["root"] = .497f;
		weightMap["Shoulder"] = .028f;
		weightMap["ForeArm"] = .016f; //elbow (beginning of ForeArm)
		weightMap["Hand"] = .006f;
		weightMap["UpLeg"] = .1f; //thigh (beginning of UpLeg)
		weightMap["Leg"] = .0465f; //knees (beginning of leg)
		weightMap["Foot"] = .0145f;
		weightMap["Head"] = .081f;
	}
}

// Function for tracking direction of motion of a bone
string MotionAnalyzer::trackDirection(Vector3D bone_pos, Plane _plane, string side, int frame, Vector3D & last_location) {
	Vector3D movement;
	string move_dir;
	float change;
	//change is a float that will determine pos/neg depending on plane
	//all we want is either TOWARD or AWAY FROM plane
	//need
	if (frame == 0) {
		last_location = bone_pos;
		return "";
	}

	else {
		movement = bone_pos - last_location;
		//p1p2 vector
		//distance is p1p2 vec dot normal vector
		Vector3D norm = _plane.getNormal();
		norm = norm.normalize();

		//get total distance from movement vector
		change = movement.dot(norm);
		last_location = bone_pos;
	}
	//now use side and change to determine if bone is moving towards or away from plane
	if ((change > 0.0f && side == "Positive") || (change < 0.0f && side == "Negative")) {
		move_dir = "Away From";
	}
	else if ((change > 0.0f && side == "Negative") || (change < 0.0f && side == "Positive")) {
		move_dir = "Towards";
	}
	return move_dir;
}

// DEPRICATED - use calcVel().
float MotionAnalyzer::calculateVelocity(Vector3D bone_pos, Plane _plane, int frames, Vector3D last_location) {
	Vector3D movement;
	float vel = 0.0;
	if (frames == 0) {
		last_location = bone_pos;
		return vel;
	}
	else {
		movement = bone_pos - last_location;
		//p1p2 vector
		//distance is p1p2 vec dot normal vector
		Vector3D norm = _plane.getNormal();
		norm = norm.normalize();
		vel = movement.dot(norm) / frame_duration;
	}
	return vel;
}

pair <Vector3D, float> MotionAnalyzer::calcBoundingSphere(vector <Vector3D> points) {
	//P1, Py, Pz
	//random, futhest from 1, furthest from y
	std::vector<Vector3D>::iterator it = points.begin();
	Vector3D P1 = points[0];//random point
	Vector3D Py, Pz;
	int i;
	float max_distance = 0.0f;
	float dist;
	//get max distance from P1
	for (it = points.begin(); it != points.end(); it++) {
		dist = P1.distance(*it);
		if (dist > max_distance) {
			max_distance = dist;
			Py = *it;
		}
	}
	//get max distance from Py
	for (it = points.begin(); it != points.end(); it++) {
		dist = Py.distance(*it);
		if (dist > max_distance) {
			max_distance = dist;
			Pz = *it;
		}
	}
	//initial center and radius
	Vector3D Q = (Py + Pz) / 2;
	float radius = (Py - Pz).magnitude();
	Vector3D G;
	for (i = 1; i < sizeof points / sizeof Vector3D; i++) {
		if (pow((points[i] - Q).magnitude(), 2) > pow(radius, 2)) {
			G = Q - ((points[i] - Q) / (points[i] - Q).magnitude())*radius;
			Q = (G + points[i]) / 2;
			radius = (points[i] - Q).magnitude();
		}
	}
	return make_pair(Q, radius);
}

/*
Velocity, Accleration and Jerk Functions
k = joint k
x = position
ti = time interval(i.e. frame)
deltaT = frame duration (GLOBAL)
V = velocity
a = Accel
j = jerk

V_k(ti) = (x_k(ti)-x_k(ti-1))/deltaT

a_k(ti) = (x_k(ti)-2x_k(ti-1)+x_k(ti-2))/deltaT^2)

j_k(ti) = (x_k(ti)-3x_k(ti-1)+3x_k(ti-2)-x_k(ti-3))/deltaT^3
*/

pair <Vector3D, float> MotionAnalyzer::calcVel(Vector3D p0, Vector3D p1) {
	Vector3D velocity;
	float v_mag;
	velocity = (p0 - p1) / frame_duration;
	v_mag = velocity.magnitude();

	pair <Vector3D, float> vel;
	vel = make_pair(velocity, v_mag);
	return vel;

}

pair <Vector3D, float> MotionAnalyzer::calcAccel(Vector3D p0, Vector3D p1, Vector3D p2) {
	Vector3D accel;
	float a_mag;
	accel = (p0 - p1 * 2 + p2) / (frame_duration * frame_duration);
	a_mag = accel.magnitude();

	pair <Vector3D, float> acc;
	acc = make_pair(accel, a_mag);
	return acc;

}

pair <Vector3D, float> MotionAnalyzer::calcJerk(Vector3D p0, Vector3D p1, Vector3D p2, Vector3D p3) {
	Vector3D jerk;
	float j_mag;
	jerk = (p0 - (p1) * 3 + (p2) * 3 - p3) / (frame_duration * frame_duration * frame_duration);
	j_mag = jerk.magnitude();

	pair <Vector3D, float> jerk_pair;
	jerk_pair = make_pair(jerk, j_mag);
	return jerk_pair;

}
//Curvature
//Mag(a_k(ti) X v_k(ti))/mag(v_k (ti))^3
//PER BONE
float MotionAnalyzer::calcCurvature(Vector3D accel, Vector3D velVector, float velMag) {

	Vector3D top = accel.cross(velVector);
	float top_mag = top.magnitude();
	return top_mag / pow(velMag, 3);

}

float MotionAnalyzer::calcRadiusOfCurvature(float curve) {
	return 1 / curve;
}

float MotionAnalyzer::calculateQoM(int frame) {
	std::map<std::string, float>::iterator it = QoMWeightMap.begin();
	float weight;//weight is represented as a percentage
	int avg_counter = 0;
	int i = 0;
	float runningSum = 0.0;
	for (i = 0; i < (int)bone_data[frame].size(); i++) {
		//avg_counter = 0;
		it = weightMap.begin();
		float tmp_vel = bone_data[frame][i].getVelocity_mag();
		std::string tmp_bone_name = bone_data[frame][i].getBone()->getName();

		// strip off "Left" or "Right" prefix
		if (tmp_bone_name.find("Right") == 0) tmp_bone_name = tmp_bone_name.substr(5);
		if (tmp_bone_name.find("Left") == 0) tmp_bone_name = tmp_bone_name.substr(4);

		while (it != weightMap.end()) {
			if (tmp_bone_name.compare(it->first) == 0) {//if we find the bone name in our weightMap
				avg_counter += 1;
				weight = it->second;
				float tmpVal = weight*tmp_vel;
				runningSum += tmpVal;
				//runningAvg /= (float)avg_counter;
				break;
			}
			else {
				it++;
			}
		}
	}
	return runningSum / (float)avg_counter;
}

Vector3D MotionAnalyzer::calculateCoM(int frame) {
	std::map<std::string, float>::iterator it = weightMap.begin();
	float weight;//weight is represented as a percentage
	int avg_counter = 0;
	int i = 0;
	Vector3D runningSum;
	for (i = 0; i < (int)bone_data[frame].size(); i++) {
		//avg_counter = 0;
		it = weightMap.begin();
		Vector3D tmp_pos = bone_data[frame][i].getBone()->getPosition();
		std::string tmp_bone_name = bone_data[frame][i].getBone()->getName();

		// strip off "Left" or "Right" prefix
		if (tmp_bone_name.find("Right") == 0) tmp_bone_name = tmp_bone_name.substr(5);
		if (tmp_bone_name.find("Left") == 0) tmp_bone_name = tmp_bone_name.substr(4);

		while (it != weightMap.end()) {
			if (tmp_bone_name.compare(it->first) == 0) {//if wefind the bone name in our weightMap
														//std::cout << "name match: " << BoneVector[frame][i].getBone()->getName() << " matches " << it->first << std::endl;
				avg_counter += 1;
				weight = it->second;
				float tmpValx = weight*tmp_pos.getX(); //no * overload? Just manually multiply x y z
				float tmpValy = weight*tmp_pos.getY();
				float tmpValz = weight*tmp_pos.getZ();
				Vector3D tmpVal(tmpValx, tmpValy, tmpValz);
				runningSum += tmpVal;
				//runningAvg /= (float)avg_counter;
				break;
			}
			else {
				it++;
			}
		}
	}
	return runningSum / (float)avg_counter;
}

void MotionAnalyzer::analyzeCurrentFrame(long frame_id, float _frame_duration)
{
	if (ourSkel == NULL) return;

	animation_frame = frame_id;
	if (_frame_duration > 0.0f) frame_duration = _frame_duration;

	// FIXIT!! possibly allow for recalculation?
	if (frame_data_valid[animation_frame]) return;

	/*Accumulate Motion Data*/
	int i;
	//PER BONE DATA
	for (i = 0; i < (int)bone_data[animation_frame].size(); i++) {
		//first set basic info for bone @ frame
		Vector3D bone_pos = bone_data[animation_frame][i].getBone()->getPosition();
		bone_data[animation_frame][i].setX(bone_pos.getX());
		bone_data[animation_frame][i].setY(bone_pos.getY());
		bone_data[animation_frame][i].setZ(bone_pos.getZ());
		bone_data[animation_frame][i].setStartPos(bone_pos);
		bone_data[animation_frame][i].setEndPos(bone_data[animation_frame][i].getBone()->getEndPosition());

		//Calculate and assign Velocity, Frame and Jerk
		//Both Magnitude and Vector
		Vector3D p0 = bone_pos;
		Vector3D p1, p2, p3;
		if (animation_frame > 3) {
			p1 = bone_data[animation_frame - 1][i].getStartPos();
			p2 = bone_data[animation_frame - 2][i].getStartPos();
			p3 = bone_data[animation_frame - 3][i].getStartPos();

			pair<Vector3D, float> vel = calcVel(p0, p1);
			bone_data[animation_frame][i].setVelocity_vec(vel.first);
			bone_data[animation_frame][i].setVelocity_mag(vel.second);

			pair<Vector3D, float> acc = calcAccel(p0, p1, p2);
			bone_data[animation_frame][i].setAcceleration_vec(acc.first);
			bone_data[animation_frame][i].setAcceleration_mag(acc.second);

			pair<Vector3D, float> jerk = calcJerk(p0, p1, p2, p3);
			bone_data[animation_frame][i].setJerk_vec(jerk.first);
			bone_data[animation_frame][i].setJerk_mag(jerk.second);

			float bone_curve = calcCurvature(acc.first, vel.first, vel.second);
			float bone_rad = calcRadiusOfCurvature(bone_curve);

			bone_data[animation_frame][i].setCurvature(bone_curve);
			bone_data[animation_frame][i].setCurvatureRadius(bone_rad);

		}
	}
	//FULL BODY DATA
	//Calculate Bounding box and sphere
	/*indexed as [MAX[x y z] MIN [x y z]]*/
	float max_min[6] = { 0.0,0.0,0.0,0.0,0.0,0.0 };
	float tmp_x, tmp_y, tmp_z = 0.0;
	for (i = 0; i < (int)bone_data[animation_frame].size(); i++) {
		Vector3D tmp = bone_data[animation_frame][i].getBone()->getPosition(); //makes all the calling easier to read/write
		tmp_x = tmp.getX();
		tmp_y = tmp.getY();
		tmp_z = tmp.getZ();
		//check x
		if (tmp_x > max_min[0]) {
			max_min[0] = tmp_x;
		}
		else if (tmp_x < max_min[3]) {
			max_min[3] = tmp_x;
		}
		//check y
		if (tmp_y > max_min[1]) {
			max_min[1] = tmp_y;
		}
		else if (tmp_y < max_min[4]) {
			max_min[4] = tmp_y;
		}
		//check z
		if (tmp_z > max_min[2]) {
			max_min[2] = tmp_z;
		}
		else if (tmp_z < max_min[5]) {
			max_min[5] = tmp_z;
		}

	}
	body_data[animation_frame].setFrame(animation_frame);
	body_data[animation_frame].setBoxParam(max_min);

	//Sphere
	//Make and initialize a Vector to pass to function
	vector <Vector3D> pointVector;
	i = 0;
	//pointVector has 'bone' items (BoneVector[animation_frame] has 'bone' elements)
	pointVector.resize(bone_data[animation_frame].size());
	std::vector<BoneData>::iterator it = bone_data[animation_frame].begin();
	for (it = bone_data[animation_frame].begin(); it != bone_data[animation_frame].end(); it++) {
		pointVector[i] = it->getStartPos();
		i++;
	}
	pair <Vector3D, float> sphere = calcBoundingSphere(pointVector);
	body_data[animation_frame].setBoundingSphere(sphere);

	//Center of Mass & Quantity of Motion Calculation
	Vector3D frame_CoM = calculateCoM(animation_frame);
	float QoM = calculateQoM(animation_frame);
	body_data[animation_frame].setCoM(frame_CoM);
	body_data[animation_frame].setQoM(QoM);

}
