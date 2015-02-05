#include "testclass.h"
#include "MotionGraph.h"

testclass::testclass(MotionGraphController* mgc, vector<vector<int> > TransitionPoints)
: StartSeq(string("swing1.bvh")), startFrame(0)
{
	/*
	MotionSequence *MS;
	MotionSequence *MS2;
	MotionSequence *MS3;
	MotionSequence *MS4;
	
	//temp vertex target list

	MotionGraphController::vertexTargets temp;
	MS = mgc->returnMotionSequenceContainerFromID("swing5.bvh").MS;
	temp.SeqID = "swing5.bvh";
	temp.SeqID2 = "swing6.bvh";
	temp.FrameNumber = MS->numFrames();
	temp.FrameNumber2 = 0;
	path.push_back(temp);
	//second transition
	MS2 = mgc->returnMotionSequenceContainerFromID("swing6.bvh").MS;
	temp.SeqID = "swing6.bvh";
	temp.SeqID2 = "swing5.bvh";
	temp.FrameNumber = MS2->numFrames();
	temp.FrameNumber2 = 0;
	path.push_back(temp);

	MS3 = mgc->returnMotionSequenceContainerFromID("swing5.bvh").MS;
	temp.SeqID = "swing5.bvh";
	temp.SeqID2 = "swing7.bvh";
	temp.FrameNumber = MS3->numFrames();
	temp.FrameNumber2 = 0;
	path.push_back(temp);

	MS4 = mgc->returnMotionSequenceContainerFromID("swing7.bvh").MS;
	temp.SeqID = "swing7.bvh";
	temp.SeqID2 = "swing8.bvh";
	temp.FrameNumber = MS4->numFrames();
	temp.FrameNumber2 = 0;
	path.push_back(temp);

	MS4 = mgc->returnMotionSequenceContainerFromID("swing8.bvh").MS;
	temp.SeqID = "swing8.bvh";
	temp.SeqID2 = "swing9.bvh";
	temp.FrameNumber = MS4->numFrames();
	temp.FrameNumber2 = 0;
	path.push_back(temp);

	MS4 = mgc->returnMotionSequenceContainerFromID("swing9.bvh").MS;
	temp.SeqID = "swing9.bvh";
	temp.SeqID2 = "swing10.bvh";
	temp.FrameNumber = MS4->numFrames();
	temp.FrameNumber2 = 0;
	path.push_back(temp);

	MS4 = mgc->returnMotionSequenceContainerFromID("swing10.bvh").MS;
	temp.SeqID = "swing10.bvh";
	temp.SeqID2 = "swing11.bvh";
	temp.FrameNumber = MS4->numFrames();
	temp.FrameNumber2 = 0;
	path.push_back(temp);



	mgc->setPath(StartSeq, startFrame, path);
	cout << "should start playing" << endl;*/

	//MotionSequence *MS;
	//MotionSequence *MS2;
	//MotionSequence *MS3;
	//MotionSequence *MS4;

	//temp vertex target list

	MotionGraphController::vertexTargets temp;
	//MS = mgc->returnMotionSequenceContainerFromID("swing1.bvh").MS;
	//MS2 = mgc->returnMotionSequenceContainerFromID("swing2.bvh").MS;
	for (unsigned long i = 0; i < TransitionPoints.size(); i++)
	{
		temp.SeqID = "swing1.bvh";
		temp.SeqID2 = "swing2.bvh";
		temp.FrameNumber = TransitionPoints[i][0];// MS->numFrames();
		temp.FrameNumber2 = TransitionPoints[i][1];// 0;
		path.push_back(temp);
		/*temp.SeqID = "swing2.bvh";
		temp.SeqID2 = "swing1.bvh";
		temp.FrameNumber = MS2->numFrames();;// MS->numFrames();
		temp.FrameNumber2 = 0;// 0;
		path.push_back(temp);*/
	}
	//second transition
	//to continue playing until the end of the file
	/*
	MS = mgc->returnMotionSequenceContainerFromID("swing7.bvh").MS;
	temp.SeqID = "swing7.bvh";
	temp.SeqID2 = "swing5.bvh";
	temp.FrameNumber =  MS->numFrames();
	temp.FrameNumber2 = 0;// 0;
	path.push_back(temp);
	*/
	mgc->setPath(StartSeq, startFrame, path);
	cout << "should start playing" << endl;
}
