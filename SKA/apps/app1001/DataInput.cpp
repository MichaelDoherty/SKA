//-----------------------------------------------------------------------------
// app1001 - Builds with SKA Version 4.0
//-----------------------------------------------------------------------------
// DataOnput.cpp
//  This class is used to convert all Euler angles from the original .bvh files to quaternions and store them into another file which will be read in by our motion graph class.
//  Quaternion conversion is done by calling functions from the Quaternion class in SKA
//  To those using this motion graph implementation I wish you the best of luck
//-----------------------------------------------------------------------------
#include "DataInput.h"

DataInput::DataInput(string fileName)
{
	//HARD CODED
	vector<string>fileNames;
	string temp;
	string FileName;
	cout << endl <<"How many files do you wish to load: ";
	cin>>fileCount;

	// create baseball vector
	baseballFiles.push_back("../../data/motion/BVH/Baseball_Swings/swing1.bvh");
	baseballFiles.push_back("../../data/motion/BVH/Baseball_Swings/swing2.bvh");
	baseballFiles.push_back("../../data/motion/BVH/Baseball_Swings/swing3.bvh");
	baseballFiles.push_back("../../data/motion/BVH/Baseball_Swings/swing4.bvh");
	baseballFiles.push_back("../../data/motion/BVH/Baseball_Swings/swing5.bvh");
	baseballFiles.push_back("../../data/motion/BVH/Baseball_Swings/swing6.bvh");
	baseballFiles.push_back("../../data/motion/BVH/Baseball_Swings/swing7.bvh");
	baseballFiles.push_back("../../data/motion/BVH/Baseball_Swings/swing8.bvh");
	baseballFiles.push_back("../../data/motion/BVH/Baseball_Swings/swing9.bvh");
	baseballFiles.push_back("../../data/motion/BVH/Baseball_Swings/swing10.bvh");
	baseballFiles.push_back("../../data/motion/BVH/Baseball_Swings/swing11.bvh");

	fileCount=baseballFiles.size();
	cout<<endl<<"Loading in "<< fileCount<<" files"<<endl;
	readFile("../../data/motion/BVH/Baseball_Swings/swing1.bvh", "testQuaternion0.bvh");
	readFile("../../data/motion/BVH/Baseball_Swings/swing2.bvh", "testQuaternion1.bvh");
}

void DataInput::readFile(string FileName, string outputName)
{
	//HARD CODED
        ifstream  data(FileName.c_str());// BVH_MOTION_FILE_PATHMOTIONS + FileName);
	ofstream output2;
	//output1.open(filename1);
	string filename2 = "../../data/motion/BVH/converted/" + outputName;
	cout << filename2 << endl;
	output2.open(filename2.c_str());
	std::string line;
	int lineNumber = 0;
	int FrameCount = 0;
	cout << FileName << endl;
	lineNumber = 0;
	Frame temp;

	// get us to the euler angles, skip rest of file
	while (!data.eof())
	{
		getline(data, line);

		// May need to change but currently 'M' is only the first character of the line in one place of the file.
		// This is two lines prior to euler angles in the document
		if (line[0] == 'M')
		{
			cout << endl << line << endl;
			//these two getlines() jump us to the first line of euler angles
			getline(data, line);
			cout << endl << line << endl;
			getline(data, line);
			cout << endl << line << endl;
			break;
		}
	}
	while (!data.eof())
	{
		cout << endl << " Frame: " << FrameCount << " created" << endl;
		temp.frameCount = FrameCount;
			//we start grabbing the euler angles here
				//Get the position for the root node
				data >> temp.root.PosX;
				data >> temp.root.PosY;
				data >> temp.root.PosZ;
				data >> temp.root.eulerAngle.z;
				data >> temp.root.eulerAngle.x;
				data >> temp.root.eulerAngle.y;
				Quaternion tempQuat;
				tempQuat.fromEuler(temp.root.eulerAngle);
				cout<< endl << tempQuat.w<<" "<<tempQuat.z<<" "<<tempQuat.x<<" "<<tempQuat.y<<endl;
				output2<<temp.root.PosX<<" "<< temp.root.PosY <<" "<< temp.root.PosZ<<" ";
				output2 << tempQuat.w<<" "<<tempQuat.z<<" "<<tempQuat.x<<" "<<tempQuat.y<<" ";

				// gather non-root joints euler angles in the frame
				for (int j = 0; j < 20; j++)
				{
					Node tempHolder;
						data >> tempHolder.eulerAngle.z;
						data >> tempHolder.eulerAngle.x;
						data >> tempHolder.eulerAngle.y;
						Quaternion tempQuat2;
						tempQuat2.fromEuler(tempHolder.eulerAngle);
						//tempHolder.quaternion=tempQuat;
						output2 << tempQuat2.w<<" "<<tempQuat2.z<<" "<<tempQuat2.x<<" "<<tempQuat2.y<<" ";
						tempHolder.nodeCount = j;
					temp.nodes.push_back(tempHolder);

				}
				output2<<endl;

				lineNumber++;
				motion.push_back(temp);


				FrameCount++;
				if(lineNumber>2000)
				{
					data.eof();
					output2.eof();
					break;

				}
			}

	//}
	cout << "HI" << endl;
}

void DataInput::printFrames()
{
	for (unsigned int i = 0; i < motion.size(); i++)
	{
		cout << endl << "frame " << motion.at(i).frameCount<<endl;
		cout<<"****************************************************";
		cout <<" root node "<< endl << " xposition: " << motion.at(i).root.PosX << " yposition: " << motion.at(i).root.PosY << " zposition: " << motion.at(i).root.PosZ << endl;
		cout << " quaternion w value: " << motion.at(i).root.quaternion.w <<endl << " quaternion x value: " << motion.at(i).root.quaternion.x  << endl<< " quaternion y value: " << motion.at(i).root.quaternion.y<< endl<< " quaternion z value: " << motion.at(i).root.quaternion.z <<endl;
		cout<<"****************************************************";

		int nodeCount = motion.at(i).nodes.size();
		for(int j=0;j<nodeCount;j++)
		{
			cout<<endl<<"Node "<<j<<endl;
			cout<<"****************************************************";
			cout << " quaternion w value: " << motion.at(i).nodes.at(j).quaternion.w << endl<< " quaternion x value: " << motion.at(i).nodes.at(j).quaternion.x <<endl <<" quaternion y value: " << motion.at(i).nodes.at(j).quaternion.y <<endl << " quaternion z value: " << motion.at(i).nodes.at(j).quaternion.z <<endl;
			cout<<"****************************************************";
		}
	}
}

void DataInput::WriteFrames()
{

	string filename2 = "../../data/motion/BVH/quaternion.bvh";
	ofstream output2;
	output2.open(filename2.c_str());
	for (unsigned int i = 0; i < motion.size(); i++)
	{

		output2<<FILENAME<<" ";
		output2 << motion.at(i).root.PosX << " "<< motion.at(i).root.PosY <<" "<< motion.at(i).root.PosZ;
		//system("pause");
		int nodeCount = motion.at(i).nodes.size();
		for(int j=0;j<nodeCount;j++)
		{

			output2 << " "<< motion.at(i).nodes.at(j).quaternion.w  << " "<< motion.at(i).nodes.at(j).quaternion.z << " "<< motion.at(i).nodes.at(j).quaternion.x << " " << motion.at(i).nodes.at(j).quaternion.y;

		}
		output2<<endl;
		cout<< endl<<" Wrote Frame " << i<< " to a file "<<endl;
	}
	output2.close();
}

void DataInput::setFileName(string FileName)
{
	FILENAME =FileName;
}
