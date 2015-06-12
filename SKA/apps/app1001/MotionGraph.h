#ifndef MOTIONGRAPH_DOT_H
#define MOTIONGRAPH_DOT_H
// SKA configuration
#include <Core/SystemConfiguration.h>
// SKA modules
#include <Math/Vector3D.h>
#include <Math/Quaternion.h>
// local application
#include "DataInput.h"
#include "boost/graph/graph_traits.hpp"
#include "boost/graph/adjacency_list.hpp"
#include <iostream>
#include <algorithm>
#include <vector>
#include "AnimationControl.h"
using namespace boost;
using namespace std;

class MotionGraph
{	
private:
	vector<string> filenames;
	int vertexNumber;
	
public:
	// contains an entire motion sequence
	struct Frame
	{
		string vertexName;
		Vector3D root_position;
		std::vector<Quaternion> joints;
		int frame_number;
		string fileName;
	};
	struct GraphNode
	{
		Frame frame_data;
		//	string color;
	};
	// need to add GraphEdge to have edges tell where we are going
	struct GraphEdge
	{
		string StartFilename;
		string EndFilename;
		int StartFrame;
		int EndFrame;
	};
	//// The only way for me to access these things is through public:
	vector<Frame> frames;
	vector<vector<int> > transitionPoints;
	
	//use vecS to to iterate for more than plus one
	//	this is the real one used
	//typedef adjacency_list<listS, listS, directedS, GraphNode, GraphEdge> DirectedGraph;
	typedef adjacency_list<listS, vecS, directedS, GraphNode, GraphEdge> DirectedGraph;
	typedef graph_traits<DirectedGraph>::vertex_iterator vertex_iter;
	//this is used to find the next following vertex
	typedef graph_traits < DirectedGraph >::adjacency_iterator neighbor_iterator;
	DirectedGraph dgraph;
	vector<vector<Frame> > allFrames;
	
	////////////
	//MotionGraph(int x);
	MotionGraph(MotionDataSpecification& motion_data_specs);
	void graphToFile();
	void fileReader(string filename);
	void fileLoader(MotionDataSpecification& motion_data_specs);
	
	//used to set a specific file's verticies edges to each other linearly
	void setLinear(int tempFrameCount, int vertexNumber);
	// used to send data of the graph.
	DirectedGraph exportGraph();
	//TO EXPORT TO GRAPH VIZ FILE
	void outPutGraphViz();
	vector<string> NameVect;
	void selectColor();
};
#endif
