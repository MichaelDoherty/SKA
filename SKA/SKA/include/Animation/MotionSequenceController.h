//-----------------------------------------------------------------------------
// MotionSequenceController.h
//    Class MotionSequenceController generates motion by reading 
//    stored poses in a MotionSequence.
//    (A motion sequence is generally read from a .AMC or .BVH mocap file.)
//    Primary function is to convert clock time into a sequence frame.
//-----------------------------------------------------------------------------
// This software is part of the Skeleton Animation Toolkit (SKA) developed 
// at the University of the Pacific, under the guidance of Michael Doherty.
// For information please contact mdoherty@pacific.edu.
//-----------------------------------------------------------------------------
// This is open software. You are free to use it as you see fit.
// The University of the Pacific and identified authors would appreciate
// being credited for any significant use, particularly if used for
// commercial projects or academic research publications.
//-----------------------------------------------------------------------------

#ifndef MOTIONSEQUENCECONTROLLER_DOT_H
#define MOTIONSEQUENCECONTROLLER_DOT_H
#include <Core/SystemConfiguration.h>
#include <Core/Array2D.h>
#include <Math/Matrix4x4.h>
#include <Animation/MotionController.h>
#include <Animation/MotionSequence.h>

// March 11, 2013 - root offset option is working, but with hacked rotation offsets.
//
// set the root offset and turn it on by calling MotionSequenceController::setRootOffset()
// disable it with MotionSequenceController::disableRootOffset

class SKA_LIB_DECLSPEC MotionSequenceController : public MotionController
{
public:
	MotionSequenceController() 
		: MotionController(), motion_sequence(NULL), prev_time(-1.0f), sequence_time(0.0f),
			apply_start_offset(false), frame_is_cached(NULL)
	{ }

	MotionSequenceController(MotionSequence* _ms);
	
	virtual ~MotionSequenceController() { }

	virtual bool isValidChannel(CHANNEL_ID _channel, float _time);

	virtual float getValue(CHANNEL_ID _channel, float _time);

	MotionSequence* getMotionSequence() { return motion_sequence; }

	virtual void setRootOffset(Vector3D& _offset_position, Vector3D& _offset_orientation, long _offset_start_frame);
	virtual void disableRootOffset();

private:
	MotionSequence* motion_sequence;
	float prev_time;		// world time at last call to getValue()
	float sequence_time;	// current time in the MotionSequence cycle
	
	// start offset position and rotation, from some arbitrary start frame
	bool apply_start_offset;
	long start_offset_frame;
	Vector3D start_offset_translation;
	Vector3D start_offset_rotation;
	Matrix4x4 start_offset_combined_transform; 
	Matrix4x4 start_offset_rotation_transform; 
	// an nx6 array to store offset root posiitons and orientations once they've been computed.
	Array2D<float> root_cache;
	bool* frame_is_cached;
};

#endif
