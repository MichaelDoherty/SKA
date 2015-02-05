#include "Connector.h"


std::vector<vector<int> > Connector(std::vector<MotionGraph::Frame> motion1, std::vector<MotionGraph::Frame> motion2)
{
	float comparisonMax = 12.0;  //Comparison value to be the max difference between two frame in the two motions
	float comparison;
	std::vector<node> unsortedComparisons; //Holds nodes of frame pairs with their comparison value
	for (unsigned int i = 0; i < motion1.size(); i += 1) //Loop through each frame of first passed motion
	//for (int i = 100; i <= 101; i++)
	{
		cout << "Looking at frame " << i << " of the first motion\n";
		for (unsigned int j = motion1.size(); j < motion2.size(); j += 1) //Loop through each frame of second passed motion
		//for (int j = 400; j <= 450; j+=20)
		{
			//if (j % 50 == 0)
				//cout << "Frame " << j << " of motion 2\n";
			comparison = 0;
			node inputNode;
			//Loop through each quaternion in frame 'i' of first passed motion
			//hard coded 20 into the loop because there should only be 20 joints but our code finds more than 20
			// joints past 20 are all empty and through of the program, when all is fixed we should be using the 
			//motion[i].joints.size() to find the number of joints 
			for (int k = 0; k < 20/*motion1[i].joints.size()*/; k++) 
			{
				// calculate the distance between joints by finding the difference in location
				Quaternion diffq = motion1[i].joints[k] - motion2[j].joints[k];
				comparison += diffq.magnitude();

			}
			if (j%100 == 0)
				cout << i << " " << j << " " << abs(comparison) << endl;
			//If the comparison is less than the comparisonMax value, store the transition frames and comparison value
			if (abs(comparison) < comparisonMax && comparison != 0 && j < motion2.size() - 30 && i < motion1.size() - 30
				&& i > 30 && j > 30)
			{
				inputNode.motion1FrameNum = i;
				inputNode.motion2FrameNum = j;
				inputNode.comparisonVal = comparison;
				unsortedComparisons.push_back(inputNode);
			}

		}
	}
	//sorting algorithm to make the list smallest to largest comparison values
	vector<node> sortedComparisons;
	while (unsortedComparisons.size() > 0)
	{
		int lowestCompValLocation = 0;
		for (unsigned int i = 1; i < unsortedComparisons.size(); i++)
		{
			if (unsortedComparisons[lowestCompValLocation].comparisonVal > unsortedComparisons[i].comparisonVal)
				lowestCompValLocation = i;
		}
		sortedComparisons.push_back(unsortedComparisons[lowestCompValLocation]);
		unsortedComparisons.erase(unsortedComparisons.begin() + lowestCompValLocation);
	}

	//separate if too close together from other already stored transition nodes
	vector<node> finalTransitionList;
	finalTransitionList.push_back(sortedComparisons[0]);
	sortedComparisons.erase(sortedComparisons.begin());
	while (sortedComparisons.size() > 0)
	{
		bool addToList = true;
		for (unsigned int i = 0; i < finalTransitionList.size(); i++)
		{
			//If the lowest comparison value in sortedComparisons frame 1 and 2 are within 10 frames of any motion already stored then don't add to final list
			if (sortedComparisons[0].motion1FrameNum < finalTransitionList[i].motion1FrameNum + 5 &&
				sortedComparisons[0].motion1FrameNum > finalTransitionList[i].motion1FrameNum - 5 &&
				sortedComparisons[0].motion2FrameNum < finalTransitionList[i].motion2FrameNum + 5 &&
				sortedComparisons[0].motion2FrameNum < finalTransitionList[i].motion2FrameNum - 5)
				addToList = false;
		}
		if (addToList)
			finalTransitionList.push_back(sortedComparisons[0]);
		sortedComparisons.erase(sortedComparisons.begin());
	}

	//return final transition list
	vector<vector<int> > returnList;
	vector<int> emptyList;
	for (unsigned int i = 0; i < finalTransitionList.size(); i++)
	{
		returnList.push_back(emptyList);
		returnList.at(i).push_back(finalTransitionList[i].motion1FrameNum);
		returnList.at(i).push_back(finalTransitionList[i].motion2FrameNum - motion1.size());
	}

	for (unsigned int i = 0; i < returnList.size(); i++)
	{
		cout << endl << "(" << returnList.at(i).at(0) << "," << returnList.at(i).at(1) << ")" << endl;
	}

	return returnList;
}
