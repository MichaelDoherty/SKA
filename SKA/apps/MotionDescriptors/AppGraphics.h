//-----------------------------------------------------------------------------
// MotionDescriptors project - Builds with SKA Version 4.0
//-----------------------------------------------------------------------------
// AppGraphics.h
//-----------------------------------------------------------------------------
#ifndef APPGRAPHICS_DOT_H
#define APPGRAPHICS_DOT_H
// SKA configuration
#include <Core/SystemConfiguration.h>
// C/C++ libraries
#include <list>
using namespace std;
// SKA modules
#include <Objects/Object.h>
#include <Objects/BoneObject.h>

// pointers to animated objects that need to be drawn (bones)
extern list<Object*> character_render_list;
// pointers to background objects that need to be drawn
//extern list<Object*> bg_render_list;
// pointers to transparent objects that need to be drawn (drawn last)
//extern list<Object*> transparent_render_list;

struct AnalysisObjects
{
	bool show_coronal_plane;
	bool show_sagittal_plane;
	bool show_transverse_plane;
	bool show_skeleton;

	Object* coronal_plane_image;
	Object* sagittal_plane_image;
	Object* transverse_plane_image;

	Object* upperarm_marker;
	Object* elbow_marker;
	Object* root_marker;
	Object* upperarm_cp_marker; // cp = projected on coronal plane
	Object* elbow_cp_marker;
	Object* upperarm_sp_marker; // sp = projected on sagittal plane
	Object* elbow_sp_marker;

	Object* spine_cp_marker;	// These mark the end of the spine vectors when aligned with upper arm markers
	Object* spine_sp_marker;

	BoneObject* arm_cp_bone;	// artificial bone from upperarm to elbow
	BoneObject* arm_sp_bone;
	BoneObject* spine_cp_bone;	// artificial bone from upperarm in direction of spine
	BoneObject* spine_sp_bone;

	BoneObject* right_dir_marker;
	BoneObject* forward_dir_marker;
	BoneObject* up_dir_marker;

	AnalysisObjects() { clear(); }

	void clear()
	{
		show_coronal_plane = true;
		show_sagittal_plane = true;
		show_transverse_plane = true;
		show_skeleton = true;
		coronal_plane_image = NULL;
		sagittal_plane_image = NULL;
		transverse_plane_image = NULL;
		upperarm_marker = NULL;
		elbow_marker = NULL;
		root_marker = NULL;
		upperarm_cp_marker = NULL;
		elbow_cp_marker = NULL;
		upperarm_sp_marker = NULL;
		elbow_sp_marker = NULL;
		spine_cp_marker = NULL;
		spine_sp_marker = NULL;
		arm_cp_bone = NULL;
		arm_sp_bone = NULL;
		spine_cp_bone = NULL;
		spine_sp_bone = NULL;
		right_dir_marker = NULL;
		forward_dir_marker = NULL;
		up_dir_marker = NULL;
	}
};
extern AnalysisObjects analysis_objects;

extern AnalysisObjects analysis_objects;

struct HudData
{
	float animation_time;
	long animation_frame;
	float flexion, extension, abduction;
	float dir_test;
	float max_flexion, max_extension, max_abduction;
	bool focus_flexion, focus_extension, focus_abduction;
	/*Trevor's code to detect right hand crossing sag. plane*/
	string bone_cross_plane;
	string bone_tracking_name;
	string plane_tracking_name;
	float relationVal;
	float velocity;
	bool animation_paused;
	float animation_time_warp;
	string take_label;
	string move_direction;

	HudData() :
		animation_time(0.0f), animation_frame(0),
		flexion(0.0f), extension(0.0f), abduction(0.0f), dir_test(0.0f),
		max_flexion(0.0f), max_extension(0.0f), max_abduction(0.0f),
		focus_flexion(true), focus_extension(true), focus_abduction(true),
		bone_cross_plane("NULL"), bone_tracking_name("NULL"), plane_tracking_name("NULL"),
		relationVal(0.0f), velocity(0.0f),  
		animation_paused(false), animation_time_warp(1.0f), 
		take_label("NO FILE LOADED"), move_direction("NULL")
	{}
};
extern HudData hud_data;

extern void launchRenderer(int argc, char **argv, void(*animation_callback)(void));

#endif // APPGRAPHICS_DOT_H
