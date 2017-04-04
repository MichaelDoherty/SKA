//-----------------------------------------------------------------------------
// MotionDescriptors project - Builds with SKA Version 4.0
//-----------------------------------------------------------------------------
// DataRecorder.h
//    DataRecorder class for storing application specific data and
//    recording it in a CSV file.
//-----------------------------------------------------------------------------
#ifndef DATARECORDER_DOT_H
#define DATARECORDER_DOT_H

#include <fstream>
#include <vector>
using namespace std;

class DataRecorder
{
public:
	DataRecorder()
	{ }

	virtual ~DataRecorder()
	{ }

	void record(float _time, int _frame, float _abduction, float _flexion, float _extension)
	{
		DataFrame df;
		df.time = _time;
		df.frame = _frame;
		df.abduction_valid = df.flexion_valid = df.extension_valid = true;
		df.abduction = _abduction;
		df.flexion = _flexion;
		df.extension = _extension;
	}

	void recordNoData(float _time, int _frame)
	{
		DataFrame df;
		df.time = _time;
		df.frame = _frame;
		data.push_back(df);
	}

	void recordAbduction(float _time, int _frame, float _abduction)
	{
		DataFrame df;
		df.time = _time;
		df.frame = _frame;
		df.abduction_valid = true;
		df.abduction = _abduction;
		data.push_back(df);
	}

	void recordFlexion(float _time, int _frame, float _flexion)
	{
		DataFrame df;
		df.time = _time;
		df.frame = _frame;
		df.flexion_valid = true;
		df.flexion = _flexion;
		data.push_back(df);
	}

	void recordExtension(float _time, int _frame, float _extension)
	{
		DataFrame df;
		df.time = _time;
		df.frame = _frame;
		df.extension_valid = true;
		df.extension = _extension;
		data.push_back(df);
	}

	float maxAbduction()
	{
		float max = 0.0f;
		for (unsigned short i = 0; i < data.size(); i++)
			if (data[i].abduction_valid)
				if (data[i].abduction > max) max = data[i].abduction;
		return max;
	}

	float maxFlexion()
	{
		float max = 0.0f;
		for (unsigned short i = 0; i < data.size(); i++)
			if (data[i].flexion_valid)
				if (data[i].flexion > max) max = data[i].flexion;
		return max;
	}

	float maxExtension()
	{
		float max = 0.0f;
		for (unsigned short i = 0; i < data.size(); i++)
			if (data[i].extension_valid)
				if (data[i].extension > max) max = data[i].extension;
		return max;
	}

	void writeToFile(string _filename)
	{
		ofstream ofs;
		ofs.open(_filename.c_str());
		ofs << "time, frame, abduction, flexion, extension";
		ofs << endl;
		for (unsigned int i = 0; i < data.size(); i++)
		{
			DataFrame df = data[i];
			ofs << df.time;
			ofs << ", ";
			ofs << df.frame;
			ofs << ", ";
			if (df.abduction_valid) ofs << df.abduction;
			else ofs << "INVALID";
			ofs << ", ";
			if (df.flexion_valid) ofs << df.flexion;
			else ofs << "INVALID";
			ofs << ", ";
			if (df.extension_valid) ofs << df.extension;
			else ofs << "INVALID";
			ofs << endl;
		}
		ofs.close();
	}

	void erase() { data.clear();  }

private:
	struct DataFrame
	{
		float time;
		int frame;
		bool abduction_valid, flexion_valid, extension_valid;
		float abduction, flexion, extension;
		DataFrame() : time(0.0f), frame(0),
			abduction_valid(false), flexion_valid(false), extension_valid(false),
			abduction(0.0f), flexion(0.0f), extension(0.0f)
		{ }
	};
	vector<DataFrame> data;
};

#endif

