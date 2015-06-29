//-----------------------------------------------------------------------------
// app1001 - Builds with SKA Version 3.1 - Sept 01, 2012 - Michael Doherty
//-----------------------------------------------------------------------------
// BVH2Quaternion.h
// Based on the DataInput code produced by COMP 259 students, fall 2014.
//
// Converts the Euler angles in a BVH motion file to quaternions.
// Stores results in a new file. New file only contains the frame data, not the BVH skeleton.

#ifndef BVH2QUATERNION_DOT_H
#define BVH2QUATERNION_DOT_H
#include <Core/SystemConfiguration.h>
#include <string>
using namespace std;

// File names are assumed to be relative to search paths registered with the DataManager.
void convertBVH2Quaternion(string& inputfile, string& outputfile);

#endif