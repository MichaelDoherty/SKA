//-----------------------------------------------------------------------------
// Signals.cpp
//   Signal generation classes.
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
#include <Signals/Signals.h>
#include <fstream>
using namespace std;
#include <Math/Math.h>
#include <Signals/FFT.h>

SignalGenerator* buildSignalGenerator(vector<SignalSpec>& specs)
{
	SignalGenerator* g = NULL;
	for (unsigned s=0; s<specs.size(); s++)
	{
		SignalGenerator* g1 = NULL;
		if (fabs(specs[s].frequency) < EPSILON)
			g1 = new ConstantGenerator(specs[s].amplitude);
		else
			g1 = new SineGenerator(specs[s].amplitude, specs[s].frequency, specs[s].phase);
		if (g == NULL) 
			g = g1;
		else
			g = new SignalAdder(g, g1);
	}
	return g;
}

static const int n=400;
static void FFTtest(SignalGenerator* sg)
{
	cout << "starting FFT computation" << endl;
	complex<float> signal[n];
	complex<float> spectrum[n];
	complex<float> recon[n];
	float time = 0.0f;
	for (unsigned short i=0; i<n; i++)
	{
		signal[i] = complex<float>(sg->signal(time),0.0f);
		time += 0.01f;
	}

	computeFFT(signal, spectrum, n);
	computeInverseFFT(spectrum, recon, n);

	ofstream out1("data/signal1.txt");
	ofstream out2("data/spectrum-r1.txt");
	ofstream out3("data/spectrum-i1.txt");
	ofstream out4("data/sigrecon-r1.txt");
	ofstream out5("data/sigrecon-i1.txt");
	for (unsigned short i=0; i<n; i++)
	{
		out1 << signal[i].real() << endl;
		out2 << spectrum[i].real() << endl;
		out3 << spectrum[i].imag() << endl;
		out4 << recon[i].real() << endl;
		out5 << recon[i].imag() << endl;
	}
	out1.close();
	out2.close();
	out3.close();
	out4.close();
	out5.close();
	vector<SignalSpec> signals;
	int n2 = extractSignalsFromSpectrum(spectrum, n, signals, 3);
	ofstream out6("data/signals.txt");
	for (unsigned short i=0; i<n2; i++)
		out6 << "A " << signals[i].amplitude 
		<< " F " << signals[i].frequency 
		<< " P " << signals[i].phase << endl;
	out6.close();

	cout << "FFT computation finished" << endl;
}

void testSignalGenerator()
{
	//CosineGenerator* cos1 = new CosineGenerator(1.0f, 20.0f);
	//SineGenerator* sin1 = new SineGenerator(1.0f, 10.0f, HALF_PI);
	//SineGenerator* sin2 = new SineGenerator(1.0f, 10.0f, HALF_PI);
	//SineGenerator* sin2 = new SineGenerator(1.0f, 2.0f, 0.0f);
	//SignalAdder* sg = new SignalAdder(sin1,sin2);

	//SignalGenerator* s = new ConstantGenerator(1.0f);
	SignalGenerator* s = new SineGenerator(1.0f, 20.0f, 0.0f);
	FFTtest(s);

	/*
	ofstream out1("data/sin1.txt");
	ofstream out2("data/sin2.txt");
	ofstream out3("data/sin3.txt");
	for (float t=0.0f; t<=3.0f; t+=0.01f)
	{
		out1 << sin1->signal(t) << endl;
		out2 << sin2->signal(t) << endl;
		out3 << sg->signal(t) << endl;
	}
	delete sg;
	out1.close();
	out2.close();
	out3.close();
	*/
}