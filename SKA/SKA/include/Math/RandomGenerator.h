//-----------------------------------------------------------------------------
// RandomGenerator.h
//   The RandomGenerator class encapsulates a seed and 
//   a variety of functions for generating random numbers.
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
// Some code in this file is derived from the works attributed below.
// Use of this code may be restricted by their ownership.
//-----------------------------------------------------------------------------

#ifndef RANDOMGENERATOR_DOT_H
#define RANDOMGENERATOR_DOT_H
#include <Core/SystemConfiguration.h>

//-----------------------------------------------------------------------------
// The randf() function is from R. Jain, 
// "The Art of Computer Systems Performance Analysis"
// John Wiley & Sons, 1991. (Page 443, Figure 26.2).
// It was retrieved on 02/17/05 from
// http://www.csee.usf.edu/~christen/tools/unif.c
//-----------------------------------------------------------------------------
// The functions uniform(), random(), expntl(), erlang(), hyperx(), 
// triang() and normal() are by M.H. MacDougall, 
// "Simulating Computer Systems", MIT Press, 1987.
// These functions were taken from Paul A. Fishwick's
// simpack distribution, retrieved on 02/06/05 from
// http://www.cise.ufl.edu/~fishwick/simpack/howtoget.html
//-----------------------------------------------------------------------------

#include <cstdlib>
#include <cmath>
#include <ctime>
using namespace std;
#include <Math/Math.h>

class SKA_LIB_DECLSPEC RandomGenerator
{
public:

	RandomGenerator(long _seed=0)
	{
		if (_seed == 0) seed = (long)time(0);
		else seed = _seed;
	}

	// UNIFORM (0.0, 1.0) RANDOM REAL NUMBER GENERATOR
	// returns a psuedo-random variate from a uniform    
	// distribution in range (0.0, 1.0)	
	double randf(void)
	{
		const long  a =      16807;  // Multiplier
		const long  m = 2147483647;  // Modulus
		const long  q =     127773;  // m div a
		const long  r =       2836;  // m mod a
		long        x_div_q;         // x divided by q
		long        x_mod_q;         // x modulo q
		long        x_new;           // New x value
		
		// RNG using integer arithmetic
		x_div_q = seed / q;
		x_mod_q = seed % q;
		x_new = (a * x_mod_q) - (r * x_div_q);
		if (x_new > 0)
			seed = x_new;
		else
			seed = x_new + m;
		
		// Return a random value between 0.0 and 1.0
		return ((double)seed/m);
	}

	// UNIFORM [a, b] RANDOM VARIATE GENERATOR
	// returns a psuedo-random variate from a uniform    
	// distribution with lower bound a and upper bound b.
	double uniform(double a, double b)
	{         
		if (a>b) throw MathException("RandomGenerator::uniform Argument Error: a > b"); 

		return(a+(b-a)*randf());
	}

	// RANDOM INTEGER GENERATOR
	// returns an integer equiprobably selected from the 
	// set of integers i, i+1, i+2, . . , n.                      
	int random(int i, int n)
	{ 
		if (i>n) throw MathException("RandomGenerator::random Argument Error: i > n"); 	

		n-=i; 
		n=int((n+1.0)*randf());
		return(i+n);
	}

	// EXPONENTIAL RANDOM VARIATE GENERATOR
	// returns a psuedo-random variate from a negative     
	// exponential distribution with mean x. 
	double expntl(double x)
	{                   
		return(-x*log(randf()));
	}

	// ERLANG RANDOM VARIATE GENERATOR
	// returns a psuedo-random variate from an erlang 
	// distribution with mean x and standard deviation s.

	double erlang(double x, double s)
	{ 
		if (s>x) throw MathException("RandomGenerator::erlang Argument Error: s > x"); 	

		int i,k; double z;
		z=x/s; 
		k=int(z*z);
		z=1.0; for (i=0; i<k; i++) z *= randf();
		return(-(x/k)*log(z));
	}

	// HYPEREXPONENTIAL RANDOM VARIATE GENERATOR
	// returns a psuedo-random variate from Morse's two-stage 
	// hyperexponential distribution with mean x and standard 
	// deviation s, s>x.  
	double hyperx(double x, double s)
	{ 
		if (s<=x) throw MathException("RandomGenerator::hyperx Argument Error: s not > x"); 	

		double cv,z,p; 
		cv=s/x; z=cv*cv; p=0.5*(1.0-sqrt((z-1.0)/(z+1.0)));
		z=(randf()>p)? (x/(1.0-p)):(x/p);
		return(-0.5*z*log(randf()));
	}

	// TRIANGULAR RANDOM VARIATE GENERATOR
	double triang(double a, double c, double b)
	// returns a psuedo-random variate from a triangular distribution 
	// with left and right being [a,b] and the mode being at point c 
	{
		double sample,point;
		point = (c-a)/(b-a);
		sample = uniform(0.0,1.0);
		if (sample <= point)
			return(sqrt(sample*(b-a)*(c-a)) + a);
		else 
			return(b - sqrt((1.0-sample)*(b-a)*(b-c)));
	}

	// NORMAL RANDOM VARIATE GENERATOR
	// returns a psuedo-random variate from a normal distribution 
	// with mean x and standard deviation s. 
	double normal(double x, double s)
	{     
		double v1,v2,w,z1; 
		static double z2=0.0;
		if (z2!=0.0)
	    {
			z1=z2; // use value from previous call 
			z2=0.0;
		}  
		else
		{
			do
			{
				v1=2.0*randf()-1.0; 
				v2=2.0*randf()-1.0; 
				w=v1*v1+v2*v2;
			}
			while (w>=1.0);
			w=sqrt((-2.0*log(w))/w); z1=v1*w; z2=v2*w;
		}
		return(x+z1*s);
	}

private:
	long seed;
};

SKA_LIB_DECLSPEC extern RandomGenerator random_generator;

#endif

