//-----------------------------------------------------------------------------
// MotionSequenceController.cpp
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

#include <Core/SystemConfiguration.h>
#include <Core/Utilities.h>
#include <Animation/MotionSequenceController.h>
#include <Animation/AnimationException.h>

MotionSequenceController::MotionSequenceController(MotionSequence* _ms) 
	: MotionController(), motion_sequence(_ms), prev_time(-1.0f), sequence_time(0.0f),
	apply_start_offset(false), frame_is_cached(NULL)
{ 
}

void MotionSequenceController::setRootOffset(Vector3D& _offset_position, Vector3D& _offset_orientation, long _offset_start_frame)
{
	apply_start_offset = true;
	start_offset_frame = _offset_start_frame;

	Vector3D orig_start_position(
		motion_sequence->getValue(CHANNEL_ID(0,CT_TX),start_offset_frame),
		motion_sequence->getValue(CHANNEL_ID(0,CT_TY),start_offset_frame),
		motion_sequence->getValue(CHANNEL_ID(0,CT_TZ),start_offset_frame));
	Vector3D orig_start_orientation(
		motion_sequence->getValue(CHANNEL_ID(0,CT_RX),start_offset_frame),
		motion_sequence->getValue(CHANNEL_ID(0,CT_RY),start_offset_frame),
		motion_sequence->getValue(CHANNEL_ID(0,CT_RZ),start_offset_frame));

	// compute root translation as position difference
	start_offset_translation = _offset_position - orig_start_position;
	Matrix4x4 so_translation_transform = Matrix4x4::translationXYZ(start_offset_translation);

	// compute root rotation as orientation difference (subtracting Euler angles)
	// this is not a valid difference, but it will work temporarily, since
	// we're primarily dealing with a 2D rotation around y axis (yaw/heading)
	start_offset_rotation = Vector3D(0.0f, _offset_orientation.yaw - orig_start_orientation.yaw, 0.0f);
	start_offset_rotation_transform = Matrix4x4::rotationZXY(start_offset_rotation);

	// combine the two root offset transforms
	start_offset_combined_transform = so_translation_transform * start_offset_rotation_transform;

	// initialize the root cache
	if (frame_is_cached != NULL) delete [] frame_is_cached;
	int num_frames = motion_sequence->numFrames();
	frame_is_cached = new bool[num_frames];
	memset(frame_is_cached, 0, num_frames);
	root_cache.resize(num_frames,6);
}

void MotionSequenceController::disableRootOffset()
{
	apply_start_offset = false;
}

bool MotionSequenceController::isValidChannel(CHANNEL_ID _channel, float _time)
{	
	if (motion_sequence == NULL) 
	{
		throw AnimationException("MotionSequenceController has no attached MotionSequence");
		return false;
	}
	return motion_sequence->isValidChannel(_channel);
}

float MotionSequenceController::getValue(CHANNEL_ID _channel, float _time)
{
	if (motion_sequence == NULL) 
		throw AnimationException("MotionSequenceController has no attached MotionSequence");

	if (!isValidChannel(_channel, _time)) 
	{
		string s = string("MotionSequenceController received request for invalid channel ") 
			+ " bone: " + toString(_channel.bone_id) + " dof: " + toString(_channel.channel_type);
		throw AnimationException(s.c_str());
	}

	float duration = motion_sequence->getDuration();
	long cycles = long(_time / duration);
	
	sequence_time = _time - duration*cycles;
	if (sequence_time > duration) sequence_time = 0.0f;

	int frame = int(motion_sequence->numFrames()*sequence_time/duration);

	float value = motion_sequence->getValue(_channel, frame);

	if (apply_start_offset)
	{
		// modify root based on offsets.
		if (_channel.bone_id == 0)
		{
			if (!frame_is_cached[frame])
			{
				Vector3D frame_position(
					motion_sequence->getValue(CHANNEL_ID(0,CT_TX), frame),
					motion_sequence->getValue(CHANNEL_ID(0,CT_TY), frame),
					motion_sequence->getValue(CHANNEL_ID(0,CT_TZ), frame));
				Vector3D frame_orientation(
					motion_sequence->getValue(CHANNEL_ID(0,CT_RX), frame),
					motion_sequence->getValue(CHANNEL_ID(0,CT_RY), frame),
					motion_sequence->getValue(CHANNEL_ID(0,CT_RZ), frame));

				frame_position = start_offset_combined_transform * frame_position;

				Matrix4x4 orig_orientation_transform = 
					Matrix4x4::rotationZXY(frame_orientation);
				Matrix4x4 new_orientation_transform = start_offset_rotation_transform*orig_orientation_transform;
				
				float p, y, r;
				// invalid rotation concatenation, Euler angles don't add properly
				p = frame_orientation.pitch + start_offset_rotation.pitch;
				y = frame_orientation.yaw + start_offset_rotation.yaw;
				r = frame_orientation.roll + start_offset_rotation.roll;
				//logout << "Frame " << frame << endl;
				//logout << "\tframe orientation: " << frame_orientation << endl;
				//logout << "\toffset rotation: " << start_offset_rotation << endl;
				frame_orientation = Vector3D(p,y,r);
				//logout << "\tnew orientation: " << frame_orientation << endl;
				
				root_cache.set(frame,0,frame_position.x);
				root_cache.set(frame,1,frame_position.y);
				root_cache.set(frame,2,frame_position.z);
				root_cache.set(frame,3,frame_orientation.pitch);
				root_cache.set(frame,4,frame_orientation.yaw);
				root_cache.set(frame,5,frame_orientation.roll);
				frame_is_cached[frame] = true;
			}
			switch(_channel.channel_type)
			{
			case CT_TX:
				value = root_cache.get(frame,0); break;
			case CT_TY:
				value = root_cache.get(frame,1); break;
			case CT_TZ:
				value = root_cache.get(frame,2); break;
			case CT_RX:
				value = root_cache.get(frame,3); break;
			case CT_RY:
				value = root_cache.get(frame,4); break;
			case CT_RZ:
				value = root_cache.get(frame,5); break;
			default:
				value = 0.0f; break;
			}
		}
	}

	return value;
}
