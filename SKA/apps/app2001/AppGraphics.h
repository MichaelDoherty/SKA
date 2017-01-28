//-----------------------------------------------------------------------------
// app2001 - Builds with SKA Version 3.1 - Dec 6, 2016 - Michael Doherty
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

	Object* rua_start_marker; // rua = right upper arm
	Object* rua_end_marker;
	Object* root_marker;
	Object* ruacp_start_marker; // ruacp = right upper arm projected on coronal plane
	Object* ruacp_end_marker;
	Object* ruasp_start_marker; // ruacp = right upper arm projected on sagittal plane
	Object* ruasp_end_marker;
	Object* spinecp_end_marker;
	Object* spinesp_end_marker;

	BoneObject* ruacp_bone;
	BoneObject* ruasp_bone;
	BoneObject* spinecp_bone;
	BoneObject* spinesp_bone;

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
		rua_start_marker = NULL; 
		rua_end_marker = NULL;
		root_marker = NULL;
		ruacp_start_marker = NULL; 
		ruacp_end_marker = NULL;
		ruasp_start_marker = NULL;
		ruasp_end_marker = NULL;
		spinecp_end_marker = NULL;
		spinesp_end_marker = NULL;
		ruacp_bone = NULL;
		ruasp_bone = NULL;
		spinecp_bone = NULL;
		spinesp_bone = NULL;
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

	HudData() : 
		animation_time(0.0f), animation_frame(0),
		flexion(0.0f), extension(0.0f), abduction(0.0f), dir_test(0.0f),
		max_flexion(0.0f), max_extension(0.0f), max_abduction(0.0f),
		focus_flexion(true), focus_extension(true), focus_abduction(true),
		animation_paused(false), animation_time_warp(1.0f), take_label("NO FILE LOADED")
	{}
};
extern HudData hud_data;

extern void launchRenderer(int argc, char **argv, void(*animation_callback)(void));

#endif // APPGRAPHICS_DOT_H
