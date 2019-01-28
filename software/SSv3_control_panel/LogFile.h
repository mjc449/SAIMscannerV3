////////////////////////////////////////////////////////////////////////////////
//Description: Header only log file library that opens, adds to, and closes a
//				simple .txt file for logging events during program execution.
//				The motivation for this class is to provide a thread-safe
//				and exception-safe log file object common to all instances of
//				the program.  Constructors and assignments that fail to open
//              the file stream as well as methods that attempt to operate on
//              a stream that has not been opened will throw stream_closed.
//
//Throws: stream_closed(m_fileName) to id the offending object
//
//Auth: Marshall Colville mjc449@cornell.edu
//
//Copyright: 2017/03/18
////////////////////////////////////////////////////////////////////////////////

#ifndef LOGFILE_H
#define LOGFILE_H

#include <fstream>
#include <vector>
#include <iterator>
#include <string>
#include <complex>

#ifdef _USE_TBB
#include <tbb/tbb.h>
#else
#include <mutex>
#endif


//This is the stream closed exception class used by the log file implementation
class stream_closed : public std::exception
{
	//The file name
	std::string fileName;

public:
	//No-argument constructor;
	stream_closed() {
		fileName = "File causing exception was unspecified";
	}

	//Specify the name of the offending file
	stream_closed(std::string& file) :
		fileName(file) {};

	//Override of the .what() method to provide the name of the offending object
	virtual const char* what() const throw() {
		std::string outputString{ "LogFile exception: " + fileName };
		return outputString.c_str();
	}
};

//The class declarations and definitions for the log file are all included here
//Most methods are implemented as function templates
//The default template is deleted, and limited specializations are provided
//Additional specializations can be added as required
class CLogFile
{
	//Macro to switch between std library and tbb depending on situation
	#ifdef _USE_TBB
		tbb::spin_mutex m_logFileMutex;
	#else
		std::mutex m_logFileMutex;
	#endif

	//Log file name
	std::string m_fileName;
	//Log file path
	std::string m_filePath;
	//File stream handle
	std::ofstream m_fileHandle;

	//Test if the stream is open and throw stream_closed if not
	void StreamTest() {
		if (m_fileHandle.is_open() != true) {
			stream_closed ex(m_fileName);
			throw ex;
		}
	}

public:
	//Explicitly delete no-arg constructor
	CLogFile() = delete;

	//Log file constructor
	//@filePath = path to directory to create the file in
	//@fileName = name of the log file
	CLogFile(const std::string& filePath, const std::string& fileName = "") :
		m_filePath(filePath),
		m_fileName(fileName)
	{
		//If the filePath isn't given return immediately so the stream is not
		//opened.  This prevents creating an unnecessary file when the user 
		//wants to declare an object without initializing it's path
		if (m_filePath.empty())
			return;
		//If no file name is given assign it a default name
		if (m_fileName.empty())
			m_fileName = "default_out.txt";
		//Check that the path string ends in '/' or '\'
		if (m_filePath.back() != ('\\' || '/'))
			m_filePath.push_back('/');
		//Check that the file name includes an extension
		//If not give it .txt
		if (m_fileName.find_first_of('.', 0) == std::string::npos)
			m_fileName += ".txt";
		//Open the stream
		m_fileHandle.open(m_filePath + m_fileName);
		//Check that the stream is open, throw if not
		if (m_fileHandle.is_open() != true)
		{
			stream_closed ex(m_fileName + std::string(" could not be opened"));
			throw ex;
		}
	}

	//Copy constructor
	CLogFile(const CLogFile& obj) = delete;

	//Move constructor
	CLogFile(CLogFile&& obj) :
		m_filePath(std::move(obj.m_filePath)),
		m_fileName(std::move(obj.m_fileName))
	{
		m_fileHandle.open(m_filePath + m_fileName);
		if (m_fileHandle.is_open() != true)
		{
			stream_closed ex(std::string("move failed on ") + m_fileName);
			throw ex;
		}
	}

	//Copy assignment operator
	CLogFile& operator =(const CLogFile& obj) = delete;

	//Move assignment operator
	CLogFile& CLogFile::operator =(CLogFile&& obj)
	{
		//Check and close an open stream
		if (m_fileHandle.is_open())
			m_fileHandle.close();
		//Move the path and name variables
		m_filePath = std::move(obj.m_filePath);
		m_fileName = std::move(obj.m_fileName);
		//Open the new stream, obj's destructor makes sure its stream is closed
		m_fileHandle.open(m_filePath + m_fileName);
		if (m_fileHandle.is_open() != true)
		{
			stream_closed ex(std::string("assignment failed on ") + m_fileName);
			throw ex;
		}
		return *this;
	}

	//Destructor closes the file, ensuring release of control
	~CLogFile()
	{
		m_logFileMutex.unlock();
		if (m_fileHandle.is_open())
			m_fileHandle.close();
	}

	//Check if the stream is open for writing
	//Returns true if open, false otherwise
	bool IsOpen()
	{
		return m_fileHandle.is_open();
	}

	//Add a line to the log file
	//@line = to be added on a new line
	template<typename T>
	void AddLine(const T& line)
	{
		StreamTest();
		m_fileHandle << line << std::endl;
	}

	//Specialization to prevent writing pointers without length specification
	template<typename T>
	void AddLine(T*) = delete;

	//Add a null-terminated char array on a single line
	//@line = null-terminated char*
	void AddLine(char* line)
	{
		StreamTest();
		std::string str{ line };
		m_fileHandle << str << std::endl;
	}

	//Add a string-literal on a single line
	//@line = string-literal
	void AddLine(const char* line)
	{
		StreamTest();
		std::string text{ line };
		m_fileHandle << text << std::endl;
	}

	//Add an array to the file on a single line
	//separated by spaces
	//@m = number of elements to add
	//@values = pointer to array
	template<typename T>
	void AddLine(const int& m, const T* values)
	{
		StreamTest();
		for (int i = 0; i < m - 1; i++)
			m_fileHandle << values[i] << " ";
		m_fileHandle << values[m - 1] << std::endl;
	}

	//Add a vector element by element separated by spaces
	//on a single line
	//@line = vector container of type T
	template<typename T>
	void AddLine(std::vector<T>& line)
	{
		StreamTest();
		std::vector<T>::iterator vi;
		for (vi = line.begin(); vi != line.end(); vi++)
			m_fileHandle << *vi << " ";
		m_fileHandle << std::endl;
	}

	//Add a value to the log file on the current line
	//@text = text or value to be added
	template<typename T>
	void AddText(const T& text)
	{
		StreamTest();
		m_fileHandle << text;
	}

	//Add an array of values to the current line
	//@m = number of elements to add
	//@values = pointer to first element of array
	template<typename T>
	void AddText(const int& m, const T* values)
	{
		StreamTest();
		m_fileHandle << " ";
		for (int i = 0; i < m; i++)
			m_fileHandle << values[i] << " ";
	}

	//Specialization to prevent adding pointers
	template<typename T>
	void AddText(T*) = delete;

	//Add a null-terminated char* to the current line
	//@text = pointer to first char to write
	void AddText(char* text)
	{
		StreamTest();
		m_fileHandle << std::string(text);
	}

	//Add a string literal to the current line
	//@text = string literal
	void AddText(const char* text)
	{
		StreamTest();
		m_fileHandle << std::string(text);
	}

	//Add a vector element by element to the current line
	//@text = vector of type T
	template<typename T>
	void AddText(std::vector<T>& text)
	{
		StreamTest();
		std::vector<T>::iterator vi;
		for (vi = text.begin(); vi != text.end(); vi++)
			m_fileHandle << *vi << " ";
	}

	//Specialization to prevent writing pointers
	template<typename T>
	void AddText(std::vector<T*>) = delete;

	//Add a vector of null-terminated char* to the current line
	//@vec = vector of char*
	void AddText(std::vector<char*>& vec)
	{
		StreamTest();
		std::vector<char*>::iterator vi;
		m_fileHandle << " ";
		for (vi = vec.begin(); vi != vec.end(); vi++)
			m_fileHandle << std::string(*vi) << " ";
	}

	//Add a vector of string-literals to the current line
	//@vec = vector of string-literals
	void AddText(std::vector<const char*>& vec)
	{
		StreamTest();
		std::vector<const char*>::iterator vi;
		m_fileHandle << " ";
		for (vi = vec.begin(); vi != vec.end(); vi++)
			m_fileHandle << std::string(*vi) << " ";
	}

	//Add contents of a vector of type T on multiple lines
	//@text = vector of type T
	template<typename T>
	void AddMultipleLines(std::vector<T>& vec)
	{
		StreamTest();
		std::vector<T>::iterator vi;
		for (vi = vec.begin(); vi != vec.end(); vi++)
			m_fileHandle << *vi << std::endl;
	}

	//Specialization to prevent writing pointers
	template<typename T>
	void AddMultipleLines(std::vector<T*>) = delete;

	//Add contents of a vector of char* on multiple lines
	//@vec = vector of char*
	void AddMultipleLines(std::vector<char*>& vec)
	{
		StreamTest();
		std::vector<char*>::iterator vi;
		for (vi = vec.begin(); vi != vec.end(); vi++)
			m_fileHandle << std::string(*vi) << std::endl;
	}

	//Add contents of a vector of string-literals 
	//on multiple lines
	//@vec = vector of string-literals
	void AddMultipleLines(std::vector<const char*>& vec)
	{
		StreamTest();
		std::vector<const char*>::iterator vi;
		for (vi = vec.begin(); vi != vec.end(); vi++)
			m_fileHandle << std::string(*vi) << std::endl;
	}

	//Add contents of a 2D array in .csv format
	//@n = number of rows
	//@M = number of columns
	//@values = pointer to first element of array
	template<typename T>
	void AddCsvArray(const int& n, const int& m, const T* values)
	{
		StreamTest();
		for (int i = 0; i < n; i++) {
			for (int j = 0; j < m; j++)
				m_fileHandle << values[i * n + j] << ",";
			m_fileHandle << std::endl;
		}
	}

	//Add contents of a 2D array of complex values in .csv format
	//@n = number of rows
	//@m = number of columns
	//@values = pointer to first element of array
	template<typename T>
	void AddCsvArray(const int&n, const int& m, const std::complex<T>* values)
	{
		StreamTest();
		for (int i = 0; i < n; i++) {
			for (int j = 0; j < m; j++)
				m_fileHandle << values[i*n + j].real() << ","
				<< values[i*n + j].imag() << ",";
			m_fileHandle << std::endl;
		}
	}

	//Specializations to prevent writing pointers
	template<typename T>
	void AddCsvArray(int, T*) = delete;
	template<typename T>
	void AddCsvArray(T*) = delete;
	template<typename T>
	void AddCsvArray(const int&, const int&, const T**) = delete;

	//Add a vector of vectors of type T
	//@values = vector of vectors of values
	template<typename T>
	void AddCsvArray(std::vector<std::vector<T>>& values)
	{
		StreamTest();
		std::vector<std::vector<T>>::iterator ri;
		std::vector<T>::iterator ci;
		for (ri = values.begin(); ri != values.end(); ri++) {
			for (ci = ri->begin(); ci != ri->end(); ci++)
				m_fileHandle << *ci << ",";
			m_fileHandle << std::endl;
		}
	}

	//Specializations to prevent writing pointers
	template<typename T>
	void AddCsvArray(std::vector<std::vector<T*>>) = delete;
	template<typename T>
	void AddCsvArray(std::vector<T*>) = delete;

	//Add a vector of arrays in .csv format
	//@m = length of each array (must be the same for all)
	//@vec = vector of arrays
	template<typename T>
	void AddCsvArray(const int& m, std::vector<T*>& vec)
	{
		StreamTest();
		std::vector<T*>::iterator vi;
		for (vi = vec.begin(); vi != vec.end(); vi++) {
			for (int i = 0; i < m; i++)
				m_fileHandle << vi->[i] << ",";
			m_fileHandle << std::endl;
		}
	}

	//Add contents of a vector separated by commas, end with lf/cr
	//@row = vector to be added as a csv row
	template<typename T>
	void AddCsvRow(std::vector<T>& row)
	{
		StreamTest();
		std::vector<T>::iterator vi;
		for (vi = row.begin(); vi != row.end(); vi++)
			m_fileHandle << *vi << ",";
		m_fileHandle << std::endl;
	}

	//Add contents of an array as a .csv row
	template<typename T>
	void AddCsvRow(const int& m, const T* row)
	{
		StreamTest();
		for (int i = 0; i < m; i++)
			m_fileHandle << row[i] << ",";
		m_fileHandle << std::endl;
	}

	//Specializations to prevent writing pointers
	template<typename T>
	void AddCsvRow(std::vector<T*>) = delete;
	template<typename T>
	void AddCsvRow(T*) = delete;
	template<typename T>
	void AddCsvRow(int, T**) = delete;

	//Add a single field to a .csv file followed by a comma
	template<typename T>
	void AddCsvField(const T& field)
	{
		StreamTest();
		m_fileHandle << field << ",";
	}

	//Add a CRLF
	void Crlf() { m_fileHandle << std::endl; }

	//Return the path to the containing directory as a string
	std::string GetPath() const { return m_filePath; }

	//Return the file name as a string
	std::string GetName() const { return m_fileName; }

	//Obtain a lock on the file
	void GetLock()
	{
		while (!m_logFileMutex.try_lock()) {}
	}

	//Release the lock on the file
	void FreeLock()
	{
		m_logFileMutex.unlock();
	}
};

#endif