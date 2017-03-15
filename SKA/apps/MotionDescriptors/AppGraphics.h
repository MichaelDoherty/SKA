//-----------------------------------------------------------------------------
// MotionDescriptors project - Builds with SKA Version 4.0
// THIS FILES IN THIS PROJECT ARE CURRENTLY JUST PLACE HOLDERS FOR 
// THE REAL CODE WHICH IS IN DEVELOPMENT (March 14 2017)
//-----------------------------------------------------------------------------
// AppGraphics.h
//    Animation controller for a Physical Therapy data analysis.
//    This controller is limited to playback of one motion file (one character)
//    at a time.
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

struct HudData
{
	float animation_time;
	long animation_frame;
	float flexion, extension, abduction;
	float dir_test;
	float max_flexion, max_extension, max_abduction;
	bool focus_flexion, focus_extension, focus_abduction;

	bool animation_paused;
	float animation_time_warp;
	string take_label;
	string analysis_mode;
	bool recording;

	float misc[10];

	HudData() : 
		animation_time(0.0f), animation_frame(0),
		flexion(0.0f), extension(0.0f), abduction(0.0f), dir_test(0.0f),
		max_flexion(0.0f), max_extension(0.0f), max_abduction(0.0f),
		focus_flexion(true), focus_extension(true), focus_abduction(true),
		animation_paused(false), animation_time_warp(1.0f), 
		take_label("NO FILE LOADED"), analysis_mode("UNDEFINED"), recording(false)
	{
		for (short i = 0; i < 10; i++) misc[i] = 0.0f;
	}
};
extern HudData hud_data;

extern void launchRenderer(int argc, char **argv, void(*animation_callback)(void));

#endif // APPGRAPHICS_DOT_H
