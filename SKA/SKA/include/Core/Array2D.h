//-----------------------------------------------------------------------------
// Array2D.h
//	 A template class implementing a 2D array stored in column-major order.
//-----------------------------------------------------------------------------
// This software is part of the Skeleton Animation Toolkit (SKA) developed by 
// at the University of the Pacific, under the guidance of Michael Doherty.
// For information please contact mdoherty@pacific.edu.
//-----------------------------------------------------------------------------
// This is open software. You are free to use it as you see fit.
// The University of the Pacific and identified authors should be
// credited for any significant use, particularly if used for commercial 
// projects or academic research publications.
//-----------------------------------------------------------------------------
// Version 1.0 - January 25, 2012 - Michael Doherty
//-----------------------------------------------------------------------------
#ifndef ARRAY2D_DOT_H
#define ARRAY2D_DOT_H
#include "Core/SystemConfiguration.h"
#include <fstream>
using namespace std;
#include "Core/BasicException.h"

class SKA_LIB_DECLSPEC Array2D_Exception : public BasicException
{
public:
	Array2D_Exception() : BasicException(string("undefined exception")) { }
	Array2D_Exception(const string& _msg) : BasicException(_msg) { }
};

template<class T> 
class SKA_LIB_DECLSPEC Array2D {

private: 
	T* m;
	int columns; 
	int rows;
	
public:

	// default constructor (NULL array)
	Array2D()
		: m(NULL)
	{
		columns = rows = 0;
	}

	// general constructor
	Array2D(int _rows, int _columns)
		: m(NULL)
	{
		resize(_rows, _columns);
	}
	
	// copy constructor
	Array2D(const Array2D& _a)
	{
		clear();
		resize(_a.getColumns(), _a.getRows());
		memcpy(m, _a.m, columns*rows*sizeof(T));
	}

	// assignment operator
	Array2D& operator= (const Array2D& _a) 
	{
		clear();
		resize(_a.rows, _a.columns);
		memcpy(m, _a.m, columns*rows*sizeof(T));
		return *this;
	}

	// destructor
	virtual ~Array2D()
	{
		clear();
	}

	// reset to a NULL array
	void clear()
	{
		if (m != NULL) delete [] m;
		m = NULL;
		columns = rows = 0;
	}

	// resize array - sets all elements to 0
	void resize(int _rows, int _columns)
	{
		clear();
		columns =_columns;
		rows =_rows;
		m = new T[columns*rows];
		memset(m, 0, columns*rows*sizeof(T));
	}

	// accessors

	T& get(int r, int c){
		boundsCheck(r,c);
		return m[rows*c+r];
	}

	void set(int r, int c, T val){
		boundsCheck(r,c);
		m[rows*c+r]=val;
	}

	T& element(int r, int c) 
	{ 
		boundsCheck(r,c);
		return m[rows*c+r]; 
	}

	int getColumns() const { return columns; }
	
	int getRows() const { return rows;	}

	T* getColumnPtr(int c)
	{
		boundsCheck(0,c);
		return &(m[rows*c]);
	}

	void MultipleColumnByScalar(int col, float scale)
	{
		boundsCheck(0,col);
		for (int r=0; r<rows; r++)
			set(r,col,get(r,col)*scale);
	}

	void MultiplyElements(Array2D<double>& x)
	{
		if ((x.columns != columns) || (x.rows != rows)) 
			throw Array2D_Exception(string("INCOMPATIBLE ARRAY TO Array2D.MultiplyElements"));
		for (int r=0; r<rows; r++)
			for (int c=0; c<columns; c++)
				set(r,c,get(r,c)*x.get(r,c));
	}

	void AddElements(Array2D<double>& x)
	{
		if ((x.columns != columns) || (x.rows != rows)) 
			throw Array2D_Exception(string("INCOMPATIBLE ARRAY TO Array2D.AddElements"));
		for (int r=0; r<rows; r++)
			for (int c=0; c<columns; c++)
				set(r,c,get(r,c)+x.get(r,c));
	}

	// This should probably be an operator<<,
	// but the template is giving the linker problems.
	void dump(ostream& out)
	{
		int r, c;
		for (r=0; r<rows; r++)
		{
			out << r << ", ";
			for (c=0; c<columns; c++)
			{
				if (c > 0) out << ", ";
				out << get(r,c);
			}
			out << endl;
		}
	}

private:
	void boundsCheck(int r, int c)
	{
		if ((r<0) || (r>=rows) || (c<0) || (c>=columns))
			throw Array2D_Exception(string("Array2D bounds error"));
	}

};

#endif



