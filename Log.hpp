/*****************************************************************************/
/*!
\file   Log.hpp
\author Yeongki Baek
\par    email: yeongki.baek\@digipen.edu
\date   08/01/2017
\brief
This is the interface file for the module
Copyright 2017, Digipen Institute of Technology
*/
/*****************************************************************************/

#pragma once
#include <ostream>
#include <chrono>
#include <fstream>

#ifndef NDEBUG
extern std::chrono::time_point<std::chrono::system_clock> performancetimer_start, performancetimer_end;

#ifdef _WIN32
#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#else
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif

#define START() performancetimer_start = std::chrono::system_clock::now()

struct tracer
{
	std::ofstream fout;
	std::ostream & cout;
	tracer(std::ostream & cout, char const * file, int line, char const * fnc);
	~tracer();

	template<typename TF, typename ... TR>
	void write(TF const& f, TR const& ... rest)
	{
		write_debug_output(cout, f);
		cout << " ";
		fout << " ";
		write(rest...);
	}
	template<typename TF>
	void write(TF const& f)
	{
		write_debug_output(cout, f);
	}
	void write();

	template<typename TF>
	void write_debug_output(std::ostream & cout, TF const& f)
	{
		cout << f;
		fout << f;		
	}
};

#define LOG(...) tracer( std::cout, __FILENAME__, __LINE__, __func__  ).write( __VA_ARGS__ )
#else
#define LOG(...)
#define START()
#define END()
#endif