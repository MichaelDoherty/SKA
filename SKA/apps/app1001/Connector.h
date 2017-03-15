//-----------------------------------------------------------------------------
// app1001 - Builds with SKA Version 4.0
//-----------------------------------------------------------------------------
// Connector.h
//    node class and Connector function
//-----------------------------------------------------------------------------
#include <Core/SystemConfiguration.h>
#include <vector>
#include <Math/Vector3D.h>
#include <Math/Quaternion.h>
#include "MotionGraph.h"

struct node {
	int motion1FrameNum;
	int motion2FrameNum;
	float comparisonVal;
};

std::vector<vector<int> > Connector(std::vector<MotionGraph::Frame> motion1, std::vector<MotionGraph::Frame> motion2);
