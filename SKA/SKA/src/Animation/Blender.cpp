//-----------------------------------------------------------------------------
// Blender.cpp
//	 Class for blending motions. 
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
#include <Animation/Blender.h>
#include <Core/SystemLog.h>

static const short MAX_INPUT_CHANNELS = 40;

MotionSequence* Blender::blend(MOTION_BLEND_SPEC& spec)
{
	MotionSequence* result = new MotionSequence;
	unsigned short i, j, frame;

	result->setNumFrames(spec.num_frames);
	result->setFrameRate(120);
	for (i=0; i<spec.channel_specs.size(); i++)
	{
		result->addChannel(spec.channel_specs[i].channel);
	}
	result->adjustStorage();

	// FIXIT! result need to resize its Array2D once all channels are defined.

	for (i=0; i<spec.channel_specs.size(); i++)
	{
		CHANNEL_BLEND_SPEC cblend = spec.channel_specs[i];
		{
			// FIXIT! This needs to account for each channels frame rate

			// compute time warp for each input channel
			float warp[MAX_INPUT_CHANNELS];
			short result_frame_range = cblend.end_frame - cblend.start_frame;
			for (j=0; j<cblend.inputs.size(); j++)
			{
				// warp from target time back to source time
				warp[j] = (cblend.inputs[j].end_frame-cblend.inputs[j].start_frame)
					/ float(result_frame_range);
			}
			
			for (frame=cblend.start_frame; frame<cblend.end_frame; frame++)
			{
				float value = 0.0f;
				for (j=0; j<cblend.inputs.size(); j++)
				{
					// FIXIT! need to interpolate
					long cframe = short((frame-cblend.start_frame) * warp[j])
						+ cblend.inputs[j].start_frame;
					if (cframe >= cblend.inputs[j].ms->numFrames())
						cframe = cblend.inputs[j].ms->numFrames()-1;
					value += cblend.inputs[j].blend_weight *
						cblend.inputs[j].ms->getValue(cblend.channel, cframe);
				}
				result->setValue(cblend.channel, frame, value);
			}
		}
	}

	return result;
}

ostream& operator<<(ostream& out, MOTION_BLEND_SPEC& spec)
{
	out << "Motion Blend Spec: number of frames " << spec.num_frames << endl;
	for (unsigned int i=0; i<spec.channel_specs.size(); i++)
	{
		CHANNEL_BLEND_SPEC& cspec = spec.channel_specs[i];
		out << "Channel Spec: start frame " << cspec.start_frame
			<< " end frame " << cspec.end_frame << endl;
		out << " channel " << cspec.channel << endl;
		for (unsigned int j=0; j<cspec.inputs.size(); j++)
		{
			BLEND_INPUT& blin = cspec.inputs[j];
			out << "  Blend Input: start frame " << blin.start_frame
				<< " end frame " << blin.end_frame << endl;
			out << "    blend weight " << blin.blend_weight << endl;
			out << "    motion seqence pointer " << blin.ms << endl;
		}
	}
	return out;
}
