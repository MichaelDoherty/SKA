//-----------------------------------------------------------------------------
// Array2D.h
//	 A template class implementing a 2D array stored in column-major order.
//-----------------------------------------------------------------------------
// This software is part of the Skeleton Animation Toolkit (SKA) developed 
// at the University of the Pacific, under the guidance of Michael Doherty.
// For information please contact mdoherty@pacific.edu.
//-----------------------------------------------------------------------------
// This is open software. You are free to use it as you see fit.
// The University of the Pacific and identified authors would appreciate
// being credited for any significant use, particularly if used for
// commercial projects or academic research publications.
//-----------------------------------------------------------------------------

#ifndef ARRAY2D_DOT_H
#define ARRAY2D_DOT_H
#include <Core/SystemConfiguration.h>
#include <fstream>
using namespace std;
#include <Core/BasicException.h>

class SKA_LIB_DECLSPEC Array2D_Exception : public BasicException
{
public:
	Array2D_Exception() : BasicException("Unspecified Array2D exception") { }
	Array2D_Exception(const char* _msg) : BasicException(_msg) { }
	Array2D_Exception(const Array2D_Exception& _other) : BasicException(_other) { }
};

template<class T> 
class SKA_LIB_DECLSPEC Array2D {

private: 
	T* m;
	long columns; 
	long rows;
	
public:

	// default constructor (NULL array)
	Array2D()
		: m(NULL)
	{
		columns = rows = 0;
	}

	// general constructor
	Array2D(long _rows, long _columns)
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
	void resize(long _rows, long _columns)
	{
		clear();
		columns =_columns;
		rows =_rows;
		m = new T[columns*rows];
		memset(m, 0, columns*rows*sizeof(T));
	}

	// add additional columns at end, preserving data in existing columns
	// new columns are initialized to zero
	void addColumns(long _num_new_cols)
	{
		long new_columns = columns + _num_new_cols;
		T* tmp = m;
		m = new T[new_columns*rows*sizeof(T)];
		memcpy(m, tmp, columns*rows*sizeof(T));
		delete [] tmp;
		memset(&(m[columns*rows]), 0, _num_new_cols*rows*sizeof(T));
		columns = new_columns;
	}

	// loadRawData - assumes _data points at _rows*_columns*sizeof(T) bytes 
	// that are properly formatted as an array of T
	void loadRawData(long _rows, long _columns, void* _data)
	{
		resize(_rows, _columns);
		memcpy(m, _data, columns*rows*sizeof(T));
	}

	// accessors 

	T& get(long r, long c){
		boundsCheck(r,c);
		return m[rows*c+r];
	}

	void set(long r, long c, T val){
		boundsCheck(r,c);
		m[rows*c+r]=val;
	}

	T& element(long r, long c) 
	{ 
		boundsCheck(r,c);
		return m[rows*c+r]; 
	}

	long getColumns() const { return columns; }
	
	long getRows() const { return rows;	}

	T* getColumnPtr(long c)
	{
		boundsCheck(0,c);
		return &(m[rows*c]);
	}

	void swapColumns(long col1, long col2)
	{
		boundsCheck(0,col1);
		boundsCheck(0,col2);
		T* p1 = getColumnPtr(col1);
		T* p2 = getColumnPtr(col2);
		T* tmp = new T[rows];
		memcpy(tmp, p1, rows*sizeof(T));
		memcpy(p1, p2, rows*sizeof(T));
		memcpy(p2, tmp, rows*sizeof(T));
		delete [] tmp;
	}

	// this will change the number of columns
	// and the contents of all columns > col.
	void removeColumn(long col)
	{
		boundsCheck(0,col);
		T* m2 = new T[(columns-1)*rows];
		long c1=0, c2=0;
		for (c1=0; c1<columns; c1++)
		{
			if (c1 != col) 
			{
				memcpy(&(m2[rows*c2]), &(m[rows*c1]), rows*sizeof(T));
				c2++;
			}
		}
		delete [] m;
		m = m2;
		columns--;
	}

	void MultiplyColumnByScalar(long col, float scale)
	{
		boundsCheck(0,col);
		for (long r=0; r<rows; r++)
			set(r,col,get(r,col)*scale);
	}

	void MultiplyElements(Array2D<double>& x)
	{
		if ((x.columns != columns) || (x.rows != rows)) 
			throw Array2D_Exception("INCOMPATIBLE ARRAY TO Array2D.MultiplyElements");
		for (long r=0; r<rows; r++)
			for (long c=0; c<columns; c++)
				set(r,c,get(r,c)*x.get(r,c));
	}

	void AddElements(Array2D<double>& x)
	{
		if ((x.columns != columns) || (x.rows != rows)) 
			throw Array2D_Exception("INCOMPATIBLE ARRAY TO Array2D.AddElements");
		for (long r=0; r<rows; r++)
			for (long c=0; c<columns; c++)
				set(r,c,get(r,c)+x.get(r,c));
	}

	// This should probably be an operator<<,
	// but the template is giving the linker problems.
	void dump(ostream& out)
	{
		long r, c;
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
	void boundsCheck(long r, long c)
	{
		if ((r<0) || (r>=rows) || (c<0) || (c>=columns))
			throw Array2D_Exception("Array2D bounds error");
	}

};

#endif



