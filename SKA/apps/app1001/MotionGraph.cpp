#include "MotionGraph.h"
#include <boost/graph/graphviz.hpp>
#include <boost/graph/iteration_macros.hpp>
#include <Core/Utilities.h>
#include "Connector.h"

// so that in motion graph controller when it creates its motion graph , it doesn't create a whole new one. It keeps it blank. but for one we need to updload the files we update with MotionGraph a(1); or any int
MotionGraph::MotionGraph()
{
	//fileLoader();


}
// can be used if you wanted to specify number of files to load
MotionGraph::MotionGraph(int x)
{
	fileLoader();


}

//This is called by fileLoader and inputs data from a single file as a motion into the motion graph
void MotionGraph::fileReader(string filename)
{
	long tempFrameCount = 0;

	ifstream data(filename.c_str());
	std::string line;
	Frame tmp_frame;
	Quaternion tmp_quat;

	while (!data.eof())
	{
		//used to store filename it can be used to identify individual motions sequences in the motion graph
		tmp_frame.fileName = filename;
		NameVect.push_back(filename + toString(tempFrameCount));

		getline(data, line);
		data >> tmp_frame.root_position.x;
		data >> tmp_frame.root_position.y;
		data >> tmp_frame.root_position.z;
		// gather non-root joints euler angles in the frame
		for (int j = 0; j < 20; j++)
		{
			data >> tmp_quat.w;
			data >> tmp_quat.z;
			data >> tmp_quat.x;
			data >> tmp_quat.y;
			tmp_frame.joints.push_back(tmp_quat);
		}
		tmp_frame.frame_number = vertexNumber;
		//push each individual frame onto a vector of frames
		frames.push_back(tmp_frame);
		tmp_frame.joints.clear();
		vertexNumber++;
		tempFrameCount++;
	}
	//int size = frames.size();
	
	//	cout << endl << "vertexNumber is " << vertexNumber << endl;
	//cout << endl << "num frames is " << size << endl;

	//Now we can initialize our graph using iterators from our above vector
	unsigned int i;

	//if we have already added to that graph
	// added empty nodes to the motion graph
	for (int k = 0; k < tempFrameCount; k++)
	{
		add_vertex(dgraph);
	}

	//initializes graph vertex iterators (vertex and nodes refer to the same thing)
	pair<vertex_iter, vertex_iter> vp;
	// iterate through motion graph and make each empty node equal to a specific element of our frame vector
	// basicaly each node in the motion graph refers to a specific frame read in from a motion file
	for (vp = vertices(dgraph), i = 0; vp.first != vp.second; ++vp.first, i++) {
		DirectedGraph::vertex_descriptor v = *(vp.first);
		// code to set a node equal to element of the frame vector 
		dgraph[v].frame_data = frames[i];
		if (i < 0)
		{
			cout << frames[i].frame_number << " = " << dgraph[v].frame_data.frame_number << endl;
			cout << "frame " << i << " root position = " << frames[i].root_position << endl;
			cout << "frame " << i << " root joint = " << frames[i].joints[0] << endl;
			cout << "frame " << i << " joint 5 = " << frames[i].joints[5] << endl;
		}
	}

	// prints out the contents of the graph
	//	cout << "Checking contents of the graph: " << endl;
	for (vp = vertices(dgraph); vp.first != vp.second; ++vp.first)
	{
		DirectedGraph::vertex_descriptor v = *vp.first;
		i = dgraph[v].frame_data.frame_number;
		if (i < 0)
		{
			cout << "frame " << i << " root position = " << dgraph[v].frame_data.root_position << endl;
			cout << "frame " << i << " root joint = " << dgraph[v].frame_data.joints[0] << endl;
			cout << "frame " << i << " joint 5 = " << dgraph[v].frame_data.joints[5] << endl;
		}
	}

	//cout << "count " << vertexNumber << endl;
	// sets edges across all the new verticies
	setLinear(tempFrameCount, vertexNumber);
	//cout << endl << endl;
	//system("pause");
	allFrames.push_back(frames);

	cout << "All Frame size: "<<allFrames.size()<<endl;
	cout << "All Frame at " << allFrames.size()-1 << " size: " << allFrames.at(allFrames.size()-1).size() << endl;
}

// handles calling files
void MotionGraph::fileLoader()
{	// HARD CODED NEED FIX

	//To test motion graph builder we will be loading in a vector of filenames related to baseball MoCAP
	vector<string>baseball;
	
	for (int i = 0; i < 2; i++)
	{
		char temp[255];
		//sprintf(temp, "../../data/motion/BVH/Baseball_Swings/swing%d.bvh", i);
		sprintf(temp, "../../data/motion/BVH/converted/testQuaternion%d.bvh", i);
		// need to split this 
		cout << endl << temp << endl;
		baseball.push_back(temp);
	}

	//int size = baseball.size();
	fileReader(baseball.at(0));
	fileReader(baseball.at(1));
	//prints out graph 
	outPutGraphViz();
	// transition points is a vector of integers which basically tells which vertex in motion1 connects to in motion 2
	transitionPoints = Connector(allFrames.at(0), allFrames.at(1));
}




MotionGraph::DirectedGraph MotionGraph::exportGraph()
{
	return(dgraph);
}
// only can be used if verticies are stored in a vecS
void MotionGraph::setLinear(int tempFrameCount, int vertexNumber)
{
	//the frame to start iterating from
	int startFrame = vertexNumber - tempFrameCount;
	//cout << "start frame start " << startFrame << endl;
	pair<vertex_iter, vertex_iter> vp;
	int i = 0;
	for (vp = vertices(dgraph), i = 0; vp.first + startFrame != vp.second; ++vp.first, i++)
	{
		DirectedGraph::vertex_descriptor v = *(vp.first + startFrame);
		DirectedGraph::vertex_descriptor v1 = *(vp.first + (startFrame - 1));

		if (i > 0)
		{
			// add edge from  the first to the next
			add_edge(v1, v, dgraph);
			//now lets add the info to the edge
			DirectedGraph::edge_descriptor e = *out_edges(v1, dgraph).first;
			// changing the first edge. its not a specific selector. like if there are 2 edges, this just chooses the first one.
			///////////////*************************Debug code to check edge iteration*********////////////
			//if (startFrame + i > vertexNumber - 2)
			//{
				/*
				cout << "vp second" << *vp.second << endl;
				cout << "vp first" << *vp.first << endl;
				cout << "vp first" << *vp.first+startFrame << endl;
				cout << "frame: " << startFrame + i << endl;
				cout << dgraph[v1].frame_data.frame_number << endl;
				cout << dgraph[v].frame_data.frame_number << endl;
				*/
			//}
			//cout << dgraph[v1].frame_data.frame_number << endl;
			//cout << dgraph[v].frame_data.frame_number << endl;
			
			/////////////////////****************** end of edge iteration debug*************/////////////////
			
			string name = dgraph[v1].frame_data.frame_number + "-" + dgraph[v].frame_data.frame_number;
			dgraph[e].StartFrame = dgraph[v1].frame_data.frame_number;
			dgraph[e].EndFrame = dgraph[v].frame_data.frame_number;


			//std::pair<neighbor_iterator, neighbor_iterator> neighbors =
				boost::adjacent_vertices(v1, dgraph);
			//iterate through all neighbors---MORE DEBUG CODE
			//for (; neighbors.first != neighbors.second; ++neighbors.first)
			//{
				//std::cout << "neighbors for  " << dgraph[v1].frame_data.frame_number << " ";
				//cout << dgraph[*neighbors.first].frame_data.frame_number << endl;
			//}
			
			/// end of debug code
		}

	}
}

// output graph to file-- use graph viz to check output(graphViz is a seperate program)
void MotionGraph::outPutGraphViz()
{
	//dp.property("node_id", get(&GraphNode::frame_data, dgraph));
	//boost::dynamic_properties dp;
	//boost::property_map<DirectedGraph, boost::vertex_name_t>::type name = boost::get(boost::vertex_name, dgraph);
	//dp.property("node_id", name);
	//auto score = boost::get(&GraphNode::color, dgraph);
	//dp.property("score", score);

	string google = "GRAPHOUTPUT.dot";
	std::ofstream dotfile(google.c_str());

	// 120101::DOHERTY next line is causing some extensive template error, maybe from boost?
	write_graphviz(dotfile, dgraph, make_label_writer(&NameVect[0]));

}
