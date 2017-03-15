//-----------------------------------------------------------------------------
// FFT.h
//   Interface to Fast Fourier Transform processing.
//   This code is non-functional if not linked to FFTW library,
//   available from www.fftw.org.
//   See ENABLE_FFTW flag in Core/SystemConfiguration.h.
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

#ifndef FFT_DOT_H
#define FFT_DOT_H

#include <Core/SystemConfiguration.h>
#include <complex>
#include <vector>
using namespace std;
#include <Animation/MotionSequence.h>
#include <Core/Array2D.h>
#include <Signals/SignalSpec.h>

int extractSignalsFromSpectrum(complex<float>* spectrum, int n,
							   vector<SignalSpec>& signals, int num_signals);

// Basic FFT functionality
bool computeFFT(float* signal, complex<float>* spectrum, int n);
bool computeFFT(complex<float>* signal, complex<float>* spectrum, int n);
bool computeInverseFFT(complex<float>* spectrum, float* signal, int n);
bool computeInverseFFT(complex<float>* spectrum, complex<float>* signal, int n);

// FFT directly integrated with a MotionSequence
class FFTfilter
{
public:
	FFTfilter()
	{
		num_frames = 0;
		motion = NULL;
	}
	~FFTfilter()
	{
	}
	void initialize(MotionSequence* _motion);
	void runFilter(int channel, int cutoff);
	bool runFilter(CHANNEL_ID& channel, int cutoff);
	void runFilters(int cutoff);
	int computeFFT(CHANNEL_ID& channel, float result[], int len);
	int computeFFT(int channel, float result[], int len);
	int computeFFT(CHANNEL_ID& channel, complex<float> result[], int len);
	int computeFFT(int channel, complex<float> result[], int len);
private:
	Array2D<float> original;
	Array2D<float> filtered;
	int num_frames;
	int num_channels;
	MotionSequence* motion;

	bool filter_channels[500];
	void setFilterChannels();
};

#endif
