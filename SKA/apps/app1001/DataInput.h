/*
	This class is used to convert all Euler angles from the original .bvh files to quaternions and store them into another file which will be read in by our motion graph class.
	
	Quaternion conversion is done by calling functions from the Quaternion class in SKA
	
	To those using this motion graph implementation I wish you the best of luck

*/

#ifndef DATAINPUT_H
#define DATAINPUT_H
#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <string>
#include <fstream> 
#include <iomanip> 
#include <Math/Vector3D.h>
#include <Math/Quaternion.h>
#include "AppConfig.h"

using namespace std;

struct RootNode
{
	Quaternion quaternion;
	double PosX, PosY, PosZ;
	Vector3D eulerAngle;
};

struct Node
{
	Quaternion quaternion;	
	Vector3D eulerAngle;
	// node number in the vector of nodes starts at 1
	int nodeCount;
};

//contains an entire motion sequence
struct Frame
{
	RootNode root;
	std::vector<Node> nodes;
	// Frame number in the vector of Frames starts at 1
	int frameCount;
};

class DataInput
{	
	private:
		std::vector<Frame> motion;
		vector<string> baseballFiles;
		string FILENAME;
		int fileCount;

	public:
		DataInput(string fileName);
		void readFile(string FileName, string outputName);
		void WriteFrames();
		//Node getNode();
		void setFileName(string FileName);
		//RootNode getRoot();
		void setRoot(double PosX, double PosY, double PosZ, double eulerZ, double eulerY, double eulerX);
		void printFrames();
};

#endif