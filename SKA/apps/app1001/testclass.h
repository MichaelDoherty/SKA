#ifndef TESTCLASS_DOT_H
#define TESTCLASS_DOT_H

#include "MotionGraph.h"
#include <Core/SystemConfiguration.h>
#include <Animation/MotionController.h>
#include <Animation/MotionSequence.h>
//#include <Windows.h>
#include <vector>
#include "AppConfig.h"
#include <boost/filesystem.hpp>
#include "boost/filesystem/path.hpp"
#include "DataManagement/DataManager.h"
#include "DataManagement/DataManagementException.h"
#include <Animation/AnimationException.h>
#include "MotionGraphController.h"
#include "MotionGraph.h"
using namespace std;

class testclass
{
public:
	testclass(MotionGraphController* mgc, vector<vector<int> > TransitionPoints);
	list<MotionGraphController::vertexTargets> path;
	MotionGraphController* mgc;
	string StartSeq;
	int startFrame;
};


#endif
