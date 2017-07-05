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
#include "ProcessControl.h"

/* Notes for current effort to extract angles and angular velocities
   in  quaternion format as time-series data for machine learning.

From: A Data-Driven Approach to Quantifying Natural Human Motion
By: Liu Ren, Alton Patrick, Alexei A. Efros, Jessica K. Hodgins, James M. Rehg

For the experiments reported here, we converted each frame of raw
motion data to a high-dimensional feature vector of angles and velocities.
For the root segment, we compute the angular velocity and
the linear velocity (in the root coordinate system of each frame).
For each joint, we compute the angular velocity. The velocities
are computed as a central difference between the joint angle or the
position on the previous frame and on the next frame. As a result,
both joint angles and their velocities can be represented by
unit quaternions (four components each). The complete set of joint
angles and velocities, together with the root's linear velocity (three
components) and angular velocity (quaternion, four components),
form a 151-dimensional feature vector for each frame. The quaternions
are transformed to be on one-half of the 4D sphere to handle
the duplicate representation of quaternions. If the orientation of a
joint crosses to the other half-sphere, we choose the alternative representation
for that quaternion and divide the motion sequence at
the boundary to create two continuous sequences. Fortunately this
problem occurs relatively rarely in natural human motion because
of human joint limits.

Work in progress:
- Verify the euler->quaternion code in Quaternion.h or redefine it from theory.
- Verify MotionAnalyzer::calcAngularVel(Quaternion& q0, Quaternion& q1) method.
- Fix zero value for angular velocity (early frames with no prior data).
*/
void MotionAnalyzer::extractJointPositionsAndOrientations() {

	ProcessControl::MOCAPTYPE skeleton_type = process_control.currentRequest().mocap_file_type;
	/*
	 "root" length=1.00000000 
	 "lhipjoint" length=2.40478992 
	 "lfemur" length=7.15780020 
	 "ltibia" length=7.49137020 
	 "lfoot" length=2.36784005 
	 "ltoes" length=1.18965995 
	 "rhipjoint" length=2.37264991 
	 "rfemur" length=7.43385983 
	 "rtibia" length=7.50907993 
	 "rfoot" length=2.41173005 
	 "rtoes" length=1.21081996 
	 "lowerback" length=2.04495001 
	 "upperback" length=2.05008006 
	 "thorax" length=2.06487989 
	 "lowerneck" length=1.75553000 
	 "upperneck" length=1.76877999 
	 "head" length=1.77192998 
	 "lclavicle" length=3.58396006 
     "lhumerus" length=4.98299980 
	 "lradius" length=3.48356009 
	 "lwrist" length=1.74178004 
	 "lhand" length=0.715259016 
	 "lfingers" length=0.576660991 
	 "lthumb" length=0.827973008 
	 "rclavicle" length=3.44818997 
     "rhumerus" length=5.24189997 
	 "rradius" length=3.44417000 
 	 "rwrist" length=1.72209001 
	 "rhand" length=0.622528970 
	 "rfingers" length=0.501899004 
	 "rthumb" length=0.720628977 
	*/
	if (skeleton_type == ProcessControl::AMC) {
		// CMU ASF
		joint_positions[sacrum] = ourSkel->getBone("root")->getEndPosition();
		joint_positions[mid_spine] = ourSkel->getBone("upperback")->getEndPosition();
		joint_positions[upper_spine] = ourSkel->getBone("lowerneck")->getEndPosition();
		joint_positions[atlas] = ourSkel->getBone("upperneck")->getEndPosition();
		joint_positions[skull_top] = ourSkel->getBone("head")->getEndPosition();

		joint_positions[left_hip] = ourSkel->getBone("lhipjoint")->getEndPosition();
		joint_positions[left_knee] = ourSkel->getBone("lfemur")->getEndPosition();
		joint_positions[left_ankle] = ourSkel->getBone("ltibia")->getEndPosition();
		joint_positions[left_toetip] = ourSkel->getBone("ltoes")->getEndPosition();
		joint_positions[right_hip] = ourSkel->getBone("rhipjoint")->getEndPosition();
		joint_positions[right_knee] = ourSkel->getBone("rfemur")->getEndPosition();
		joint_positions[right_ankle] = ourSkel->getBone("rtibia")->getEndPosition();
		joint_positions[right_toetip] = ourSkel->getBone("rtoes")->getEndPosition();

		joint_positions[left_shoulder] = ourSkel->getBone("lclavicle")->getEndPosition();
		joint_positions[left_elbow] = ourSkel->getBone("lhumerus")->getEndPosition();
		joint_positions[left_wrist] = ourSkel->getBone("lradius")->getEndPosition();
		joint_positions[left_fingertip] = ourSkel->getBone("lfingers")->getEndPosition();
		joint_positions[right_shoulder] = ourSkel->getBone("rclavicle")->getEndPosition();
		joint_positions[right_elbow] = ourSkel->getBone("rhumerus")->getEndPosition();
		joint_positions[right_wrist] = ourSkel->getBone("rradius")->getEndPosition();
		joint_positions[right_fingertip] = ourSkel->getBone("rfingers")->getEndPosition();

		// FIXIT:170630 mapping between bone orientations and joint orientations is not yet verified
		joint_orientations[sacrum] = ourSkel->getBone("root")->getOrientation();
		joint_orientations[mid_spine] = ourSkel->getBone("upperback")->getOrientation();
		joint_orientations[upper_spine] = ourSkel->getBone("lowerneck")->getOrientation();
		joint_orientations[atlas] = ourSkel->getBone("upperneck")->getOrientation();
		joint_orientations[skull_top] = Vector3D(0.0f, 0.0f, 0.0f);

		joint_orientations[left_hip] = ourSkel->getBone("lhipjoint")->getOrientation();
		joint_orientations[left_knee] = ourSkel->getBone("lfemur")->getOrientation();
		joint_orientations[left_ankle] = ourSkel->getBone("ltibia")->getOrientation();
		joint_orientations[left_toetip] = Vector3D(0.0f, 0.0f, 0.0f);
		joint_orientations[right_hip] = ourSkel->getBone("rhipjoint")->getOrientation();
		joint_orientations[right_knee] = ourSkel->getBone("rfemur")->getOrientation();
		joint_orientations[right_ankle] = ourSkel->getBone("rtibia")->getOrientation();
		joint_orientations[right_toetip] = Vector3D(0.0f, 0.0f, 0.0f);

		joint_orientations[left_shoulder] = ourSkel->getBone("lclavicle")->getOrientation();
		joint_orientations[left_elbow] = ourSkel->getBone("lhumerus")->getOrientation();
		joint_orientations[left_wrist] = ourSkel->getBone("lradius")->getOrientation();
		joint_orientations[left_fingertip] = Vector3D(0.0f, 0.0f, 0.0f);
		joint_orientations[right_shoulder] = ourSkel->getBone("rclavicle")->getOrientation();
		joint_orientations[right_elbow] = ourSkel->getBone("rhumerus")->getOrientation();
		joint_orientations[right_wrist] = ourSkel->getBone("rradius")->getOrientation();
		joint_orientations[right_fingertip] = Vector3D(0.0f, 0.0f, 0.0f);
	}
	/*
	"root" 
	"Hips__0""Spine"
	  "c""Neck""Head"
	  "Spine1__1""LeftShoulder""LeftArm" "LeftForeArm" "LeftHand" 
	  "Spine1__2""RightShoulder""RightArm""RightForeArm""RightHand" 
	"Hips__1""LeftUpLeg""LeftLeg""LeftFoot""LeftToeBase" 
	"Hips__2""RightUpLeg""RightLeg""RightFoot""RightToeBase"
	*/
	if (skeleton_type == ProcessControl::BVH) {
		// OptiTrack BVH
		joint_positions[sacrum] = ourSkel->getBone("Spine")->getPosition();
		joint_positions[mid_spine] = ourSkel->getBone("Spine1__0")->getPosition();
		joint_positions[upper_spine] = ourSkel->getBone("Neck")->getPosition();
		joint_positions[atlas] = ourSkel->getBone("Head")->getPosition();
		joint_positions[skull_top] = ourSkel->getBone("Head")->getEndPosition();

		joint_positions[left_hip] = ourSkel->getBone("LeftUpLeg")->getPosition();
		joint_positions[left_knee] = ourSkel->getBone("LeftLeg")->getPosition();
		joint_positions[left_ankle] = ourSkel->getBone("LeftFoot")->getPosition();
		joint_positions[left_toetip] = ourSkel->getBone("LeftToeBase")->getEndPosition();
		joint_positions[right_hip] = ourSkel->getBone("RightUpLeg")->getPosition();
		joint_positions[right_knee] = ourSkel->getBone("RightLeg")->getPosition();
		joint_positions[right_ankle] = ourSkel->getBone("RightFoot")->getPosition();
		joint_positions[right_toetip] = ourSkel->getBone("RightToeBase")->getEndPosition();

		joint_positions[left_shoulder] = ourSkel->getBone("LeftArm")->getPosition();
		joint_positions[left_elbow] = ourSkel->getBone("LeftForeArm")->getPosition();
		joint_positions[left_wrist] = ourSkel->getBone("LeftHand")->getPosition();
		joint_positions[left_fingertip] = ourSkel->getBone("LeftHand")->getEndPosition();
		joint_positions[right_shoulder] = ourSkel->getBone("RightArm")->getPosition();
		joint_positions[right_elbow] = ourSkel->getBone("RightForeArm")->getPosition();
		joint_positions[right_wrist] = ourSkel->getBone("RightHand")->getPosition();
		joint_positions[right_fingertip] = ourSkel->getBone("RightHand")->getEndPosition();

		// FIXIT:170630 mapping between bone orientations and joint orientations is not yet verified
		joint_orientations[sacrum] = ourSkel->getBone("Spine")->getOrientation();
		joint_orientations[mid_spine] = ourSkel->getBone("Spine1__0")->getOrientation();
		joint_orientations[upper_spine] = ourSkel->getBone("Neck")->getOrientation();
		joint_orientations[atlas] = ourSkel->getBone("Head")->getOrientation();
		joint_orientations[skull_top] = Vector3D(0.0f, 0.0f, 0.0f);

		joint_orientations[left_hip] = ourSkel->getBone("LeftUpLeg")->getOrientation();
		joint_orientations[left_knee] = ourSkel->getBone("LeftLeg")->getOrientation();
		joint_orientations[left_ankle] = ourSkel->getBone("LeftFoot")->getOrientation();
		joint_orientations[left_toetip] = Vector3D(0.0f, 0.0f, 0.0f);
		joint_orientations[right_hip] = ourSkel->getBone("RightUpLeg")->getOrientation();
		joint_orientations[right_knee] = ourSkel->getBone("RightLeg")->getOrientation();
		joint_orientations[right_ankle] = ourSkel->getBone("RightFoot")->getOrientation();
		joint_orientations[right_toetip] = Vector3D(0.0f, 0.0f, 0.0f);

		joint_orientations[left_shoulder] = ourSkel->getBone("LeftArm")->getOrientation();
		joint_orientations[left_elbow] = ourSkel->getBone("LeftForeArm")->getOrientation();
		joint_orientations[left_wrist] = ourSkel->getBone("LeftHand")->getOrientation();
		joint_orientations[left_fingertip] = Vector3D(0.0f, 0.0f, 0.0f);
		joint_orientations[right_shoulder] = ourSkel->getBone("RightArm")->getOrientation();
		joint_orientations[right_elbow] = ourSkel->getBone("RightForeArm")->getOrientation();
		joint_orientations[right_wrist] = ourSkel->getBone("RightHand")->getOrientation();
		joint_orientations[right_fingertip] = Vector3D(0.0f, 0.0f, 0.0f);
	}
}

void MotionAnalyzer::storeResults(const string& directory, const string& tag)
{
	string fname = directory + tag + string("pos") + string(".csv");
	ofstream ofs(fname);
	if (!ofs) return;

	// first line has column labels
	ofs << "frame, ";
	for (int joint = 0; joint < num_joints; joint++) {
		if (joint > 0) ofs << ", ";
		string joint_name = toString(JointID(joint));
		ofs << joint_name << "_x, " << joint_name << "_y, " << joint_name << "_z, "
			<< joint_name << "_rx, " << joint_name << "_ry, " << joint_name << "_rz, "
			<< joint_name << "_qw, " << joint_name << "_qx, " << joint_name << "_qy, " << joint_name << "_qz, "
			<< joint_name << "_vqw, " << joint_name << "_vqx, " << joint_name << "_vqy, " << joint_name << "_vqz";

	}
	ofs << endl;

	// one line of data for each frame
	for (unsigned int frame = 0; frame < joint_data.size(); frame++) {
		ofs << frame << ", ";
		for (int joint = 0; joint < num_joints; joint++) {
			if (joint > 0) ofs << ", ";
			Vector3D p = joint_data[frame][joint].position;
			Vector3D r = joint_data[frame][joint].orientation_euler;
			Quaternion q = joint_data[frame][joint].orientation_quat;
			Quaternion vq = joint_data[frame][joint].ang_velocity_quat;
			ofs << p.x << ", " << p.y << ", " << p.z << ", "
				<< r.x << ", " << r.y << ", " << r.z << ", "
				<< q.w << ", " << q.x << ", " << q.y << ", " << q.z << ", "
				<< vq.w << ", " << vq.x << ", " << vq.y << ", " << vq.z;
		}
		ofs << endl;
	}
}

void MotionAnalyzer::initialize(long _num_frames, float _frame_duration, Skeleton* _skel) {
	
	// Reset storage structures
	joint_data.clear();
	body_data.clear();
	frame_data_calculated.clear();

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
		//resize outer vector...
		joint_data.resize(num_frames);
		body_data.resize(num_frames);
		frame_data_calculated.resize(num_frames);
		for (i = 0; i < (int)frame_data_calculated.size(); i++) {
			frame_data_calculated[i] = false;
		}
		for (i = 0; i < (int)joint_data.size(); i++) {
			//...and each inner vector
			joint_data[i].resize(num_joints);
		}

		for (i = 0; i < num_frames; i++) {
			for (j = 0; j < num_joints; j++) {
				joint_data[i][j].joint_name = toString(JointID(i));
				joint_data[i][j].frame = i;
			}
		}

		// reinitialize bone parameters (weight maps)
		//Init QoMWeightMap (all 1's for now)
		QoMWeightMap.clear();
		for (i = 0; i < num_joints; i++) {
			string boneName = joint_data[0][i].joint_name;
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
	unsigned int i;
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
	for (i = 1; i < sizeof(points) / sizeof(Vector3D); i++) {
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

pair <Quaternion, float> MotionAnalyzer::calcAngularVel(Quaternion& q0, Quaternion& q1) {
// code from https://www.gamedev.net/forums/topic/347752-quaternion-and-angular-velocity/
//	quaternion q = q1*conj(q0);
//	double len = sqrt(q.x*q.x + q.y*q.y + q.z*q.z)
//	if (len <= epsilon)return vector3(2 * q.x, 2 * q.y, 2 * q.z);
//	double angle = 2 * atan2(len, q.w);
//	return q.xyz()*(angle / (len*dt));
	double epsilon = 0.00001f;
	Quaternion q = q1*conjugate(q0);
	double len = sqrt(q.x*q.x + q.y*q.y + q.z*q.z);
	if (len <= epsilon) {
		Quaternion dq = Quaternion(0.0f, 2.0f * q.x, 2.0f * q.y, 2.0f * q.z);
		float dq_mag = dq.magnitude();
		return pair<Quaternion, float>(dq, dq_mag);
	}
	double angle = 2 * atan2(len, q.w);
	double scale = angle / (len*frame_duration);
	Quaternion dq = Quaternion(0.0f, float(scale*q.x), float(scale*q.y), float(scale*q.z));
	float dq_mag = dq.magnitude();
	return pair<Quaternion, float>(dq, dq_mag);
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
	std::map<string, float>::iterator it = QoMWeightMap.begin();
	float weight;//weight is represented as a percentage
	int avg_counter = 0;
	int i = 0;
	float runningSum = 0.0;
	for (i = 0; i < (int)joint_data[frame].size(); i++) {
		//avg_counter = 0;
		it = weightMap.begin();
		float tmp_vel = joint_data[frame][i].velocity_mag;
		string tmp_bone_name = joint_data[frame][i].joint_name;

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
	for (i = 0; i < (int)joint_data[frame].size(); i++) {
		//avg_counter = 0;
		it = weightMap.begin();
		Vector3D tmp_pos = joint_data[frame][i].position;
		string tmp_bone_name = joint_data[frame][i].joint_name;

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

	// protect against out of range frame numbers (generally due to looping)
	if ((animation_frame < 0) || (animation_frame >= (int)frame_data_calculated.size())) return;
	// check if the data for this frame has already been processed
	if (frame_data_calculated[animation_frame]) return;

	// A bit unnecessary to extract and store positions, and then copy them,
	// but that makes the abstraction to different skeleton types a bit cleaner.
	extractJointPositionsAndOrientations();
	for (int j = 0; j < num_joints; j++) {
		joint_data[animation_frame][j].position = joint_positions[j];
		joint_data[animation_frame][j].orientation_euler = joint_orientations[j];
		Quaternion q;
		// FIXIT! CMU ASM: build rotation transform as RxRyRz
		//        UOP BVH: build rotation transform as RyRxRz
		// The following function needs to handle different application orders.
		q.fromEuler(joint_orientations[j]);
		joint_data[animation_frame][j].orientation_quat = q;
	}

	// data for individual joints

	for (int i = 0; i < num_joints; i++) {
		Vector3D p0, p1, p2, p3;
		pair<Vector3D, float> vel, acc, jerk;

		if (animation_frame > 0) {
			p0 = joint_data[animation_frame][i].position;
			p1 = joint_data[animation_frame - 1][i].position;
			vel = calcVel(p0, p1);
			joint_data[animation_frame][i].velocity_vec = vel.first;
			joint_data[animation_frame][i].velocity_mag = vel.second;
		}
		else {
			joint_data[animation_frame][i].velocity_vec = Vector3D(0.0f,0.0f,0.0f);
			joint_data[animation_frame][i].velocity_mag = 0.0f;
		}
		if (animation_frame > 1) {
			p2 = joint_data[animation_frame - 2][i].position;
			acc = calcAccel(p0, p1, p2);
			joint_data[animation_frame][i].acceleration_vec = acc.first;
			joint_data[animation_frame][i].acceleration_mag = acc.second;
		}
		else {
			joint_data[animation_frame][i].acceleration_vec = Vector3D(0.0f, 0.0f, 0.0f);
			joint_data[animation_frame][i].acceleration_mag = 0.0f;
		}
		if (animation_frame > 2) {
			p3 = joint_data[animation_frame - 3][i].position;
			jerk = calcJerk(p0, p1, p2, p3);
			joint_data[animation_frame][i].jerk_vec = jerk.first;
			joint_data[animation_frame][i].jerk_mag = jerk.second;
		}
		else {
			joint_data[animation_frame][i].jerk_vec = Vector3D(0.0f, 0.0f, 0.0f);
			joint_data[animation_frame][i].jerk_mag = 0.0f;
		}

		if (animation_frame > 1)
		{
			float bone_curve = calcCurvature(acc.first, vel.first, vel.second); 
			float bone_rad = calcRadiusOfCurvature(bone_curve);
			joint_data[animation_frame][i].curvature = bone_curve;
			joint_data[animation_frame][i].curvature_radius = bone_rad;
		}
		else {
			joint_data[animation_frame][i].curvature = 0.0f;
			joint_data[animation_frame][i].curvature_radius = 0.0f;
		}

		if (animation_frame > 0) {
			Quaternion q0 = joint_data[animation_frame][i].orientation_quat;
			Quaternion q1 = joint_data[animation_frame - 1][i].orientation_quat;
			pair<Quaternion, float> avel = calcAngularVel(q1, q0);
			joint_data[animation_frame][i].ang_velocity_quat = avel.first;
			joint_data[animation_frame][i].ang_velocity_mag = avel.second;
		}
		else {
			joint_data[animation_frame][i].ang_velocity_quat = Quaternion();
			joint_data[animation_frame][i].ang_velocity_mag = 0.0f;
		}
	}

	// data for full body

	body_data[animation_frame].setFrame(animation_frame);

	// compute bounding box
	float maxf = std::numeric_limits<float>::max();
	float minf = -maxf;
	Vector3D bbmin(maxf,maxf,maxf), bbmax(minf,minf,minf);
	for (unsigned int i = 0; i < joint_data[animation_frame].size(); i++) {
		Vector3D tmp = joint_data[animation_frame][i].position;
		float x = tmp.getX();
		float y = tmp.getY();
		float z = tmp.getZ();
		if (x > bbmax.x) bbmax.x = x;
		if (x < bbmin.x) bbmin.x = x;
		if (y > bbmax.y) bbmax.y = y;
		if (y < bbmin.y) bbmin.y = y;
		if (z > bbmax.z) bbmax.z = z;
		if (z < bbmin.z) bbmin.z = z;
	}
	body_data[animation_frame].setBoundingBox(bbmin, bbmax);

	// compute bounding sphere
	vector <Vector3D> pointVector;
	int i = 0;
	pointVector.resize(joint_data[animation_frame].size());
	std::vector<JointData>::iterator it = joint_data[animation_frame].begin();
	for (it = joint_data[animation_frame].begin(); it != joint_data[animation_frame].end(); it++) {
		pointVector[i] = it->position;
		i++;
	}
	pair <Vector3D, float> sphere = calcBoundingSphere(pointVector);
	body_data[animation_frame].setBoundingSphere(sphere);

	// compute center of mass
	Vector3D frame_CoM = calculateCoM(animation_frame);
	body_data[animation_frame].setCoM(frame_CoM);

	// compute quantity of motion
	float QoM = calculateQoM(animation_frame);
	body_data[animation_frame].setQoM(QoM);
}
