//-----------------------------------------------------------------------------
// MotionDescriptors project - Builds with SKA Version 4.0
//-----------------------------------------------------------------------------
// AppMain.cpp
//    The main program is mostly the connection between openGL,
//    SKA and application specific code. It also controls the order of
//    initialization, before control is turned over to openGL.
//-----------------------------------------------------------------------------
// SKA configuration.
#include <Core/SystemConfiguration.h>
// C/C++ libraries
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
using namespace std;
// openGL library
//#include <GL/glut.h>
// SKA modules
#include <Core/BasicException.h>
#include <Core/SystemTimer.h>

// local application
#include "AppConfig.h"
#include "AppGraphics.h"
#include "AnimationControl.h"
#include "CameraControl.h"
#include "InputProcessing.h"
#include "Plane.h"
#include "DataRecorder.h"
#include "BoneData.h"
#include "FullBodyData.h"

bool DISPLAY_ALL_RESULTS = false;
/*These 2 global variables are needed to persist data across frames*/
Vector3D LAST_LOC;
string GLOBAL_MOVE;
//initialize 2d vector of Bone data for frame x Bone data struct
//frame -> (Bone -> BoneData) //get data for all bones for a single frame
//will have number of frames x number of bones
//Parallel arrays per frame(both indexed by frame)
vector<vector<BoneData>> BoneVector;
vector<FullBodyData> FullBodyVector;
std::map<std::string, float> weightMap;//used for CoM
std::map<std::string, float> QoMWeightMap; //used for QoM
Skeleton* ourSkel;
float frame_duration;
// flexion sample:   PTsubjects/40414/A/Take 2016-11-08 05.41.17 PM.bvh
// extension sample: PTsubjects/40414/A/Take 2016-11-08 05.42.02 PM.bvh
// abduction sample: PTsubjects/40414/A/Take 2016-11-08 05.49.30 PM.bvh

enum DATAMODE {	FLEXION, EXTENSION, ABDUCTION };
string toString(DATAMODE dm) {
	switch (dm) {
	case FLEXION: return string("FLEXION");
	case EXTENSION: return string("EXTENSION");
	case ABDUCTION: return string("ABDUCTION");
	default: return string("UNKNOWN");
	}
}

string toString(bool x) {
	if (x) return string("TRUE");
	else return string("FALSE");
}

struct ProcessingRequest {
	string take_label;
	string motion_file;
	string result_file;
	DATAMODE data_mode;
	bool loop;
	float init_skip_time;
	ProcessingRequest(string& _take_label, string& _motion_file, string& _result_file,
		DATAMODE _data_mode=FLEXION, bool _loop = false, float _init_skip_time = 5.0f)
		: take_label(_take_label), motion_file(_motion_file), result_file(_result_file), data_mode(_data_mode),
  		  loop(_loop), init_skip_time(_init_skip_time) {}
};

class ProcessControl {
public:
	ProcessControl() { reset(); }
	void reset() {
		requests.clear();
		animation_enabled = false;
		curr_request = -1;
	}
	bool active() { return curr_request < (short)requests.size();  }
	bool advance() { curr_request++; return active(); }
	bool animationIsEnabled() { return animation_enabled;  }
	void enableAnimation() { animation_enabled = true; }
	void disableAnimation() { animation_enabled = false; }

	/*Trevor's code for input file enabling bone tracking*/
	void enableBoneTracking() { bone_tracking = true;  }
	void disableBoneTracking() { bone_tracking = false; }
	bool trackingIsEnabled() { return bone_tracking; }
	/*End Trevor's code*/

	void addRequest(ProcessingRequest& r) {
		requests.push_back(r);
		if (curr_request < 0) { curr_request = 0; }
	}

	DATAMODE dataMode() { return requests[curr_request].data_mode; }
	bool loop() { return requests[curr_request].loop; }
	float skip() { return requests[curr_request].init_skip_time; }
	string takeLabel() { return requests[curr_request].take_label; }
	string motionFile() { return requests[curr_request].motion_file; }
	string resultFile() { return requests[curr_request].result_file; }
	/*Set and Gets for bone tracking*/
	void setBoneName(string name) { bone_tracking_name = name; }
	string getBoneName() { return bone_tracking_name; }
	void setPlaneName(string plane) { plane_name = plane; }
	string getPlaneName() { return plane_name; }
	void setFrameStep(int frames) { frame_step = frames; }
	int getFrameStep() { return frame_step; }

private:
	int frame_step = 1;
	bool animation_enabled;
	bool bone_tracking;
	string bone_tracking_name;
	string plane_name;
	vector<ProcessingRequest> requests;
	short curr_request = -1;
public:
	void printProcessControl()
	{
		cout << "Process Control: " << endl;
		if (animation_enabled) cout << "ANIMATION IS ENABLED" << endl;
		else cout << "ANIMATION IS DISABLED" << endl;
		if (bone_tracking) cout << "BONE TRACKING ENABLED\nTRACKING: " << bone_tracking_name << "AND" << plane_name << endl;
		else cout << "BONE TRACKING DISABLED" << endl;
		unsigned short p;
		for (p = 0; p < requests.size(); p++)
		{
			ProcessingRequest preq = requests[p];
			cout << "Request " << p << " ... " << endl;
			cout << "motion file: " << preq.motion_file << endl;
			cout << "result file: " << preq.result_file << endl;
			cout << "mode: " << preq.data_mode << endl;
			if (preq.loop) cout << "loop: on" << endl;
			else cout << "loop: off" << endl;
			cout << "skip time: " << preq.init_skip_time << " seconds" << endl;
		}
	}
};
static ProcessControl process_control;

/*
command file format:
INPUT_FOLDER=<path>
OUTPUT_FOLDER=<path>
ANIMATION=<Y|N>
PROCESS <file> MODE=<F|E|A> LOOP=<Y|N> SKIP=<number>
*/

bool isPrefix(string& s, string& prefix) {
	return s.find(prefix) == 0;
}

bool readCommandFile()
{
	string input_folder("");
	string output_folder("");
	string line;
	ifstream cf("skacommands.txt");
	if (!cf) return false;

	while (getline(cf, line))
	{
		if (line.length() < 1) continue;

		if (isPrefix(line, string("INPUT_FOLDER=")))
		{
			input_folder = line.substr(14,line.length()-15);
			if (input_folder[input_folder.length() - 1] != '\\') input_folder.append("\\");
		}
		else if (isPrefix(line, string("OUTPUT_FOLDER=")))
		{
			output_folder = line.substr(15, line.length()-16);
			if (output_folder[output_folder.length() - 1] != '\\') output_folder.append("\\");
		}
		else if (isPrefix(line, string("ANIMATION=")))
		{
			if (line[10] == 'Y') process_control.enableAnimation();
			else if (line[10] == 'N') process_control.disableAnimation();
		}
		/*Trevor's code for tracking bone enabling*/
		else if (isPrefix(line, string("TRACK_BONE=")))
		{
			if (line[11] == 'Y') process_control.enableBoneTracking();
			else if (line[11] == 'N') process_control.disableBoneTracking();
		}
		else if (isPrefix(line, string("BONE_NAME="))) {
			//find bone name
			line = line.substr(9);
			int quote1 = line.find_first_of('\"');
			int quote2 = line.find_first_of('\"', quote1 + 1);
			string bone = line.substr(quote1 + 1, (quote2 - quote1) - 1);
			process_control.setBoneName(bone);
		}
		else if (isPrefix(line, string("PLANE="))) {
			//find plane name
			line = line.substr(6);
			int quote1 = line.find_first_of('\"');
			int quote2 = line.find_first_of('\"', quote1 + 1);
			string plane_string = line.substr(quote1 + 1, (quote2 - quote1) - 1);
			process_control.setPlaneName(plane_string);
		}
		else if (isPrefix(line, string("MOTION_SKIP="))) {
			line = line.substr(6);
			int quote1 = line.find_first_of('\"');
			int quote2 = line.find_first_of('\"', quote1 + 1);
			string frame_string = line.substr(quote1 + 1, (quote2 - quote1) - 1);
			int frames = atoi(frame_string.c_str());
			if (frames == 0)
				process_control.setFrameStep(1);
			else
				process_control.setFrameStep(frames);
			//cout << frames;
		}
		/*End Trevor's code*/
		else if (isPrefix(line, string("PROCESS")))
		{
			line = line.substr(7);
			int quote1 = line.find_first_of('\"');
			int quote2 = line.find_first_of('\"', quote1 + 1);
			string filename = line.substr(quote1+1, (quote2-quote1)-1);
			line = line.substr(quote2 + 1);

			char mode = 'E';
			char loop = 'N';
			float skip = 5.0f;

			int i = line.find("MODE=");
			if (i != string::npos) mode = line[i + 5];
			i = line.find("LOOP=");
			if (i != string::npos) loop = line[i + 5];
			i = line.find("SKIP=");
			if (i != string::npos) {
				string stmp = line.substr(i + 5);
				skip = float(atof(stmp.c_str()));
			}

			DATAMODE dm = EXTENSION;
			switch (mode) {
			case 'F': dm = FLEXION; break;
			case 'E': dm = EXTENSION; break;
			case 'A': dm = ABDUCTION; break;
			};
			bool loop_on = false;
			if (loop == 'Y') loop_on = true;

			ProcessingRequest preq(filename,
				input_folder + filename + ".bvh", output_folder + filename + ".csv",
				dm, loop_on, skip);
			process_control.addRequest(preq);
		}
		else
		{
			cerr << "INVALID COMMAND LINE: " << line << endl;
		}
	}
	cf.close();
	return true;
}

static DataRecorder data_recorder;

// abduction = lift of arm in coronal plane
// flexion = arm forward in sagittal plane
// extension = arm backward in sagittal plane
static float flexion=0.0f, extension=0.0f, abduction=0.0f;
static float dir_test = 0.0f;
static float max_flexion = 0.0f, max_extension = 0.0f, max_abduction = 0.0f;
static long animation_frame = 0;
static float animation_time = 0.0f;

static void resetMaxValues() {
	max_flexion = 0.0f; max_extension = 0.0f; max_abduction = 0.0f;
}

static bool real_time_mode = false;  //  true = step in clock time, false = step frame-by-frame
static bool animation_has_looped = false;
static bool goto_next_process = false;

// probably unnecessary - used in case any cleanup is needed on exit.
void shutDown(int _exit_code)
{
	exit(_exit_code);
}

/*
Function for tracking bone position.
Part of GUI code.
*/
string trackDirection(Vector3D bone_pos, Plane _plane, string side, int frame, Vector3D & last_location) {
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



pair <Vector3D, float> calcBoundingSphere(vector <Vector3D> points) {
	//P1, Py, Pz
	//random, futhest from 1, furthest from y
	std::vector<Vector3D>::iterator it = points.begin();
	Vector3D P1 = points[0];//random point
	Vector3D Py, Pz;
	int i;
	float max_distance = 0.0f;
	float dist;
	//get max distance from P1
	for (it = points.begin(); it!= points.end(); it++) {
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

pair <Vector3D, float> calcVel(Vector3D p0, Vector3D p1) {
	Vector3D velocity;
	float v_mag;
	velocity = (p0-p1) / frame_duration;
	v_mag = velocity.magnitude();

	pair <Vector3D, float> vel;
	vel = make_pair(velocity, v_mag);
	return vel;
	
}

pair <Vector3D, float> calcAccel(Vector3D p0, Vector3D p1, Vector3D p2) {
	Vector3D accel;
	float a_mag;
	accel = (p0 - p1 * 2 + p2) / (frame_duration * frame_duration);
	a_mag = accel.magnitude();

	pair <Vector3D, float> acc;
	acc = make_pair(accel, a_mag);
	return acc;

}

pair <Vector3D, float> calcJerk(Vector3D p0, Vector3D p1, Vector3D p2, Vector3D p3) {
	Vector3D jerk;
	float j_mag;
	jerk = (p0 - (p1)*3 + (p2)*3-p3) /(frame_duration * frame_duration * frame_duration);
	j_mag = jerk.magnitude();

	pair <Vector3D, float> jerk_pair;
	jerk_pair = make_pair(jerk, j_mag);
	return jerk_pair;

}
//Curvature
//Mag(a_k(ti) X v_k(ti))/mag(v_k (ti))^3
//PER BONE
float calcCurvature(Vector3D accel, Vector3D velVector, float velMag) {

	Vector3D top = accel.cross(velVector);
	float top_mag = top.magnitude();
	return top_mag / pow(velMag, 3);

}

float calcRadiusOfCurvature(float curve) {
	return 1 / curve;
}

/*
DEPRICATED - use calcVel().
Part of GUI code.
*/
float calculateVelocity(Vector3D bone_pos, Plane _plane, int frames, Vector3D last_location) {
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
		vel = movement.dot(norm)/frame_duration;
	}
	return vel;
}

float calculateQoM(int frame) {
	std::map<std::string, float>::iterator it = QoMWeightMap.begin();
	float weight;//weight is represented as a percentage
	int avg_counter = 0;
	int i = 0;
	float runningSum =0.0;
	for (i = 0; i < (int)BoneVector[frame].size(); i++) {
		//avg_counter = 0;
		it = weightMap.begin();
		float tmp_vel = BoneVector[frame][i].getVelocity_mag();
		std::string tmp_bone_name = BoneVector[frame][i].getBone()->getName();

		// strip off "Left" or "Right" prefix
		if (tmp_bone_name.find("Right") == 0) tmp_bone_name = tmp_bone_name.substr(5);
		if (tmp_bone_name.find("Left") == 0) tmp_bone_name = tmp_bone_name.substr(4);

		while (it != weightMap.end()) {
			if (tmp_bone_name.compare(it->first) == 0) {//if wefind the bone name in our weightMap
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


Vector3D calculateCoM(int frame) {
	std::map<std::string, float>::iterator it = weightMap.begin();
	float weight;//weight is represented as a percentage
	int avg_counter = 0;
	int i = 0;
	Vector3D runningSum;
	for (i = 0; i < (int)BoneVector[frame].size(); i++) {
		//avg_counter = 0;
		it = weightMap.begin();
		Vector3D tmp_pos = BoneVector[frame][i].getBone()->getPosition();
		std::string tmp_bone_name = BoneVector[frame][i].getBone()->getName();

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
	return runningSum/(float)avg_counter;
}

void processPTData()
{
	// extract necessary data from the skeleton in AnimationController
	Vector3D junk; // throw away data
	Vector3D root_position, root_orientation;
	Vector3D rightupperarm_pos, rightelbow_pos;
	Vector3D tailbone_pos, neck_pos;
	Vector3D rightshoulder_pos, leftshoulder_pos;
	/*Trevor's Vec3D for right hand*/
	Vector3D rightHand_pos;
	/*Trevor's Vec3D for bone to be tracked*/
	Vector3D trackBone_pos;
	//Hacking this to convert string to const char * for getBonePosition()
	string trackBone_name = process_control.getBoneName();
	const char * trackBone_name_char = trackBone_name.c_str();
	string plane_name = process_control.getPlaneName();

	if (!anim_ctrl.getBonePosition("root", junk, root_position))
		cerr << "Failed to find bone \"root\"" << endl;
	if (!anim_ctrl.getBoneOrientation("root", root_orientation))
		cerr << "Failed to find bone \"root\"" << endl;

	if (!anim_ctrl.getBonePosition("RightArm", rightupperarm_pos, rightelbow_pos))
		cerr << "Failed to find bone \"RightArm\"" << endl;

	if (!anim_ctrl.getBonePosition("Spine", tailbone_pos, junk))
		cerr << "Failed to find bone \"Spine\"" << endl;
	if (!anim_ctrl.getBonePosition("Neck", neck_pos, junk))
		cerr << "Failed to find bone \"Neck\"" << endl;
	if (!anim_ctrl.getBonePosition("LeftShoulder", leftshoulder_pos, junk))
		cerr << "Failed to find bone \"LeftShoulder\"" << endl;
	if (!anim_ctrl.getBonePosition("RightShoulder", rightshoulder_pos, junk))
		cerr << "Failed to find bone \"RightShoulder\"" << endl;
	/*Trevor's var*/
	if (!anim_ctrl.getBonePosition("RightHand", rightHand_pos, junk))
		cerr << "Failed to find bone \"RightHand\"" << endl;
	if (process_control.trackingIsEnabled()) {
		if (!anim_ctrl.getBonePosition(trackBone_name_char, trackBone_pos, junk))
			cerr << "Failed to find bone \"" << trackBone_name_char << "\"" << endl;
	}

	//---- analytical computations

	// New plan:
	// 1) define coronal plane from sholders and hips
	// 2) define sagittal plan perpendicular to coronal plane and aligned with spine
	// 3) define transverse plane perpendicular to both the coronal and sagital planes
	// 4) define directional vectors (forward/backward, left/right, up/down) perpendicular to the various planes.
	// 5) use directional vectors to define test cones for the three possible measurements
	// 6) compute measurements relative to the directional vectors

	Vector3D cplane1 = leftshoulder_pos - tailbone_pos;
	cplane1.normalize();
	Vector3D cplane2 = rightshoulder_pos - tailbone_pos;
	cplane2.normalize();

	Plane coronal_plane(cplane1, cplane2);
	coronal_plane.moveTo(root_position);
	Vector3D forward_dir = coronal_plane.getNormal();
	Vector3D backward_dir = forward_dir*-1;

	Vector3D spine_dir = tailbone_pos - neck_pos;
	spine_dir = spine_dir.normalize();

	Plane sagittal_plane(spine_dir*-10.f, forward_dir);
	sagittal_plane.moveTo(root_position);
	Vector3D left_dir = sagittal_plane.getNormal();
	Vector3D right_dir = left_dir*-1;

	Plane transverse_plane(forward_dir, left_dir);
	transverse_plane.moveTo(root_position);
	Vector3D up_dir = transverse_plane.getNormal();
	Vector3D down_dir = up_dir*-1;


	// right upper arm direction
	Vector3D arm_dir = rightelbow_pos - rightupperarm_pos;
	arm_dir = arm_dir.normalize();


	/*GUI Motion tracking

	***
	This section of the code was originally written to test functions and display results in GUI.
	Depricated, but kept for reference purposes.
	~Trevor
	***

	float relationval = 0.0f;
	string side = "Positive";
	float velocity = 0.0;

	//if input file specifies tracking
	if (process_control.trackingIsEnabled()) {
		//default to sagittal plane tracking
		Vector3D trackBone_sp = sagittal_plane.projectPointOntoPlane(trackBone_pos);

		if (plane_name == "transverse") {
			trackBone_sp = transverse_plane.projectPointOntoPlane(trackBone_pos);
		}
		else if (plane_name == "coronal") {
			trackBone_sp = coronal_plane.projectPointOntoPlane(trackBone_pos);
		}
		Vector3D relation = trackBone_pos - trackBone_sp;
		//Use dot product to determine side bone is on
		if (plane_name == "transverse") {
			relationval = relation.dot(up_dir);
		}
		else if (plane_name == "coronal") {
			relationval = relation.dot(forward_dir);
		}
		else {
			//default to sagittal
			relationval = relation.dot(left_dir);
		}
		if (relationval > 0.0f) {
			side = "Positive";
		}
		else {
			side = "Negative";
		}

		/*Determine velocity and directional movement using side which was calculated above
		if (plane_name == "sagittal") {
			//velocity = calculateVelocity(trackBone_pos, sagittal_plane, 1, LAST_LOC);
			if (animation_frame % process_control.getFrameStep() == 0) {
				GLOBAL_MOVE = trackDirection(trackBone_pos, sagittal_plane, side, animation_frame, LAST_LOC);
			}
		}
		else if (plane_name == "transverse") {
			//velocity = calculateVelocity(trackBone_pos, transverse_plane, 1, LAST_LOC);
			if (animation_frame % process_control.getFrameStep() == 0) {
				GLOBAL_MOVE = trackDirection(trackBone_pos, transverse_plane, side, animation_frame, LAST_LOC);
			}
		}
		else if (plane_name == "coronal") {
			//velocity = calculateVelocity(trackBone_pos, coronal_plane, 1, LAST_LOC);
			if (animation_frame % process_control.getFrameStep() == 0) {
				GLOBAL_MOVE = trackDirection(trackBone_pos, coronal_plane, side, animation_frame, LAST_LOC);
			}
		}
	}
	End GUI Motion Tracking*/


	/*Accumulate Motion Data*/
	int i;
	//PER BONE DATA
	for (i = 0; i < (int)BoneVector[animation_frame].size(); i++) {
		//first set basic info for bone @ frame
		Vector3D bone_pos = BoneVector[animation_frame][i].getBone()->getPosition();
		BoneVector[animation_frame][i].setX(bone_pos.getX());
		BoneVector[animation_frame][i].setY(bone_pos.getY());
		BoneVector[animation_frame][i].setZ(bone_pos.getZ());
		BoneVector[animation_frame][i].setStartPos(bone_pos);
		BoneVector[animation_frame][i].setEndPos(BoneVector[animation_frame][i].getBone()->getEndPosition());

		//Calculate and assign Velocity, Frame and Jerk
		//Both Magnitude and Vector
		Vector3D p0 = bone_pos;
		Vector3D p1, p2, p3;
		if (animation_frame > 3) {
			p1 = BoneVector[animation_frame - 1][i].getStartPos();
			p2 = BoneVector[animation_frame - 2][i].getStartPos();
			p3 = BoneVector[animation_frame - 3][i].getStartPos();

			pair<Vector3D, float> vel = calcVel(p0, p1);
			BoneVector[animation_frame][i].setVelocity_vec(vel.first);
			BoneVector[animation_frame][i].setVelocity_mag(vel.second);

			pair<Vector3D, float> acc = calcAccel(p0, p1, p2);
			BoneVector[animation_frame][i].setAcceleration_vec(acc.first);
			BoneVector[animation_frame][i].setAcceleration_mag(acc.second);

			pair<Vector3D, float> jerk = calcJerk(p0, p1, p2, p3);
			BoneVector[animation_frame][i].setJerk_vec(jerk.first);
			BoneVector[animation_frame][i].setJerk_mag(jerk.second);

			float bone_curve = calcCurvature(acc.first, vel.first, vel.second);
			float bone_rad = calcRadiusOfCurvature(bone_curve);

			BoneVector[animation_frame][i].setCurvature(bone_curve);
			BoneVector[animation_frame][i].setCurvatureRadius(bone_rad);

		}
	}	
	//FULL BODY DATA
	//Calculate Bounding box and sphere
	/*indexed as [MAX[x y z] MIN [x y z]]*/
	float max_min[6] = { 0.0,0.0,0.0,0.0,0.0,0.0 };
	float tmp_x, tmp_y, tmp_z = 0.0;
	for (i = 0; i < (int)BoneVector[animation_frame].size(); i++) {
		Vector3D tmp = BoneVector[animation_frame][i].getBone()->getPosition(); //makes all the calling easier to read/write
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
	FullBodyVector[animation_frame].setFrame(animation_frame);
	FullBodyVector[animation_frame].setBoxParam(max_min);

	//Sphere
	//Make and initialize a Vector to pass to function
	vector <Vector3D> pointVector;
	i = 0;
	//pointVector has 'bone' items (BoneVector[animation_frame] has 'bone' elements)
	pointVector.resize(BoneVector[animation_frame].size());
	std::vector<BoneData>::iterator it = BoneVector[animation_frame].begin();
	for (it = BoneVector[animation_frame].begin(); it != BoneVector[animation_frame].end(); it++) {
		pointVector[i] = it->getStartPos();
		i++;
	}
	pair <Vector3D, float> sphere = calcBoundingSphere(pointVector);
	FullBodyVector[animation_frame].setBoundingSphere(sphere);

	//Center of Mass & Quantity of Motion Calculation
	Vector3D frame_CoM = calculateCoM(animation_frame);
	float QoM = calculateQoM(animation_frame);
	FullBodyVector[animation_frame].setCoM(frame_CoM);
	FullBodyVector[animation_frame].setQoM(QoM);
	
	


	/*
	bool atest, etest, ftest;
	// check for abduction
	atest = rad2deg(acos(arm_dir.dot(right_dir))) < 50.0f;
	// check for extension
	etest = (rad2deg(acos(arm_dir.dot(backward_dir))) < 50.0f) || (rad2deg(acos(arm_dir.dot(up_dir))) < 50.0f);
	// check for flexion
	ftest = (rad2deg(acos(arm_dir.dot(forward_dir))) < 50.0f) || (rad2deg(acos(arm_dir.dot(up_dir))) < 50.0f);
	*/

	// project arm and spine onto coronal plane
	Vector3D rightupperarm_pos_cp = coronal_plane.projectPointOntoPlane(rightupperarm_pos);
	Vector3D rightelbow_pos_cp = coronal_plane.projectPointOntoPlane(rightelbow_pos);
	Vector3D adj_neck_pos = rightupperarm_pos_cp + spine_dir*5.0f;
	Vector3D neck_pos_cp = coronal_plane.projectPointOntoPlane(adj_neck_pos);
	// project arm and spine onto sagittal plane
	Vector3D rightupperarm_pos_sp = sagittal_plane.projectPointOntoPlane(rightupperarm_pos);
	Vector3D rightelbow_pos_sp = sagittal_plane.projectPointOntoPlane(rightelbow_pos);
	adj_neck_pos = rightupperarm_pos_sp + spine_dir*5.0f;
	Vector3D neck_pos_sp = sagittal_plane.projectPointOntoPlane(adj_neck_pos);

	// abduction = angle between spine and arm when lifted in the coronal plane
	Vector3D ac = rightelbow_pos_cp - rightupperarm_pos_cp;
	Vector3D sc = neck_pos_cp - rightupperarm_pos_cp;
	ac = ac.normalize();
	sc = sc.normalize();
	abduction = rad2deg(acos(ac.dot(sc)));

	// flexion = (positive) angle between spine and arm_in_sagittal_plane
	// extension = (negative) angle between spine and arm_in_sagittal_plane
	Vector3D as = rightelbow_pos_sp - rightupperarm_pos_sp;
	Vector3D ss = neck_pos_sp - rightupperarm_pos_sp;
	as = as.normalize();
	ss = ss.normalize();
	if (process_control.dataMode() == FLEXION)
	{
		flexion = rad2deg(acos(as.dot(ss)));
		extension = 0.0f;
	}
	else if (process_control.dataMode() == EXTENSION)
	{
		flexion = 0.0f;
		extension = rad2deg(acos(as.dot(ss)));
	}

	//if (process_control.dataMode() == ABDUCTION)

	//if (atest && (abduction > max_abduction)) max_abduction = abduction;
	//if (ftest && (flexion > max_flexion)) max_flexion = flexion;
	//if (etest && (extension > max_extension)) max_extension = extension;

	if (animation_time >= process_control.skip())
	{
		switch (process_control.dataMode()) {
		case ABDUCTION:
			if (abduction > max_abduction) max_abduction = abduction;
			break;
		case EXTENSION:
			if (extension > max_extension) max_extension = extension;
			break;
		case FLEXION:
			if (flexion > max_flexion) max_flexion = flexion;
			break;
		}
	}

	//---- data recording

	if (!animation_has_looped)
	{
		if (animation_time < process_control.skip())
			data_recorder.recordNoData(animation_time, animation_frame);
		else
			switch (process_control.dataMode()) {
			case ABDUCTION:
				data_recorder.recordAbduction(animation_time, animation_frame, abduction);
				break;
			case EXTENSION:
				data_recorder.recordExtension(animation_time, animation_frame, extension);
				break;
			case FLEXION:
				data_recorder.recordFlexion(animation_time, animation_frame, flexion);
				break;
			}
	}

	//---- visualization

	if (process_control.animationIsEnabled())
	{
		analysis_objects.rua_start_marker->moveTo(rightupperarm_pos);
		analysis_objects.rua_end_marker->moveTo(rightelbow_pos);
		analysis_objects.root_marker->moveTo(root_position);
		analysis_objects.root_marker->rotateTo(root_orientation);

		analysis_objects.ruacp_start_marker->moveTo(rightupperarm_pos_cp);
		analysis_objects.ruacp_end_marker->moveTo(rightelbow_pos_cp);
		analysis_objects.spinecp_end_marker->moveTo(neck_pos_cp);

		analysis_objects.ruasp_start_marker->moveTo(rightupperarm_pos_sp);
		analysis_objects.ruasp_end_marker->moveTo(rightelbow_pos_sp);
		analysis_objects.spinesp_end_marker->moveTo(neck_pos_sp);

		analysis_objects.ruacp_bone->setEndpoints(rightupperarm_pos_cp, rightelbow_pos_cp);
		analysis_objects.ruasp_bone->setEndpoints(rightupperarm_pos_sp, rightelbow_pos_sp);
		analysis_objects.spinecp_bone->setEndpoints(rightupperarm_pos_cp, neck_pos_cp);
		analysis_objects.spinesp_bone->setEndpoints(rightupperarm_pos_sp, neck_pos_sp);

		Vector3D p = root_position;
		analysis_objects.right_dir_marker->setEndpoints(p, p+(right_dir*20.0f));
		analysis_objects.forward_dir_marker->setEndpoints(p, p + (forward_dir*20.0f));
		analysis_objects.up_dir_marker->setEndpoints(p, p + (up_dir*20.0f));

		// (these should be extracted from the three Plane objects)
		analysis_objects.coronal_plane_image->moveTo(coronal_plane.getOrigin());
		analysis_objects.coronal_plane_image->rotateTo(root_orientation);
		analysis_objects.sagittal_plane_image->moveTo(sagittal_plane.getOrigin());
		analysis_objects.sagittal_plane_image->rotateTo(root_orientation);
		analysis_objects.transverse_plane_image->moveTo(transverse_plane.getOrigin());
		analysis_objects.transverse_plane_image->rotateTo(root_orientation);

		// FIXIT:170119 visibility adjustment should be done in AppGraphics
		analysis_objects.coronal_plane_image->setVisibility(analysis_objects.show_coronal_plane);
		analysis_objects.ruacp_start_marker->setVisibility(analysis_objects.show_coronal_plane);
		analysis_objects.ruacp_end_marker->setVisibility(analysis_objects.show_coronal_plane);
		analysis_objects.spinecp_end_marker->setVisibility(analysis_objects.show_coronal_plane);
		analysis_objects.ruacp_bone->setVisibility(analysis_objects.show_coronal_plane);
		analysis_objects.spinecp_bone->setVisibility(analysis_objects.show_coronal_plane);

		analysis_objects.sagittal_plane_image->setVisibility(analysis_objects.show_sagittal_plane);
		analysis_objects.ruasp_start_marker->setVisibility(analysis_objects.show_sagittal_plane);
		analysis_objects.ruasp_end_marker->setVisibility(analysis_objects.show_sagittal_plane);
		analysis_objects.spinesp_end_marker->setVisibility(analysis_objects.show_sagittal_plane);
		analysis_objects.ruasp_bone->setVisibility(analysis_objects.show_sagittal_plane);
		analysis_objects.spinesp_bone->setVisibility(analysis_objects.show_sagittal_plane);

		analysis_objects.transverse_plane_image->setVisibility(analysis_objects.show_transverse_plane);

		//analysis_objects.coronal_plane_image->setVisibility(true);
		//analysis_objects.sagittal_plane_image->setVisibility(true);
		//analysis_objects.transverse_plane_image->setVisibility(true);

		analysis_objects.right_dir_marker->setVisibility(false);
		analysis_objects.forward_dir_marker->setVisibility(false);
		analysis_objects.up_dir_marker->setVisibility(false);

		// copy data to HUD
		hud_data.flexion = flexion;
		hud_data.extension = extension;
		hud_data.abduction = abduction;
		/* Uncomment this section if using GUI tracking
		if (process_control.trackingIsEnabled()) {
			hud_data.bone_cross_plane = side;
			hud_data.relationVal = relationval;
		}
		hud_data.bone_tracking_name = trackBone_name;
		hud_data.plane_tracking_name = plane_name;
		hud_data.move_direction = GLOBAL_MOVE;
		hud_data.velocity = velocity;
		*/
		/*End Trevor's code*/
		hud_data.dir_test = dir_test;
		hud_data.max_flexion = max_flexion;
		hud_data.max_extension = max_extension;
		hud_data.max_abduction = max_abduction;
		hud_data.animation_frame = animation_frame;
		hud_data.animation_time = animation_time;
		hud_data.animation_paused = anim_ctrl.isFrozen();
		hud_data.animation_time_warp = anim_ctrl.getTimeWarp();
		if (DISPLAY_ALL_RESULTS)
		{
			hud_data.focus_abduction = true;
			hud_data.focus_extension = true;
			hud_data.focus_flexion = true;
		}
		else
		{
			hud_data.focus_abduction = process_control.dataMode() == ABDUCTION;
			hud_data.focus_extension = process_control.dataMode() == EXTENSION;
			hud_data.focus_flexion = process_control.dataMode() == FLEXION;
		}
	}
}

void loadNextMotion() {

	if (!process_control.active()) shutDown(0);

	cout << "Processing " << process_control.takeLabel() << endl;
	cout << "  mode = " << toString(process_control.dataMode()) << endl;
	cout << "  loop = " << toString(process_control.loop()) << endl;
	cout << "  skip time = " << process_control.skip() << " seconds" << endl;
	anim_ctrl.loadCharacter(process_control.motionFile());
	cout << "  frames = " << anim_ctrl.numFrames() << endl;
	/*Trevor's Code: Show if tracking is enabled*/
	if (process_control.trackingIsEnabled()) {
		cout << "  Tracking is enabled\n  Tracking " << process_control.getBoneName() << " bone" << endl;
		cout << "  Across " << process_control.getPlaneName() << " plane" << endl;
		cout << "  Tracking Skip: " << process_control.getFrameStep() << endl;
	}
	else {
		cout << "  Tracking is disabled\n";
	}

	analysis_objects.show_coronal_plane = true;
	analysis_objects.show_sagittal_plane = true;
	analysis_objects.show_transverse_plane = true;
	switch (process_control.dataMode()) {
	case ABDUCTION:
		analysis_objects.show_coronal_plane = true; break;
	case EXTENSION:
	case FLEXION:
		analysis_objects.show_sagittal_plane = true; break;
	}

	if (!anim_ctrl.isReady())
	{
		logout << "main(): Unable to load character. Aborting program." << endl;
		shutDown(1);
	}
	hud_data.take_label = process_control.takeLabel();

	// Need to create the bones, even if we're not using graphics.
	// The skeleton uses the bone objects to store bone endpoints.
	// FIXIT:170123 Need to delete the old bones before clearing the render list.
	// FIXIT:170123 Verify the AnimationControl.loadCharacter properly cleans up from the previous load.
	character_render_list.clear();
	anim_ctrl.getRenderList(character_render_list);

	/*
	Initializing Bone Data Vector
	*/
	//reset bone vector for each animation
	BoneVector.clear();
	FullBodyVector.clear();
	//now make room for num frames
	ourSkel = anim_ctrl.getSkeleton();
	int i, j;
	int bone_count = ourSkel->numBones();
	//resize outer vector...
	FullBodyVector.resize(anim_ctrl.numFrames() + 1);
	BoneVector.resize(anim_ctrl.numFrames()+1);
	for (i = 0; i < (int)BoneVector.size(); i++) {
		//...and each inner vector
		BoneVector[i].resize(bone_count);
	}

	//empty bone to initialize vector
	for (i = 0; i < anim_ctrl.numFrames()+1; i++) {
		for (j = 0; j < bone_count; j++) {
			Bone *tmpBone = ourSkel->getBone(j);
			BoneData this_bone(tmpBone, i);
			BoneVector[i][j] = this_bone;
		}

	}
	frame_duration = anim_ctrl.getFrameDuration();
	//Init QoMWeightMap (all 1's for now)
	for (i = 0; i < bone_count; i++) {
		std::string boneName = BoneVector[0][i].getBone()->getName();
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
	// reset clock so that the time spent loading is not included in animation time
	system_timer.reset();
}

void updateAnimation()
{
	if (goto_next_process) {
		process_control.advance();
		loadNextMotion();
		resetMaxValues();
		goto_next_process = false;
		animation_has_looped = false;
	}

	// Determine how much time has passed since the previous frame.
	double elapsed_time = system_timer.elapsedTime();

	// Check to see if any user inputs have been received since the last frame.
	input_processor.processInputs((float)elapsed_time);

	if (!anim_ctrl.isReady())
	{
		animation_time = 0.0f;
		animation_frame = 0;
		return;
	}

	bool status = false;
	if (real_time_mode)
		status = anim_ctrl.updateAnimation((float)elapsed_time);
	else
		status = anim_ctrl.framestepAnimation();

	if (anim_ctrl.looped())
	{
		if (!process_control.loop())
			goto_next_process = true;
		if (!animation_has_looped) {
			animation_has_looped = true;
			switch (process_control.dataMode()) {
			case ABDUCTION:
				cout << "max abduction: " << data_recorder.maxAbduction() << endl;
				break;
			case FLEXION:
				cout << "max flexion: " << data_recorder.maxFlexion() << endl;
				break;
			case EXTENSION:
				cout << "max extension: " << data_recorder.maxExtension() << endl;
				break;
			}
			data_recorder.writeToFile(process_control.resultFile());
			data_recorder.erase();
		}
	}
	animation_time = anim_ctrl.getAnimationTime();
	animation_frame = anim_ctrl.getAnimationFrame();

	processPTData();
}

int main(int argc, char **argv)
{
	if (!readCommandFile())
	{
		cerr << "No command file found. Exiting program." << endl;
		exit(1);
	}
	//process_control.printProcessControl();

	if (!process_control.active())
	{
		cerr << "No processing requests found. Exiting program." << endl;
		exit(1);
	}

	// load first motion before entering the processing loops
	loadNextMotion();
	goto_next_process = false;
	animation_has_looped = false;

	if (process_control.animationIsEnabled())
	{
		// initialize openGL and enter its rendering loop.
		try
		{
			// Start the global system timer/clock.
			system_timer.reset();

			// Initialize the graphics module.
			launchRenderer(argc, argv, updateAnimation);
		}
		catch (BasicException& excpt)
		{
			logout << "BasicException caught at top level." << endl;
			logout << "Exception message: " << excpt.msg << endl;
			logout << "Aborting program." << endl;
			cerr << "Aborting due to exception. See log file for details." << endl;
			shutDown(1);
		}
	}
	else
	{
		real_time_mode = false;
		while (true)
		{
			updateAnimation();
		}
		shutDown(0);
	}
}

