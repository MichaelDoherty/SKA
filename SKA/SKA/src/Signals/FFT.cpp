//-----------------------------------------------------------------------------
// FFT.cpp
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

#include <Core/SystemConfiguration.h>
#include <cstdio>
#include <cstdlib>
#include <fstream>
using namespace std;
#include <Signals/FFT.h>
#include <Animation/Skeleton.h>
#if ENABLE_FFTW==1
#include <fftw3.h>
#endif

// Locates strongest signals from a Fourier Transform spectrum
// inputs:  spectrum: the FT 
//          n: length of the FT
//          num_signals: number of signals to extract
// outputs: signals: the extracted signals
//          return value: number of signals actually extracted (<= num_signals)
// note: phase is currently set for SINE waves
int extractSignalsFromSpectrum(complex<float>* spectrum, int n,
							   vector<SignalSpec>& signals, int num_signals)
{
	int i, j, k;
	int* sig_indexes = new int[num_signals];
	float* norms = new float[n];
	for (k=0; k<n; k++)
		norms[k] = norm(spectrum[k]);
	float max = FLT_MAX;
	// FIXIT! this is incredibly naive
	for (i=0; i<num_signals; i++)
	{
		int maxk = -1;
		float best = FLT_MIN;
		for (k=0; k<n; k++)
		{
			if (norms[k] < max)
			{
				if (norms[k] > best)
				{
					maxk = k;
					best = norms[k];
				}
			}
		}
		sig_indexes[i] = maxk;
		max = norms[maxk]; // (minus epsilon)
	}
	for (j=0; j<i; j++)
	{
		complex<float> c = spectrum[sig_indexes[j]];
		SignalSpec ss;
		ss.amplitude = 2.0f*sqrt(c.real()*c.real()+c.imag()*c.imag())/n;
		ss.frequency = float(sig_indexes[j]*100)/n;
		ss.phase = atan2(c.real(), -c.imag());
		signals.push_back(ss);
	}
	delete [] sig_indexes;
	delete [] norms;
	return i;
}


// Structure for converting data to format required by FFTW algorithms
#if ENABLE_FFTW==1
struct FFTW_Array
{
	long n;
	fftw_complex* data;
	FFTW_Array()
	{
		n = 0;
		data = NULL;
	}
	FFTW_Array(long _n)
	{
		data = NULL;
		resize(_n);
	}
	~FFTW_Array()
	{
		if (data != NULL) fftw_free(data);
	}
	void resize(long _n)
	{
		if (data != NULL) fftw_free(data);
		n = _n;
		data = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * n);
	}
	void load(float* f)
	{
		for (long i=0; i<n; i++) { data[i][0] = f[i]; data[i][1] = 0.0; }
	}
	void extract(float* f)
	{
		for (long i=0; i<n; i++) f[i] = (float)data[i][0];
	}
	void load(complex<float>* f)
	{
		for (long i=0; i<n; i++) { data[i][0] = f[i].real(); data[i][1] = f[i].imag(); }
	}
	void extract(complex<float>* f)
	{
		for (long i=0; i<n; i++)  
			f[i] = complex<float>((float)data[i][0],(float)data[i][1]); 
	}
};
#endif // ENABLE_FFTW==1

bool computeFFT(float* signal, complex<float>* spectrum, int n)
{
#if ENABLE_FFTW==1
	FFTW_Array in, out;
	in.resize(n);
	out.resize(n);
	in.load(signal);
	
	fftw_plan fwd_plan;
	fwd_plan = fftw_plan_dft_1d(n, in.data, out.data, FFTW_FORWARD, FFTW_ESTIMATE);
	fftw_execute(fwd_plan); 
	fftw_destroy_plan(fwd_plan);

	out.extract(spectrum);
	return true;
#else // ENABLE_FFTW==0
	return false;
#endif // ENABLE_FFTW
}

bool computeFFT(complex<float>* signal, complex<float>* spectrum, int n)
{
#if ENABLE_FFTW==1
	FFTW_Array in, out;
	in.resize(n);
	out.resize(n);
	in.load(signal);
	
	fftw_plan fwd_plan;
	fwd_plan = fftw_plan_dft_1d(n, in.data, out.data, FFTW_FORWARD, FFTW_ESTIMATE);
	fftw_execute(fwd_plan); 
	fftw_destroy_plan(fwd_plan);

	out.extract(spectrum);
	return true;
#else // ENABLE_FFTW==0
	return false;
#endif // ENABLE_FFTW
}

bool computeInverseFFT(complex<float>* spectrum, float* signal,int n)
{
#if ENABLE_FFTW==1
	FFTW_Array in, out;
	in.resize(n);
	out.resize(n);
	in.load(spectrum);

	fftw_plan bwd_plan;
	bwd_plan = fftw_plan_dft_1d(n, in.data, out.data, FFTW_BACKWARD, FFTW_ESTIMATE);
	fftw_execute(bwd_plan); 
	fftw_destroy_plan(bwd_plan);

	out.extract(signal);
	for (int i=0; i<n; i++) { signal[i] /= n; }
	return true;
#else // ENABLE_FFTW==0
	return false;
#endif // ENABLE_FFTW
}

bool computeInverseFFT(complex<float>* spectrum, complex<float>* signal,int n)
{
#if ENABLE_FFTW==1
	FFTW_Array in, out;
	in.resize(n);
	out.resize(n);
	in.load(spectrum);

	fftw_plan bwd_plan;
	bwd_plan = fftw_plan_dft_1d(n, in.data, out.data, FFTW_BACKWARD, FFTW_ESTIMATE);
	fftw_execute(bwd_plan); 
	fftw_destroy_plan(bwd_plan);

	out.extract(signal);
	for (int i=0; i<n; i++) { signal[i] /= complex<float>(float(n),0.0f); }
	return true;
#else // ENABLE_FFTW==0
	return false;
#endif // ENABLE_FFTW
}

bool computeInverseFFT(float* input, float* output, int n)
{
#if ENABLE_FFTW==1
	FFTW_Array fft, signal;
	fft.resize(n);
	signal.resize(n);
	fft.load(input);

	fftw_plan bwd_plan;
	bwd_plan = fftw_plan_dft_1d(n, fft.data, signal.data, FFTW_BACKWARD, FFTW_ESTIMATE);
	fftw_execute(bwd_plan); 
	fftw_destroy_plan(bwd_plan);
	for (int i=0; i<n; i++) { signal.data[i][0] /= n; signal.data[i][1] /= n;}

	signal.extract(output);
	return true;
#else // ENABLE_FFTW==0
	return false;
#endif // ENABLE_FFTW
}

bool computeFFT2(float* input, float* output1, float* output2, int n)
{
#if ENABLE_FFTW==1
	FFTW_Array signal, fft, recon;
	signal.resize(n);
	fft.resize(n);
	recon.resize(n);
	signal.load(input);
	
	fftw_plan fwd_plan;
	fwd_plan = fftw_plan_dft_1d(n, signal.data, fft.data, FFTW_FORWARD, FFTW_ESTIMATE);
	fftw_execute(fwd_plan); 
	fftw_destroy_plan(fwd_plan);

	fft.extract(output1);

	fftw_plan bwd_plan;
	bwd_plan = fftw_plan_dft_1d(n, fft.data, recon.data, FFTW_BACKWARD, FFTW_ESTIMATE);
	fftw_execute(bwd_plan); 
	fftw_destroy_plan(bwd_plan);
	for (int i=0; i<n; i++) { recon.data[i][0] /= n; recon.data[i][1] /= n;}

	recon.extract(output2);
	return true;
#else // ENABLE_FFTW==0
	return false;
#endif // ENABLE_FFTW
}

#if ENABLE_FFTW==1
static void runFFTwithFilter(FFTW_Array& signal, FFTW_Array& fft, FFTW_Array& recon_signal, int cutoff=-1)
{
	int n = signal.n;
	
	fft.resize(n);
	recon_signal.resize(n);
	
	fftw_plan fwd_plan, bwd_plan;

	fwd_plan = fftw_plan_dft_1d(n, signal.data, fft.data, FFTW_FORWARD, FFTW_ESTIMATE);
	fftw_execute(fwd_plan); 
	fftw_destroy_plan(fwd_plan);

	if (cutoff > 0)
		for (int i=cutoff; i<n; i++) { fft.data[i][0] = fft.data[i][1] = 0.0f; }

	bwd_plan = fftw_plan_dft_1d(n, fft.data, recon_signal.data, FFTW_BACKWARD, FFTW_ESTIMATE);
	fftw_execute(bwd_plan); 
	fftw_destroy_plan(bwd_plan);
	for (int i=0; i<n; i++) { recon_signal.data[i][0] /= n; recon_signal.data[i][1] /= n;}
}

static void runFFTwithFilter(float* signal, complex<float>* spectrum, float* recon_signal, int n, int cutoff=-1)
{
	computeFFT(signal, spectrum, n);

	if (cutoff > 0)
		for (int i=cutoff; i<n; i++) { spectrum[i] = complex<float>(0.0f,0.0f); }

	computeInverseFFT(spectrum, recon_signal, n);

	for (int i=0; i<n; i++) { recon_signal[i] /= n; }
}
#endif // ENABLE_FFTW

void FFTfilter::setFilterChannels()
{
	for (int channel=0; channel<num_channels; channel++)
	{
		filter_channels[channel] = (channel>=6);
	}
}

void FFTfilter::initialize(MotionSequence* _motion)
{
	motion = _motion;
	num_frames = motion->numFrames();
	num_channels = motion->numChannels();
	original.resize(num_frames, num_channels);
	filtered.resize(num_frames, num_channels);
	setFilterChannels();

	for (int channel=0; channel<num_channels; channel++)
	{
		float* ptr = motion->getChannelPtr(channel);
		memcpy(original.getColumnPtr(channel), ptr, num_frames*sizeof(float));
		memcpy(filtered.getColumnPtr(channel), ptr, num_frames*sizeof(float));
	}
}

void FFTfilter::runFilters(int cutoff)
{
	printf("FFTfilter running %d\n", cutoff);

	if (motion==NULL) return;

	for (int channel=0; channel<num_channels; channel++)
		if (filter_channels[channel])
			runFilter(channel, cutoff);
}

bool FFTfilter::runFilter(CHANNEL_ID& channel, int cutoff)				
{
	short i = motion->getChannelIndex(channel);
	if (i<0) return false;
	runFilter(i, cutoff);
	return true;
}

void FFTfilter::runFilter(int channel, int cutoff)				
{
	float* original_data = original.getColumnPtr(channel);
	float* filtered_data = filtered.getColumnPtr(channel);
#if ENABLE_FFTW==1
	if (cutoff < 0)
	{
		memcpy(filtered_data, original_data, num_frames*sizeof(float));
	}
	else
	{
		FFTW_Array signal, fft, recon_signal;
		signal.resize(num_frames);
		fft.resize(num_frames);
		recon_signal.resize(num_frames);
		signal.load(original_data);
		runFFTwithFilter(signal, fft, recon_signal, cutoff);
		recon_signal.extract(filtered_data);
	}
	memcpy(motion->data.getColumnPtr(channel), filtered_data, num_frames*sizeof(float));
#else // ENABLE_FFTW==0
	memcpy(filtered_data, original_data, num_frames*sizeof(float));
#endif // ENABLE_FFTW
}

int FFTfilter::computeFFT(CHANNEL_ID& channel, float result[], int len)
{
	short i = motion->getChannelIndex(channel);
	if (i<0) return -1;
	return computeFFT(i, result, len);
}



int FFTfilter::computeFFT(int channel, float result[], int len)
{
#if ENABLE_FFTW==1
	int n = num_frames;
	float* original_data = original.getColumnPtr(channel);
	float* fft_input = new float[n];
	memcpy(fft_input, original_data, n*sizeof(float));
	float* fft_data = new float[n];

	FFTW_Array signal, fft, recon_signal;
	signal.resize(n);
	fft.resize(n);
	signal.load(fft_input);
	runFFTwithFilter(signal, fft, recon_signal);
	fft.extract(fft_data);

	if (n > len) n = len;
	memcpy(result, fft_data, n*sizeof(float));
	delete [] fft_data;

	return n;
#else // ENABLE_FFTW==0
	return 0;
#endif // ENABLE_FFTW
}

int FFTfilter::computeFFT(CHANNEL_ID& channel, complex<float> result[], int len)
{
	short i = motion->getChannelIndex(channel);
	if (i<0) return -1;
	return computeFFT(i, result, len);
}

// INWORK - modify this to extract both real and imaginary components
int FFTfilter::computeFFT(int channel, complex<float> result[], int len)
{
#if ENABLE_FFTW==1
	int n = num_frames;
	float* original_data = original.getColumnPtr(channel);
	float* fft_input = new float[n];
	memcpy(fft_input, original_data, n*sizeof(float));
	float* fft_data = new float[n];

	FFTW_Array signal, fft, recon_signal;
	signal.resize(n);
	fft.resize(n);
	signal.load(fft_input);
	runFFTwithFilter(signal, fft, recon_signal);
	fft.extract(fft_data);

	if (n > len) n = len;
	memcpy(result, fft_data, n*sizeof(float));
	delete [] fft_data;

	return n;
#else // ENABLE_FFTW==0
	return 0;
#endif // ENABLE_FFTW
}