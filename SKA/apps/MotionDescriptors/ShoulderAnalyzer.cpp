//-----------------------------------------------------------------------------
// MotionDescriptors project - Builds with SKA Version 4.0
//-----------------------------------------------------------------------------
// ShoulderAnalyzer.cpp
// Author: Michael Doherty - Fall 2016 - Spring 2017
//   Originally developed to support Laurel/Davenport physical therapy project.
//-----------------------------------------------------------------------------
// SKA configuration
#include <Core/SystemConfiguration.h>
// local application
#include "ShoulderAnalyzer.h"
#include "DataRecorder.h"
#include "ProcessControl.h"
#include "AppGraphics.h"

static DataRecorder data_recorder;

// abduction = lift of arm in coronal plane
// flexion = arm forward in sagittal plane
// extension = arm backward in sagittal plane
static float flexion = 0.0f, extension = 0.0f, abduction = 0.0f;
static float dir_test = 0.0f;
static float max_flexion = 0.0f, max_extension = 0.0f, max_abduction = 0.0f;
static long animation_frame = 0;
static float animation_time = 0.0f;

void ShoulderAnalyzer::resetMaxValues() {
	max_flexion = 0.0f; max_extension = 0.0f; max_abduction = 0.0f;
}

void ShoulderAnalyzer::storeResults() {
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

void ShoulderAnalyzer::processPTData() {
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

	if (!process_control.animationHasLooped())
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
		analysis_objects.right_dir_marker->setEndpoints(p, p + (right_dir*20.0f));
		analysis_objects.forward_dir_marker->setEndpoints(p, p + (forward_dir*20.0f));
		analysis_objects.up_dir_marker->setEndpoints(p, p + (up_dir*20.0f));

		// (these should be extracted from the three Plane objects)
		analysis_objects.coronal_plane_image->moveTo(coronal_plane.getPointInPlane());
		analysis_objects.coronal_plane_image->rotateTo(root_orientation);
		analysis_objects.sagittal_plane_image->moveTo(sagittal_plane.getPointInPlane());
		analysis_objects.sagittal_plane_image->rotateTo(root_orientation);
		analysis_objects.transverse_plane_image->moveTo(transverse_plane.getPointInPlane());
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
